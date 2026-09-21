#pragma once

#include <stdint.h>

/*@
    terminates \true;
    exits \false;
    assigns \nothing;

    ensures -1610612736 <= \result <= 1610612736;

    behavior is_less:
        assumes num < -1610612736;
        ensures \result == -1610612736;
    behavior in_range:
        assumes -1610612736 <= num <= 1610612736;
        ensures \result == num;
    behavior is_greater:
        assumes num > 1610612736;
        ensures \result == 1610612736;
    
    complete behaviors;
    disjoint behaviors;
*/
inline int32_t clamp_s(const int64_t num) {
    if (num < -1610612736) {
        return -1610612736;
    } else if (num > 1610612736) { // 12*(2^27) or 48*(2^25)
        return 1610612736;
    } else {
        return num;
    }
}


/*@
    terminates \true;
    exits \false;
    assigns \nothing;

    ensures -52776558133248 <= \result <= 52776558133248;

    behavior is_less:
        assumes num < -52776558133248;
        ensures \result == -52776558133248;
    behavior in_range:
        assumes -52776558133248 <= num <= 52776558133248;
        ensures \result == num;
    behavior is_greater:
        assumes num > 52776558133248;
        ensures \result == 52776558133248;
    
    complete behaviors;
    disjoint behaviors;
*/
inline int64_t clamp_w(const int64_t num) {
    if (num < -52776558133248) {
        return -52776558133248;
    } else if (num > 52776558133248) { // 48*(2^40)
        return 52776558133248;
    } else {
        return num;
    }
}


extern const int32_t stretch_lut[65537];


/*@
    requires \valid_read(stretch_lut + (0 .. 65536));
    
    terminates \true;
    exits \false;
    assigns \nothing;

    ensures \result == stretch_lut[num];
*/
inline int32_t stretch(const uint16_t num) {
    return stretch_lut[num];
}


/*@
    axiomatic Stretch_LUT_Is_Sorted {
        predicate is_monotonically_increasing(int32_t * array, integer len) = 
            \forall integer i, j; (0 <= i < j < len - 1) ==> (array[i] < array[j]);
        
        axiom stretch_lut_is_monotonically_increasing:
            is_monotonically_increasing((int32_t *) &(stretch_lut[0]), 65536);
    }
*/


/*@
    requires \valid_read(stretch_lut + (0 .. 65536));
    requires is_monotonically_increasing((int32_t *) &(stretch_lut[0]), 65537);
    requires stretch_lut[0] < stretch_lut[65536];
    
    terminates \true;
    exits \false;
    assigns \nothing;

    ensures 0 <= \result < 65536;
    
    behavior num_is_too_small:
        assumes num < stretch_lut[0];
        ensures \result == 0;
    
    behavior num_is_too_large:
        assumes num >= stretch_lut[65536];
        ensures \result == 65535;
    
    behavior num_is_in_range:
        assumes stretch_lut[0] <= num < stretch_lut[65536];
        ensures stretch_lut[\result] <= num < stretch_lut[\result + 1];
    
    complete behaviors;
    disjoint behaviors;
*/
inline uint16_t squash(const int32_t num) {
    //@ assert stretch_lut[0] < stretch_lut[65536];
    if (num < stretch_lut[0]) { return 0; }
    if (num >= stretch_lut[65536]) { return 65535; }

    //@ assert stretch_lut[0] <= num < stretch_lut[65536];

    uint32_t low = 0;
    uint32_t high = 65535;

    /*@
        loop invariant 0 <= low <= high < 65536;
        loop invariant stretch_lut[low] <= num < stretch_lut[high + 1];
        loop assigns low, high;
        loop variant high - low;
    */
    while (low < high) {
        //@ assert (high <= 65535) && (low >= 0) && (low < high);
        //@ assert (low + (high - low) / 2) < 65537;
        uint32_t mid = low + (high - low) / 2;
        
        if (stretch_lut[mid + 1] <= num) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    
    //@ assert 0 <= low < 65536;
    return (uint16_t)(low);
}