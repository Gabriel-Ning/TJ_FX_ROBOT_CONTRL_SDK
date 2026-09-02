#include "Robot.h"
#include "stdio.h"
#include "stdlib.h"
#include <errno.h>
#include <limits.h>
#include <string.h>
#ifdef CMPL_LIN
#include <sched.h>
#include <time.h>
#endif


static CRobot* m_InsRobot = NULL;

bool CRobot::OnClearChDataA()
{
	if (m_InsRobot == NULL)
	{
		return false;
	}
	DDSS t;
	long si = sizeof(DDSS);
	long num = m_InsRobot->m_ACB1.ReadBuf((unsigned char*)&t, si);
	while (num > 0)
	{
		num = m_InsRobot->m_ACB1.ReadBuf((unsigned char*)&t, si);
	}
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Clear 485 cache of A arm\n");

	return true;
}


bool CRobot::OnClearChDataB()
{
	if (m_InsRobot == NULL)
	{
		return false;
	}
	DDSS t;
	long si = sizeof(DDSS);
	long num = m_InsRobot->m_ACB2.ReadBuf((unsigned char*)&t, si);
	while (num > 0)
	{
		num = m_InsRobot->m_ACB2.ReadBuf((unsigned char*)&t, si);
	}
    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Clear 485 cache of B arm\n");
	return true;
}


long CRobot::OnGetChDataA(unsigned char data_ptr[256], long* ret_ch)
{
	if (m_InsRobot == NULL)
	{
		return 0;
	}
	DDSS t;
	long si = sizeof(DDSS);
	long num =  m_InsRobot->m_ACB1.ReadBuf((unsigned char *) & t, si);
	if (num == 0)
	{
		return num;
	}

    memset(data_ptr,0,256);
	*ret_ch = t.m_SUB_CH;
	memcpy(data_ptr, t.m_Data, t.m_Size);

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Get 485 of A arm: \nchannel =%d\n",ret_ch);
		// 同时显示十六进制和字符形式
		printf("data:\n");
        for (int i = 0; i < 256; ++i) {
            printf("%02x ", data_ptr[i]);

            if ((i + 1) % 16 == 0) {
                printf("  ");
                for (int j = i-15; j <= i; j++) {
                    printf("%c", (data_ptr[j] >= 32 && data_ptr[j] <= 126) ? data_ptr[j] : '.');
                }
                printf("\n");
            }
        }
	    printf("\ndata size=%d \n",t.m_Size);

	}
	return t.m_Size;

}
bool CRobot::OnSetChDataA(unsigned char* data_ptr, long size_int, long set_ch)
{
	if (size_int <= 0 || size_int >256)
	{
		return false;
	}
	if (m_InsRobot == NULL)
	{
		return false;
	}

	long serial = m_InsRobot->pDDSS1->m_Serial + 1;
	if (serial > 1000000)
	{
		serial = 1;
	}
	m_InsRobot->pDDSS1->m_Serial = serial;
	m_InsRobot->pDDSS1->m_Size = size_int;
	m_InsRobot->pDDSS1->m_SUB_CH = set_ch;
	memcpy(m_InsRobot->pDDSS1->m_Data, data_ptr, size_int);
	sendto(m_InsRobot->_tosock_, (char*)m_InsRobot->m_SendBuf1, sizeof(DDSS)+2, 0, (struct sockaddr*)&m_InsRobot->_to, sizeof(m_InsRobot->_to));

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set 485 of A arm: \nchannel =%d\n",set_ch);
		// 同时显示十六进制和字符形式
		printf("data:\n");
        for (int i = 0; i < 256; ++i) {
            printf("%02x ", data_ptr[i]);

            if ((i + 1) % 16 == 0) {
                printf("  ");
                for (int j = i-15; j <= i; j++) {
                    printf("%c", (data_ptr[j] >= 32 && data_ptr[j] <= 126) ? data_ptr[j] : '.');
                }
                printf("\n");
            }
        }
	    printf("\ndata size=%d \n",size_int);
	}

	return true;
}






long CRobot::OnGetChDataB(unsigned char data_ptr[256], long* ret_ch)
{
//	printf("get 485 B :1\n");
	if (m_InsRobot == NULL)
	{
//		printf("get 485 B :2\n");
		return 0;
	}
	DDSS t;
	long si = sizeof(DDSS);
	long num = m_InsRobot->m_ACB2.ReadBuf((unsigned char*)&t, si);
	if (num == 0)
	{
//		printf("get 485 B :3\n");
		return num;
	}


	memset(data_ptr, 0, 256);
	*ret_ch = t.m_SUB_CH;
	memcpy(data_ptr, t.m_Data, t.m_Size);

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Get 485 of B arm: \nchannel =%d\n",ret_ch);
		// 同时显示十六进制和字符形式
		printf("data:\n");
        for (int i = 0; i < 256; ++i) {
            printf("%02x ", data_ptr[i]);

            if ((i + 1) % 16 == 0) {
                printf("  ");
                for (int j = i-15; j <= i; j++) {
                    printf("%c", (data_ptr[j] >= 32 && data_ptr[j] <= 126) ? data_ptr[j] : '.');
                }
                printf("\n");
            }
        }
	    printf("\ndata size=%d \n",t.m_Size);

	}
	return t.m_Size;
}
bool CRobot::OnSetChDataB(unsigned char* data_ptr, long size_int, long set_ch)
{
	if (size_int <= 0 || size_int >256)
	{
		return false;
	}
	if (m_InsRobot == NULL)
	{
		return false;
	}

	long serial = m_InsRobot->pDDSS2->m_Serial + 1;
	if (serial > 1000000)
	{
		serial = 1;
	}
	m_InsRobot->pDDSS2->m_Serial = serial;
	m_InsRobot->pDDSS2->m_Size = size_int;
	m_InsRobot->pDDSS2->m_SUB_CH = set_ch;
	memcpy(m_InsRobot->pDDSS2->m_Data, data_ptr, size_int);
	sendto(m_InsRobot->_tosock_, (char*)m_InsRobot->m_SendBuf2, sizeof(DDSS) + 2, 0, (struct sockaddr*)&m_InsRobot->_to, sizeof(m_InsRobot->_to));

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set 485 of B arm: channel =%d\n",set_ch);
		// 同时显示十六进制和字符形式
		printf("data:\n");
        for (int i = 0; i < 256; ++i) {
            printf("%02x ", data_ptr[i]);

            if ((i + 1) % 16 == 0) {
                printf("  ");
                for (int j = i-15; j <= i; j++) {
                    printf("%c", (data_ptr[j] >= 32 && data_ptr[j] <= 126) ? data_ptr[j] : '.');
                }
                printf("\n");
            }
        }
	    printf("\ndata size=%d \n",size_int);
	}

	return true;
}

long CRobot::OnGetSDKVersion()
{
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: SDK version %d\n",SDK_VERSION);
	return SDK_VERSION;
}

bool CRobot::OnGetIoStats(MarvinIoStats* stats)
{
	if (m_InsRobot == NULL || stats == NULL) return false;
	stats->tick_count = m_InsRobot->m_IoTickCount.load(std::memory_order_relaxed);
	stats->deadline_miss_count = m_InsRobot->m_IoDeadlineMissCount.load(std::memory_order_relaxed);
	stats->max_lateness_ns = m_InsRobot->m_IoMaxLatenessNs.load(std::memory_order_relaxed);
	stats->publish_count = m_InsRobot->m_PublishCount.load(std::memory_order_relaxed);
	stats->overwrite_count = m_InsRobot->m_OverwriteCount.load(std::memory_order_relaxed);
	stats->send_attempt_count = m_InsRobot->m_SendAttemptCount.load(std::memory_order_relaxed);
	stats->send_success_count = m_InsRobot->m_SendSuccessCount.load(std::memory_order_relaxed);
	stats->send_error_count = m_InsRobot->m_SendErrorCount.load(std::memory_order_relaxed);
	stats->last_send_errno = m_InsRobot->m_LastSendErrno.load(std::memory_order_relaxed);
	stats->send_work_total_ns = m_InsRobot->m_SendWorkTotalNs.load(std::memory_order_relaxed);
	stats->recv_work_total_ns = m_InsRobot->m_RecvWorkTotalNs.load(std::memory_order_relaxed);
	stats->send_work_max_ns = m_InsRobot->m_SendWorkMaxNs.load(std::memory_order_relaxed);
	stats->recv_work_max_ns = m_InsRobot->m_RecvWorkMaxNs.load(std::memory_order_relaxed);
	stats->io_sched_policy = m_InsRobot->m_IoSchedPolicy.load(std::memory_order_relaxed);
	stats->io_sched_priority = m_InsRobot->m_IoSchedPriority.load(std::memory_order_relaxed);
	return true;
}

