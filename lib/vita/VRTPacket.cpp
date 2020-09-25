/*
 * VRTPacket.cpp
 *
 *  Created on: Jul 22, 2020
 *      Author: rfec
 */

#include "VRTPacket.h"
#include <stdio.h>
#include <time.h>

namespace gr {
namespace sandia_utils {

VRTPacket::VRTPacket()
{
    stream_id = 0;
    ts_epoch = 0;
    ts_frac = 0;
    word_cnt = 0;

    state = BLANK;

    return;
} // end constructor

VRTPacket::VRTPacket(const VRTPacket& rhs) { copy(rhs); } // end copy constructor

VRTPacket::~VRTPacket()
{
    payload.clear();

    return;
} // end deconstructor


void VRTPacket::copy(const VRTPacket& rhs)
{
    stream_id = rhs.stream_id;
    class_id = rhs.class_id;
    ts_epoch = rhs.ts_epoch;
    ts_frac = rhs.ts_frac;
    word_cnt = rhs.word_cnt;
    state = rhs.state;
    payload = rhs.payload;

    header.copy(rhs.header);
    class_id.copy(rhs.class_id);
    trailer.copy(rhs.trailer);

    return;
}

/**
 * Unpacks a 32byte word.
 *
 * @param word_in - incoming word
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int VRTPacket::unpack(uint32_t word_in)
{
    int ans = 0;
    int stat;

    switch (state) {
    case (BLANK): {
        stat = header.unpack(word_in);
        if (stat == 1) {
            // printf("Got header, early print ");
            // header.debug_print();

            state = GOT_HEADER;
            payload.clear();
            word_cnt = 1;

            ans = 2;
        } else {
            // printf( "VRTPacket::unpack() bad eyes\n" );
        }

        break;
    }
    case (GOT_HEADER): {
        state = GOT_STREAM_ID;
        if (header.getType() == PacketType::SIGNAL_DATA ||
            header.getType() == PacketType::EXT_DATA) {
            // recurse
            ans = unpack(word_in);
        } else {

            stream_id = word_in;
            word_cnt++;
            ans = 2;
        }

        break;
    }
    case (GOT_STREAM_ID): {
        if (header.isC()) {
            class_id.unpack(word_in);

            state = GOT_C1;
            word_cnt++;

            ans = 2;
        } else {
            // recruse
            state = GOT_C2;
            ans = unpack(word_in);
        }
        break;
    }
    case (GOT_C1): {
        class_id.unpack(word_in);

        state = GOT_C2;
        word_cnt++;

        ans = 2;

        break;
    }
    case (GOT_C2): {
        state = GOT_EPOCH;
        if (header.getTsi() != TSI::NO_TSI) {
            ts_epoch = word_in;
            word_cnt++;
            ans = 2;
        } else {
            // recurse
            ans = unpack(word_in);
        }

        break;
    }
    case (GOT_EPOCH): {
        if (header.getTsf() != TSF::NO_TSF) {
            ts_frac = word_in;
            ts_frac = ts_frac << 32;
            state = GOT_F1;
            word_cnt++;

            ans = 2;

        } else {
            state = EATTING;
            ans = unpack(word_in);
        }

        break;
    }
    case (GOT_F1): {
        ts_frac |= word_in;
        state = EATTING;
        word_cnt++;

        // check for 0 payload
        if (word_cnt == header.getPacketSize()) {
            state = DONE;
            ans = 1;
        } else {
            ans = 2;
        }

        break;
    }

    case (EATTING): {
        int trailer_sz = 0;

        // figure out if we have a trailer
        if (header.getType() == PacketType::SIGNAL_DATA ||
            header.getType() == PacketType::SIGNAL_DATA_ID) {
            if (header.getIndicators() & 0x04) {
                trailer_sz = 1;
            }
        }

        // do we need more data?
        if (header.getPacketSize() - trailer_sz - word_cnt > 0) {
            // eat payload data!
            payload.push_back(word_in);
            word_cnt++;

            // what next
            if (header.getPacketSize() - trailer_sz - word_cnt > 0) {
                // more data still to eat
                ans = 2;
            } else {
                // ate all the data
                if (trailer_sz) {
                    state = GOT_PAYLOAD;
                    ans = 2;
                } else {
                    // DONE!
                    state = DONE;
                    ans = 1;
                }
            }

        } else {
            // error getting here? Fake it done
            // TODO make this better
            // printf( "VRTPacket::unpack() bad fish\n" );
            state = BLANK;
            ans = 0;
        }

        break;
    }
    case (GOT_PAYLOAD): {
        trailer.unpack(word_in);
        state = DONE;
        ans = 1;

        break;
    }
    case (DONE): {
        break;
    }
    default: {
        // reset
        printf("VRTPacket::unpack() bad squirel\n");
        state = BLANK;
        break;
    }

    } // end switch

    return ans;
} // end unpack

/**
 * Debug tool, print out to STDOUT
 */
void VRTPacket::debug_print(void)
{
    printf("VRTPacket\n");

    header.debug_print();

    if (header.getType() != PacketType::SIGNAL_DATA &&
        header.getType() != PacketType::EXT_DATA) {
        printf("\tStream ID: %u\n", stream_id);
    }

    if (header.isC()) {
        // printf( "\tClass ID: %lu\n", class_id );
        class_id.debug_print();
    }

    if (header.getTsi() != TSI::NO_TSI) {
        if (header.getTsi() == TSI::UTC) {
            time_t temp;
            temp = ts_epoch;
            printf("\tEpoch: %u\t%s", ts_epoch, asctime(gmtime((time_t*)&temp)));
        } else {
            printf("\tEpoch: %u\n", ts_epoch);
        }
    }
    if (header.getTsf() != TSF::NO_TSF) {
        if (header.getTsf() == TSF::REAL_TIME) {
            printf("\tFrac: %lf\n", (double)ts_frac * 1e-12);
        } else {
            printf("\tFrac: %lu\n", ts_frac);
        }
    }

    printf("\tPayload Size: %lu\n", payload.size());

    if (header.getType() == PacketType::SIGNAL_DATA ||
        header.getType() == PacketType::SIGNAL_DATA_ID) {
        if (header.getIndicators() & 0x04) {
            trailer.debug_print();
        }
    }

    return;
} // end debug_print

/**
 * Resets packet to unpack another one
 *
 */
void VRTPacket::reset(void)
{
    state = BLANK;
    word_cnt = 0;
    payload.clear();

    class_id.reset();

    return;
}


/**
 * Returns the type of packet this is
 *
 * @return PacketType
 */
PacketType VRTPacket::getType(void) { return header.getType(); }

// uint64_t VRTPacket::getClassId() const
const vita_class_id& VRTPacket::getClassId() const { return class_id; }


vita_header* VRTPacket::getHeader() { return &header; }

uint32_t VRTPacket::getStreamId() const { return stream_id; }

void VRTPacket::setStreamId(uint32_t streamId) { stream_id = streamId; }

const vita_trailer& VRTPacket::getTrailer() const { return trailer; }

uint32_t VRTPacket::getTsEpoch() const { return ts_epoch; }

void VRTPacket::setTsEpoch(uint32_t tsEpoch) { ts_epoch = tsEpoch; }

uint64_t VRTPacket::getTsFrac() const { return ts_frac; }

void VRTPacket::setTsFrac(uint64_t tsFrac) { ts_frac = tsFrac; }

/**
 * Returns pointer to payload vectory
 *
 * @return *vector<uint32_t>
 */
std::vector<uint32_t>* VRTPacket::getPayload(void) { return &payload; }

} // namespace sandia_utils
} /* namespace gr */
