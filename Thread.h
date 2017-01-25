#ifndef THREAD_H
#define THREAD_H
#pragma once
#include <process.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
//最下面有例子看
typedef void *HANDLE;
class Thread
{
public:
       void start();
        virtual void run();
        HANDLE getThread();
private:
        HANDLE hThread;
        static void agent(void *p);
};

void Thread::start()
{
        hThread =(HANDLE)_beginthread(agent, 0, (void *)this);
}
void Thread::run()
{
        cout << "Base Thread" << endl;
}
void Thread::agent(void *p)
{
        Thread *agt = (Thread *)p;
        agt->run();
}
HANDLE Thread::getThread()
{
        return hThread;
}
#endif //THREAD_H
   
/*
#endif //THREAD_H
测试文件
#include "Thread.h"
#include <windows.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class DerivedThread: public Thread
{
public:
        void run();
};
void DerivedThread::run()
{
        cout << "Derived Thread" << endl;
}
int main(int argc, char *argv[])
{
        DerivedThread *dt = new DerivedThread();
        dt->start();
        WaitForSingleObject(dt->getThread(), INFINITE);
}

*/