bool CRobot::OnSendPVT_A(char* local_file, long serial)
{
    // printf("10\n");
	if (serial < 0 || serial >= 100)
	{
		return false;
	}
	char remote[256];
	memset(remote,0,256);
	sprintf(remote, "\/home\/FUSION\/Config\/pvt\/user0\/%d.txt", serial);
	// printf("11\n");
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Send A arm pvt of serial=%d to local=%s\n ", serial, local_file);
	return OnSendFile(local_file, remote);
}

bool CRobot::OnSendPVT_B(char* local_file, long serial)
{
	if (serial < 0 || serial >= 100)
	{
		return false;
	}
	char remote[256];
	memset(remote, 0, 256);
	sprintf(remote, "\/home\/FUSION\/Config\/pvt\/user1\/%d.txt", serial);
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Send B arm pvt of serial=%d to local=%s\n ", serial, local_file);
	return OnSendFile(local_file, remote);
}
bool  CRobot:: OnSendFile( char* local_file, char* remote_file)
{
	// printf("12\n");
	if (m_InsRobot == NULL)
	{
	    // printf("13\n");
		return false;
	}

	if (m_InsRobot->SendFile(local_file, remote_file) == FX_TRUE)
	{
	    // printf("14\n");
	    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: send file local file:%s, remote file: %s\n ", local_file, remote_file);
		return true;
	}
	return  false;
}

bool CRobot::OnRecvFile(char* local_file, char* remote_file)
{
	if (m_InsRobot == NULL)
	{
		return false;
	}

	if (m_InsRobot->RecvFile(local_file, remote_file) == FX_TRUE)
	{
		return true;
	}
	return  false;
}

#ifdef CMPL_WIN
void CALLBACK CallBackFunc2(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	m_InsRobot->DoRecv();
	m_InsRobot->DoSend();
}
#endif

CRobot::CRobot()
{
    m_LocalLogTag = true;
	m_ParaSerial = 1;
	m_GatherTag = 0;
	m_NextPublishSequence = 0;
	m_IoTickCount = 0;
	m_IoDeadlineMissCount = 0;
	m_IoMaxLatenessNs = 0;
	m_PublishCount = 0;
	m_OverwriteCount = 0;
	m_SendAttemptCount = 0;
	m_SendSuccessCount = 0;
	m_SendErrorCount = 0;
	m_LastSendErrno = 0;
	m_SendWorkTotalNs = 0;
	m_RecvWorkTotalNs = 0;
	m_SendWorkMaxNs = 0;
	m_RecvWorkMaxNs = 0;
	m_IoSchedPolicy = SCHED_OTHER;
	m_IoSchedPriority = 0;
	for (size_t i = 0; i < sizeof(m_IoSendSlots) / sizeof(m_IoSendSlots[0]); ++i)
	{
		m_IoSendSlots[i].state.store(IO_SLOT_FREE, std::memory_order_relaxed);
		m_IoSendSlots[i].sequence.store(0, std::memory_order_relaxed);
		m_IoSendSlots[i].length = 0;
	}
	miss_cnt = 0;
	old_serial_tag = FX_FALSE;
	#ifdef CMPL_WIN
	m_TimeEventID = 0;
	#endif
	#ifdef CMPL_LIN
	m_IoThreadRunning = false;
	m_IoThreadStarted = false;
	m_RecvThreadStarted = false;
	#endif
	memset(&m_DCSS, 0, sizeof(DCSS));
	m_LastGatherTag = FX_FALSE;
	m_GatherTag = FX_FALSE;
	m_RunState = 0;
	m_LinkTag = FX_FALSE;
	_local_sock = INVALID_SOCKET;
	_tosock_ = INVALID_SOCKET;


	pDDSS1 = (DDSS*)&m_SendBuf1[2];
	memset(pDDSS1, 0, sizeof(DDSS));
	m_SendBuf1[0] = 'C';
	m_SendBuf1[1] = 'H';
	pDDSS1->m_Serial = 1;
	pDDSS1->m_CH = 1;


	pDDSS2 = (DDSS*)&m_SendBuf2[2];
	memset(pDDSS2, 0, sizeof(DDSS));
	m_SendBuf2[0] = 'C';
	m_SendBuf2[1] = 'H';
	pDDSS2->m_Serial = 1;
	pDDSS2->m_CH = 2;



}

#ifdef CMPL_LIN
namespace
{
const long kIoPeriodNs = 1000000L;

void AddNanoseconds(struct timespec* value, long nanoseconds)
{
	value->tv_nsec += nanoseconds;
	while (value->tv_nsec >= 1000000000L)
	{
		value->tv_nsec -= 1000000000L;
		value->tv_sec += 1;
	}
}

int64_t DifferenceNs(const struct timespec& lhs, const struct timespec& rhs)
{
	return static_cast<int64_t>(lhs.tv_sec - rhs.tv_sec) * 1000000000LL +
		static_cast<int64_t>(lhs.tv_nsec - rhs.tv_nsec);
}

bool ParseLongEnvironment(const char* name, long minimum, long maximum, long* value)
{
	const char* text = getenv(name);
	if (text == NULL || *text == '\0') return false;
	char* end = NULL;
	errno = 0;
	const long parsed = strtol(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0' || parsed < minimum || parsed > maximum)
	{
		return false;
	}
	*value = parsed;
	return true;
}

void AtomicMax(std::atomic<uint64_t>* target, uint64_t value)
{
	uint64_t observed = target->load(std::memory_order_relaxed);
	while (value > observed && !target->compare_exchange_weak(
		observed, value, std::memory_order_relaxed)) {}
}
}

void CRobot::ConfigureIoThread()
{
	long priority = 0;
	if (ParseLongEnvironment("MARVIN_IO_RT_PRIORITY", 1, 99, &priority))
	{
		struct sched_param param;
		memset(&param, 0, sizeof(param));
		param.sched_priority = static_cast<int>(priority);
		const int result = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
		if (result != 0 && m_LocalLogTag)
		{
			fprintf(stderr, "[Marvin SDK]: cannot set I/O SCHED_FIFO priority %ld: %s\n",
				priority, strerror(result));
		}
	}

	long cpu = 0;
	if (ParseLongEnvironment("MARVIN_IO_CPU", 0, CPU_SETSIZE - 1, &cpu))
	{
		cpu_set_t set;
		CPU_ZERO(&set);
		CPU_SET(static_cast<int>(cpu), &set);
		const int result = pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
		if (result != 0 && m_LocalLogTag)
		{
			fprintf(stderr, "[Marvin SDK]: cannot pin I/O thread to CPU %ld: %s\n",
				cpu, strerror(result));
		}
	}
	int policy = SCHED_OTHER;
	struct sched_param actual;
	memset(&actual, 0, sizeof(actual));
	pthread_getschedparam(pthread_self(), &policy, &actual);
	m_IoSchedPolicy.store(policy, std::memory_order_relaxed);
	m_IoSchedPriority.store(actual.sched_priority, std::memory_order_relaxed);
}

void* CRobot::IoThreadEntry(void* arg)
{
	CRobot* robot = static_cast<CRobot*>(arg);
	robot->ConfigureIoThread();

	struct timespec deadline;
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	while (robot->m_IoThreadRunning.load(std::memory_order_acquire))
	{
		AddNanoseconds(&deadline, kIoPeriodNs);

		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		const int64_t already_late_ns = DifferenceNs(now, deadline);
		if (already_late_ns >= kIoPeriodNs)
		{
			const uint64_t skipped = static_cast<uint64_t>(already_late_ns / kIoPeriodNs);
			robot->m_IoDeadlineMissCount.fetch_add(skipped, std::memory_order_relaxed);
			for (uint64_t i = 0; i < skipped; ++i) AddNanoseconds(&deadline, kIoPeriodNs);
		}

		int sleep_result;
		do
		{
			sleep_result = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
		} while (sleep_result == EINTR && robot->m_IoThreadRunning.load(std::memory_order_acquire));

		if (!robot->m_IoThreadRunning.load(std::memory_order_acquire)) break;

		clock_gettime(CLOCK_MONOTONIC, &now);
		const int64_t lateness_ns = DifferenceNs(now, deadline);
		if (lateness_ns > 0)
		{
			uint64_t observed = robot->m_IoMaxLatenessNs.load(std::memory_order_relaxed);
			const uint64_t lateness = static_cast<uint64_t>(lateness_ns);
			while (lateness > observed &&
				!robot->m_IoMaxLatenessNs.compare_exchange_weak(
					observed, lateness, std::memory_order_relaxed)) {}
		}

		robot->m_IoTickCount.fetch_add(1, std::memory_order_relaxed);
		// Sending first keeps command latency independent of receive backlog.
		struct timespec work_start, after_send;
		clock_gettime(CLOCK_MONOTONIC, &work_start);
		robot->DoSend();
		clock_gettime(CLOCK_MONOTONIC, &after_send);
		const uint64_t send_ns = static_cast<uint64_t>(DifferenceNs(after_send, work_start));
		robot->m_SendWorkTotalNs.fetch_add(send_ns, std::memory_order_relaxed);
		AtomicMax(&robot->m_SendWorkMaxNs, send_ns);
	}
	return NULL;
}

void* CRobot::RecvThreadEntry(void* arg)
{
	CRobot* robot = static_cast<CRobot*>(arg);
	struct timespec deadline;
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	while (robot->m_IoThreadRunning.load(std::memory_order_acquire))
	{
		AddNanoseconds(&deadline, kIoPeriodNs);
		int result;
		do { result = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL); }
		while (result == EINTR && robot->m_IoThreadRunning.load(std::memory_order_acquire));
		if (!robot->m_IoThreadRunning.load(std::memory_order_acquire)) break;
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);
		robot->DoRecv();
		clock_gettime(CLOCK_MONOTONIC, &finish);
		const uint64_t recv_ns = static_cast<uint64_t>(DifferenceNs(finish, start));
		robot->m_RecvWorkTotalNs.fetch_add(recv_ns, std::memory_order_relaxed);
		AtomicMax(&robot->m_RecvWorkMaxNs, recv_ns);
	}
	return NULL;
}

