#include "stdafx.h"

#include "TSocket.h"

 

#ifdef _DEBUG

#undef THIS_FILE

static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW

#endif

 

//////////////////////////////////////////////////////////////////////

// Construction/Destruction

//////////////////////////////////////////////////////////////////////

 

TSocket::TSocket()

{

m_bMode = FALSE;//UDP

m_ulRemoteIP = 0;

m_ulLocalIP = 0;

}

 

TSocket::~TSocket()

{

//释放Winsock库

WSACleanup();

}

 

BOOL TSocket::LoadSocket()

{

//加载Winsock库

WSADATA wsa;

if (WSAStartup(MAKEWORD(1,1),&wsa) != 0)

{

return FALSE;   //加载失败

}

return TRUE;

}

 

 

BOOL TSocket::CreateSocketSer(BOOL bMode)

{

m_bMode = bMode;

 

//创建SOCKET

if (m_bMode)

{

m_socket = socket(AF_INET,SOCK_STREAM,0);//TCP流式

}else

{

m_socket = socket(AF_INET,SOCK_DGRAM,0);//UDP方式

}

 

return TRUE;

}

 

 

SOCKET TSocket::CreateSocket( BOOL bMode )

{

m_bMode = bMode;

SOCKET socketCr;

 

//创建SOCKET

if (m_bMode)

{

socketCr = socket(AF_INET,SOCK_STREAM,0);//TCP方式

}else

{

socketCr = socket(AF_INET,SOCK_DGRAM,0);//UDP方式

}

 

return socketCr;

}

 

BOOL TSocket::BingSocket(u_long ulIP,u_short usPort)

{

//配置监听地址、绑定监听端口

SOCKADDR_IN Sersock;//用于服务器的监听SOCKET

ZeroMemory(&Sersock,sizeof(Sersock));

if(ulIP==0)

{

Sersock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

}else

{

Sersock.sin_addr.S_un.S_addr = htonl(ulIP);//IP

}

Sersock.sin_family = AF_INET;

Sersock.sin_port = htons(usPort);//端口号

int nbind = bind(m_socket,(SOCKADDR *)&Sersock,sizeof(SOCKADDR));

if (SOCKET_ERROR==nbind)

{

closesocket(m_socket);

return FALSE;

}

 

return TRUE;

}

 

DWORD WINAPI TSocket::TcpRecvProc( LPVOID lpParameter )

{

TSocket* psocket = ((RECVPARAM *)lpParameter)->psocket;

HWND hwnd = ((RECVPARAM *)lpParameter)->hwnd;

delete lpParameter;//释放内存

listen(psocket->m_socket,5);

SOCKADDR_IN addrClient;

int len=sizeof(SOCKADDR);

 

char szrecvbuf[MAX_MSG_LEN];

char szsendBuf[50];

ZeroMemory(szrecvbuf,sizeof(szrecvbuf));

ZeroMemory(szsendBuf,sizeof(szsendBuf));

 

SOCKET sockSer;

while(psocket->m_bIsRun)

{

//用accept函数接收客户方的连接

sockSer=accept(psocket->m_socket,(SOCKADDR*)&addrClient,&len);

if (INVALID_SOCKET==sockSer)

continue;



sprintf(szsendBuf,"Welcome %s to here!",inet_ntoa(addrClient.sin_addr));

//在客户方套接字clientSocket上使用send发送数据

send(sockSer,szsendBuf,strlen(szsendBuf)+1,0);

 

//接收的数据

recv(sockSer,szrecvbuf,50,0);

psocket->m_strData.Format(_T("IP:%s,端口:%d 说：%s"),

inet_ntoa(addrClient.sin_addr),addrClient.sin_port,szrecvbuf);

::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)psocket->m_strData.GetBuffer(0));

//关闭本次连接

closesocket(sockSer);

 

//休息100S

Sleep(100);

}

 

//关闭socket

closesocket(psocket->m_socket);

return 0;

}

 

DWORD WINAPI TSocket::UdpRecvProc( LPVOID lpParameter )

