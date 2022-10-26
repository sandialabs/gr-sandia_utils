/*
 * VRTPacket.h
 *
 *  Created on: Jul 22, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_VRTPACKET_H_
#define LIB_VITA_VRTPACKET_H_

#include <stdint.h>
#include <vector>

#include "vitabaseabs.h"
#include "vitaclassid.h"
#include "vitaheader.h"
#include "vitatrailer.h"
#include <gnuradio/sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/**
 * This class represents all VRT Packets.
 *
 */
class SANDIA_UTILS_API VRTPacket : public vita_base_abs
{
protected:
    enum UnpackState {
        BLANK,
        GOT_HEADER,
        GOT_STREAM_ID,
        GOT_C1,
        GOT_C2,
        GOT_EPOCH,
        GOT_F1,
        EATTING,
        GOT_PAYLOAD,
        DONE
    };

    vita_header header;
    uint32_t stream_id;
    vita_class_id class_id;
    // uint64_t class_id;
    uint32_t ts_epoch;
    uint64_t ts_frac;
    std::vector<uint32_t> payload;
    vita_trailer trailer;

    UnpackState state;
    uint16_t word_cnt;

public:
    VRTPacket();
    VRTPacket(const VRTPacket& rhs);
    virtual ~VRTPacket();

    void copy(const VRTPacket& rhs);


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

    /**
     * Resets packet to unpack another one
     *
     */
    virtual void reset(void);


    /**
     * Returns the type of packet this is
     *
     * @return PacketType
     */
    PacketType getType(void);

    /**
     * Returns pointer to payload vectory
     *
     * @return *vector\<uint32_t\>
     */
    std::vector<uint32_t>* getPayload(void);


    // uint64_t getClassId() const;
    const vita_class_id& getClassId() const;
    // void setClassId( uint64_t classId );

    vita_header* getHeader();
    uint32_t getStreamId() const;
    void setStreamId(uint32_t streamId);
    const vita_trailer& getTrailer() const;
    uint32_t getTsEpoch() const;
    void setTsEpoch(uint32_t tsEpoch);
    uint64_t getTsFrac() const;
    void setTsFrac(uint64_t tsFrac);
};
// end class VRTPacket

} // namespace sandia_utils
} /* namespace gr */

#endif /* LIB_VITA_VRTPACKET_H_ */