bool CRobot::StartIoThread()
{
	if (m_IoThreadStarted) return true;
	m_IoThreadRunning.store(true, std::memory_order_release);
	const int result = pthread_create(&m_IoThread, NULL, &CRobot::IoThreadEntry, this);
	if (result != 0)
	{
		m_IoThreadRunning.store(false, std::memory_order_release);
		if (m_LocalLogTag)
		{
			fprintf(stderr, "[Marvin SDK]: cannot start I/O thread: %s\n", strerror(result));
		}
		return false;
	}
	m_IoThreadStarted = true;
	const int recv_result = pthread_create(&m_RecvThread, NULL, &CRobot::RecvThreadEntry, this);
	if (recv_result != 0)
	{
		m_IoThreadRunning.store(false, std::memory_order_release);
		pthread_join(m_IoThread, NULL);
		m_IoThreadStarted = false;
		if (m_LocalLogTag)
		{
			fprintf(stderr, "[Marvin SDK]: cannot start receive thread: %s\n", strerror(recv_result));
		}
		return false;
	}
	m_RecvThreadStarted = true;
	return true;
}

void CRobot::StopIoThread()
{
	if (!m_IoThreadStarted) return;
	m_IoThreadRunning.store(false, std::memory_order_release);
	pthread_join(m_IoThread, NULL);
	if (m_RecvThreadStarted)
	{
		pthread_join(m_RecvThread, NULL);
		m_RecvThreadStarted = false;
	}
	m_IoThreadStarted = false;
	// Preserve the historical expectation that a successful OnSetSend directly
	// before OnRelease is not silently discarded.
	DoSend();
}
#endif

bool CRobot::OnRelease()
{
	if (m_InsRobot == NULL)
	{
		return true;
	}
#ifdef CMPL_WIN
	timeKillEvent(m_InsRobot->m_TimeEventID);
	Sleep(10);
#endif
	#ifdef CMPL_LIN
	m_InsRobot->StopIoThread();
	if (m_InsRobot->_local_sock != INVALID_SOCKET)
	{
		close(m_InsRobot->_local_sock);
		m_InsRobot->_local_sock = INVALID_SOCKET;
	}
	if (m_InsRobot->_tosock_ != INVALID_SOCKET)
	{
		close(m_InsRobot->_tosock_);
		m_InsRobot->_tosock_ = INVALID_SOCKET;
	}
	#endif
	delete m_InsRobot;
	m_InsRobot = NULL;
    //if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Robot released\n");
    printf("[Marvin SDK]: Robot released\n");
	return true;
}

CRobot::~CRobot()
{
	
}

CRobot* CRobot::GetIns()
{
	if (m_InsRobot == NULL)
	{
		m_InsRobot = new CRobot();
	}
	return m_InsRobot;
}



void CRobot::OnLocalLogOn()
{
    GetIns();
    m_InsRobot->m_LocalLogTag = true;
}
void CRobot::OnLocalLogOff()
{
    GetIns();
    m_InsRobot->m_LocalLogTag = false;
}

bool CRobot::OnGetBuf(DCSS* ret)
{
	if (m_InsRobot == NULL)
	{
		return false;
	}
	memcpy(ret,&m_InsRobot->m_DCSS,sizeof(m_DCSS));
//	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Subscribe robot info succeed \n");
	return true;
}

bool CRobot::OnLinkTo(FX_UCHAR ip1, FX_UCHAR ip2, FX_UCHAR ip3, FX_UCHAR ip4)
{
	GetIns();
	if (m_InsRobot->m_LinkTag == FX_TRUE)
	{
		return false;
	}
#ifdef CMPL_WIN
	WSADATA wsadata;
	int ret;
	ret = WSAStartup(0x101, &wsadata);
	if (ret != 0)
	{
		return false;
	}
#endif
	memset(&m_InsRobot->_local, 0, sizeof(m_InsRobot->_local));
	m_InsRobot->_localLen = sizeof(sockaddr_in);
	m_InsRobot->_local.sin_family = AF_INET;
	m_InsRobot->_local.sin_port = htons(4730);
	m_InsRobot->_local.sin_addr.s_addr = INADDR_ANY;
	m_InsRobot->_local_sock = socket(AF_INET, SOCK_DGRAM, 0);
	unsigned long on = 1;
#ifdef CMPL_WIN
	if (0 != ioctlsocket(m_InsRobot->_local_sock, FIONBIO, &on))
	{
		return false;
	}
#endif
#ifdef CMPL_LIN
	if (0 != ioctl(m_InsRobot->_local_sock, FIONBIO, &on))
	{
		return false;
	}
#endif
//	bind(m_InsRobot->_local_sock, (struct sockaddr*)&m_InsRobot->_local, sizeof(_local));
	// 绑定Socket并检查结果
    if (bind(m_InsRobot->_local_sock, (struct sockaddr*)&m_InsRobot->_local, sizeof(m_InsRobot->_local)) != 0)
    {
        if(m_InsRobot->m_LocalLogTag == true) printf("port bind failure, possibly occupied by another program\n");
        // 关闭socket连接
        #ifdef CMPL_WIN
            closesocket(m_InsRobot->_local_sock);
        #endif
        #ifdef CMPL_LIN
            close(m_InsRobot->_local_sock);
        #endif
        m_InsRobot->_local_sock = 0;
        return false;
    }
	memset(&m_InsRobot->_to, 0, sizeof(_to));
	char ip_str[100];
	sprintf(ip_str, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_InsRobot->_toLen = sizeof(sockaddr_in);
	m_InsRobot->_to.sin_family = AF_INET;
	m_InsRobot->_to.sin_port = htons(4729);
	m_InsRobot->_to.sin_addr.s_addr = inet_addr(ip_str);
	m_InsRobot->_tosock_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_InsRobot->_tosock_ == INVALID_SOCKET)
	{
		close(m_InsRobot->_local_sock);
		m_InsRobot->_local_sock = INVALID_SOCKET;
		return false;
	}
#ifdef CMPL_LIN
	on = 1;
	if (ioctl(m_InsRobot->_tosock_, FIONBIO, &on) != 0)
	{
		close(m_InsRobot->_local_sock);
		close(m_InsRobot->_tosock_);
		m_InsRobot->_local_sock = INVALID_SOCKET;
		m_InsRobot->_tosock_ = INVALID_SOCKET;
		return false;
	}
#endif

	m_InsRobot->m_LinkTag = FX_TRUE;
//	if(m_InsRobot->m_LocalLogTag == true)
//	{
//	    printf("[Marvin SDK]: Robot connected  IP=%d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
//	}
#ifdef CMPL_WIN
	m_InsRobot->m_TimeEventID = timeSetEvent(1, 1, CallBackFunc2, (DWORD)NULL, TIME_PERIODIC);    //??1ms�䣤���騰?��?
#endif
	#ifdef CMPL_LIN
	if (!m_InsRobot->StartIoThread())
	{
		m_InsRobot->m_LinkTag = FX_FALSE;
		close(m_InsRobot->_local_sock);
		close(m_InsRobot->_tosock_);
		m_InsRobot->_local_sock = INVALID_SOCKET;
		m_InsRobot->_tosock_ = INVALID_SOCKET;
		return false;
	}
	#endif
	m_InsRobot->m_RunState = 0;

	m_InsRobot->m_ip1 = ip1;
	m_InsRobot->m_ip2 = ip2;
	m_InsRobot->m_ip3 = ip3;
	m_InsRobot->m_ip4 = ip4;

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Robot connected  IP=%d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
	}
	return true;

}


