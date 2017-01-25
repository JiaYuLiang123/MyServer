#include "stdafx.h"
#define _AFXDLL
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

BOOL SendFtpFile(CString ftpSite, CString user, CString password, int port, CString localFile, CString remoteFile)
{
 if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
      return FALSE;// catastropic error! MFC can't initialize

 CInternetSession sess(_T("SendFile"));
 CFtpConnection* pConnect = NULL;

 try
 {
  pConnect = sess.GetFtpConnection(ftpSite, user, password, port, FALSE);//ftp.gnu.org  //ftp.microsoft.com
  /*

  //此处注释是获取ftp服务器的文件列表
  CFtpFileFind finder(pConnect);// use a file find object to enumerate files
  BOOL bWorking = finder.FindFile(_T("*"));// start looping
  while (bWorking)
  {
     bWorking = finder.FindNextFile();
     //printf("%s\n", finder.GetFileURL());
  }
  */

  BOOL putFileRes = FtpPutFile(
   HINTERNET(*pConnect),  //HINTERNET hConnect,
   localFile,   //LPCTSTR lpszLocalFile,
   remoteFile,   //LPCTSTR lpszNewRemoteFile,
   FTP_TRANSFER_TYPE_BINARY, //DWORD dwFlags,
   NULL      //DWORD_PTR dwContext
   );
  if (putFileRes == FALSE)
  {
    //printf("putFileRes = FALSE : %d\n", GetLastError());
    if (pConnect != NULL) 
    {
      pConnect->Close();
      delete pConnect;
    }
   return FALSE;
  }
 }
 catch (CInternetException* pEx)
 {
   TCHAR sz[1024];
   pEx->GetErrorMessage(sz, 1024);
   //printf_s("ERROR!  %s\n", sz);
   pEx->Delete();
 }
 // if the connection is open, close it
 if (pConnect != NULL) 
 {
   pConnect->Close();
   delete pConnect;
 }
 return TRUE;
}

BOOL GetFtpFile(CString ftpSite, CString user, CString password, int port, CString localFile, CString remoteFile)
{
  if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
      return -1;// catastropic error! MFC can't initialize

 CInternetSession sess(_T("GetFile"));

 CFtpConnection* pConnect = NULL;

 try
 {
   pConnect = sess.GetFtpConnection(ftpSite, user, password, port, FALSE);//ftp.gnu.org

   /*
   // use a file find object to enumerate files
   CFtpFileFind finder(pConnect);
   // start looping
   BOOL bWorking = finder.FindFile(_T("*"));
   while (bWorking)
   {
     bWorking = finder.FindNextFile();
     //CString tmpStr = finder.GetFileURL();
     printf("%s\n", finder.GetFileURL());
   }
  */

  BOOL getFileRes = FtpGetFile(
   HINTERNET(*pConnect),  //HINTERNET hConnect,
   remoteFile,            //LPCTSTR lpszRemoteFile,
   localFile,             //LPCTSTR lpszNewFile,
   FALSE,                 //BOOL fFailIfExists,
   FILE_ATTRIBUTE_NORMAL,     //DWORD dwFlagsAndAttributes,
   FTP_TRANSFER_TYPE_BINARY,  //DWORD dwFlags,
   NULL                       //DWORD_PTR dwContext
   );
  if (getFileRes == FALSE)
  {
    pConnect->Close();
    delete pConnect;
    return FALSE;
    //printf("getFileRes = FALSE : %d\n", GetLastError());
  }
 }
 catch (CInternetException* pEx)
 {
   TCHAR sz[1024];
   pEx->GetErrorMessage(sz, 1024);
   //printf_s("ERROR!  %s\n", sz);
   pEx->Delete();
 }

 // if the connection is open, close it
 if (pConnect != NULL) 
 {
   pConnect->Close();
   delete pConnect;
 }
 return TRUE;
}




/*
int _tmain(int argc, _TCHAR* argv[])
{
  SendFtpFile("192.168.0.214", "user", "123456", 21, "d:\\Se7en.htm", "a\\Se7en.htm");
  GetFtpFile("192.168.0.214", "user", "123456", 21, "F:\\Se7en.htm", "a\\Se7en.htm");
 return 0;
}
*/