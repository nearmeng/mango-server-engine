#ifndef TAPP_CPP_H_
#define TAPP_CPP_H_

#include "tapp/tapp.h"
#include "tlog/tlog.h"
#include "tloghelp/tlogload.h"
#include <string>

namespace tapp {

class AppLauncher;
class IApp;
class IAppControllable;
class IAppThreadable;
class TappCtxObject;
class AppDelegate;

/// @brief  Ӧ�ý���Launcher��App�������е���ڡ�App�����ȫ����Դ������
class AppLauncher {
    public:
        /// @brief  app���е���ں��� - ���������ļ�
        ///
        /// @param[in]  app           app����ָ��
        /// @param[in]  argc          �����в��� argc
        /// @param[in]  argv          �����в��� argv
        /// @param[in]  metalib_ptr   �����ļ�tdr Ԫ����������
        /// @param[in]  cfg_name      �����ļ����ýṹmeta����
        /// @param[in]  runcumu_name  �����ļ�runcumu meta����
        /// @param[in]  runstatus_name�����ļ�runstatus meta����
        ///
        /// @return <0: failed
        static int Run(IApp* app, int argc, char **argv,
                       unsigned char* metalib_ptr, const char* cfg_name,
                       const char* runcumu_name, const char* runstatus_name);

        /// @brief  app���е���ں��� - �������ļ�
        ///
        /// @param[in]  app           app����ָ��
        /// @param[in]  argc          �����в��� argc
        /// @param[in]  argv          �����в��� argv
        ///
        /// @return <0: failed
        static int Run(IApp* app, int argc, char **argv);

    public:
        /// @brief  ��ȡȫ��tappctx����ʵ��
        ///
        /// @return ȫ��tappctx����ʵ��������
        static TappCtxObject& tappctx_object();

        /// @brief  ��ȡ�ı���־���
        ///
        /// @return  �ı���־���ָ��
        static LPTLOGCATEGORYINST tlogcat();

        /// @brief  ��ȡ����������־���
        ///
        /// @return  ����������־���ָ��
        static LPTLOGCATEGORYINST data_tlogcat();

        /// @brief  ��ȡ�˵���־���
        ///
        /// @return  �˵���־���ָ��
        static LPTLOGCATEGORYINST bill_tlogcat();

    public:
        /// @brief  ��ȡ��ǰ���н���appָ�룬��ͨ��dynamic_castת����ԭʼapp��ʵ��ָ��
        static IApp* GetUserApp();

    public:
        /// @brief  ���͸���tidx�����߳����ݣ�tidx��1��ʼ
        /// @note   ����ӿڱ�����Run����app�ɹ�����ܵ���
        ///
        /// @param[in]  data  �������ݵ�ָ��
        /// @param[in]  size  ���͵����ݳ���
        /// @param[in]  tidx  �����̱߳��롣��1��ʼ�����Ϊnthread
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int SendDataToWorkerThread(const char* data, size_t size, uint32_t tidx);

        /// @brief  ���͸���tidx�����߳����ݣ�tidx��1��ʼ
        /// @note   ����ӿڱ�����Run����app�ɹ�����ܵ���
        ///
        /// @param[in]  data  �������ݵ�ָ��
        /// @param[in]  size  ���͵����ݳ���
        /// @param[in]  tidx  �����̱߳��롣��1��ʼ�����Ϊnthread
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int ForwardDataToWorkerThread(const char* data, size_t size, uint32_t tidx);

        /// @brief  ���͸���tidx�����߳����ݣ�tidx��1��ʼ
        /// @note   ����ӿڱ�����Run����app�ɹ�����ܵ���
        ///
        /// @param[in]  data  �������ݵ�ָ��
        /// @param[in]  size  ���͵����ݳ���
        /// @param[in]  tidx  �����̱߳��롣��1��ʼ�����Ϊnthread
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int ForwardvDataToWorkerThread(const struct iovec* vec, size_t count, uint32_t tidx);