long CRobot::OnSetIntPara(char paraName[30], long setValue)
{
	if (paraName[29] != 0)
	{
		return -1;
	}

	if (OnClearSet() == false)
	{
		return -1;
	}

	long add_size = 1 + sizeof(FX_CHAR) * 32 + sizeof(FX_INT32) + sizeof(FX_FLOAT) + sizeof(FX_INT16);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return -1;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 150;
	m_InsRobot->m_Slen++;

	FX_CHAR* pName = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += 30 * sizeof(FX_CHAR);
	FX_CHAR* pType = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);
	FX_CHAR* pIns = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);


	FX_INT32* pValueInt = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_FLOAT* pValueFloat = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);

	FX_INT16* pSerial = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT16);

	long i;
	for (i = 0; i < 30; i++)
	{
		pName[i] = 0;
		if (paraName[i] != 0)
		{
			pName[i] = paraName[i];
		}
	}
	
	m_InsRobot->m_ParaSerial++;
	if (m_InsRobot->m_ParaSerial >= 99)
	{
		m_InsRobot->m_ParaSerial = 1;
	}
	long serial = m_InsRobot->m_ParaSerial;
	*pType = 1;
	*pIns = 101;
	*pValueInt = setValue;
	*pValueFloat = 0;
	*pSerial = serial;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;

    if(m_InsRobot->m_LocalLogTag == true)
    {
        printf("[Marvin SDK]: Set int parameter: %s, value=%ld\n",paraName,setValue);
    }

    CRobot::OnSetSend();

	for (i = 0; i < 50; i++)
	{
#ifdef CMPL_WIN
		Sleep(2);
#endif
#ifdef CMPL_LIN
		usleep(2000);
#endif
		long ret_s = m_InsRobot->m_DCSS.m_ParaRetSerial;
		if (ret_s % 100 == serial)
		{
			long ret_v = ret_s / 100;
			return ret_v;
		}
	}
	return -2;
}

long CRobot::OnSetFloatPara(char paraName[30], double setValue)
{
	if (paraName[29] != 0)
	{
		return -1;
	}

	if (OnClearSet() == false)
	{
		return -1;
	}

	long add_size = 1 + sizeof(FX_CHAR) * 32 + sizeof(FX_INT32) + sizeof(FX_FLOAT) + sizeof(FX_INT16);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return -1;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 150;
	m_InsRobot->m_Slen++;

	FX_CHAR* pName = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += 30 * sizeof(FX_CHAR);
	FX_CHAR* pType = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);
	FX_CHAR* pIns = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);


	FX_INT32* pValueInt = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_FLOAT* pValueFloat = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);

	FX_INT16* pSerial = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT16);

	long i;
	for (i = 0; i < 30; i++)
	{
		pName[i] = 0;
		if (paraName[i] != 0)
		{
			pName[i] = paraName[i];
		}
	}

	m_InsRobot->m_ParaSerial++;
	if (m_InsRobot->m_ParaSerial >= 99)
	{
		m_InsRobot->m_ParaSerial = 1;
	}
	long serial = m_InsRobot->m_ParaSerial;
	*pType = 2;
	*pIns = 102;
	*pValueInt = 0;
	*pValueFloat = setValue;
	*pSerial = serial;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
    if(m_InsRobot->m_LocalLogTag == true)
    {
        printf("[Marvin SDK]: Set float parameter: %s, value=%lf\n",paraName,setValue);
    }
	CRobot::OnSetSend();

	for (i = 0; i < 50; i++)
	{
#ifdef CMPL_WIN
		Sleep(2);
#endif
#ifdef CMPL_LIN
		usleep(2000);
#endif


		long ret_s = m_InsRobot->m_DCSS.m_ParaRetSerial;
		if (ret_s % 100 == serial)
		{
			long ret_v = ret_s / 100;
			return ret_v;
		}
	}
	return -2;
}
long CRobot::OnGetIntPara(char paraName[30], long* retValue)
{
	if (paraName[29] != 0)
	{
		return -1;
	}

	if (OnClearSet() == false)
	{
		return -1;
	}

	long add_size = 1 + sizeof(FX_CHAR) * 32 + sizeof(FX_INT32) + sizeof(FX_FLOAT) + sizeof(FX_INT16);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return -1;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 150;
	m_InsRobot->m_Slen++;

	FX_CHAR* pName = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += 30 * sizeof(FX_CHAR);
	FX_CHAR* pType = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);
	FX_CHAR* pIns = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);


	FX_INT32* pValueInt = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_FLOAT* pValueFloat = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);

	FX_INT16* pSerial = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT16);

	long i;
	for (i = 0; i < 30; i++)
	{
		pName[i] = 0;
		if (paraName[i] != 0)
		{
			pName[i] = paraName[i];
		}
	}

	m_InsRobot->m_ParaSerial++;
	if (m_InsRobot->m_ParaSerial >= 99)
	{
		m_InsRobot->m_ParaSerial = 1;
	}
	long serial = m_InsRobot->m_ParaSerial;
	*pType = 1;
	*pIns = 103;
	*pValueInt = 0;
	*pValueFloat = 0;
	*pSerial = serial;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;

	CRobot::OnSetSend();

	for (i = 0; i < 50; i++)
	{
#ifdef CMPL_WIN
		Sleep(2);
#endif
#ifdef CMPL_LIN
		usleep(2000);
#endif


    long ret_s = m_InsRobot->m_DCSS.m_ParaRetSerial;
    if (ret_s % 100 == serial)
    {
        long ret_v = ret_s / 100;
        if (ret_v == 0)
        {
            *retValue = m_InsRobot->m_DCSS.m_ParaValueI;
        }
        if(m_InsRobot->m_LocalLogTag == true)
        {

            if (retValue != nullptr) {
            printf("[Marvin SDK]: Get int parameter: %s, value=%ld\n",paraName, *retValue);
        } else {
            printf("[Marvin SDK] retValue is null pointer.\n");
        }

        }
        return ret_v;
    }
	}
	return -2;
}
long CRobot::OnGetFloatPara(char paraName[30], double* retValue)
{
	if (paraName[29] != 0)
	{
		return -1;
	}

	if (OnClearSet() == false)
	{
		return -1;
	}

	long add_size = 1 + sizeof(FX_CHAR) * 32 + sizeof(FX_INT32) + sizeof(FX_FLOAT) + sizeof(FX_INT16);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return -1;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 150;
	m_InsRobot->m_Slen++;

	FX_CHAR* pName = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += 30 * sizeof(FX_CHAR);
	FX_CHAR* pType = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);
	FX_CHAR* pIns = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);


	FX_INT32* pValueInt = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_FLOAT* pValueFloat = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);

	FX_INT16* pSerial = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT16);

	long i;
	for (i = 0; i < 30; i++)
	{
		pName[i] = 0;
		if (paraName[i] != 0)
		{
			pName[i] = paraName[i];
		}
	}

	m_InsRobot->m_ParaSerial++;
	if (m_InsRobot->m_ParaSerial >= 99)
	{
		m_InsRobot->m_ParaSerial = 1;
	}
	long serial = m_InsRobot->m_ParaSerial;
	*pType = 2;
	*pIns = 104;
	*pValueInt = 0;
	*pValueFloat = 0;
	*pSerial = serial;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;

	CRobot::OnSetSend();

	for (i = 0; i < 50; i++)
	{
#ifdef CMPL_WIN
		Sleep(2);
#endif
#ifdef CMPL_LIN
		usleep(2000);
#endif


    long ret_s = m_InsRobot->m_DCSS.m_ParaRetSerial;
    if (ret_s % 100 == serial)
    {
        long ret_v = ret_s / 100;
        if (ret_v == 0)
        {
            *retValue = m_InsRobot->m_DCSS.m_ParaValueF;
        }

            if(m_InsRobot->m_LocalLogTag == true)
        {

            if (retValue != nullptr) {
            printf("[Marvin SDK]: Get float parameter: %s, value=%lf\n",paraName, *retValue);
        } else {
            printf("[Marvin SDK] retValue is null pointer.\n");
        }

        }
        return ret_v;
    }
	}
	return -2;
}

