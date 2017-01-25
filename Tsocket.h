#if !defined(AFX_TSOCKET_H__ECFF7A02_DCAF_455D_97C3_0C1D465D977B__INCLUDED_)

#define AFX_TSOCKET_H__ECFF7A02_DCAF_455D_97C3_0C1D465D977B__INCLUDED_

 

#if _MSC_VER > 1000

#pragma once

#endif // _MSC_VER > 1000
#include <WinSock2.h>
#include <atlstr.h>

#pragma comment(lib,"ws2_32.lib")

 

#define WM_RECVDATA WM_USER+500 //定义接收到数据发出的消息号

#define MAX_MSG_LEN 1500 //最大的消息长度(MTU)

#define DEFAULTPORT 1991 //定义默认服务端口

 

class TSocket;

 

struct RECVPARAM 

{

TSocket* psocket;//指定创建的socket

HWND hwnd;//指定需要处理接收消息的窗口句柄

};

 

class TSocket  

{

public:

TSocket();

virtual ~TSocket();

 

public:

//加载Winsock库

BOOL LoadSocket();

BOOL CreateSocketSer(BOOL bMode);//创建socket用于服务器端

SOCKET CreateSocket(BOOL bMode);//创建socket用于客户端

BOOL BingSocket(u_long ulIP=0,u_short usPort=DEFAULTPORT);//绑定端口、IP(默认所有IP)

BOOL Start(RECVPARAM* recvPar);

BOOL Stop();

SOCKET GetSocket();//得到当前socket

CString m_strData;//发出去的数据

 

BOOL m_bIsRun; //是否继续运行

 

u_long m_ulLocalIP;//本地IP

u_short m_usLocalPort;//本地端口

u_long m_ulRemoteIP;//远程IP

u_short m_usRemotePort;//远程端口

 

//发送数据

DWORD SendData(CString strSend);

//TCP发送线程

static DWORD WINAPI TcpSendProc(LPVOID lpParameter);

//UDP发送线程

static DWORD WINAPI UdpSendProc(LPVOID lpParameter);

//TCP发送数据

DWORD TcpSend();

//UDP发送数据

DWORD UdpSend();

 

private:

BOOL m_bMode; //通信方式 1--TCP 0--UDP

SOCKET m_socket;//套接字

HANDLE m_hRecv; //接收线程的句柄

 

//TCP接收线程

static DWORD WINAPI TcpRecvProc(LPVOID lpParameter);

//UDP接收线程

static DWORD WINAPI UdpRecvProc(LPVOID lpParameter);

 

};

 

#endif // !defined(AFX_TSOCKET_H__ECFF7A02_DCAF_455D_97C3_0C1D465D977B__INCLUDED_)