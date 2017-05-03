#ifndef FEEDRATE_H
#define FEEDRATE_H
#include "tag.h"

#ifdef __cplusplus
extern "C" {
#endif

// Input / Output tagged value
struct TaggedValue {
    unsigned tag;
    double value;
};

bool calculate(const TaggedValue* in, unsigned in_size, TaggedValue* out, unsigned out_size);

#ifdef __cplusplus
}
#endif

#endif
