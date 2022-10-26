/*
 * vitarxlistener.h
 *
 *  Created on: Jul 27, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_VITARXLISTENER_H_
#define LIB_VITA_VITARXLISTENER_H_

#include "VRTPacket.h"

namespace gr {
namespace sandia_utils {

/**
 * Listener interface class
 */
class vita_rx_listener
{
public:
    vita_rx_listener();
    virtual ~vita_rx_listener();


    /**
     * Called when a packet is received
     *
     * @param type - type of packet received
     * @param pkt - the packet received
     */
    virtual void received_packet(PacketType type, VRTPacket* pkt) = 0;


}; // end class vita_rx_listener

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* LIB_VITA_VITARXLISTENER_H_ */
