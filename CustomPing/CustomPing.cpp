#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <iostream>
#include <synchapi.h>

#pragma comment (lib, "IPHLPAPI.lib")
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int hostname_to_ip_address(char *hostname, in_addr *ipv4_addr)
{
    const ADDRINFO hints =
    {
        AI_CANONNAME,
        PF_INET,
        SOCK_STREAM
    };

    ADDRINFO* addr_info_result = new ADDRINFO();
    int err = getaddrinfo(hostname, "http", &hints, &addr_info_result);
    
    switch (err)
    {
        case 0: 
        {
            *ipv4_addr = ((sockaddr_in*)addr_info_result->ai_addr)->sin_addr;
            break;
        }
        case HOST_NOT_FOUND:
        {
            cout << "Invalid host name" << endl;
            break;
        }
        case TRY_AGAIN:
        {
            cout << "Not enough memory"<< endl;
            break;
        }
        case WSA_NOT_ENOUGH_MEMORY:
        {
            cout << "Not enough memory"<< endl;
            break;
        }   
        default:
        {
            cout << "hostname_to_ip_address, an error occured: " << err << endl;
            break;
        }
    }
    return GetLastError();
}

int ping(char *hostname, int timeout, ICMP_ECHO_REPLY *reply)
{ 
    in_addr* addr = new in_addr();
    hostname_to_ip_address(hostname, addr);

    HANDLE icmp_handle = IcmpCreateFile();
    if (icmp_handle == INVALID_HANDLE_VALUE)
        cout << "Cannot create ICMP hanlde" << endl;
    else {
        int reply_size = sizeof(ICMP_ECHO_REPLY);
        ICMP_ECHO_REPLY* reply_buffer = new ICMP_ECHO_REPLY();
        IcmpSendEcho(icmp_handle, (addr->S_un).S_addr, NULL, 0, NULL, reply_buffer, reply_size, timeout);
        switch (reply_buffer->Status)
        {
            case 0: 
            {
                *reply = *reply_buffer;
                break;
            }
            case IP_REQ_TIMED_OUT:
            {
                cout << "Timeout!" << endl;
                break;
            }   
            case IP_BAD_DESTINATION:
            {
                cout << "Bad destination" << endl;
                break;
            }
            default:
            {
                cout << "ping, an error occured: " << reply_buffer->Status << endl;
            }
        }
        
    }
    return GetLastError();
}

int main()
{
    int err;
    WSADATA wsa_startup_result;
    WSAStartup(MAKEWORD(1, 1), &wsa_startup_result);

    char* hostname = new char[100];
    cout << "input hostname: ";
    cin >> hostname;

    char str_addr[100];
    in_addr *addr = new in_addr();

    err = hostname_to_ip_address(hostname, addr);
    if (err)
        return err;

    int packets_count;
    cout << "packets count: ";
    cin >> packets_count;

    inet_ntop(AF_INET, addr, str_addr, 100);
    cout << "IP of " << hostname << ": " << str_addr << std::endl;

    ICMP_ECHO_REPLY* reply = new ICMP_ECHO_REPLY();
    const int TIMEOUT = 5000;

    int delivered_packets = 0;
    for (int i = 0; i < packets_count; i++) {
      err = ping(hostname, TIMEOUT, reply);
      if (!err)
          delivered_packets++;
      std::cout << "Time: " << reply->RoundTripTime << " ms, TTL: " << (int)reply->Options.Ttl << endl;
      Sleep(1000);
    }
    cout << "Delivered packets: " << delivered_packets << endl;
    WSACleanup();
    return 0;
}