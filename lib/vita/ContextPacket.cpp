/*
 * ContextPacket.cpp
 *
 *  Created on: Aug 4, 2020
 *      Author: sandia
 */

#include "ContextPacket.h"
#include "cifs_defs.h"
#include <stdio.h>

namespace gr {
namespace sandia_utils {

ContextPacket::ContextPacket()
{
    cif[0] = 0;
    cif[1] = 0;
    cif[2] = 0;
    cif[3] = 0;
    cif[4] = 0;
    cif[5] = 0;
    cif[6] = 0;
    cif[7] = 0;

    change = false;

    return;
}

ContextPacket::~ContextPacket()
{
    values.clear();

    return;
}

/**
 * Unpacks a 32byte word.
 *
 * @param word_in - incoming word
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int ContextPacket::unpack(uint32_t word_in)
{
    int ans = 0;

    ans = VRTPacket::unpack(word_in);

    if (ans == 1) {
        context_decode();
    }

    return ans;
}

int ContextPacket::unpack(const VRTPacket& rhs)
{
    int ans = 1;

    VRTPacket::copy(rhs);
    context_decode();

    return ans;
}


/**
 * Debug tool, print out to STDOUT
 */
void ContextPacket::debug_print(void)
{
    VRTPacket::debug_print();

    printf("ContextPacket\n");

    printf("\tCIF 0 %8x\n", cif[0]);
    for (int i = 1; i <= 7; i++) {
        if (cif[0] & (0x01 << i)) {
            printf("\tCIF %d %8x\n", i, cif[i]);
        }
    } // end for(i
    printf("\tChange: %d\n", change);

    for (size_t i = 0; i < values.size(); i++) {
        printf("\tCIF Value %d %lf\n", values.at(i)->getId(), values.at(i)->getValue());
    }

    return;
}

/**
 * Resets packet to unpack another one
 *
 */
void ContextPacket::reset(void)
{
    VRTPacket::reset();

    cif[0] = 0;
    cif[1] = 0;
    cif[2] = 0;
    cif[3] = 0;
    cif[4] = 0;
    cif[5] = 0;
    cif[6] = 0;
    cif[7] = 0;
    change = false;

    values.clear();

    return;
}

/**
 * Returns all decoded values
 *
 * @return
 */
std::vector<CifValue*>* ContextPacket::getValues(void) { return &values; }

/**
 * Decodes Payload block into context information
 */
void ContextPacket::context_decode(void)
{
    if (payload.size() > 1) {
        // grab CIF fields first
        grab_cifs();

        unpack_cifs();
    } else {
        // payload isn't big enough
    }

    return;
}


/**
 * grabs CIF[0-7] from payload
 */
void ContextPacket::grab_cifs(void)
{
    int i;

    cif[0] = payload_pop();

    for (i = 1; i <= 7; i++) {
        if (cif[0] & (0x01 << i)) {
            cif[i] = payload_pop();
        } else {
            cif[i] = 0;
        }

    } // end for(i

    return;
} // end grab_cifs

/**
 * Unpacks cifs based off cif[]
 */
void ContextPacket::unpack_cifs(void)
{
    int i, j;
    int stat = 1;

    for (i = 0; i < 8 && stat; i++) // loop over cif[]
    {
        for (j = 31; j >= 0 && stat; j--) {
            if (cif[i] & (0x01 << j)) {
                // printf("Detected CIF mask %d %d\n", i, j );
                if (i == 0 && j == 31) {
                    // special change bit
                    change = true;
                } else {
                    stat = decode_cif(i, j);
                    if (stat == 0) {
                        printf(
                            "ContextPacket::unpack_cifs() decode error on %d %d\n", i, j);
                    }
                }
            } // end if(
        }     // end for(j

    } // end for(i

    return;
}

/**
 * Helper method to pop the front of the payload
 *
 * @return uint32_t
 */
uint32_t ContextPacket::payload_pop(void)
{
    uint32_t ans = 0;

    ans = payload.at(0);
    payload.erase(payload.begin());

    return ans;
}

bool ContextPacket::isChange() const { return change; }

void ContextPacket::setChange(bool change) { this->change = change; }

/**
 * Decodes a specific CIF value
 *
 * @param idx - CIF index
 * @param bit - CIF bit position
 * @return int - 0 on error, 1 on success
 */
int ContextPacket::decode_cif(int idx, int bit)
{
    int ans = 0;
    uint64_t raw;
    //double rawf;
    //int stat;

    struct cif_decode_ring_type* def = NULL;

    def = cif_find_def(idx, bit);
    if (def) {
        raw = 0;
        for (int i = 0; i < def->word_len; i++) {
            raw = raw << 32;
            raw |= payload_pop();
        } // end for(i

        // printf(" CIF raw 0x%16lx\n", raw );

        if (def->double_type) {
            CifValue* cv = new CifValue(def->idx, def->bit, def->radix_pt, raw);
            values.push_back(cv);
        }

        ans = 1;
    } // end if( def

    return ans;
}
} /* namespace sandia_utils */
} /* namespace gr */