        /// @brief  �ӹ����߳��н�������
        ///
        /// @param[in, out]  data  �������ݵĻ�����
        /// @param[in, out]  size  ���룺�������ݵĻ�������С�����:���ݰ���С
        /// @param[in]       tidx  �߳�idx��Ĭ��Ϊ0(����ȥ�����߳����հ�)
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int RecvDataFromWorkerThread(char* data, size_t* size, uint32_t tidx=0);

        /// @brief  �ӹ����߳��н������� - ���ڴ濽��
        ///
        /// @param[out]  data  ���ݰ�ָ��
        /// @param[out]  size  ���ݰ���С
        /// @param[in]  tidx  �߳�idx��Ĭ��Ϊ0(����ȥ�����߳����հ�)
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int PeekDataFromWorkerThread(char** p_data, size_t* size, uint32_t tidx=0);

        /// @brief  �ӹ����߳���ɾ������ - ��PeekDataFromWorkerThread���ʹ��
        ///
        /// @param[in]  tidx  �߳�idx��Ĭ��Ϊ0(ɾ�����һ��PeekDataFromWorkerThread�����߳����յ��İ�)
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int DeleteDataFromWorkerThread(uint32_t tidx=0);

        /// @brief  ���ӹ����߳��յ�����Ϣ��backward��ԭʼλ��
        ///
        /// @param[in]  data  ���ݰ�ָ��
        /// @param[in]  size  ���ݰ���С
        ///
        /// @note �������ȵ���PeekDataFromWorkerThread �� RecvDataFromWorkerThread
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int BackwardDataFromWorkerThread(const char* data, size_t size);

        /// @brief  ���ӹ����߳��յ�����Ϣ��backward��ԭʼλ��
        ///
        /// @param[in]  vec    vecָ��
        /// @param[in]  count  iovec�����С
        ///
        /// @note �������ȵ���PeekDataFromWorkerThread �� RecvDataFromWorkerThread
        ///
        /// @return 0 success, !0 failed
        /// @see tapp_error_string()
        static int BackwardvDataFromWorkerThread(const struct iovec* vec, size_t count);

    private:
        AppLauncher();
        virtual ~AppLauncher();
};

/// @brief  Tappҵ����� �ӿ���
/// @note   ���л���tapp��ҵ�񶼱����ṩһ������IApp��ʵ���࣬������Ӧtapp����¼�
class IApp {
    public:
        IApp(){};
        virtual ~IApp(){};

    /* ������Ҫ��ע���¼��ص� */
    public:
        /// @brief ��ʼ��
        ///
        /// @param[in] cfg       ���ýṹ��ָ��
        /// @param[in] runcumu   runcumu�ṹ��ָ��
        /// @param[in] runstatus runstatus�ṹ��ָ��
        /// @param[in] tlogcat   �ı���־���ʵ��ָ��
        ///
        /// @return !0: �����˳�
        ///
        /// @note �������Ҫ���á�rundata�����ԣ����Բ�����cfg��runcumu��runstatus
        virtual int OnInit(void* cfg, void* runcumu, void* runstatus,
                         LPTLOGCATEGORYINST tlogcat);

        /// @brief ��Դ����
        virtual int OnCleanUp();

        /// @brief �����ѭ�����᲻�ϴ���
        /// @return <0: �������̵�ǰ���ڿ���
        virtual int OnUpdate();

        /// @brief ��ܶ�ʱ���¼�
        ///
        /// @note ��ʱ��ʱ������������ʱ--timerָ��������ͨ��
        ///       AppLauncher::tappctx_object()��ȡ��Tapp�����Ķ��󣬵���set_timer_ms������
        ///       ͬʱ��Ҫע����ǣ�����Ķ�ʱ����������ȫ׼ȷ�ģ�ȡ����OnUpdate����ʱ���Ϳ�������ʱ��
        virtual int OnTick();

