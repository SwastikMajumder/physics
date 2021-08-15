#include <windows.h>
#include <wchar.h>
#include <stdio.h>

#define STR_EQUAL 0

#define ASK_SOUL(x, y) (scanf(x, y))
#define TELL_SOUL(x, y) (printf(x, y))

#define UNIVERSE_LIFE_SPAN 10000

DWORD Space = 0;
LPWSTR RulerCommand = L"+";
DWORD Motivation = 0;
DWORD AbsoluteTime = 0;

DWORD WINAPI Mind(LPVOID Param){
    while(1){
        Space += Motivation * (DWORD)(wcscmp(RulerCommand, L"-") == STR_EQUAL ? -1 : 1);
        TELL_SOUL("%d\n", Space);
        Sleep(2000);
        AbsoluteTime += 1000;
    }
    return 0;
}

int main(int argc, char *argv[]){
    DWORD ThreadId;
    HANDLE ThreadHandle;
    DWORD Param = NULL;
    ThreadHandle = CreateThread(NULL, 0, Mind, &Param, 0, &ThreadId);

    while(AbsoluteTime < UNIVERSE_LIFE_SPAN){
        ASK_SOUL("%d", &Motivation);
    }

    if (ThreadHandle != NULL){
        WaitForSingleObject(ThreadHandle,INFINITE);
        CloseHandle(ThreadHandle);
    }

    return 0;
}
