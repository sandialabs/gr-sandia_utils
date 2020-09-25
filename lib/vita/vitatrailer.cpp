/*
 * vitatrailer.cpp
 *
 *  Created on: Jul 20, 2020
 *      Author: rfec
 */

#include <stdio.h>
#include <string.h>

#include "vitatrailer.h"

namespace gr {
namespace sandia_utils {

vita_trailer::vita_trailer()
{
    e = false;
    context_count = 0;

    memset(enables, 0x00, sizeof(enables));
    memset(indicators, 0x00, sizeof(enables));

    return;
}
vita_trailer::vita_trailer(const vita_trailer& rhs) { copy(rhs); }

vita_trailer::~vita_trailer() { return; }

void vita_trailer::copy(const vita_trailer& rhs)
{
    e = rhs.e;
    context_count = rhs.context_count;

    memcpy(enables, rhs.enables, sizeof(enables));
    memcpy(indicators, rhs.indicators, sizeof(indicators));

    return;
}

const uint8_t vita_trailer::getEnable(int idx) const
{
    uint8_t ans = 0;

    if (0 <= idx < 12) {
        ans = enables[idx];
    }

    return ans;
}

const uint8_t vita_trailer::getIndicators(int idx) const
{
    uint8_t ans = 0;

    if (0 <= idx < 12) {
        ans = indicators[idx];
    }

    return ans;
}

/**
 * Unpacks a 32byte word.
 *
 * @param word_in - incoming word
 * @return int - 0 = fail, 1 = done, 2 = need more
 */
int vita_trailer::unpack(uint32_t word_in)
{
    int ans = 0;
    int i;

    memset(enables, 0x00, sizeof(enables));
    memset(indicators, 0x00, sizeof(enables));

    e = (word_in & 0x00000080) == 0x80;
    context_count = word_in & 0x7f;

    for (i = 31; i > 19; i--) {
        if (word_in & (0x01 << i)) {
            enables[31 - i] = 1;
        }
    }

    for (i = 19; i > 7; i--) {
        if (word_in & (0x01 << i)) {
            indicators[19 - i] = 1;
        }
    }

    return ans;
} // end unpack


/**
 * Debug tool, print out to STDOUT
 */
void vita_trailer::debug_print(void)
{
    int i;

    printf("vita_trailer\n");
    for (i = 0; i < 12; i++) {
        printf("\tIndicator %d\tEn:%d\tInd:%d\n", i, enables[i], indicators[i]);
    }

    printf("\tE:%d\n", e);
    printf("\tContext Count: %d\n", context_count);

    return;
} // end debug_print

/**
 * Return the SampleFrameIndicator
 *
 * @return SampleFrameInd -
 */
const SampleFrameInd vita_trailer::getSampleFrameIndicator(void) const
{
    SampleFrameInd ans = SF_NONE;

    if (enables[8] && enables[9]) {
        // bits are enabled
        ans = (SampleFrameInd)((indicators[8] << 1) | indicators[9]);
    }

    return ans;
}

} // namespace sandia_utils
} /* namespace gr */
