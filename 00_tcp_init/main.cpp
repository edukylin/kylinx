#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

// Properties >> Linker >> Input >> Additional_Dependencies
#pragma comment(lib, "ws2_32.lib")
    
int main(int argc, char* argv[])
{
    // Initiates use of the Winsock 2 DLL by a process;
    WORD    vers = MAKEWORD(2, 2); // WORD wVersionRequested;
    WSADATA data = { 0 };          // LPWSADATA lpWSAData;
    WSAStartup(vers, &data);
    
    // ********** Beg socket programming ********** //
    // ******************************************** //
    // ********** End socket programming ********** //
    
    // Terminates use of the Winsock 2 DLL;
    WSACleanup();


    return 0;
}