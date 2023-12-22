#include "socket.h"
#include <cstdio>

static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 20;
static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 200;
static constexpr size_t REMOTE_PORT = 5050; // standard HTTP port
    

SocketDemo::SocketDemo() : _net(NetworkInterface::get_default_instance())
{
}

SocketDemo::~SocketDemo()
{
    if (_net) {
        _net->disconnect();
    }
}

void SocketDemo::run()
{
    if (!_net) {
        printf("Error! No network interface found.\r\n");
        return;
    }

    /* if we're using a wifi interface run a quick scan */
    if (_net->wifiInterface()) {
        /* the scan is not required to connect and only serves to show visible access points */
        wifi_scan();
    }

    printf("Connecting to the network...\r\n");

    nsapi_size_or_error_t result = _net->connect();
    if (result != 0) {
        printf("Error! _net->connect() returned: %d\r\n", result);
        return;
    }

    print_network_info();

    /* opening the socket only allocates resources */
    result = _socket.open(_net);
    if (result != 0) {
        printf("Error! _socket.open() returned: %d\r\n", result);
        return;
    }

    SocketAddress address;

    if (!resolve_hostname(address)) {
        return;
    }

    address.set_port(REMOTE_PORT);

    /* we are connected to the network but since we're using a connection oriented
        * protocol we still need to open a connection on the socket */

    printf("Opening connection to remote port %d\r\n", REMOTE_PORT);

    result = _socket.connect(address);
    if (result != 0) {
        printf("Error! _socket.connect() returned: %d\r\n", result);
        return;
    }
    printf("Wifi connection established! \r\n");
}

bool SocketDemo::resolve_hostname(SocketAddress &address)
{
    const char hostname[] = MBED_CONF_APP_HOSTNAME;

    /* get the host address */
    printf("\nResolve hostname %s\r\n", hostname);
    nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
    if (result != 0) {
        printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
        return false;
    }

    printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

    return true;
}

bool SocketDemo::send_http_request(const char* data)
{
    char buffer[100];
    snprintf(buffer, sizeof(buffer),
    "GET /stm32_data?data=%s HTTP/1.1\r\n"
    "Host: 192.168.200.234\r\n"
    "Connection: keep-alive\r\n"
    "\r\n", 
    data);

    nsapi_size_t bytes_to_send = strlen(buffer);
    nsapi_size_or_error_t bytes_sent = 0;

    while (bytes_to_send) {
        bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
        if (bytes_sent < 0) {
            printf("Error! _socket.send() returned: %d\r\n", bytes_sent);
            return false;
        } else {
            printf("sent %d bytes\r\n", bytes_sent);
        }

        bytes_to_send -= bytes_sent;
    }

    return true;
}

bool SocketDemo::is_socket_closed() {
    char buffer[1];
    _socket.set_blocking(false);  // Set the socket to non-blocking mode
    nsapi_size_or_error_t result = _socket.recv(buffer, sizeof(buffer));

    _socket.set_blocking(true);  // Set the socket back to blocking mode

    if (result == 0) {
        // recv returned 0, which means socket is closed
        printf("Socket is closed.\r\n");
        return true;
    } else if (result < 0 && result != NSAPI_ERROR_WOULD_BLOCK) {
        // recv returned an error other than WOULD_BLOCK, which also indicates a closed socket or another error
        printf("Socket error: %d\r\n", result);
        return true;
    }

    // Socket is still open
    printf("socket is still open \r\n");
    return false;
}

bool SocketDemo::receive_http_response()
{
    char buffer[MAX_MESSAGE_RECEIVED_LENGTH];
    int remaining_bytes = MAX_MESSAGE_RECEIVED_LENGTH;
    int received_bytes = 0;

    /* loop until there is nothing received or we've ran out of buffer space */
    nsapi_size_or_error_t result = remaining_bytes;
    while (result > 0 && remaining_bytes > 0) {
        result = _socket.recv(buffer + received_bytes, remaining_bytes);
        if (result < 0) {
            printf("Error! _socket.recv() returned: %d\r\n", result);
            return false;
        }

        received_bytes += result;
        remaining_bytes -= result;
    }

    /* the message is likely larger but we only want the HTTP response code */
    printf("received %d bytes:\r\n%.*s\r\n\r\n", received_bytes, strstr(buffer, "\n") - buffer, buffer);

    return true;
}

void SocketDemo::wifi_scan()
{
    WiFiInterface *wifi = _net->wifiInterface();

    WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

    /* scan call returns number of access points found */
    int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

    if (result <= 0) {
        printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
        return;
    }

    printf("%d networks available:\r\n", result);

    for (int i = 0; i < result; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("\r\n");
}

void SocketDemo::print_network_info()
{
    /* print the network info */
    SocketAddress a;
    _net->get_ip_address(&a);
    printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    _net->get_netmask(&a);
    printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    _net->get_gateway(&a);
    printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
}
