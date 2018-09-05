#include "utils.h"
#include "taginfo.h"

std::string fcc(uint32_t c) {
    if (tag_name(c))
        return tag_name(c);
    return {
        static_cast<char>((c & 0xFF000000ul) >> 24),
        static_cast<char>((c & 0xFF0000ul) >> 16),
        static_cast<char>((c & 0xFF00ul) >> 8),
        static_cast<char>(c & 0xFFul)
    };
}
