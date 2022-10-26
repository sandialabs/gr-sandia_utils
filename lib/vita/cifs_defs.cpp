/*
 * cifs_defs.cpp
 *
 *  Created on: Aug 5, 2020
 *      Author: sandia
 */

#include "cifs_defs.h"

struct cif_decode_ring_type cif_decode_ring[] = { { 0, 29, "Bandwidth", 2, true, 20 },
                                                  { 0, 28, "IF Ref", 2, true, 20 },
                                                  { 0, 27, "RF Ref", 2, true, 20 },
                                                  { 0, 21, "Sample Rate", 2, true, 20 } };


/**
 * Finds a cif decode ring definintion in the decoder ring
 *
 * @param idx - CIF index
 * @param bit - CIF bit position
 * @return struct cif_decode_ring_type - found entry or NULL on not found
 */
struct cif_decode_ring_type* cif_find_def(int idx, int bit)
{
    struct cif_decode_ring_type* ans = NULL;
    int sz;

    sz = sizeof(cif_decode_ring) / sizeof(cif_decode_ring_type);
    // printf("cif_find_def() %d entries in global decoder ring\n", sz );

    for (int i = 0; i < sz; i++) {
        if (cif_decode_ring[i].idx == idx) {
            if (cif_decode_ring[i].bit == bit) {
                ans = &cif_decode_ring[i];
                break;
            }
        }
    } // end for(i

    return ans;
} // end cif_find_def
