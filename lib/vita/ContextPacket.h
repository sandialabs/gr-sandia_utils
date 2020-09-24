/*
 * ContextPacket.h
 *
 *  Created on: Aug 4, 2020
 *      Author: rfec
 */

#ifndef LIB_VITA_CONTEXTPACKET_H_
#define LIB_VITA_CONTEXTPACKET_H_

#include "CifValue.h"
#include "VRTPacket.h"
#include <vector>

namespace gr {
namespace sandia_utils {

class SANDIA_UTILS_API ContextPacket : public VRTPacket
{
private:
    uint32_t cif[8];
    std::vector<CifValue*> values;
    bool change;

public:
    ContextPacket();
    virtual ~ContextPacket();

    /**
     * Unpacks a 32byte word.
     *
     * @param word_in - incoming word
     * @return int - 0 = fail, 1 = done, 2 = need more
     */
    virtual int unpack(uint32_t word_in);

    int unpack(const VRTPacket& rhs);

    /**
     * Debug tool, print out to STDOUT
     */
    virtual void debug_print(void);

    /**
     * Resets packet to unpack another one
     *
     */
    virtual void reset(void);

    /**
     * Returns all decoded values
     *
     * @return
     */
    std::vector<CifValue*>* getValues(void);

    bool isChange() const;
    void setChange(bool change);

private:
    /**
     * Decodes Payload block into context information
     */
    void context_decode(void);


    /**
     * grabs CIF[0-7] from payload
     */
    void grab_cifs(void);

    /**
     * Unpacks cifs based off cif[]
     */
    void unpack_cifs(void);

    /**
     * Helper method to pop the front of the payload
     *
     * @return uint32_t
     */
    uint32_t payload_pop(void);

    /**
     * Decodes a specific CIF value
     *
     * @param idx - CIF index
     * @param bit - CIF bit position
     * @return int - 0 on error, 1 on success
     */
    int decode_cif(int idx, int bit);
};

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* LIB_VITA_CONTEXTPACKET_H_ */
