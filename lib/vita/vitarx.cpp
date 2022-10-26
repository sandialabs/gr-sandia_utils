/*
 * vitarx.cpp
 *
 *  Created on: Jul 22, 2020
 *      Author: sandia
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ContextPacket.h"
#include "vitarx.h"

namespace gr {
namespace sandia_utils {

vita_rx::vita_rx(int _port)
{
    port = _port;
    sockFd = -1;
    clientFd = -1;
    running = false;
    connected = false;
    handle = 0;

    packBuf = 0x00000000;
    packBufState = 0;

    cnt_rx_byte = 0;

    return;
}

vita_rx::~vita_rx()
{
    stop();

    return;
}

/**
 * start receiver
 *
 * @return int - 0 on error, 1 on success
 */
int vita_rx::start(void)
{
    int ans = 0;

    if (!running) {
        // start up
        if (setup_socket()) {
            // start thread
            pthread_create(&handle, NULL, vita_rx_task_launch, this);

            running = true;
            ans = 1;
        }
    } // end if( !running

    return ans;
}

/**
 * Stop receiver
 *
 * @return int - 0 on error, 1 on success
 */
int vita_rx::stop(void)
{
    int ans = 0;

    if (running) {
        running = false;

        // kill thread
        pthread_join(handle, NULL);

        close_socket();

        ans = 1;
    }

    return ans;
}

/**
 * Sets up the server socket
 *
 * @return int - 0 on fail, 1 on success
 */
int vita_rx::setup_socket(void)
{
    int ans = 0;
    int opt = 1;
    struct sockaddr_in address;
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd != -1) {
        if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0) {
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);


            if (setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
                0) {
                perror("Error setting RX timeout");
            }
            if (setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) <
                0) {
                perror("Error setting TX timeout");
            }


            if (bind(sockFd, (struct sockaddr*)&address, sizeof(address)) == 0) {
                if (listen(sockFd, 1) == 0) {
                    // we are good!
                    printf("vita_rx listening on port %d\n", port);
                    ans = 1;
                } else {
                    perror("listen error");
                }
            } else {
                perror("bind error");
            }

        } else {
            perror("setsockopt");
        }
    } else {
        perror("Socket Error");
    }


    return ans;
} // end setup_socket

/**
 * Closes the server socket
 *
 * @return int - 0 on fail, 1 on success
 */
int vita_rx::close_socket(void)
{
    int ans = 0;

    if (connected) {
        close(clientFd);
        connected = false;
    }

    close(sockFd);

    rxPacket.reset();

    return ans;
}


void* vita_rx_task_launch(void* args)
{
    vita_rx* me;

    me = (vita_rx*)args;

    me->rx_task(NULL);

    return NULL;
}

/**
 * Thread task for running TCP receive in
 *
 * @param args -
 * @return void*
 */
void* vita_rx::rx_task(void* args)
{
    uint8_t readBuf[128];
    int stat;

    while (running) {
        if (connected) {
            stat = read(clientFd, readBuf, sizeof(readBuf));
            // printf("vita_rx::rx_task() read %d\n", stat );
            if (stat > 0) {
                // got data
                processData(readBuf, stat);
            } else if (stat == 0) {
                printf("vita_rx::rx_task() client disconnect\n");
                close(clientFd);
                connected = false;
            } else {
                // printf("vita_rx::rx_task() errno %s\n", strerror(errno) );
                if (errno == EAGAIN) {
                    // ignore
                } else {
                    perror("Client read error");
                    // client error, disconnect
                    close(clientFd);
                    connected = false;
                }
            }
        } else {
            acceptClient();
        }

    } // end while( running

    return NULL;
} // end rx_tasks


/**
 * Processes read data
 *
 * @param buf - buffer with data
 * @param sz - size of buffer
 */
void vita_rx::processData(uint8_t* buf, int sz)
{
    int i;
    int stat;
    //uint32_t raw;

    for (i = 0; i < sz; i++) {
        // printf("%2x ", buf[i] & 0xff );
        cnt_rx_byte++;

        packBuf = (packBuf << 8) | (buf[i] & 0xff);
        packBufState++;

        if (packBufState >= 4) {
            packBufState = 0;

            // process the word

            // printf("\nRaw Word %08x\n", packBuf );
            stat = rxPacket.unpack(packBuf);
            if (stat == 1) {
                processPacket();
            } else if (stat == 0) {
                // printf(" Error rxPacket.unpacket() \n");
                // TODO how do we handle
            }
        }
    } // end for(i

    return;
} // end processData


/**
 * Handles processing of a fully received packet
 */
void vita_rx::processPacket(void)
{
    switch (rxPacket.getType()) {
    case (PacketType::CONTEXT): {
        ContextPacket cp;
        cp.unpack(rxPacket);

        fireReceived(&cp);
        break;
    }
    default: {
        fireReceived(&rxPacket);
        break;
    }
    }

    // reset it
    rxPacket.reset();

    return;
} // end processPacket


/**
 * trys to accept a client connection
 */
void vita_rx::acceptClient(void)
{
    struct sockaddr_in clientAddr;
    int addrlen = sizeof(clientAddr);
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;


    // wait around for a connect
    clientFd = accept(sockFd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrlen);
    if (clientFd >= 0) {
        // success, got a client
        printf("rx_task() client connected %s\n", inet_ntoa(clientAddr.sin_addr));

        if (setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
            0) {
            perror("Error setting Client RX timeout");
        }
        if (setsockopt(clientFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) <
            0) {
            perror("Error setting Client TX timeout");
        }

        connected = true;
    } else {
        if (errno == EAGAIN) {
            // ignore
        } else {
            perror(" Socket Accept: ");
            printf(" Error %d %s\n", errno, strerror(errno));
        }
    }

    return;
} // end acceptClient


/**
 * Adds a listener to the class
 *
 * @param listener - listener implementation
 */
void vita_rx::add_listener(vita_rx_listener* listener)
{
    if (listener != NULL) {
        listeners.push_back(listener);
    }

    return;
}

void vita_rx::fireReceived(VRTPacket* pkt)
{
    // printf("vita_rx::fireReceived() end pkt addr %lx\n", cnt_rx_byte );
    for (vita_rx_listener* l : listeners) {
        try {
            l->received_packet(pkt->getType(), pkt);
        } catch (...) {
            printf("Exception thrown in listener\n");
        }
    } // end foreach( listener

    return;
}


} /* namespace sandia_utils */
} /* namespace gr */
