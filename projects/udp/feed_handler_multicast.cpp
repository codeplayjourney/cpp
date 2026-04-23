#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "market_data.h"
#include <atomic>
#include <csignal>
#include <arpa/inet.h>

#define PORT "9000"

std::atomic<bool> running{true};
void signal_handler(int signal) {
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler); // which signal, what fn to call when signal arrives
    // setup
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints); // so wont contain garbage
    hints.ai_family   = AF_INET; // ipv4
    hints.ai_socktype = SOCK_DGRAM; // datagram socket (UDP)
    hints.ai_flags    = AI_PASSIVE; // AI_PASSIVE = "set sin_addr to 0.0.0.0 so I can receive from anyone"
    
    // hints.ai_protocol = 0  ← left as 0 meaning "don't care, figure it out":
    /*
    AF_INET + SOCK_DGRAM → only one possible protocol → UDP = 17
    AF_INET + SOCK_STREAM → only one possible protocol → TCP = 6
    */

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }
    // note: getaddrinfo() creates res linked list in memory, we will free it later. Fills all fields including sin_addr, sin_port, etc.

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // create socket with same family, type, protocol as res.
    // creates a socket object inside OS kernel, returns a file descriptor (integer) that we can use to refer to that socket in future calls. The socket is not yet associated with any local port or remote address until we call bind() or connect().
    if (sock == -1) { perror("socket"); return 1; }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("239.0.0.1"); // Sets which multicast group to join. inet_addr() converts the string "239.0.0.1" into a 4-byte binary IP address — same conversion getaddrinfo() does internally.
    mreq.imr_interface.s_addr = INADDR_ANY; // Which network interface to receive multicast on. INADDR_ANY = 0.0.0.0 = let the OS pick the best interface automatically. Could also specify 192.168.1.42 to only receive on WiFi specifically.
    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof mreq);
        

    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        // takes the address from res->ai_addr and associates it with the socket.
        // This means that the socket will listen for incoming packets sent to that address and port. Since we used AI_PASSIVE, the address will be INADDR_ANY, which means we will receive packets sent to any of our IP addresses on that port.
        perror("bind"); return 1;
    }
    // set 1 second timeout so recvfrom() doesn't block forever
    struct timeval tv;
    tv.tv_sec  = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    /*
    Each parameter:
    sock — which socket to configure. Same file descriptor from socket().
    SOL_SOCKET — which level of the networking stack this option applies to. SOL_SOCKET means the socket layer itself (as opposed to IPPROTO_IP for IP layer or IPPROTO_TCP for TCP layer).
    SO_RCVTIMEO — which option to set. SO_RCVTIMEO = "receive timeout". Tells the socket how long to wait before giving up on recvfrom().
    &tv — pointer to the value. Here a timeval struct with 1 second.
    sizeof tv — how many bytes the value is. setsockopt needs this because different options have different value sizes.
    */

    freeaddrinfo(res);

    printf("feed handler: listening on port %s\n", PORT);

    // receive loop
    MarketUpdate update;
    struct sockaddr_in sender; // declared to hold the sender's address, will be filled in by recvfrom() when we receive a packet. We can use this to know who sent the packet and reply if needed.
    /*
    recall:
    struct sockaddr_in {
        short int      sin_family;   // AF_INET
        unsigned short sin_port;     // port number
        struct in_addr sin_addr;     // IP address
        char           sin_zero[8];  // padding
    };
    */
    socklen_t sender_len = sizeof sender; // need to specify the size of the sender address struct when calling recvfrom(), so that recvfrom() knows how much space it has to write the sender's address into. After recvfrom() returns, sender_len will contain the actual size of the sender's address that was written, differs for ipv4 and ipv6
    /* example of recvfrom() for ipv6:
    struct sockaddr_in6 sender6;
    socklen_t sender6_len = sizeof sender6;
    recvfrom(sock, &update, sizeof update, 0, (struct sockaddr*)&sender, &sender_len); // recvfrom() always expects struct sockaddr*
    */

    while (running) {
        int bytes = recvfrom(sock, &update, sizeof update, 0,
                             (struct sockaddr*)&sender, &sender_len);
        if (bytes == -1) { 
            // errno is a global integer variable defined in <errno.h> that the OS sets automatically whenever a system call fails.
            // When any system call returns -1 (meaning error), it also sets errno to a specific error code telling you why it failed:
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // errno is now set to some error code
                // EAGAIN      = 11  — timeout, try again
                // EWOULDBLOCK = 11  — same as EAGAIN on most platforms
                // EBADF       = 9   — bad file descriptor
                // EINTR       = 4   — interrupted by signal
                // etc.
                // not a real error — just timeout expired
                // recvfrom waited 1 second, no packet arrived
                continue;  // timeout, no packet — check running and loop back
            }
            perror("recvfrom"); break; 
        }

        printf("received: %s %.2f x %d %c  ts=%llu\n",
               update.symbol, update.price,
               update.quantity, update.side,
               update.timestamp);
    }

    close(sock);
    return 0;
}