        /// @brief �����ļ�reload�¼����˽ӿ�
        ///
        /// @note ҵ���յ�reload�������������ļ�Ԥ��顣
        /// @return !0: ���ò��Ϸ������ò��ᱻ�滻�����ᴥ��OnReload�¼�
        ///          0: ���úϷ������ûᱻ�滻���µģ�����OnReload�¼�
        /// @note �ر�˵�������ҵ����Ҫ���������ã���Ҫ�������б������õĸ���
        /// @note Ĭ�Ϸ���-1�����ҵ��ϣ��֧�������ļ�reload��������������ӿ�
        virtual int CheckCfgPreOnReload(void* new_cfg);

        /// @brief �����ļ�reload�¼�
        /// @note ҵ������ڴ�����ȸ�������
        virtual int OnReload(void* cfg);

        /// @brief ���̿����¼�
        ///
        /// @param[in] sleep_ms idle sleep����ֵ
        /// @note �������ע�Ļ���Ĭ������ sleep_msָ���ĺ���ʱ��
        virtual int OnIdle(int sleep_ms);

        /// @brief ����tapp�����ġ��ڳ�ʼ��֮ǰ��һ��Hook�¼�����ܻ���ô˽ӿ�
        /// @note ���ҵ����Ҫ������ǰ�������ѡ���Ҫʵ�ִ���ӿ�
        ///       ע�⣺�˽ӿ�ֻ���ṩһ��Hook����Ҫҵ�����е���AppLauncher::tappctx_object()
        ///       ȡ��tappctx���ã�Ȼ���ٵ�����Ӧ��set�ӿ���ɣ�ͨ���ڴ�����version�汾ֵ��
        ///       ������ tappctx ʾ���� set_tapp_formated_version���������巽���ο�TappCtxObject�ĸ��ӿ�˵����
        ///
        /// @return <0�����ʼ��ʧ��ֱ���˳���Ĭ�Ϸ���0���Ҳ����κδ���
        virtual int SetTappCtxPara();

    /* ������Ҫ��ע���¼��ص� - ���������С������˳��¼�����tbus�Զ˽���״̬���֪ͨ */
    public:
        /// @brief �յ�stop�¼�
        /// @return <0 �˳���ѭ���� Ĭ�Ϸ���-1�����˳���ѭ����
        virtual int OnStop();

        /// @brief �յ�quit�¼�
        /// @return <0 �˳���ѭ���� Ĭ�Ϸ���-1�����˳���ѭ����
        virtual int OnQuit();

        /// @brief ���ҵ���Զ���������ѡ��ڳ�ʼ��֮ǰ��һ��Hook�¼�����ܻ���ô˽ӿ�
        /// @note ���ҵ������Ҫ����Ҫʵ�ִ˽ӿ�
        ///       ע�⣺�˽ӿ�ֻ���ṩһ��Hook������ע����������Ҫ���е���tapp_register_option
        ///
        /// @return !0, ��ʼ��ʧ���˳���Ĭ�Ϸ���0
        virtual int AddUserDefinedCmdLine();

        /// @brief tbusͨ���Զ˽���״̬�����仯�¼�
        ///
        /// @param[in]  bus_addr  �Զ�bus��ַ
        /// @param[in]  new_state �Զ˷���״̬
        ///
        /// @note ���������tbus����̽�⣬�ҹ���tbus�Զ˷���״̬����Ҫ��׽���¼��������Ӧ�Ĳ���
        ///       ����tbus����������������ҵ�����������������ƣ���Ҫ����tbus����̽��
        virtual int onBusPeerStateFlipped(TBUSADDR bus_addr, int new_state);

