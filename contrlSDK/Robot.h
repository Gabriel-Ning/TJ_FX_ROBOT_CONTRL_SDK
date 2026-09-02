#ifndef FX_ROBOT_SDK_H_ 
#define FX_ROBOT_SDK_H_
#include "FxRtCSDef.h"
#include "FxType.h"
#include  "ACB.h"
#include "TCPFileClient.h"
#include <atomic>
#include <stdint.h>
#ifdef CMPL_LIN
	#include <pthread.h>
#endif
#ifdef CMPL_WIN
	#include <windows.h>
	//#include <mmiscapi2.h>
	#pragma comment(lib,"winmm.lib")
	#include <winsock.h>
	#include <stdio.h>
	#include <stdint.h>
	#pragma comment(lib,"ws2_32.lib")
#endif
#include "PointSet.h"
#define    SDK_VERSION   1003

struct MarvinIoStats
{
	uint64_t tick_count;
	uint64_t deadline_miss_count;
	uint64_t max_lateness_ns;
	uint64_t publish_count;
	uint64_t overwrite_count;
	uint64_t send_attempt_count;
	uint64_t send_success_count;
	uint64_t send_error_count;
	int32_t last_send_errno;
	uint64_t send_work_total_ns;
	uint64_t recv_work_total_ns;
	uint64_t send_work_max_ns;
	uint64_t recv_work_max_ns;
	int32_t io_sched_policy;
	int32_t io_sched_priority;
};



class  CRobot
{

public:

    static void OnLocalLogOn();
    static void OnLocalLogOff();
	virtual ~CRobot();

	static bool OnClearChDataA();
	static bool OnClearChDataB();

	static long OnGetChDataA(unsigned char data_ptr[256], long* ret_ch);
	static bool OnSetChDataA(unsigned char data_ptr[256], long size_int,long set_ch);
	static long OnGetChDataB(unsigned char data_ptr[256], long* ret_ch);
	static bool OnSetChDataB(unsigned char data_ptr[256], long size_int, long set_ch);


	static bool OnSendPVT_A(char* local_file, long serial);
	static bool OnSendPVT_B(char* local_file, long serial);
	static long OnGetSDKVersion();
	static bool OnGetIoStats(MarvinIoStats* stats);
	static bool OnSendFile(char* local_file, char* remote_file);
	static bool OnRecvFile(char* local_file, char* remote_file);
	static long OnSetIntPara(char paraName[30],long setValue);
	static long OnSetFloatPara(char paraName[30], double setValue);
	static long OnGetIntPara(char paraName[30],long * retValue);
	static long OnGetFloatPara(char paraName[30],double * retValue);
	static long OnSavePara();
	static bool OnGetBuf(DCSS * ret);

	static bool OnStartGather(long targetNum, long targetID[35], long recordNum);
	static bool OnStopGather();
	static bool OnSaveGatherData(char * path);
	static bool OnSaveGatherDataCSV(char* path);

	static bool OnLinkTo(FX_UCHAR ip1, FX_UCHAR ip2, FX_UCHAR ip3, FX_UCHAR ip4);
	static bool OnRelease();

	static bool OnClearSet();
	

	static bool OnSetTargetState_A(int state);
	static bool OnSetTool_A(double kinePara[6], double dynPara[10]);
	static bool OnSetJointLmt_A(int velRatio, int AccRatio);
	static bool OnSetJointKD_A(double K[7], double D[7]);
	static bool OnSetCartKD_A(double K[7], double D[7], int type);
	static bool OnSetEefRot_A(int fcType, double CartCtrlPara[7]);
	static bool OnSetDragSpace_A(int zsType);
	static bool OnSetForceCtrPara_A(int fcType, double fxDir[6], double fcCtrlPara[7], double fcAdjLmt);
	static bool OnSetJointCmdPos_A(double joint[7]);
	static bool OnSetForceCmd_A(double force);
	static bool OnSetPVT_A(int id);
	static bool OnSetImpType_A(int type);
	static bool OnSetTargetState_B(int state);
	static bool OnSetTool_B(double kinePara[6], double dynPara[10]);
	static bool OnSetJointLmt_B(int velRatio, int AccRatio);
	static bool OnSetJointKD_B(double K[7], double D[7]);
	static bool OnSetCartKD_B(double K[6], double D[6],int type);
	static bool OnSetEefRot_B(int fcType, double CartCtrlPara[7]);
	static bool OnSetDragSpace_B(int zsType);
	static bool OnSetForceCtrPara_B(int fcType, double fxDir[6], double fcCtrlPara[7], double fcAdjLmt);
	static bool OnSetJointCmdPos_B(double joint[7]);
	static bool OnSetForceCmd_B(double force);
	static bool OnSetImpType_B(int type);
	static bool OnSetPVT_B(int id);

