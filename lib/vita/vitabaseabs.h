/*
 * vitabaseabs.h
 *
 *  Created on: Jul 20, 2020
 *      Author: rfec
 */

#ifndef LIB_VITA_VITABASEABS_H_
#define LIB_VITA_VITABASEABS_H_

#include <stdint.h>

namespace gr {
namespace sandia_utils {

/**
 * Base Abstract class for common things
 */
class vita_base_abs
{
public:
    /**
     * Constructor
     */
    vita_base_abs();

    /**
     * Deconstructor
     */
    virtual ~vita_base_abs();

    /**
     * Unpacks a 32byte word.
     *
     * @param word_in - incoming word
     * @return int - 0 = fail, 1 = done, 2 = need more
     */
    virtual int unpack(uint32_t word_in) { return 0; }

    /**
     * Packs
     *
     * @return uint32_t
     */
    virtual uint32_t pack(void) { return 0; }

    /**
     * Debug tool, print out to STDOUT
     */
    virtual void debug_print(void) = 0;

}; // end class vita_base_abs

} // namespace sandia_utils
} /* namespace gr */

#endif /* LIB_VITA_VITABASEABS_H_ */
