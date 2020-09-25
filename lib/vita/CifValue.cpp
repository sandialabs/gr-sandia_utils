/*
 * CifValue.cpp
 *
 *  Created on: Aug 6, 2020
 *      Author: rfec
 */
#include "CifValue.h"
#include <stdio.h>

namespace gr {
namespace sandia_utils {

CifValue::CifValue()
{
    id = 0;
    value = 0;
}
CifValue::CifValue(int idx, int bit, int radix, uint64_t raw)
{
    id = 100 * idx + bit;
    value = radix_decode(raw, radix);

    return;
}
CifValue::CifValue(int _id, double _val)
{
    id = _id;
    value = _val;
}

int CifValue::getId() const { return id; }

void CifValue::setId(int id) { this->id = id; }

double CifValue::getValue() const { return value; }

void CifValue::setValue(double value) { this->value = value; }

CifValue::~CifValue() { return; }


/**
 * Decodes VITA values that have integer & fractional points
 *
 * @param in - raw input value
 * @param radixPt - bit position of radix point
 * @return double - double value
 */
double CifValue::radix_decode(uint64_t in, int radixPt)
{
    double ans = 0;
    uint64_t whole, frac, mask = 0;
    uint64_t div;

    div = 1 << radixPt;

    // pull out the whole and frac parts
    whole = in >> radixPt;

    for (int i = 0; i < radixPt; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    frac = in & mask;

    // printf("radix_decode() %lx %lx %lx %lu\n", whole, mask, frac, div);

    ans = whole;
    ans += (double)frac / (double)div;

    return ans;
} // end radix_decode

} /* namespace sandia_utils */
} /* namespace gr */