long CRobot::OnSavePara()
{
	if (OnClearSet() == false)
	{
		return -1;
	}

	long add_size = 1 + sizeof(FX_CHAR) * 32 + sizeof(FX_INT32) + sizeof(FX_FLOAT) +  sizeof(FX_INT16);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return -1;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 150;
	m_InsRobot->m_Slen++;
	
	FX_CHAR* pName = (FX_CHAR *) &m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += 30 * sizeof(FX_CHAR);
	FX_CHAR* pType = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen]; 
	m_InsRobot->m_Slen += sizeof(FX_CHAR);
	FX_CHAR* pIns = (FX_CHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_CHAR);


	FX_INT32* pValueInt = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_FLOAT* pValueFloat = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);

	FX_INT16* pSerial = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	m_InsRobot->m_Slen += sizeof(FX_INT16);

	long i;
	for ( i = 0; i < 30; i++)
	{
		pName[i] = 0;
	}
	m_InsRobot->m_ParaSerial++;
	if (m_InsRobot->m_ParaSerial >= 99)
	{
		m_InsRobot->m_ParaSerial = 1;
	}
	long serial = m_InsRobot->m_ParaSerial;
	*pType = 0;
	*pIns = 105;
	*pValueInt = 0;
	*pValueFloat = 0;
	*pSerial = serial;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Save parameters\n");
	}
	CRobot::OnSetSend();

	for ( i = 0; i < 50; i++)
	{
#ifdef CMPL_WIN
		Sleep(2);
#endif
#ifdef CMPL_LIN
		usleep(2000);
#endif


		long ret_s = m_InsRobot->m_DCSS.m_ParaRetSerial;
		if (ret_s % 100 == serial)
		{
			long ret_v = ret_s / 100;
			return ret_v;
		}
	}
	return -2;

}

void CRobot::DoRecv()
{
	if (m_LinkTag == FX_FALSE)
	{
		return;
	}

	_localLen = sizeof(_local);

#ifdef CMPL_WIN
		int Len = recvfrom(_local_sock, recvbuf, 2000, 0, (struct sockaddr*)&_local, &_localLen);
#endif
#ifdef CMPL_LIN
		int Len = recvfrom(_local_sock, recvbuf, 2000, 0, (struct sockaddr*)&_local, (socklen_t*)&_localLen);
#endif

	if (Len == sizeof(DCSS) + 2)
	{
		if (recvbuf[0] == 'F' && recvbuf[1] == 'X')
		{
			DCSS* p = (DCSS*)&recvbuf[2];
			memcpy(&m_DCSS, p, sizeof(m_DCSS));
			if (m_InsRobot->m_GatherTag == 1)
			{
				if (m_GatherRecordNum >= m_GatherRecordMaxNum)
				{
					m_GatherTag = 4;
				}
				else
				{
					double v[40];
					for (long i = 0; i < m_GatherItemSize; i++)
					{
						v[i+2] = *m_GatherItem[i];
					}
					v[0] = m_DCSS.m_Out[0].m_OutFrameSerial;
					//if(m_InsRobot->m_LocalLogTag == true) printf("%lf\n",v[0]);
					v[1] = m_DCSS.m_Out[1].m_OutFrameSerial;
					m_GatherSet.OnSetPoint(v);
					m_GatherRecordNum++;
				}
			}
			
			if (m_InsRobot->m_GatherTag == 2)
			{
				m_InsRobot->m_GatherTag = 4;
			}

			if (old_serial_tag == FX_FALSE)
			{
				old_serial_tag = true;
				old_serial = m_DCSS.m_Out[0].m_OutFrameSerial;
			}
			else
			{
				old_serial += 1;
				old_serial %= 1000000;
				if (old_serial != m_DCSS.m_Out[0].m_OutFrameSerial)
				{
					miss_cnt++;
					old_serial = m_DCSS.m_Out[0].m_OutFrameSerial;
				}
				else
				{
					miss_cnt = 0;
				}
			}
		}
	}
	else if(Len == sizeof(DDSS) + 2)
	{
		if (recvbuf[0] == 'C' && recvbuf[1] == 'H')
		{
			DDSS* p = (DDSS*)&recvbuf[2];
			if (p->m_CH == 1)
			{
				m_ACB1.WriteBuf((unsigned char *)p, sizeof(DDSS));
			}

			if (p->m_CH == 2)
			{
				m_ACB2.WriteBuf((unsigned char*)p, sizeof(DDSS));
			}

		}
	}

}
void CRobot::DoSend()
{
	IoSendSlot* selected = NULL;
	uint64_t newest_sequence = 0;
	for (size_t i = 0; i < sizeof(m_IoSendSlots) / sizeof(m_IoSendSlots[0]); ++i)
	{
		IoSendSlot& slot = m_IoSendSlots[i];
		const uint64_t sequence = slot.sequence.load(std::memory_order_relaxed);
		if (slot.state.load(std::memory_order_acquire) == IO_SLOT_READY &&
			(selected == NULL || sequence > newest_sequence))
		{
			selected = &slot;
			newest_sequence = sequence;
		}
	}
	if (selected == NULL) return;

	unsigned char expected = IO_SLOT_READY;
	if (!selected->state.compare_exchange_strong(
		expected, IO_SLOT_READING, std::memory_order_acquire, std::memory_order_relaxed)) return;

	// Drop older queued commands. For teleoperation, freshness is more important
	// than replaying a backlog after a scheduling delay.
	for (size_t i = 0; i < sizeof(m_IoSendSlots) / sizeof(m_IoSendSlots[0]); ++i)
	{
		IoSendSlot& slot = m_IoSendSlots[i];
		if (&slot == selected) continue;
		if (slot.state.load(std::memory_order_acquire) == IO_SLOT_READY &&
			slot.sequence.load(std::memory_order_relaxed) < newest_sequence)
		{
			expected = IO_SLOT_READY;
			if (slot.state.compare_exchange_strong(
				expected, IO_SLOT_FREE, std::memory_order_release, std::memory_order_relaxed))
			{
				m_OverwriteCount.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

	const long send_length = selected->length;
	m_SendAttemptCount.fetch_add(1, std::memory_order_relaxed);
	const int sent = sendto(_tosock_, selected->data, send_length, 0,
		(struct sockaddr*)&_to, sizeof(_to));
	if (sent == send_length)
	{
		m_SendSuccessCount.fetch_add(1, std::memory_order_relaxed);
		m_LastSendErrno.store(0, std::memory_order_relaxed);
	}
	else
	{
		m_SendErrorCount.fetch_add(1, std::memory_order_relaxed);
		const int send_errno = sent < 0 ? errno : EMSGSIZE;
		m_LastSendErrno.store(send_errno, std::memory_order_relaxed);
		if (send_errno == EAGAIN || send_errno == EWOULDBLOCK || send_errno == ENOBUFS)
		{
			selected->state.store(IO_SLOT_READY, std::memory_order_release);
			return;
		}
	}
	selected->state.store(IO_SLOT_FREE, std::memory_order_release);
}

bool CRobot::OnStopGather()
{
	GetIns();
	if (m_InsRobot->m_LinkTag == false)
	{
		return false;
	}

	if (m_InsRobot->m_GatherTag != 1)
	{
		return false;
	}

	m_InsRobot->m_GatherTag = 2;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Stop collect data\n");
	}
	return true;
}

bool CRobot::OnSaveGatherData(char* path)
{
	GetIns();
	printf("OnSaveGatherData set saved path=%s\n",path);
	if (m_InsRobot->m_LinkTag == false)
	{
		return false;
	}

	if (m_InsRobot->m_GatherTag != 4)
	{
		return false;
	}

	bool ret =  m_InsRobot->m_GatherSet.OnSave(path);
	m_InsRobot->m_GatherTag = 0;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Save collected data to %s\n",path);
	}
	return ret;

}


bool CRobot::OnSaveGatherDataCSV(char* path)
{
	GetIns();
//	printf("1\n");
    printf("OnSaveGatherDataCSV set saved path=%s\n",path);
	if (m_InsRobot->m_LinkTag == false)
	{
	    printf("2\n");
		return false;
	}

	if (m_InsRobot->m_GatherTag != 4)
	{
		printf("3\n");
		return false;
	}
    printf("5\n");
	bool ret = m_InsRobot->m_GatherSet.OnSaveCSV(path);
	printf("6\n");
	m_InsRobot->m_GatherTag = 0;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Collected csv saved path=%s\n",path);
	}
	return ret;

}

