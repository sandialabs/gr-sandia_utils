/*
 * vitaclassid.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: rfec
 */

#include <stdio.h>

#include "vitaclassid.h"

namespace gr {
namespace sandia_utils {

vita_class_id::vita_class_id()
{
    reset();

    return;
}
vita_class_id::vita_class_id(const vita_class_id& rhs) { copy(rhs); }

vita_class_id::~vita_class_id() { return; }

void vita_class_id::copy(const vita_class_id& rhs)
{
    pad_bit_count = rhs.pad_bit_count;
    oui = rhs.oui;
    info_class_code = rhs.info_class_code;
    packet_class_code = rhs.packet_class_code;

    return;
}

/**
 * Resets all member variables to defaults
 */
void vita_class_id::reset(void)
{
    pad_bit_count = 0;
    oui = 0;
    info_class_code = 0;
    packet_class_code = 0;

    unpackState = 0;

    return;
}


/**
 * Unpacks a 32byte word.
 *
 * @param word_in - incoming word
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int vita_class_id::unpack(uint32_t word_in)
{
    int ans = 0;

    switch (unpackState) {
    case (0): {
        // first word
        pad_bit_count = (word_in >> 27) & 0x1f;
        oui = word_in & 0x00ffffff;

        unpackState = 1;
        ans = 2;
        break;
    }
    case (1): {
        // second word
        info_class_code = (word_in >> 16) & 0x0000ffff;
        packet_class_code = word_in & 0x0000ffff;

        unpackState = 2;
        ans = 1;
        break;
    }
    default: {
        // unknown
        ans = 0;
        break;
    }
    } // end switch

    return ans;
} // end unpack

uint16_t vita_class_id::getInfoClassCode() const { return info_class_code; }

void vita_class_id::setInfoClassCode(uint16_t infoClassCode)
{
    info_class_code = infoClassCode;
}

uint32_t vita_class_id::getOui() const { return oui; }

void vita_class_id::setOui(uint32_t oui) { this->oui = oui; }

uint16_t vita_class_id::getPacketClassCode() const { return packet_class_code; }

void vita_class_id::setPacketClassCode(uint16_t packetClassCode)
{
    packet_class_code = packetClassCode;
}

uint8_t vita_class_id::getPadBitCount() const { return pad_bit_count; }

void vita_class_id::setPadBitCount(uint8_t padBitCount) { pad_bit_count = padBitCount; }

/**
 * Debug tool, print out to STDOUT
 */
void vita_class_id::debug_print(void)
{

    printf("vita_class_id\n");
    printf("\tPad Bit Count: %u\n", pad_bit_count);
    printf("\tOUI: %u\n", oui);
    printf("\tInfo Class: %u\n", info_class_code);
    printf("\tPacket Class: %u\n", packet_class_code);

    return;
} // end debug_print

} // namespace sandia_utils
} /* namespace gr */
