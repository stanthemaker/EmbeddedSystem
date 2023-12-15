#ifndef SOCKET_DEMO_H
#define SOCKET_DEMO_H

#include "mbed.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"

class SocketDemo {
public:
    SocketDemo();
    ~SocketDemo();
    void run();
    bool send_http_request(const char*);
private:
    bool resolve_hostname(SocketAddress &address);
    bool receive_http_response();
    void wifi_scan();
    void print_network_info();

    NetworkInterface *_net;
    char output[500];
    TCPSocket _socket;
};
#endif
