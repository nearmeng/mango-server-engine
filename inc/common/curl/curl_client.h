#ifndef _CURL_CLIENT_H
#define _CURL_CLIENT_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <cstring>
#if defined(WIN32) || defined(WIN64)
#include "win/curl.h"
#else
#include "linux/curl.h"
#endif

#define CURL_CONNECT_TIMEOUT_MS  5000
#define CURL_TRANSFER_TIMEOUT_MS 10000
#define REST_MAX_CB_DATA_LEN     (512) 
#define REST_CLIENT_RUN_INTERVAL 500   // 500 ms
#define MAX_CURL_CONTEXT_COUNT	 (4096)

typedef std::map<std::string, std::string> header_map;

typedef int (*fn_rsp_callback)(int ret, const std::string& response,
                               const void *cb_data, int cb_len);

struct rest_context {
    CURL *handle;
    curl_slist *headers;
    fn_rsp_callback callback;
    int cb_data_len;
    char cb_data[REST_MAX_CB_DATA_LEN];
    std::string content;

    rest_context()
        : handle(curl_easy_init()),
          headers(NULL),
          callback(NULL),
          cb_data_len(0)
    {}

    ~rest_context() {
        if (handle) {
            curl_easy_cleanup(handle);
            handle = NULL;
        }

        if (headers) {
            curl_slist_free_all(headers);
            headers = NULL;
        }
    }

    void add_headers(const header_map& headers) {
        header_map::const_iterator it, end;
        for (it = headers.begin(), end = headers.end(); it != end; ++it) {
            std::string str(it->first);
            str.append(": ").append(it->second);
            this->headers = curl_slist_append(this->headers, str.c_str());
        }
    }

    int set_cb_data(const void *data, int len) {
        if (!data || len <= 0)
            return 0;

        if(len >= REST_MAX_CB_DATA_LEN)
        {
            CRI("len <= REST_MAX_CB_DATA_LEN");
            return -1;
        }

        memcpy(cb_data, data, len);
        cb_data_len = len;

        return 0;
    }

    void reset() {
        curl_easy_cleanup(handle);
		handle = curl_easy_init();
        if (headers) {
            curl_slist_free_all(headers);
            headers = NULL;
        }
        callback = NULL;
        cb_data_len = 0;
        content.clear();
    }
};

class CCurlClient
{
public:
    CCurlClient();
    ~CCurlClient();

	int init();
	int fini();
	inline static CCurlClient& instance(void);
	
	int set_host(const char *host);
    const std::string& host() const {
        return host_;
    }

    int get(const char *uri, fn_rsp_callback callback,
            const std::string *parameters, const header_map *headers,
            const void *cb_data, int cb_len);

    int post(const char *uri, fn_rsp_callback callback,
             const header_map *headers, const char *payload, size_t len,
             const void *cb_data, int cb_len);

    int mainloop(void);

	header_map create_header_map() {
		header_map headers;
		headers["Content-Type"] = "application/x-www-form-urlencoded";
		return headers;
	}

private:
    rest_context *init_context(fn_rsp_callback callback, const header_map *headers,
                               const void *cb_data, int cb_len);

    std::vector<rest_context*> free_contexts_;
    std::set<rest_context*>    busy_contexts_;

    std::string host_;
    CURLM *mh_;

	static CCurlClient ms_Instance;
};

inline CCurlClient& CCurlClient::instance(void)
{
	return ms_Instance;
}

#endif // CURL_CLIENT_H
