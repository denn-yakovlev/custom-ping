#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <iostream>

#pragma comment (lib, "IPHLPAPI.lib")
#pragma comment (lib, "Ws2_32.lib")


int hostname_to_ip_address(const char *hostname, in_addr *ipv4_addr)
{
    
    WSADATA wsa_startup_result;
    WSAStartup(MAKEWORD(1, 1), &wsa_startup_result);

    const ADDRINFO hints =
    {
        AI_CANONNAME,
        PF_INET,
        SOCK_STREAM
    };
    ADDRINFO* addr_info_result = new ADDRINFO();
    getaddrinfo(hostname, "http", &hints, &addr_info_result);

    *ipv4_addr = ((sockaddr_in*)addr_info_result->ai_addr)->sin_addr;
    return GetLastError();
}

int main()
{
    const char* hostname = "google.com";
    char str_addr[100];
    in_addr *addr = new in_addr();

    int err = hostname_to_ip_address(hostname, addr);
    if (err)
        return err;

    inet_ntop(AF_INET, addr, str_addr, 100);
    std::cout << "IP of " << hostname << ": " << str_addr << std::endl;
    WSACleanup();
    return 0;
}