bool CRobot::OnStartGather(long targetNum, long targetID[35], long recordNum)
{
	GetIns();
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Collect data settings\n");
		printf("targetNum=%d\n",targetNum);
		printf("targetID= [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",
		targetID[0],targetID[1],targetID[2],targetID[3],targetID[4],targetID[5],targetID[6],
		targetID[7],targetID[8],targetID[9],targetID[10],targetID[11],targetID[12],targetID[13],
		targetID[14],targetID[15],targetID[16],targetID[17],targetID[18],targetID[19],targetID[20],
		targetID[21],targetID[22],targetID[23],targetID[24],targetID[25],targetID[26],targetID[27],
		targetID[28],targetID[29],targetID[30],targetID[31],targetID[32],targetID[33],targetID[34]);
		printf("recordNum=%d\n",recordNum);
	}

	
	if (m_InsRobot->m_LinkTag == false)
	{
		return false;
	}

	if (targetNum <= 0)
	{
		return false;
	}
	if (targetNum >= 35)
	{
		targetNum = 35;
	}

	if (m_InsRobot->m_GatherTag == 1)
	{
		return false;
	}


	long i;
	for ( i = 0; i < targetNum; i++)
	{
		long v = targetID[i];
		long rob = v / 100;
		if (rob > 1)
		{
			return false;
		}
		long grp = (v % 100) / 10;
		long pos = v % 10;
		if (pos > 7)
		{
			return false;
		}

		if (grp == 9 && pos > 6)
		{
			return false;
		}
		if (rob == 0)
		{
			if (grp == 0)
			{
				m_InsRobot->m_GatherItem[i] = & m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_Pos[pos];
			}
			else if (grp == 1)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_Vel[pos];
			}
			else if (grp == 2)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_PosE[pos];
			}
			else if (grp == 3)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_Cmd[pos];
			}
			else if (grp == 4)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_CToq[pos];
			}
			else if (grp == 5)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_FB_Joint_SToq[pos];

			}
			else if (grp == 6)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_EST_Joint_Firc[pos];
			}
			else if (grp == 7)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_EST_Joint_Firc_Dot[pos];
			}
			else if (grp == 8)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_EST_Joint_Force[pos];
			}
			else if (grp == 9)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[0].m_EST_Cart_FN[pos];
			}
		}
		else
		{
			if (grp == 0)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_Pos[pos];
			}
			else if (grp == 1)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_Vel[pos];
			}
			else if (grp == 2)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_PosE[pos];
			}
			else if (grp == 3)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_Cmd[pos];
			}
			else if (grp == 4)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_CToq[pos];
			}
			else if (grp == 5)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_FB_Joint_SToq[pos];

			}
			else if (grp == 6)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_EST_Joint_Firc[pos];
			}
			else if (grp == 7)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_EST_Joint_Firc_Dot[pos];
			}
			else if (grp == 8)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_EST_Joint_Force[pos];
			}
			else if (grp == 9)
			{
				m_InsRobot->m_GatherItem[i] = &m_InsRobot->m_DCSS.m_Out[1].m_EST_Cart_FN[pos];
			}

		}
	}

	m_InsRobot->m_GatherItemSize = targetNum;
	m_InsRobot->m_GatherRecordNum = 0;

	if (recordNum < 1000)
	{
		recordNum = 1000;
	}
	if (recordNum > 5000000 / targetNum)
	{
		recordNum = 5000000 / targetNum;
	}
	if (recordNum > 1000000)
	{
		recordNum = 1000000;
	}
	m_InsRobot->m_GatherRecordMaxNum = recordNum;
	int t = (targetNum + 2);
	m_InsRobot->m_GatherSet.OnInit((PoinType)t, recordNum);
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Data collected, targetNum=%ld, recordNum=%ld\n",targetNum,recordNum);
	    printf("targetID= [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",
		targetID[0],targetID[1],targetID[2],targetID[3],targetID[4],targetID[5],targetID[6],
		targetID[7],targetID[8],targetID[9],targetID[10],targetID[11],targetID[12],targetID[13],
		targetID[14],targetID[15],targetID[16],targetID[17],targetID[18],targetID[19],targetID[20],
		targetID[21],targetID[22],targetID[23],targetID[24],targetID[25],targetID[26],targetID[27],
		targetID[28],targetID[29],targetID[30],targetID[31],targetID[32],targetID[33],targetID[34]);
	}
	m_InsRobot->m_GatherTag = true;

	return true;


}

bool CRobot::OnClearSet()
{
	// This is a producer-owned staging buffer. Published frames live in the
	// independent I/O mailbox, so an outstanding UDP send cannot make staging busy.
	m_InsRobot->m_SendBuf[0] = 'F';
	m_InsRobot->m_SendBuf[1] = 'X';
	m_InsRobot->m_SendBuf[2] = 0;
	m_InsRobot->m_Slen = 3;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Clear set\n");
	}

	return true;
}




bool CRobot::OnSetJointLmt_A(int velRatio, int AccRatio)
{
	long add_size = 1 + sizeof(FX_INT16) * 2;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 103;
	m_InsRobot->m_Slen++;
	FX_INT16* pv = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	if (velRatio < 1)
	{
		velRatio = 1;
	}
	if (velRatio > 100)
	{
		velRatio = 100;
	}

	if (AccRatio < 1)
	{
		AccRatio = 1;
	}
	if (AccRatio > 100)
	{
		AccRatio = 100;
	}

	pv[0] = velRatio;
	pv[1] = AccRatio;
	

	m_InsRobot->m_Slen += sizeof(FX_INT16) * 2;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set A arm velRatio=%d, AccRatio=%d \n",velRatio,AccRatio);
	}

	return true;

}

bool CRobot::OnSetPVT_A(int id)
{
	if (id < 0 || id >= 100)
	{
		return false;
	}
	long add_size = 1 + sizeof(FX_UCHAR);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 110;
	m_InsRobot->m_Slen++;

	FX_UCHAR* pv = (FX_UCHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	pv[0] = id;
	m_InsRobot->m_Slen += sizeof(FX_UCHAR);
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;


// 	printf("%c%c %d %d %d\n",m_InsRobot->m_SendBuf[0]
// 		,m_InsRobot->m_SendBuf[1]
// 		,m_InsRobot->m_SendBuf[2]
// 	,m_InsRobot->m_SendBuf[3]
// ,m_InsRobot->m_SendBuf[4]);
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set A arm PVT id=%d\n",id);
	}

	return true;
}



bool CRobot::OnSetPVT_B(int id)
{
	if (id < 0 || id >= 100)
	{
		return false;
	}
	long add_size = 1 + sizeof(FX_UCHAR);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 210;
	m_InsRobot->m_Slen++;

	FX_UCHAR* pv = (FX_UCHAR*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	pv[0] = id;
	m_InsRobot->m_Slen += sizeof(FX_UCHAR);
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set B arm PVT id=%d\n",id);
	}

	return true;
}

bool CRobot::OnSetForceCmd_A(double force)
{
	long add_size = 1 + sizeof(FX_FLOAT);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 109;
	m_InsRobot->m_Slen++;

	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	pv[0] = force;
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;

	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set A arm force cmd force=%lf\n",force);
	}

	return true;
}

