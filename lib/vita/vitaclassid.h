/*
 * vitaclassid.h
 *
 *  Created on: Aug 11, 2020
 *      Author: rfec
 */

#ifndef LIB_VITA_VITACLASSID_H_
#define LIB_VITA_VITACLASSID_H_

#include "vitabaseabs.h"
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

class SANDIA_UTILS_API vita_class_id : public vita_base_abs
{
private:
    uint8_t pad_bit_count;
    uint32_t oui;
    uint16_t info_class_code;
    uint16_t packet_class_code;
    int unpackState;

public:
    vita_class_id();
    vita_class_id(const vita_class_id& rhs);
    virtual ~vita_class_id();

    void copy(const vita_class_id& rhs);

    /**
     * Resets all member variables to defaults
     */
    void reset(void);


    /**
     * Unpacks a 32byte word.
     *
     * @param word_in - incoming word
     * @return int - 0 = fail, 1 = done, 2 = need more
     */
    virtual int unpack(uint32_t word_in);

    /**
     * Debug tool, print out to STDOUT
     */
    virtual void debug_print(void);


    uint16_t getInfoClassCode() const;
    void setInfoClassCode(uint16_t infoClassCode);
    uint32_t getOui() const;
    void setOui(uint32_t oui);
    uint16_t getPacketClassCode() const;
    void setPacketClassCode(uint16_t packetClassCode);
    uint8_t getPadBitCount() const;
    void setPadBitCount(uint8_t padBitCount);
};
// end class vita_class_id

} // namespace sandia_utils
} /* namespace gr */

#endif /* LIB_VITA_VITACLASSID_H_ */
