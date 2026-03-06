#ifndef IO_HELPERS_H
#define IO_HELPERS_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

namespace DLC {

inline uint16_t swap16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

inline uint32_t swap32(uint32_t val) {
    return ((val << 24) & 0xff000000) |
           ((val << 8)  & 0x00ff0000) |
           ((val >> 8)  & 0x0000ff00) |
           ((val >> 24) & 0x000000ff);
}

class EndianStream {
public:
    EndianStream(std::iostream& s) : stream(s), bigEndian(true) {}

    void setBigEndian(bool be) { bigEndian = be; }
    bool isBigEndian() const { return bigEndian; }

    uint16_t readU16() {
        uint16_t val = 0;
        stream.read(reinterpret_cast<char*>(&val), 2);
        return bigEndian ? swap16(val) : val;
    }

    uint32_t readU32() {
        uint32_t val = 0;
        stream.read(reinterpret_cast<char*>(&val), 4);
        return bigEndian ? swap32(val) : val;
    }

    uint16_t readU16LE() {
        uint16_t val = 0;
        stream.read(reinterpret_cast<char*>(&val), 2);
        return val; // Simplified for LE hosts
    }

    uint32_t readU32LE() {
        uint32_t val = 0;
        stream.read(reinterpret_cast<char*>(&val), 4);
        return val;
    }

    void writeU16(uint16_t val) {
        uint16_t out = bigEndian ? swap16(val) : val;
        stream.write(reinterpret_cast<const char*>(&out), 2);
    }

    void writeU32(uint32_t val) {
        uint32_t out = bigEndian ? swap32(val) : val;
        stream.write(reinterpret_cast<const char*>(&out), 4);
    }

    std::string readString(size_t len) {
        if (len == 0) return "";
        std::vector<char> buf(len);
        stream.read(buf.data(), len);
        return std::string(buf.data(), len);
    }

    void writeString(const std::string& s) {
        stream.write(s.data(), s.size());
    }

    std::u16string readUTF16(size_t wchCount) {
        std::u16string res;
        res.reserve(wchCount);
        for (size_t i = 0; i < wchCount; ++i) {
            uint16_t val = 0;
            stream.read(reinterpret_cast<char*>(&val), 2);
            res += static_cast<char16_t>(bigEndian ? swap16(val) : val);
        }
        return res;
    }

    std::u16string readUTF16LE(size_t wchCount) {
        std::u16string res;
        res.reserve(wchCount);
        for (size_t i = 0; i < wchCount; ++i) {
            uint16_t val = 0;
            stream.read(reinterpret_cast<char*>(&val), 2);
            res += static_cast<char16_t>(val);
        }
        return res;
    }

    void writeUTF16(const std::u16string& s) {
        for (char16_t cp : s) {
            uint16_t val = static_cast<uint16_t>(cp);
            uint16_t out = bigEndian ? swap16(val) : val;
            stream.write(reinterpret_cast<const char*>(&out), 2);
        }
    }

    std::string readUTF() {
        uint16_t len = readU16();
        return readString(len);
    }

    void writeUTF(const std::string& s) {
        writeU16(static_cast<uint16_t>(s.size()));
        writeString(s);
    }

private:
    std::iostream& stream;
    bool bigEndian;
};

} // namespace DLC

#endif // IO_HELPERS_H
