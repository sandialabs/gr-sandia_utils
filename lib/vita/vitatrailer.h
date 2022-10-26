/*
 * vitatrailer.h
 *
 *  Created on: Jul 20, 2020
 *      Author: sandia
 */

#ifndef LIB_VITA_VITATRAILER_H_
#define LIB_VITA_VITATRAILER_H_

#include "vitabaseabs.h"
#include "vitatypes.h"
#include <gnuradio/sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

class SANDIA_UTILS_API vita_trailer : public vita_base_abs
{
private:
    uint8_t enables[12];
    uint8_t indicators[12];
    int context_count;
    bool e;

public:
    vita_trailer();
    vita_trailer(const vita_trailer& rhs);
    virtual ~vita_trailer();

    void copy(const vita_trailer& rhs);

    int getContextCount() const { return context_count; }

    bool isE() const { return e; }

    const uint8_t getEnable(int idx) const;

    const uint8_t getIndicators(int idx) const;

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
     * Return the SampleFrameIndicator
     *
     * @return SampleFrameInd -
     */
    const SampleFrameInd getSampleFrameIndicator(void) const;
};
// end class vita_trailer

} // namespace sandia_utils
} /* namespace gr */

#endif /* LIB_VITA_VITATRAILER_H_ */
