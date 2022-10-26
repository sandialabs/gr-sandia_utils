/*
 * cifs_defs.h
 *
 *  Created on: Aug 5, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_CIFS_DEFS_H_
#define LIB_VITA_CIFS_DEFS_H_

#include <string>

struct cif_decode_ring_type {
    int idx;
    int bit;
    std::string cif_name;
    int word_len;
    bool double_type;
    int radix_pt;
};

/**
 * Finds a cif decode ring definintion in the decoder ring
 *
 * @param idx - CIF index
 * @param bit - CIF bit position
 * @return struct cif_decode_ring_type - found entry or NULL on not found
 */
struct cif_decode_ring_type* cif_find_def(int idx, int bit);

extern struct cif_decode_ring_type cif_decode_ring[];

#endif /* LIB_VITA_CIFS_DEFS_H_ */
