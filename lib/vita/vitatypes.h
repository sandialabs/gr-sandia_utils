/*
 * vitatypes.h
 *
 *  Created on: Jul 21, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_VITATYPES_H_
#define LIB_VITA_VITATYPES_H_

namespace gr {
namespace sandia_utils {

enum PacketType {
    SIGNAL_DATA = 0x00,
    SIGNAL_DATA_ID = 0x01,
    EXT_DATA = 0x02,
    EXT_DATA_ID = 0x03,
    CONTEXT = 0x04,
    EXT_CONTEX = 0x05,
    COMMAND = 0x06,
    EXT_COMMAND = 0x07,

    UNKNOWN = 0xff
};

enum TSI { NO_TSI = 0, UTC = 1, GPS = 2, OTHER = 3 };

enum TSF { NO_TSF = 0, SAMPLE_COUNT = 1, REAL_TIME = 2, FREE_COUNT = 3 };

/**
 * Sample Frame state indicators, stored in Trailer
 * SF_NONE - this is not part of a Sample Frame
 * SF_FIRST - first VRT Packet of the Sample Frame
 * SF_MID - Middle VRT Packet of a Sample Frame
 * SF_END - Ending VRT Packet of a Sample Frame
 */
enum SampleFrameInd { SF_NONE = 0, SF_FIRST = 1, SF_MID = 2, SF_END = 3 };
} // namespace sandia_utils
} // namespace gr

#endif /* LIB_VITA_VITATYPES_H_ */
