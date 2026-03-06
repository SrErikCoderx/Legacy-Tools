#ifndef LOC_FORMAT_H
#define LOC_FORMAT_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include "io_helpers.h"

namespace DLC {

class LocTable {
public:
    uint32_t version;
    std::vector<std::string> strings;

    bool load(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        if (!stream.read(reinterpret_cast<char*>(&version), 4)) return false;
        version = swap32(version); // Assuming BE

        uint32_t count;
        if (!stream.read(reinterpret_cast<char*>(&count), 4)) return false;
        count = swap32(count);

        if (count > 100000) return false;

        std::vector<uint32_t> offsets(count);
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t off;
            stream.read(reinterpret_cast<char*>(&off), 4);
            offsets[i] = swap32(off);
        }

        strings.clear();
        for (uint32_t i = 0; i < count; ++i) {
            stream.seekg(offsets[i]);
            uint16_t len;
            stream.read(reinterpret_cast<char*>(&len), 2);
            len = swap16(len);
            strings.push_back(es.readString(len));
        }
        return true;
    }

    bool save(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        es.writeU32(version);
        es.writeU32(static_cast<uint32_t>(strings.size()));

        uint32_t currentOffset = 8 + (static_cast<uint32_t>(strings.size()) * 4);
        std::vector<uint32_t> offsets;
        for (const auto& s : strings) {
            offsets.push_back(currentOffset);
            currentOffset += 2 + static_cast<uint32_t>(s.size());
        }

        for (uint32_t off : offsets) {
            es.writeU32(off);
        }

        for (const auto& s : strings) {
            es.writeUTF(s);
        }
        return true;
    }
};

} // namespace DLC

#endif // LOC_FORMAT_H