bool CRobot::OnSetJointCmdPos_A(double joint[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 7;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 108;
	m_InsRobot->m_Slen++;

	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	
	pv[0] = joint[0];
	pv[1] = joint[1];
	pv[2] = joint[2];
	pv[3] = joint[3];
	pv[4] = joint[4];
	pv[5] = joint[5];
	pv[6] = joint[6];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 7;
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true)
	{
	    printf("[Marvin SDK]: Set A arm joint cmd pose, joints=[%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",joint[0],joint[1],joint[2],joint[3],joint[4],joint[5],joint[6]);
	}

	return true;
}
bool CRobot::OnSetForceCtrPara_A(int fcType, double fxDir[6], double fcCtrlPara[7], double fcAdjLmt)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 107;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = fcType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = fxDir[0];
	pv[1] = fxDir[1];
	pv[2] = fxDir[2];
	pv[3] = fxDir[3];
	pv[4] = fxDir[4];
	pv[5] = fxDir[5];
	pv[6] = fcCtrlPara[0];
	pv[7] = fcCtrlPara[1];
	pv[8] = fcCtrlPara[2];
	pv[9] = fcCtrlPara[3];
	pv[10] = fcCtrlPara[4];
	pv[11] = fcCtrlPara[5];
	pv[12] = fcCtrlPara[6];
	pv[13] = fcAdjLmt;

	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm force control parameters\n fcType=%d,\n",fcType);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcDirection=[%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf],\n",fxDir[0],fxDir[1],fxDir[2],fxDir[3],fxDir[4],fxDir[5]);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcCtrlPara=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\n",fcCtrlPara[0],fcCtrlPara[1],fcCtrlPara[2],fcCtrlPara[3],fcCtrlPara[4],fcCtrlPara[5],fcCtrlPara[6]);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcAdjLmt=%lf\n",fcAdjLmt);
	return true;
}
bool CRobot::OnSetDragSpace_A(int zsType)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 6 + sizeof(FX_INT32) ;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 106;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = zsType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = 0;
	pv[1] = 0;
	pv[2] = 0;
	pv[3] = 0;
	pv[4] = 0;
	pv[5] = 0;



	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 6;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm drag space type=%d\n",zsType);
	return true;
}
bool CRobot::OnSetCartKD_A(double K[7], double D[7],int type)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 105;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = K[0];
	pv[1] = K[1];
	pv[2] = K[2];
	pv[3] = K[3];
	pv[4] = K[4];
	pv[5] = K[5];
	pv[6] = K[6];


	pv[7] = D[0];
	pv[8] = D[1];
	pv[9] = D[2];
	pv[10] = D[3];
	pv[11] = D[4];
	pv[12] = D[5];
	pv[13] = D[6];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_INT32* pi = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pi[0] = type;
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm Cart k=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\nD=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\ntype=%d\n",
	K[0],K[1],K[2],K[3],K[4],K[5],K[6],
	D[0],D[1],D[2],D[3],D[4],D[5],D[6],
	type);

	return true;
}

bool CRobot::OnSetEefRot_A(int fcType, double CartCtrlPara[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 107;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = fcType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = 0;
	pv[1] = 0;
	pv[2] = 0;
	pv[3] = 0;
	pv[4] = 0;
	pv[5] = 0;
	pv[6] = CartCtrlPara[0];
	pv[7] = CartCtrlPara[1];
	pv[8] = CartCtrlPara[2];
	pv[9] = CartCtrlPara[3];
	pv[10] = CartCtrlPara[4];
	pv[11] = CartCtrlPara[5];
	pv[12] = CartCtrlPara[6];
	pv[13] = 0;

	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm flange Cart rotation parameters.\n fcType=%d,\n",fcType);
    if(m_InsRobot->m_LocalLogTag == true) printf("CartCtrlPara=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\n",CartCtrlPara[0],CartCtrlPara[1],CartCtrlPara[2],CartCtrlPara[3],CartCtrlPara[4],CartCtrlPara[5],CartCtrlPara[6]);
	return true;
}

bool CRobot::OnSetJointKD_A(double K[7], double D[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 104;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = K[0];
	pv[1] = K[1];
	pv[2] = K[2];
	pv[3] = K[3];
	pv[4] = K[4];
	pv[5] = K[5];
	pv[6] = K[6];

	
	pv[7] = D[0];
	pv[8] = D[1];
	pv[9] = D[2];
	pv[10] = D[3];
	pv[11] = D[4];
	pv[12] = D[5];
	pv[13] = D[6];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm joint k=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\nD=[%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",
	K[0],K[1],K[2],K[3],K[4],K[5],K[6],
	D[0],D[1],D[2],D[3],D[4],D[5],D[6]);

	return true;
}
bool CRobot::OnSetTool_A(double kinePara[6], double dynPara[10])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 16;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 102;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = kinePara[0];
	pv[1] = kinePara[1];
	pv[2] = kinePara[2];
	pv[3] = kinePara[3];
	pv[4] = kinePara[4];
	pv[5] = kinePara[5];

	pv[6] = dynPara[0];
	pv[7] = dynPara[1];
	pv[8] = dynPara[2];
	pv[9] = dynPara[3];
	pv[10] = dynPara[4];
	pv[11] = dynPara[5];
	pv[12] = dynPara[6];
	pv[13] = dynPara[7];
	pv[14] = dynPara[8];
	pv[15] = dynPara[9];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 16;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm tool kineParams=[%lf,%lf,%lf,%lf,%lf,%lf]\n dynamicParams=[%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",
	kinePara[0],kinePara[1],kinePara[2],kinePara[3],kinePara[4],kinePara[5],
	dynPara[0],dynPara[1],dynPara[2],dynPara[3],dynPara[4],dynPara[5],dynPara[6],dynPara[7],dynPara[8],dynPara[9]);

	return true;
}
bool CRobot::OnSetTargetState_A(int state)
{
	long add_size = 1 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 101;
	m_InsRobot->m_Slen++;
	FX_INT32* pv = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = state;

	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm state=%d\n",state);
	return true;
}

bool CRobot::OnSetImpType_A(int type)
{
	long add_size = 1 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 111;
	m_InsRobot->m_Slen++;
	FX_INT32* pv = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = type;

	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set A arm impedance type=%d\n",type);
	return true;
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool CRobot::OnSetJointLmt_B(int velRatio, int AccRatio)
{
	long add_size = 1 + sizeof(FX_INT16) * 2;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 203;
	m_InsRobot->m_Slen++;
	FX_INT16* pv = (FX_INT16*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	if (velRatio < 1)
	{
		velRatio = 1;
	}
	if (velRatio > 100)
	{
		velRatio = 100;
	}

	if (AccRatio < 1)
	{
		AccRatio = 1;
	}
	if (AccRatio > 100)
	{
		AccRatio = 100;
	}

	pv[0] = velRatio;
	pv[1] = AccRatio;


	m_InsRobot->m_Slen += sizeof(FX_INT16) * 2;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm velRatio=%d, AccRatio=%d \n",velRatio,AccRatio);
	return true;

}

bool CRobot::OnSetForceCmd_B(double force)
{
	long add_size = 1 + sizeof(FX_FLOAT);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 209;
	m_InsRobot->m_Slen++;

	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	pv[0] = force;
	m_InsRobot->m_Slen += sizeof(FX_FLOAT);
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm force cmd force=%lf\n",force);
	return true;
}

bool CRobot::OnSetJointCmdPos_B(double joint[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 7;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 208;
	m_InsRobot->m_Slen++;

	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];

	pv[0] = joint[0];
	pv[1] = joint[1];
	pv[2] = joint[2];
	pv[3] = joint[3];
	pv[4] = joint[4];
	pv[5] = joint[5];
	pv[6] = joint[6];
	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 7;
	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm joint cmd pose, joints=[%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",
	joint[0],joint[1],joint[2],joint[3],joint[4],joint[5],joint[6]);

	return true;
}
bool CRobot::OnSetForceCtrPara_B(int fcType, double fxDir[6], double fcCtrlPara[7], double fcAdjLmt)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 207;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = fcType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = fxDir[0];
	pv[1] = fxDir[1];
	pv[2] = fxDir[2];
	pv[3] = fxDir[3];
	pv[4] = fxDir[4];
	pv[5] = fxDir[5];


	pv[6] = fcCtrlPara[0];
	pv[7] = fcCtrlPara[1];
	pv[8] = fcCtrlPara[2];
	pv[9] = fcCtrlPara[3];
	pv[10] = fcCtrlPara[4];
	pv[11] = fcCtrlPara[5];
	pv[12] = fcCtrlPara[6];
	pv[13] = fcAdjLmt;

	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm force control parameters\n fcType=%d,\n",fcType);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcDirection=[%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf],\n",fxDir[0],fxDir[1],fxDir[2],fxDir[3],fxDir[4],fxDir[5]);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcCtrlPara=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\n",fcCtrlPara[0],fcCtrlPara[1],fcCtrlPara[2],fcCtrlPara[3],fcCtrlPara[4],fcCtrlPara[5],fcCtrlPara[6]);
	if(m_InsRobot->m_LocalLogTag == true) printf("fcAdjLmt=%lf\n",fcAdjLmt);

	return true;
}
bool CRobot::OnSetDragSpace_B(int zsType)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 6 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 206;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = zsType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = 0;
	pv[1] = 0;
	pv[2] = 0;
	pv[3] = 0;
	pv[4] = 0;
	pv[5] = 0;



	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 6;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm drag space type=%d\n",zsType);
	return true;
}
bool CRobot::OnSetCartKD_B(double K[6], double D[6],int type)
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 205;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = K[0];
	pv[1] = K[1];
	pv[2] = K[2];
	pv[3] = K[3];
	pv[4] = K[4];
	pv[5] = K[5];
	pv[6] = K[6];


	pv[7] = D[0];
	pv[8] = D[1];
	pv[9] = D[2];
	pv[10] = D[3];
	pv[11] = D[4];
	pv[12] = D[5];
	pv[13] = D[6];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_INT32* pi = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pi[0] = type;
	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm Cart k=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\nD=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\ntype=%d\n",
	K[0],K[1],K[2],K[3],K[4],K[5],K[6],
	D[0],D[1],D[2],D[3],D[4],D[5],D[6],
	type);

	return true;
}
bool CRobot::OnSetEefRot_B(int fcType, double CartCtrlPara[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 207;
	m_InsRobot->m_Slen++;

	FX_INT32* pv1 = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv1[0] = fcType;

	m_InsRobot->m_Slen += sizeof(FX_INT32);
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = 0;
	pv[1] = 0;
	pv[2] = 0;
	pv[3] = 0;
	pv[4] = 0;
	pv[5] = 0;
	pv[6] = CartCtrlPara[0];
	pv[7] = CartCtrlPara[1];
	pv[8] = CartCtrlPara[2];
	pv[9] = CartCtrlPara[3];
	pv[10] = CartCtrlPara[4];
	pv[11] = CartCtrlPara[5];
	pv[12] = CartCtrlPara[6];
	pv[13] = 0;

	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm flange Cart rotation parameters.\n fcType=%d,\n",fcType);
    if(m_InsRobot->m_LocalLogTag == true) printf("CartCtrlPara=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\n",CartCtrlPara[0],CartCtrlPara[1],CartCtrlPara[2],CartCtrlPara[3],CartCtrlPara[4],CartCtrlPara[5],CartCtrlPara[6]);
	return true;

}
bool CRobot::OnSetJointKD_B(double K[7], double D[7])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 14;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 204;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = K[0];
	pv[1] = K[1];
	pv[2] = K[2];
	pv[3] = K[3];
	pv[4] = K[4];
	pv[5] = K[5];
	pv[6] = K[6];


	pv[7] = D[0];
	pv[8] = D[1];
	pv[9] = D[2];
	pv[10] = D[3];
	pv[11] = D[4];
	pv[12] = D[5];
	pv[13] = D[6];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 14;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm joint k=[%lf,%lf,%lf,%lf,%lf,%lf,%lf],\nD=[%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",
	K[0],K[1],K[2],K[3],K[4],K[5],K[6],
	D[0],D[1],D[2],D[3],D[4],D[5],D[6]);

	return true;
}
bool CRobot::OnSetTool_B(double kinePara[6], double dynPara[10])
{
	long add_size = 1 + sizeof(FX_FLOAT) * 16;

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 202;
	m_InsRobot->m_Slen++;
	FX_FLOAT* pv = (FX_FLOAT*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = kinePara[0];
	pv[1] = kinePara[1];
	pv[2] = kinePara[2];
	pv[3] = kinePara[3];
	pv[4] = kinePara[4];
	pv[5] = kinePara[5];

	pv[6] = dynPara[0];
	pv[7] = dynPara[1];
	pv[8] = dynPara[2];
	pv[9] = dynPara[3];
	pv[10] = dynPara[4];
	pv[11] = dynPara[5];
	pv[12] = dynPara[6];
	pv[13] = dynPara[7];
	pv[14] = dynPara[8];
	pv[15] = dynPara[9];


	m_InsRobot->m_Slen += sizeof(FX_FLOAT) * 16;

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm tool kineParams=[%lf,%lf,%lf,%lf,%lf,%lf]\n dynamicParams=[%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf]\n",
	kinePara[0],kinePara[1],kinePara[2],kinePara[3],kinePara[4],kinePara[5],
	dynPara[0],dynPara[1],dynPara[2],dynPara[3],dynPara[4],dynPara[5],dynPara[6],dynPara[7],dynPara[8],dynPara[9]);

	return true;
}
bool CRobot::OnSetTargetState_B(int state)
{
	long add_size = 1 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 201;
	m_InsRobot->m_Slen++;
	FX_INT32* pv = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = state;

	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm state=%d\n",state);

	return true;
}