    /* ������Ҫ��ע���¼��ص� - ����ģʽ */
    public:
        /// @brief ���ҵ�����ģʽ����
        /// @note �����Ҫ���ƿ���ģʽ����Ҫʵ��IAppControllable�ӿ��ಢʵ����һ��ȫ�ֶ��󣬲�ͨ���˽ӿڷ���
        /// @return ���ؿ���ģʽ���������ʵ�ִ���ӿڣ�Ĭ�Ϸ���NULL
        ///
        /// @note ���뱣֤���صĶ��������������app�����ڼ䶼��Ч
        virtual IAppControllable* GetAppControllableObject();

    public:
        /// @brief ���ҵ��worker-dispatcher���߳�ģ�Ͷ���
        /// @note �����Ҫ����tapp���̣߳���Ҫʵ��IAppThreadable�ӿ��ಢʵ����һ��ȫ�ֶ��󣬲�ͨ���˽ӿڷ���
        /// @return ���ض��߳�ģʽ���������ʵ�ִ���ӿڣ�Ĭ�Ϸ���NULL
        ///
        /// @note ���뱣֤���صĶ��������������app�����ڼ䶼��Ч
        virtual IAppThreadable* GetAppThreadableObject();

    /* һ�㲻��Ҫ��ע���¼��ص� */
    public:
        /// @brief ������̰�����Ϣ
        /// @return 0:�����Ϣ��ͬʱ���������������ɰ�����Ϣ
        ///        !0:ֻ����˽ӿڵ���Ϣ���������������ɰ�����Ϣ
        ///        Ĭ��ֻ���������ɵİ�����Ϣ������һ������²���ʵ�ִ���ӿڡ�
        virtual int PrintUsage(int argc, char** argv);

        /// @brief ע�����������Ϣ
        /// @return ���ط���������Ϣ
        /// @note һ����windows����ģʽ�£��������Ʒ����������Ϣ
        virtual const char* GetServiceDescription();

        /// @brief ���ҵ����̰汾��Ϣ
        /// @note  ����ͨ��set_tapp_formated_version����tapp��ʽ�İ汾�ţ���ȥʵ�ִ���ӿڣ�����Ĭ����Ϊ��
        virtual void PrintAppVersion();
};

/// @brief Tapp ����ģʽ�ӿ���
/// @note ���ҵ����Ҫ������Ӧ�Ŀ���ģʽ����Ҫ�ṩһ��������ITappControllable��ʵ����
class IAppControllable {
    public:
        IAppControllable(){}
        virtual ~IAppControllable(){}

    /* console���ƶ� �� server�����ƶ˵Ĺ�ϵ���ֿ������Ϊclient �� server�Ĺ�ϵ
     */
    public:
        /* console ���ƶ� */
        /// @brief console�˳�ʼ���ӿ�
        /// @note ����ģʽ�£���ܻ��������ô���ӿ�
        virtual int InitConsoleController();

        /// @brief console������ӿ�
        /// @note ����ģʽ�£���ܻ��������ô���ӿ�
        virtual int FiniConsoleController();

        /// @brief console�������й��˽ӿ�
        /// @return 0��ֻ��console�˴��������������ƶ�
        ///        !0: ֱ�ӷ��͸������ƶ�
        virtual int PreprocessCmdLine(int argc, const char** argv);

        /// @brief �����ƿ���������Ӧ�¼�
        virtual int OnBinaryCtrlResponse(const char* meta_name,
                                         const char* buff, size_t buf_len);

        // server �����ƶ�
        /// @brief �յ�console ����helpѯ���¼����ڴ����֧�ֵĿ�������
        /// @return ���ؿ��ư�����Ϣ
        virtual const char* OnGetCtrlUsage();

        /// @brief �յ�console ��������������������ڴ�����Ӧ
        virtual int OnCmdLineRequest(int argc, const char** argv);

