/*
 * qa_manual_rx.cpp
 *
 *  Created on: Jul 23, 2020
 *      Author: rfec
 */

#include <stdio.h>
#include <stdlib.h>

#include "vitarx.h"

using namespace gr::sandia_utils;

// defs
class VRFMon : public vita_rx_listener
{
public:
    virtual void received_packet(PacketType type, VRTPacket* pkt)
    {
        pkt->debug_print();
        return;
    }
};

// vars
int port = 8207;
vita_rx* vitaRx;

// funcs
void banner(void);
void init(void);
void shutdown(void);
void control(void);


/**
 * main entry point
 *
 * @param argc -
 * @param argv -
 * @return int
 */
int main(int argc, char** argv)
{

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    banner();

    init();

    control();

    shutdown();


    return 0;
} // end main


void banner(void)
{
    printf("+-----------------\n");
    printf("| VITA 49 Receiver\n");
    printf("|");
    printf("| Usage: qa_manual_rx {port}\n");
    printf("+-----------------\n");
    printf("\n");

    return;
} // end banner


void init(void)
{
    vitaRx = new vita_rx(port);
    vitaRx->add_listener(new VRFMon());

    vitaRx->start();

    return;
}

void shutdown(void)
{
    vitaRx->stop();
    delete vitaRx;

    return;
}

void control(void)
{
    char r = 'a';

    while (r != 'q') {
        printf("---------------\n");
        printf("q\tquit\n");
        printf("  Selection -> ");
        scanf(" %c", &r);

        switch (r) {
        default: {
            break;
        }
        } // end switch
    }     // end while

    return;
} // end control