bool CRobot::OnSetImpType_B(int type)
{
	long add_size = 1 + sizeof(FX_INT32);

	if (add_size + m_InsRobot->m_Slen >= 1400)
	{
		return false;
	}

	m_InsRobot->m_SendBuf[m_InsRobot->m_Slen] = 211;
	m_InsRobot->m_Slen++;
	FX_INT32* pv = (FX_INT32*)&m_InsRobot->m_SendBuf[m_InsRobot->m_Slen];
	pv[0] = type;

	m_InsRobot->m_Slen += sizeof(FX_INT32);

	FX_UCHAR* pnum = (FX_UCHAR*)&m_InsRobot->m_SendBuf[2];
	(*pnum)++;
    if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Set B arm impedance type=%d\n",type);

	return true;
}

/// ////////////////////////////////


bool CRobot::OnSetSend()
{
	if (m_InsRobot == NULL || m_InsRobot->m_Slen <= 0 || m_InsRobot->m_Slen > 1400) return false;

	IoSendSlot* selected = NULL;
	bool overwriting = false;
	// Prefer a free slot. If the I/O thread is behind, reclaim a READY (never
	// READING) slot so publishing remains non-blocking and latest-wins.
	for (int pass = 0; pass < 2 && selected == NULL; ++pass)
	{
		const unsigned char wanted = pass == 0 ? IO_SLOT_FREE : IO_SLOT_READY;
		for (size_t i = 0; i < sizeof(m_InsRobot->m_IoSendSlots) / sizeof(m_InsRobot->m_IoSendSlots[0]); ++i)
		{
			IoSendSlot& slot = m_InsRobot->m_IoSendSlots[i];
			unsigned char expected = wanted;
			if (slot.state.compare_exchange_strong(
				expected, IO_SLOT_WRITING, std::memory_order_acquire, std::memory_order_relaxed))
			{
				selected = &slot;
				overwriting = pass != 0;
				break;
			}
		}
	}
	if (selected == NULL) return false;

	memcpy(selected->data, m_InsRobot->m_SendBuf, static_cast<size_t>(m_InsRobot->m_Slen));
	selected->length = m_InsRobot->m_Slen;
	selected->sequence.store(m_InsRobot->m_NextPublishSequence.fetch_add(
		1, std::memory_order_relaxed) + 1, std::memory_order_relaxed);
	selected->state.store(IO_SLOT_READY, std::memory_order_release);
	m_InsRobot->m_PublishCount.fetch_add(1, std::memory_order_relaxed);
	if (overwriting) m_InsRobot->m_OverwriteCount.fetch_add(1, std::memory_order_relaxed);
	m_InsRobot->m_Slen = 0;
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: OnSetSend\n");
	return true;
}

bool CRobot::OnUpdateSystem(char * local_path)
{
	if (!m_InsRobot->SendFile(local_path, (char *)"/home/FUSION/Tmp/ctrl_package.tar"))
	{
		return false;
	}
	char name[30];
	memset(name, 0, 30);
	sprintf(name, "UPDATES");
	OnSetIntPara(name, 0);
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: SDK update!\n");
	return true;
}

bool CRobot::OnDownloadLog(char* local_path)
{
	
	if (!m_InsRobot->RecvFile(local_path, (char*)"/home/FUSION/log/LOG.txt"))
	{
		return false;
	}
	if(m_InsRobot->m_LocalLogTag == true) printf("[Marvin SDK]: Send log to host:%s,\n",local_path);
	return true;
}



FX_BOOL CRobot::SendFile(char* local_file, char* remote_file)
{
	// printf("15\n");
	if (m_LinkTag == false)
	{
	    // printf("16\n");
		return FX_FALSE;
	}
	CTCPFileClient cln;
	if (cln.OnLinkTo(m_ip1, m_ip2, m_ip3, m_ip4, 10240) == false)
	{
		// printf("17\n");
		return FX_FALSE;
	}
	bool ret = cln.OnSendFile(local_file, remote_file);
	cln.OnQuit();
	if (ret == true)
	{
	    // printf("18\n");
		return FX_TRUE;
	}

	// printf("19\n");
	return FX_FALSE;
}



FX_BOOL CRobot::RecvFile(char* local_file, char* remote_file)
{
	if (m_LinkTag == false)
	{
		return FX_FALSE;
	}
	CTCPFileClient cln;
	if (cln.OnLinkTo(m_ip1, m_ip2, m_ip3, m_ip4, 10240) == false)
	{
		return FX_FALSE;
	}
	bool ret = cln.OnGetFile(local_file, remote_file);
	cln.OnQuit();
	if (ret == true)
	{
		return FX_TRUE;
	}
	return FX_FALSE;
}
