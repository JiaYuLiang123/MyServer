// demo.cpp : 定义控制台应用程序的入口点。
//
#pragma once
#include "stdafx.h"
//#include "Afx.h"
#include <windows.h>
#include <iostream>
#include "all.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{

    char command[100];
	//cout<<"input command:"<<endl;
	//cin>>command;
	while(strcmp(command,"end")!=0)
	{
	  cout<<"input command:"<<endl;
	  cin>>command;
	switch (strlen(command))
	{
	case 0:cout<<"nothing"<<endl;break;
	case 1:   
		if(strcmp(command,"\n")==0)
	    {
	        cout<<"输入了一个回车"<<endl;
			break;
	    } 
				if(strcmp(command,"?")==0)
	    {
	        cout<<"end "<<endl;
			break;
	    } 
			 
	case 2:break;
	case 3:break;
   
	
	}

	}
	cout<<"thx using"<<endl;
	Sleep(1000);
	return 0;
}

