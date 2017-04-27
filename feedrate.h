#ifndef FEEDRATE_H
#define FEEDRATE_H

#ifdef __cplusplus
extern "C" {
#endif

// Input / Output value types
enum {
    tag_CutterDiameterAtDepth = 'Dcap',
    tag_FeedPerTooth = '  fz',
    tag_CutterTeeth = '  Zn',
    tag_EffectiveCutterTeeth = 'ctth',
    tag_Feedrate = 'feed',
    tag_SpindleSpeed = 'sspd',
};

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
