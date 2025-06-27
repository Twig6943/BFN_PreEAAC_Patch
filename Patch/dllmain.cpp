#include <windows.h>
#include <string.h> 

typedef char* (*FuncType)(unsigned long long, int);

void WriteAddress(LPVOID Address)
{
    BYTE data = 0xEB;
    DWORD oldProtect;
    if (VirtualProtect(Address, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        memcpy(Address, &data, 1);
        VirtualProtect(Address, 1, oldProtect, &oldProtect);
    }
}

DWORD WINAPI PatchThreadProc(LPVOID param_1)
{
    //Patch Blaze
    HMODULE EngineBuildInfo = LoadLibraryA("Engine.BuildInfo.dll");
    if (EngineBuildInfo)
    {
        DWORD* Target = (DWORD*)((BYTE*)EngineBuildInfo + 0x3018);
        DWORD oldProtect;
        if (VirtualProtect(Target, sizeof(DWORD), PAGE_READWRITE, &oldProtect))
        {
            *Target = 0x379d6b;
            VirtualProtect(Target, sizeof(DWORD), oldProtect, &oldProtect);
        }
    }

    WriteAddress((LPVOID)0x141758411); //Patch Game Version
    WriteAddress((LPVOID)0x1417fd2d8); //Patch Anticheat Error

	const char message[] = "Pre EAAC Patch."; //Must not be too long

	//Wait for the function to become available
    FuncType func = (FuncType)0x141249d30;
    char* Target = nullptr;
    while ((Target= func(0xc4efa065ULL, 1)) == nullptr)
    {
        Sleep(500);
    }

	//Copy the message
    size_t TargetLen = strlen(Target);
    strcpy_s(Target, TargetLen, message);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        HANDLE thread = CreateThread(nullptr, 0, PatchThreadProc, nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
