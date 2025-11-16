// A TcpHeartbeatClient with behavior
// Working
//step 1 - Init — initWinsock() calls WSAStartup. On success the client proceeds; on failure it logs an error and stops.
// step 2 - Connect loop — Main loop checks isConnected(). If not connected it calls connectToServer() which creates a socket, validates the IP, and attempts connect(). On success logs [INFO] Connected; on failure closes socket, logs error, sleeps 5s, and retries.
// step 3 - Heartbeat send — If connected, sendHeartbeat() sends "PING". On send error it logs and signals failure (leading to disconnect + reconnect).
// step 4 - Receive & validate — receiveResponse() sets a 5s recv timeout, calls recv(), handles errors (timeout or closed socket), and checks the payload. If response equals "PONG" it returns success; otherwise it logs a warning and returns failure.
// step 5 - Post-check & cleanup — If send/receive fails the client disconnect()s (closes socket, marks disconnected) and retries after backoff. If both succeed it waits 10s then repeats. Destructor calls disconnect() and WSACleanup() on shutdown.

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>

using namespace std;
using namespace std::chrono;

class TcpHeartbeatClient {
public:
    TcpHeartbeatClient(const string& ip, uint16_t port)
        : server_ip(ip), server_port(port), sockfd(INVALID_SOCKET) {}

    ~TcpHeartbeatClient() {
        disconnect();
        WSACleanup();
    }

    bool initWinsock() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            cerr << "[ERROR] WSAStartup failed\n";
            return false;
        }
        return true;
    }

    void run() {
        if (!initWinsock()) return;

        while (true) {
            if (!isConnected())
                connectToServer();

            if (isConnected()) {
                if (!sendHeartbeat() || !receiveResponse()) {
                    disconnect();
                    continue;
                }
                this_thread::sleep_for(seconds(10));
            } else {
                this_thread::sleep_for(seconds(5));
            }
        }
    }

private:
    string server_ip;
    uint16_t server_port;
    SOCKET sockfd;

    bool isConnected() const {
        return sockfd != INVALID_SOCKET;
    }

    void connectToServer() {
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd == INVALID_SOCKET) {
            cerr << "[ERROR] Socket creation failed\n";
            return;
        }

        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);

        if (InetPtonA(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
            cerr << "[ERROR] Invalid IP address\n";
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            return;
        }

        cout << "[INFO] Connecting to " << server_ip << ':' << server_port << "...\n";

        if (connect(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == SOCKET_ERROR) {
            cerr << "[ERROR] Connection failed\n";
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            return;
        }

        cout << "[INFO] Connected\n";
    }

    void disconnect() {
        if (sockfd != INVALID_SOCKET) {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            cout << "[INFO] Disconnected\n";
        }
    }

    bool sendHeartbeat() {
        const char* msg = "PING";
        if (send(sockfd, msg, (int)strlen(msg), 0) == SOCKET_ERROR) {
            cerr << "[ERROR] Send failed\n";
            return false;
        }
        cout << "[INFO] Sent: PING\n";
        return true;
    }

    bool receiveResponse() {
        char buffer[1024] = {0};
        int timeoutMs = 5000;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT)
                cerr << "[WARN] No response in 5s\n";
            else
                cerr << "[WARN] Recv failed\n";
            return false;
        } else if (received == 0) {
            cerr << "[WARN] Server closed connection\n";
            return false;
        }

        string resp(buffer, received);
        resp.erase(remove_if(resp.begin(), resp.end(), ::isspace), resp.end());

        cout << "[INFO] Received: " << resp << '\n';

        if (resp == "PONG") {
            cout << "[INFO] Server Alive\n";
            return true;
        }

        cerr << "[WARN] Unexpected response\n";
        return false;
    }
};

int main() {
    TcpHeartbeatClient client("192.168.1.100", 5000);
    client.run();
    return 0;
}
