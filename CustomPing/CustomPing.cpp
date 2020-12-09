#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <iostream>
#include <time.h>
#include <synchapi.h>

#pragma comment (lib, "IPHLPAPI.lib")
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int hostname_to_ip_address(const char *hostname, in_addr *ipv4_addr)
{
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

int ping(const char *hostname, int timeout, ICMP_ECHO_REPLY *reply)
{ 
    in_addr* addr = new in_addr();
    hostname_to_ip_address(hostname, addr);

    HANDLE icmp_handle = IcmpCreateFile();
    int reply_size = sizeof(ICMP_ECHO_REPLY);
    ICMP_ECHO_REPLY* reply_buffer = new ICMP_ECHO_REPLY();
    IcmpSendEcho(icmp_handle, (addr->S_un).S_addr, NULL, 0, NULL, reply_buffer, reply_size, timeout);
    *reply = *reply_buffer;
    return GetLastError();
}

int main()
{
    WSADATA wsa_startup_result;
    WSAStartup(MAKEWORD(1, 1), &wsa_startup_result);

    const char* hostname = "google.com";
    char str_addr[100];
    in_addr *addr = new in_addr();

    hostname_to_ip_address(hostname, addr);
    inet_ntop(AF_INET, addr, str_addr, 100);
    std::cout << "IP of " << hostname << ": " << str_addr << std::endl;

    ICMP_ECHO_REPLY* reply = new ICMP_ECHO_REPLY();
    const int TIMEOUT = 5000;

    while(true) {
      clock_t start = clock();
      int err = ping(hostname, TIMEOUT, reply);
      if (err)
        return err;
      clock_t end = clock();
      double seconds = (double)(end - start) / CLOCKS_PER_SEC;
      std::cout << "Time: " << reply->RoundTripTime << " ms" << endl;
      Sleep(1000);
    }

    WSACleanup();
    return 0;
}