#include "curl/curl_client.h"

#include <errno.h>
#include <algorithm>

CCurlClient CCurlClient::ms_Instance;

using namespace std;

static void destroy_context(rest_context *ctx) {
    if(ctx == NULL)
        return ;
        
    if (ctx->handle) {
        curl_easy_cleanup(ctx->handle);
        ctx->handle = NULL;
    }

    if (ctx->headers) {
        curl_slist_free_all(ctx->headers);
        ctx->headers = NULL;
    }
    delete ctx;
}

static int curl_proc_log(CURL *handle, curl_infotype type, char *data, size_t size, void *) {
    (void)handle;
    const char *text = NULL;

    switch (type) {
    case CURLINFO_TEXT:
    default: /* in case a new one is introduced to shock us */
        return 0;

    case CURLINFO_HEADER_OUT:
        text = "=> send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> send ssl data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= recv ssl data";
        break;
    }

    DBG("curl %s, %d, %s", text, size, data);
    return 0;
}

static size_t curl_proc_rsp(char *ptr, size_t size, size_t nmemb, void *userp) {
    rest_context *ctx = (rest_context *) userp;
    size_t total_len = size * nmemb;
    ctx->content.append(ptr, total_len);
    return total_len;
}

CCurlClient::CCurlClient() {
	mh_ = NULL;
}

CCurlClient::~CCurlClient() {
    
}

int CCurlClient::init() {
    curl_global_init(CURL_GLOBAL_ALL);
    mh_ = curl_multi_init();

	return 0;
}

int CCurlClient::set_host(const char *host) {
	if(host!=NULL)
	{
		host_ = host;
		return 0;
	}
	else
	{
		return -1;
	}
}

int CCurlClient::fini() {
    std::for_each(free_contexts_.begin(), free_contexts_.end(), destroy_context);
    std::for_each(busy_contexts_.begin(), busy_contexts_.end(), destroy_context);
    free_contexts_.clear();
    busy_contexts_.clear();

    curl_multi_cleanup(mh_);
    curl_global_cleanup();

	mh_ = NULL;
	return 0;
}

int CCurlClient::get(const char *uri, fn_rsp_callback callback,
                     const std::string *parameters, const header_map *headers,
                     const void *cb_data, int cb_len) {
    rest_context *ctx = init_context(callback, headers, cb_data, cb_len);
    if(ctx == NULL)
    {
        CRI("ctx is NULL");
        return -1;
    }

    std::string url(host_);

    if (uri)
        url.append(uri);

    if (parameters)
        url = url + '?' + *parameters;

    curl_easy_setopt(ctx->handle, CURLOPT_URL, url.c_str());

    CURLMcode ret = curl_multi_add_handle(mh_, ctx->handle);
    if (ret != CURLM_OK) {
        destroy_context(ctx);
        return -1;
    }

    busy_contexts_.insert(ctx);

    return 0;
}

int CCurlClient::post(const char *uri, fn_rsp_callback callback,
                      const header_map *headers, const char *payload, size_t len,
                      const void *cb_data, int cb_len) {
    if(payload == 0 || len <= 0)
    {
        return -1;
    }


    rest_context *ctx = init_context(callback, headers, cb_data, cb_len);
    if(ctx == NULL)
    {
        return -1;
    }
    
    std::string url(host_);

    if (uri)
        url.append(uri);

    curl_easy_setopt(ctx->handle, CURLOPT_URL, url.c_str());

    curl_easy_setopt(ctx->handle, CURLOPT_POST, 1);
    curl_easy_setopt(ctx->handle, CURLOPT_POSTFIELDSIZE, len);
    curl_easy_setopt(ctx->handle, CURLOPT_COPYPOSTFIELDS, payload);

    CURLMcode ret = curl_multi_add_handle(mh_, ctx->handle);
    if (ret != CURLM_OK) {
        destroy_context(ctx);
        return -1;
    }

    busy_contexts_.insert(ctx);

    return 0;
}

int CCurlClient::mainloop() {
    int running = 0;

	if (mh_ == NULL)
		return 0;

	curl_multi_perform(mh_, &running);

    int msg_left = 0;
    CURLMsg *msg = curl_multi_info_read(mh_, &msg_left);

    while (msg) {
        if (msg->msg == CURLMSG_DONE) {

            rest_context *ctx = NULL;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &ctx);

            const void *cb_data = (const void *) ctx->cb_data;
            if (ctx->cb_data_len <= 0)
                cb_data = NULL;

            ctx->callback(msg->data.result, ctx->content, cb_data, ctx->cb_data_len);

            busy_contexts_.erase(ctx);
            free_contexts_.push_back(ctx);
            curl_multi_remove_handle(mh_, msg->easy_handle);
        }

        msg = curl_multi_info_read(mh_, &msg_left);
    }

    return 0;
}

rest_context *CCurlClient::init_context(fn_rsp_callback callback, const header_map *headers,
                                        const void *cb_data, int cb_len) {
    rest_context *ctx = NULL;

	if (free_contexts_.size() + busy_contexts_.size() >= MAX_CURL_CONTEXT_COUNT)
	{
		CRI("too many context created!, free_context %d buy_contexts %d", free_contexts_.size(), busy_contexts_.size());
		return NULL;
	}

    do {
        if (!free_contexts_.empty()) {
            ctx = *free_contexts_.rbegin();
            free_contexts_.pop_back();
            ctx->reset();
            break;
        }

        ctx = new rest_context();
		if (ctx)
		{
			INF("begin to new rest context, free_contexts %d busy_contexts %d total_count %d", free_contexts_.size(), busy_contexts_.size(),
							free_contexts_.size() + busy_contexts_.size());
			break;
		}

        return NULL;

    } while (0);

    CURL *h = ctx->handle;
    if (!h) {
        destroy_context(ctx);
        return NULL;
    }

    ctx->callback = callback;
    int ret = ctx->set_cb_data(cb_data, cb_len);
    if (ret != 0) {
        destroy_context(ctx);
        return NULL;
    }

    curl_easy_setopt(h, CURLOPT_CONNECTTIMEOUT_MS, CURL_CONNECT_TIMEOUT_MS);
    curl_easy_setopt(h, CURLOPT_TIMEOUT_MS, CURL_TRANSFER_TIMEOUT_MS);

    if (tlog_category_is_priority_enabled(g_pSvrLogCat, TLOG_PRIORITY_DEBUG)) {
        curl_easy_setopt(h, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(h, CURLOPT_DEBUGFUNCTION, curl_proc_log);
    }
    curl_easy_setopt(h, CURLOPT_SSL_VERIFYPEER, 0L); 
    curl_easy_setopt(h, CURLOPT_SSL_VERIFYHOST, 0L); 
    
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, curl_proc_rsp);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, (void *) ctx);

    if (headers) {
        ctx->add_headers(*headers);
        if (ctx->headers)
            curl_easy_setopt(h, CURLOPT_HTTPHEADER, ctx->headers);
    }

    curl_easy_setopt(h, CURLOPT_PRIVATE, ctx);

    return ctx;
}
