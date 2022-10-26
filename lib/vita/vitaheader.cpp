/*
 * vitaheader.cpp
 *
 *  Created on: Jul 21, 2020
 *      Author: sandia
 */

#include "vitaheader.h"
#include <stdio.h>

namespace gr {
namespace sandia_utils {

vita_header::vita_header()
{
    type = PacketType::UNKNOWN;
    c = false;
    indicators = 0x00;
    tsi = TSI::NO_TSI;
    tsf = TSF::NO_TSF;
    pkt_count = 0;
    packet_size = 0;

    return;
}

vita_header::vita_header(const vita_header& rhs) { copy(rhs); }

vita_header::~vita_header() { return; }

void vita_header::copy(const vita_header& rhs)
{
    type = rhs.type;
    c = rhs.c;
    indicators = rhs.indicators;
    tsi = rhs.tsi;
    tsf = rhs.tsf;
    pkt_count = rhs.pkt_count;
    packet_size = rhs.packet_size;

    return;
}


/**
 * Unpacks a 32byte word.
 *
 * @param word_in - incoming word
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int vita_header::unpack(uint32_t word_in)
{
    int ans = 0;

    packet_size = word_in & 0x0000ffff;

    pkt_count = bit_extract(word_in, 16, 4);
    tsf = (TSF)bit_extract(word_in, 20, 2);
    tsi = (TSI)bit_extract(word_in, 22, 2);
    indicators = bit_extract(word_in, 24, 3);
    c = bit_extract(word_in, 27, 1);
    type = (PacketType)bit_extract(word_in, 28, 4);

    ans = isSane();


    return ans;
} // end unpack

/**
 * Determines if the head is sane
 *
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int vita_header::isSane(void)
{
    if (type != PacketType::SIGNAL_DATA && type != PacketType::SIGNAL_DATA_ID &&
        type != PacketType::EXT_DATA && type != PacketType::EXT_DATA_ID &&
        type != PacketType::CONTEXT && type != PacketType::EXT_CONTEX &&
        type != PacketType::COMMAND && type != PacketType::EXT_COMMAND) {
        // printf("Detected bad PacketType\n");
        return 0;
    }

    if (packet_size < calcMinSize()) {
        // printf("Detected too small condition %d %d\n", packet_size, calcMinSize() );
        return 0;
    }

    return 1;
}

/**
 * Calculates a minimum packet size
 *
 * @return int - size value
 */
int vita_header::calcMinSize(void)
{
    int ans = 1;

    if (tsi != TSI::NO_TSI) {
        ans += 1;
    }
    if (tsf != TSF::NO_TSF) {
        ans += 2;
    }
    if (c) {
        ans += 2;
    }

    return ans;
}

/**
 * Packs
 *
 * @return uint32_t
 */
uint32_t vita_header::pack(void) { return 0; }

/**
 * Debug tool, print out to STDOUT
 */
void vita_header::debug_print(void)
{
    printf("vita_header\n");
    printf("\tType: %d\n", type);
    printf("\tC ID: %d\n", c);
    printf("\tIndicators: %x\n", indicators);
    printf("\tTSI: %u\n", tsi);
    printf("\tTSF: %u\n", tsf);
    printf("\tCount: %u\n", pkt_count);
    printf("\tSize: %u\n", packet_size);

    return;
}

/**
 * extracts a bit field from a word
 *
 * @param in - input word
 * @param offset - bit shift offset to start of field
 * @param len - bit length of field
 * @return uint32_t - extracted value
 */
uint32_t vita_header::bit_extract(uint32_t in, int offset, int len)
{
    uint32_t ans = 0;
    uint32_t mask;

    mask = (1 << len) - 1;
    in = in >> offset;

    ans = in & mask;

    return ans;
} // end bit_extract

bool vita_header::isC() const { return c; }

uint8_t vita_header::getIndicators() const { return indicators; }

uint16_t vita_header::getPacketSize() const { return packet_size; }

uint8_t vita_header::getPktCount() const { return pkt_count; }

TSF vita_header::getTsf() const { return tsf; }

TSI vita_header::getTsi() const { return tsi; }

PacketType vita_header::getType() const { return type; }
void vita_header::setType(PacketType type) { this->type = type; }


} // namespace sandia_utils
} /* namespace gr */
