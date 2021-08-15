#include <windows.h>
#include <wchar.h>
#include <stdio.h>

#define STR_EQUAL 0

#define ASK_SOUL(x, y) (scanf(x, y))
#define TELL_SOUL(x, y) (printf(x, y))

#define FAKE_TIME(x) (Sleep(x))

#define MIND(x, y) main(x, y)

#define DIE return 0
#define LIFE_SPAN 10000

DWORD ViewPort = 0;
LPWSTR Ruler = L"+";
DWORD Curosity = 0;
DWORD Clock = 0;

DWORD WINAPI Space(LPVOID Param){
    while(1){
        ViewPort += Curosity * (DWORD)(wcscmp(Ruler, L"-") == STR_EQUAL ? -1 : 1);
        TELL_SOUL("%d\n", ViewPort);
        FAKE_TIME(2000);
        Clock += 1000;
    }
    return 0;
}

int MIND(int argc, char *argv[]){
    DWORD ThreadId;
    HANDLE ThreadHandle;
    DWORD Param = NULL;
    ThreadHandle = CreateThread(NULL, 0, Space, &Param, 0, &ThreadId);

    while(Clock < LIFE_SPAN){
        ASK_SOUL("%d", &Curosity);
    }

    if (ThreadHandle != NULL){
        WaitForSingleObject(ThreadHandle,INFINITE);
        CloseHandle(ThreadHandle);
    }

    DIE;
}
