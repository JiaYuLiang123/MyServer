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

//�ͷ�Winsock��

WSACleanup();

}

 

BOOL TSocket::LoadSocket()

{

//����Winsock��

WSADATA wsa;

if (WSAStartup(MAKEWORD(1,1),&wsa) != 0)

{

return FALSE;   //����ʧ��

}

return TRUE;

}

 

 

BOOL TSocket::CreateSocketSer(BOOL bMode)

{

m_bMode = bMode;

 

//����SOCKET

if (m_bMode)

{

m_socket = socket(AF_INET,SOCK_STREAM,0);//TCP��ʽ

}else

{

m_socket = socket(AF_INET,SOCK_DGRAM,0);//UDP��ʽ

}

 

return TRUE;

}

 

 

SOCKET TSocket::CreateSocket( BOOL bMode )

{

m_bMode = bMode;

SOCKET socketCr;

 

//����SOCKET

if (m_bMode)

{

socketCr = socket(AF_INET,SOCK_STREAM,0);//TCP��ʽ

}else

{

socketCr = socket(AF_INET,SOCK_DGRAM,0);//UDP��ʽ

}

 

return socketCr;

}

 

BOOL TSocket::BingSocket(u_long ulIP,u_short usPort)

{

//���ü�����ַ���󶨼����˿�

SOCKADDR_IN Sersock;//���ڷ������ļ���SOCKET

ZeroMemory(&Sersock,sizeof(Sersock));

if(ulIP==0)

{

Sersock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

}else

{

Sersock.sin_addr.S_un.S_addr = htonl(ulIP);//IP

}

Sersock.sin_family = AF_INET;

Sersock.sin_port = htons(usPort);//�˿ں�

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

delete lpParameter;//�ͷ��ڴ�

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

//��accept�������տͻ���������

sockSer=accept(psocket->m_socket,(SOCKADDR*)&addrClient,&len);

if (INVALID_SOCKET==sockSer)

continue;



sprintf(szsendBuf,"Welcome %s to here!",inet_ntoa(addrClient.sin_addr));

//�ڿͻ����׽���clientSocket��ʹ��send��������

send(sockSer,szsendBuf,strlen(szsendBuf)+1,0);

 

//���յ�����

recv(sockSer,szrecvbuf,50,0);

psocket->m_strData.Format(_T("IP:%s,�˿�:%d ˵��%s"),

inet_ntoa(addrClient.sin_addr),addrClient.sin_port,szrecvbuf);

::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)psocket->m_strData.GetBuffer(0));

//�رձ�������

closesocket(sockSer);

 

//��Ϣ100S

Sleep(100);

}

 

//�ر�socket

closesocket(psocket->m_socket);

return 0;

}

 

DWORD WINAPI TSocket::UdpRecvProc( LPVOID lpParameter )

{

TSocket* psocket = ((RECVPARAM *)lpParameter)->psocket;

HWND hwnd = ((RECVPARAM *)lpParameter)->hwnd;

delete lpParameter;//�ͷ��ڴ�

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

 

psocket->m_strData.Format(_T("IP:%s,�˿�:%d ˵��%s"),

inet_ntoa(sockRecv.sin_addr),sockRecv.sin_port,szrecvbuf);

 

::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)psocket->m_strData.GetBuffer(0));

 

//��Ϣ100S

Sleep(1000);

}

 

//�ر�socket

closesocket(psocket->m_socket);

CloseHandle(psocket->m_hRecv);

 

return 0;

}

 

BOOL TSocket::Start( RECVPARAM* recvPar )

{

m_bIsRun = TRUE;

 

//����������Ϣ���߳�

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

//Kill�̣߳�����

if (m_hRecv!=NULL)

{

TerminateThread(m_hRecv,0);

//�ر�socket

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

 

//�Ƚ�������

int nRet = connect(SocketSend,(SOCKADDR*)&SocketSendIn,sizeof(SOCKADDR));

if (nRet==SOCKET_ERROR)

{

return -1;

}

 

nRet = recv(SocketSend,szBuf,sizeof(szBuf),0);

if (nRet==0)//��ʱ

{

return -2;

}

 

//��ʼ��������

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

//��������

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