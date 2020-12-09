#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <iostream>

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

    int err = hostname_to_ip_address(hostname, addr);
    if (err)
        return err;

    ICMP_ECHO_REPLY* reply = new ICMP_ECHO_REPLY();
    const int TIMEOUT = 5000;
    ping(hostname, TIMEOUT, reply);

    inet_ntop(AF_INET, addr, str_addr, 100);
    cout << "IP of " << hostname << ": " << str_addr << endl;
    cout << "Time: " << reply->RoundTripTime << "ms, TTL: " << (int)(reply->Options.Ttl) << endl;
    WSACleanup();
    return 0;
}