        /// @brief �յ�console ����Ķ��������������ڴ�����Ӧ
        virtual int OnBinaryCtrlRequest(const char* meta_name,
                                        const char* buff, size_t buf_len);
};

/// @brief Tapp ���߳�ģʽ�ӿ���
/// @note ���ҵ����Ҫ֧��worker-dispatcher���߳�ģʽ����Ҫ�ṩһ��������IAppThreadable��ʵ����
class IAppThreadable {
    public:
        IAppThreadable(){}
        virtual ~IAppThreadable(){}

    public:
        /// @brief worker�߳�Init�¼�
        ///
        /// @param[in]  thread_ctx  �����߳�ctx
        /// @param[in]  cfg         ���������ļ�ָ��
        /// @param[in]  runcumu     �����߳�runcumuָ��
        /// @param[in]  runstatus   �����߳�runstatusָ��
        virtual int OnThreadInit(LPTAPPTHREADCTX thread_ctx, void* arg,
                                 void* cfg, void* runcumu, void* runstatus);

        /// @brief worker�߳�Update�¼�
        ///
        /// @param[in]  thread_ctx  �����߳�ctx
        virtual int OnThreadUpdate(LPTAPPTHREADCTX thread_ctx, void* arg);

        /// @brief worker�߳�CleanUp�¼��ص�
        ///
        /// @param[in]  thread_ctx  �����߳�ctx
        virtual int OnThreadCleanUp(LPTAPPTHREADCTX thread_ctx, void* arg);

        /// @brief worker�߳�Reload�¼��ص�
        ///
        /// @param[in]  thread_ctx  �����߳�ctx
        /// @param[in]  cfg         reload���������ָ��
        virtual int OnThreadReload(LPTAPPTHREADCTX thread_ctx, void* arg, void* cfg);

        /// @brief worker�߳�Tick�¼��ص�
        ///
        /// @param[in]  thread_ctx  �����߳�ctx
        virtual int OnThreadTick(LPTAPPTHREADCTX thread_ctx, void* arg);

        /// @brief  Ϊÿ��worker�̴߳����û��Զ����������
        ///
        /// @return NULL ����Ҫ�Զ������
        ///        !NULL ����worker�̲߳�����ȫ�ֶ���
        ///
        /// @note !!!! ���뱣֤���صı�����ͨ��malloc ���� new��������� !!!!
        virtual void* NewThreadArgvObject();

        /// @brief �ͷ�Ϊÿ��worker�̴߳����Ĳ�������
        ///
        /// @param[in]  obj NewThreadArgvObject�����Ķ���
        ///
        /// @return 0 success, !0 failed
        ///
        /// @note ��NewThreadArgvObject����ʹ��
        /// @note ���뱣֤NewThreadArgvObject���ط�NULL����ʱ��DeleteDataFromWorkerThread��ʵ��ȥ�ͷŴ˶���
        virtual int DeleteThreadArgvObject(void* obj);

        /// @brief �߳�runcumuԪ��������
        virtual const char* GetThreadRunCumuName();
        /// @brief �߳�runstatusԪ��������
        virtual const char* GetThreadRunStatusName();
        /// @brief �߳�����
        virtual const char* GetThreadName();
};

class TappCtxObject {
    /* set ��ӿ� */
    public:
        /* ���ýӿ� - tdr Ԫ������ؽӿ� */

        /* ���������ļ���tdrԪ���������� */
        void set_tdr_metalib(unsigned char metalib[]) {
            tapp_ctx_ref_.iLib = (intptr_t)metalib;
        }

        /* ���ýṹ��meta�� */
        void set_cfg_metaname(char* name) {
            tapp_ctx_ref_.stConfData.pszMetaName = name;
        }

        /* runcumu�ṹ��meta�� */
        void set_runcumu_metaname(char* name) {
            tapp_ctx_ref_.stRunDataCumu.pszMetaName = name;
        }

        /* runstatus�ṹ��meta�� */
        void set_runstatus_metaname(char* name) {
            tapp_ctx_ref_.stRunDataStatus.pszMetaName = name;
        }

