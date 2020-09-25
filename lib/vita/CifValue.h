/*
 * CifValue.h
 *
 *  Created on: Aug 6, 2020
 *      Author: rfec
 */

#ifndef LIB_VITA_CIFVALUE_H_
#define LIB_VITA_CIFVALUE_H_

#include <sandia_utils/api.h>
#include <stdint.h>

namespace gr {
namespace sandia_utils {

/**
 * Represents a single CifValue
 */
class SANDIA_UTILS_API CifValue
{
private:
    int id;
    double value;

public:
    CifValue();
    CifValue(int idx, int bit, int radix, uint64_t raw);
    CifValue(int _id, double _val);

    virtual ~CifValue();


    int getId() const;
    void setId(int id);
    double getValue() const;
    void setValue(double value);

private:
    /**
     * Decodes VITA values that have integer & fractional points
     *
     * @param in - raw input value
     * @param radixPt - bit position of radix point
     * @return double - double value
     */
    double radix_decode(uint64_t in, int radixPt = 20);
};
// end class CifValue

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* LIB_VITA_CIFVALUE_H_ */
