#include "tag.h"
#include <stdexcept>

struct {
    uint32_t tag;
    const char* name;
} constexpr const tag_info[] = {
    { tag_CuttingSpeed, "CuttingSpeed" },
    { tag_FeedPerTooth, "FeedPerTooth" },
    { tag_CutterTeeth, "CutterTeeth" },
};

constexpr const char* tag_name(uint32_t tag) {
    for (auto& i : tag_info) {
        if (i.tag == tag)
            return i.name;
    }
    return nullptr;
}