        /* tapp�淶�µİ汾ֵ */
        /* �汾�����ʽΪ major.minor.rev.build */
        void set_tapp_formated_version(int major, int minor, int rev, int build) {
            tapp_ctx_ref_.uiVersion = TAPP_MAKE_VERSION(major, minor, rev, build);
        }

    public:
        /* ���ýӿ� - ���á�tbus��ؽӿ� */

        /* ������ģʽ */
        void set_no_load_cfg() {
            tapp_ctx_ref_.iNoLoadConf = 1;
        }

        /* bus key */
        void set_buskey(char* key) {
            tapp_ctx_ref_.pszGCIMKey = key;
        }

        /* bus id */
        void set_bus_addr(char* addr) {
            tapp_ctx_ref_.pszId = addr;
        }

        /* use bus */
        void set_use_bus_on() {
            tapp_ctx_ref_.iUseBus = 1;
        }

        /* NOT use bus */
        void set_use_bus_off() {
            tapp_ctx_ref_.iUseBus = 0;
        }

        /* bus timeout gap */
        void set_bus_timeout_gap(int max_timeout) {
            tapp_ctx_ref_.iBusTimeOutGap = max_timeout;
        }

        /* bus use saved time */
        void set_bus_use_saved_time(bool use_saved) {
            tapp_ctx_ref_.iBusUseFreshTime = use_saved ? 1 : 0;
        }

        /* bus heartbeat gap */
        void set_bus_heartbeat_gap(int heartbeat_gap) {
            tapp_ctx_ref_.iBusHeartBeatGap = heartbeat_gap;
        }

        /* bus check peer gap */
        void set_bus_check_peer_gap(int checkpeer_gap) {
            tapp_ctx_ref_.iBusCheckPeerGap = checkpeer_gap;
        }

    public:
        /* ���߳���ؽӿ� */

        /* �����߳��� */
        void set_worker_thread_num(int num) {
            tapp_ctx_ref_.iNthreads = num;
        }

        /* ���߳��빤���߳�ͨ��ͨ����С */
        void set_thread_channel_size(size_t size) {
            tapp_ctx_ref_.iChannelSize = size;
        }

    public:
        /* �����ӿ� */

        /* xml�����ļ���ʽ */
        void set_cfg_format(enum tagTDRXmlDataFormat format) {
            tapp_ctx_ref_.iConfigFileFormat = format;
        }

        /* ҵ�� id */
        void set_bussiness_id(int id) {
            tapp_ctx_ref_.iBusinessID = id;
        }

        /* ��� tsm key */
        void set_tsm_idx_key(char* key) {
            tapp_ctx_ref_.pszTsmIdxKey = key;
        }

        /* tick ��ʱ������ */
        void set_timer_ms(int timer) {
            tapp_ctx_ref_.iTimer = timer;
        }

        /* idle ���� */
        void set_idle_count(int count) {
            tapp_ctx_ref_.iIdleCount = count;
        }

        /* idle ����ʱ�� */
        void set_idle_sleep(int time_ms) {
            tapp_ctx_ref_.iIdleSleep = time_ms;
        }

        /* stat��־��� - �ر� */
        void set_stat_log_off() {
            tapp_ctx_ref_.iNotOutputStat = 1;
        }

        /* stat��־��� - ���� */
        void set_stat_log_on() {
            tapp_ctx_ref_.iNotOutputStat = 0;
        }

    /* get ��ӿ� */
    public:
        /* ���ýӿ� - ҵ��������� */
        /* ���ýṹ��ָ�룬��Ҫ�ϲ�����ת�� */
        void* get_cfg_ptr() {
            return tapp_ctx_ref_.stConfData.pszBuff;
        }

        size_t get_cfg_size() const {
            return tapp_ctx_ref_.stConfData.iLen;
        }

        intptr_t get_cfg_meta() const {
            return tapp_ctx_ref_.stConfData.iMeta;
        };