{

TSocket* psocket = ((RECVPARAM *)lpParameter)->psocket;

HWND hwnd = ((RECVPARAM *)lpParameter)->hwnd;

delete lpParameter;//释放内存

SOCKADDR_IN sockRecv;

int nlen = sizeof(SOCKADDR);

char szrecvbuf[MAX_MSG_LEN];

ZeroMemory(szrecvbuf,sizeof(szrecvbuf));

int nrecv;

while (psocket->m_bIsRun)

{

nrecv=recvfrom(psocket->m_socket,szrecvbuf,sizeof(szrecvbuf),0,(SOCKADDR *)&sockRecv,&nlen);

if (SOCKET_ERROR == nrecv)

break;

 

psocket->m_strData.Format(_T("IP:%s,端口:%d 说：%s"),

inet_ntoa(sockRecv.sin_addr),sockRecv.sin_port,szrecvbuf);

 

::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)psocket->m_strData.GetBuffer(0));

 

//休息100S

Sleep(1000);

}

 

//关闭socket

closesocket(psocket->m_socket);

CloseHandle(psocket->m_hRecv);

 

return 0;

}

 

BOOL TSocket::Start( RECVPARAM* recvPar )

{

m_bIsRun = TRUE;

 

//创建接收消息的线程

if (m_bMode)

{

m_hRecv = CreateThread(NULL,0,TcpRecvProc,(LPVOID)recvPar,0,NULL);

}else

{

m_hRecv = CreateThread(NULL,0,UdpRecvProc,(LPVOID)recvPar,0,NULL);

}

 

return TRUE;

}

 

BOOL TSocket::Stop()

{

//Kill线程？？？

if (m_hRecv!=NULL)

{

TerminateThread(m_hRecv,0);

//关闭socket

closesocket(m_socket);

}

 

m_bIsRun = FALSE;

return TRUE;

}

 

DWORD TSocket::TcpSend()

{

if (m_ulRemoteIP==0)

return -1;

 

SOCKET SocketSend;

SocketSend=CreateSocket(TRUE);

if (SocketSend==INVALID_SOCKET)

{

return -1;

}

 

SOCKADDR_IN SocketSendIn;

SocketSendIn.sin_family = AF_INET;

SocketSendIn.sin_addr.S_un.S_addr = htonl(m_ulRemoteIP);

SocketSendIn.sin_port = htons(m_usRemotePort);

char szBuf[50];

 

//先建立连接

int nRet = connect(SocketSend,(SOCKADDR*)&SocketSendIn,sizeof(SOCKADDR));

if (nRet==SOCKET_ERROR)

{

return -1;

}

 

nRet = recv(SocketSend,szBuf,sizeof(szBuf),0);

if (nRet==0)//超时

{

return -2;

}

 

//开始发送数据

send(SocketSend,m_strData.GetBuffer(0),m_strData.GetLength(),0);

closesocket(SocketSend);

 

return 0;

}

 

DWORD TSocket::UdpSend()

{

if (m_ulRemoteIP==0)

return -1;

SOCKET SocketSend;

SocketSend=CreateSocket(FALSE);

if (SocketSend==INVALID_SOCKET)

{

return -1;

}

SOCKADDR_IN SocketSendIn;

SocketSendIn.sin_family = AF_INET;

SocketSendIn.sin_addr.S_un.S_addr = htonl(m_ulRemoteIP);

SocketSendIn.sin_port = htons(m_usRemotePort);

int nSenlen=m_strData.GetLength()+1;

sendto(SocketSend,m_strData,nSenlen,0,(SOCKADDR *)&SocketSendIn,sizeof(SOCKADDR));

 

closesocket(SocketSend);

return 0;

}

 

DWORD TSocket::SendData( CString strSend )

{

//保存数据

m_strData = strSend;

 

if (m_bMode)

{

CreateThread(NULL,0,TcpSendProc,(LPVOID)this,0,NULL);

}else

{

CreateThread(NULL,0,UdpSendProc,(LPVOID)this,0,NULL);

}

return 0;

}

 

SOCKET TSocket::GetSocket()

{

return m_socket;

}

 

DWORD WINAPI TSocket::TcpSendProc( LPVOID lpParameter )

{

TSocket* pThis = (TSocket*)lpParameter;

return pThis->TcpSend();

}

 

DWORD WINAPI TSocket::UdpSendProc( LPVOID lpParameter )

{

TSocket* pThis = (TSocket*)lpParameter;

return pThis->UdpSend();

}