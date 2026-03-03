#ifndef DLC_UTILS_H
#define DLC_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace DLC {

inline std::string utf16le_to_utf8(const std::u16string& utf16) {
    std::string utf8;
    for (char16_t cp : utf16) {
        if (cp < 0x80) {
            utf8 += static_cast<char>(cp);
        } else if (cp < 0x800) {
            utf8 += static_cast<char>(0xC0 | (cp >> 6));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            utf8 += static_cast<char>(0xE0 | (cp >> 12));
            utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }
    return utf8;
}

inline std::u16string utf8_to_utf16le(const std::string& utf8) {
    std::u16string utf16;
    for (size_t i = 0; i < utf8.size(); ) {
        uint32_t cp = 0;
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        size_t len = 0;
        if (c < 0x80) { cp = c; len = 1; }
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 2; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
        else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }

        if (i + len > utf8.size()) break;
        for (size_t j = 1; j < len; ++j) {
            cp = (cp << 6) | (static_cast<unsigned char>(utf8[i + j]) & 0x3F);
        }
        i += len;

        if (cp < 0x10000) {
            utf16 += static_cast<char16_t>(cp);
        } else {
            // Surrogate pairs
            cp -= 0x10000;
            utf16 += static_cast<char16_t>(0xD800 | (cp >> 10));
            utf16 += static_cast<char16_t>(0xDC00 | (cp & 0x3FF));
        }
    }
    return utf16;
}

} // namespace DLC

#endif // DLC_UTILS_H