        /* reload���ýṹ��ָ�룬��Ҫ�ϲ�����ת�� */
        void* get_reload_cfg_ptr() {
            return tapp_ctx_ref_.stConfPrepareData.pszBuff;
        }

        size_t get_reload_cfg_size() const {
            return tapp_ctx_ref_.stConfPrepareData.iLen;
        }

        /* runcumu�ṹ��ָ�룬��Ҫ�ϲ�����ת�� */
        void* get_runcumu_ptr() {
            return tapp_ctx_ref_.stRunDataCumu.pszBuff;
        }

        size_t get_runcumu_size() const {
            return tapp_ctx_ref_.stRunDataCumu.iLen;
        }

        /* runstatus�ṹ��ָ�룬��Ҫ�ϲ�����ת�� */
        void* get_runstatus_ptr() {
            return tapp_ctx_ref_.stRunDataStatus.pszBuff;
        }

        size_t get_runstatus_size() {
            return tapp_ctx_ref_.stRunDataStatus.iLen;
        }

    public:
        /* ���ýӿ� - ���̱�ʶ��� */
        /* tbus��� */
        int get_bus_handler() const {
            return tapp_ctx_ref_.iBus;
        }

        /* ҵ��id */
        int get_bussiness_id() const {
            return tapp_ctx_ref_.iBusinessID;
        }

        /* ���� id */
        int get_bus_id() const {
            return tapp_ctx_ref_.iId;
        }

        /* ���� id - string��ʽ */
        const char* get_bus_id_string() const {
            return tapp_ctx_ref_.pszId;
        }

        /* �Ƿ�ʹ��tbus */
        bool is_use_bus() const {
            return tapp_ctx_ref_.iUseBus;
        }

        const char* get_buskey() const {
            return tapp_ctx_ref_.pszGCIMKey;
        }

    public:
        /* ϵͳʱ�� */
        struct timeval get_curr_timeval() {
            return tapp_ctx_ref_.stCurr;
        }

    public:
        /* ���߳���� */
        /* �����߳����� */
        int get_worker_thread_num() const {
            return tapp_ctx_ref_.iNthreads;
        }

        /* �̼߳�ͨ�Ŷ��д�С - ��λ��Byte */
        size_t get_thread_channel_size() const {
            return tapp_ctx_ref_.iChannelSize;
        }

    public:
        /* app ������ */
        const char* get_app_name() const {
            return (tapp_ctx_ref_.pszApp != NULL) ? tapp_ctx_ref_.pszApp : "";
        }

        /* ���tsm idx */
        const char* get_tsm_idx_key() const {
            return tapp_ctx_ref_.pszTsmIdxKey ? tapp_ctx_ref_.pszTsmIdxKey : "";
        }

        /* mbase key */
        const char* get_mbase_key() const {
            return tapp_ctx_ref_.pszMBaseKey;
        }

        /* tapp ��ʽ�汾�� */
        unsigned long long get_tapp_formated_version() const {
            return tapp_ctx_ref_.uiVersion;
        }

    public:
        /* ��ȫ͸���ĳ�Ա */

        /* epoll wait time */
        int get_epoll_wait() const {
            return tapp_ctx_ref_.iEpollWait;
        }

        /* �Ƿ� resume ģʽ */
        bool is_resume_mode() const {
            return (TAPP_START_MODE_RESUME == tapp_ctx_ref_.iStartMode) ?
                true : false;
        }

    public:
        inline LPTAPPCTX tapp_ctx() {
            return &tapp_ctx_ref_;
        }

    private:
        TappCtxObject(TAPPCTX& ctx) : tapp_ctx_ref_(ctx){}
        TappCtxObject& operator=(const TappCtxObject&);
        TappCtxObject();

    private:
        TAPPCTX& tapp_ctx_ref_;
        friend class AppDelegate;
};

}

#endif
