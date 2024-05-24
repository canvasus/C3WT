#pragma once

#define wraparound(amt, low, high)\
  ({\
    typeof(amt) _amt = (amt);\
    typeof(low) _low = (low);\
    typeof(high) _high = (high);\
    (_amt < _low) ? _high : ((_amt > _high) ? _low : _amt);\
     })


