/*
 * vitarx.h
 *
 *  Created on: Jul 22, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_VITARX_H_
#define LIB_VITA_VITARX_H_

#include <pthread.h>

#include <vector>

#include "VRTPacket.h"
#include "vitarxlistener.h"

namespace gr {
namespace sandia_utils {
void* vita_rx_task_launch(void* args);

/**
 * VITA 49 TCP receiver
 */
class vita_rx
{
private:
    int port;
    int sockFd;
    int clientFd;
    volatile bool running;
    bool connected;

    pthread_t handle;

    VRTPacket rxPacket;
    volatile uint32_t packBuf;
    volatile int packBufState;

    std::vector<vita_rx_listener*> listeners;

    volatile uint64_t cnt_rx_byte;

public:
    /**
     * Constructor
     *
     * @param _port = TCP port to listen on
     */
    vita_rx(int _port);
    virtual ~vita_rx();

    /**
     * start receiver
     *
     * @return int - 0 on error, 1 on success
     */
    int start(void);

    /**
     * Stop receiver
     *
     * @return int - 0 on error, 1 on success
     */
    int stop(void);


    /**
     * Thread task for running TCP receive in
     *
     * @param args -
     * @return void*
     */
    void* rx_task(void* args);

    /**
     * Adds a listener to the class
     *
     * @param listener - listener implementation
     */
    void add_listener(vita_rx_listener* listener);


private:
    /**
     * Sets up the server socket
     *
     * @return int - 0 on fail, 1 on success
     */
    int setup_socket(void);

    /**
     * Closes the server socket
     *
     * @return int - 0 on fail, 1 on success
     */
    int close_socket(void);

    /**
     * Processes read data
     *
     * @param buf - buffer with data
     * @param sz - size of buffer
     */
    void processData(uint8_t* buf, int sz);

    /**
     * Handles processing of a fully received packet
     */
    void processPacket(void);

    /**
     * trys to accept a client connection
     */
    void acceptClient(void);

protected:
    void fireReceived(VRTPacket* pkt);


}; // end class vita_rx

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* LIB_VITA_VITARX_H_ */
