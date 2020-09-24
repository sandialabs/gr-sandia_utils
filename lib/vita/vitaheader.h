/*
 * vitaheader.h
 *
 *  Created on: Jul 21, 2020
 *      Author: rfec
 */

#ifndef LIB_VITA_VITAHEADER_H_
#define LIB_VITA_VITAHEADER_H_


#include <stdint.h>

#include <sandia_utils/api.h>

#include "vitabaseabs.h"
#include "vitatypes.h"

namespace gr {
namespace sandia_utils {

/**
 * Represents the very first word of all VITA 49 packets
 */
class SANDIA_UTILS_API vita_header : public vita_base_abs
{
private:
    PacketType type;
    bool c;
    uint8_t indicators;
    TSI tsi;
    TSF tsf;
    uint8_t pkt_count;
    uint16_t packet_size;

public:
    vita_header();
    vita_header(const vita_header& rhs);
    virtual ~vita_header();

    void copy(const vita_header& rhs);

    /**
     * Unpacks a 32byte word.
     *
     * @param word_in - incoming word
     * @return int - 0 = fail, 1 = done, 2 = need more
     */
    virtual int unpack(uint32_t word_in);

    /**
     * Packs
     *
     * @return uint32_t
     */
    virtual uint32_t pack(void);

    /**
     * Debug tool, print out to STDOUT
     */
    virtual void debug_print(void);


    bool isC() const;
    uint8_t getIndicators() const;
    uint16_t getPacketSize() const;
    uint8_t getPktCount() const;
    TSF getTsf() const;
    TSI getTsi() const;
    PacketType getType() const;
    void setType(PacketType type);

private:
    /**
     * extracts a bit field from a word
     *
     * @param in - input word
     * @param offset - bit shift offset to start of field
     * @param len - bit length of field
     * @return uint32_t - extracted value
     */
    uint32_t bit_extract(uint32_t in, int offset, int len);

    /**
     * Determines if the head is sane
     *
     * @return int - 0 = fail, 1 = done, 2 = need more
     */
    int isSane(void);

    /**
     * Calculates a minimum packet size
     *
     * @return int - size value
     */
    int calcMinSize(void);
};
// end class vita_header

} // namespace sandia_utils
} /* namespace gr */

#endif /* LIB_VITA_VITAHEADER_H_ */