	static bool OnSetSend();

	static bool OnUpdateSystem(char* local_path);
	static bool OnDownloadLog(char* local_path);


	void DoRecv();
	void DoSend();

    bool       m_LocalLogTag;
protected:
	CRobot();
	static CRobot* GetIns();
	long       m_ParaSerial;
	long       m_GatherTag;
	FX_FLOAT * m_GatherItem[40];
	long       m_GatherItemSize;
	long       m_GatherRecordMaxNum;
	long       m_GatherRecordNum;
	CPointSet m_GatherSet;
	FX_UINT32 miss_cnt;
	FX_INT32 old_serial;
	FX_BOOL m_LinkTag;
	FX_BOOL old_serial_tag;
#ifdef CMPL_WIN
	MMRESULT m_TimeEventID;
#endif
	FX_BOOL m_LastGatherTag;
#ifdef CMPL_LIN
	static void* IoThreadEntry(void* arg);
	static void* RecvThreadEntry(void* arg);
	bool StartIoThread();
	void StopIoThread();
	void ConfigureIoThread();
	pthread_t m_IoThread;
	pthread_t m_RecvThread;
	std::atomic<bool> m_IoThreadRunning;
	bool m_IoThreadStarted;
	bool m_RecvThreadStarted;
#endif	
	DCSS    m_DCSS;
	DCSS    m_temp_dcss;//win

	DCSS    m_DCSS_Send;
	FX_UCHAR m_RunState;

	SOCKET _local_sock;
	SOCKET _tosock_;
	struct sockaddr_in _to;
	int    _toLen;
	struct sockaddr_in _local;
	int		_localLen;
	int    _from_valid = 0;

	int server_sockaddr_in_len_;

	char recvbuf[2000];

	char m_SendBuf[1400];
	long m_Slen;
	enum { IO_SLOT_FREE = 0, IO_SLOT_WRITING = 1, IO_SLOT_READY = 2, IO_SLOT_READING = 3 };
	struct IoSendSlot
	{
		std::atomic<unsigned char> state;
		std::atomic<uint64_t> sequence;
		long length;
		char data[1400];
	};
	// Extra slots absorb producer/consumer CAS overlap without blocking the controller thread.
	IoSendSlot m_IoSendSlots[8];
	std::atomic<uint64_t> m_NextPublishSequence;
	std::atomic<uint64_t> m_IoTickCount;
	std::atomic<uint64_t> m_IoDeadlineMissCount;
	std::atomic<uint64_t> m_IoMaxLatenessNs;
	std::atomic<uint64_t> m_PublishCount;
	std::atomic<uint64_t> m_OverwriteCount;
	std::atomic<uint64_t> m_SendAttemptCount;
	std::atomic<uint64_t> m_SendSuccessCount;
	std::atomic<uint64_t> m_SendErrorCount;
	std::atomic<int32_t> m_LastSendErrno;
	std::atomic<uint64_t> m_SendWorkTotalNs;
	std::atomic<uint64_t> m_RecvWorkTotalNs;
	std::atomic<uint64_t> m_SendWorkMaxNs;
	std::atomic<uint64_t> m_RecvWorkMaxNs;
	std::atomic<int32_t> m_IoSchedPolicy;
	std::atomic<int32_t> m_IoSchedPriority;
	FX_BOOL SendFile(char* local_file, char* remote_file);
	FX_BOOL RecvFile(char* local_file, char* remote_file);

	unsigned char m_ip1;
	unsigned char m_ip2;
	unsigned char m_ip3;
	unsigned char m_ip4;


	CACB  m_ACB1;
	CACB  m_ACB2;
	char m_SendBuf1[600];
	char m_SendBuf2[600];
	DDSS* pDDSS1;
	DDSS* pDDSS2;

};

#endif
