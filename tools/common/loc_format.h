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

        strings.clear();
        for (uint32_t i = 0; i < count; ++i) {
            uint16_t len;
            if (!stream.read(reinterpret_cast<char*>(&len), 2)) break;
            len = swap16(len);

            std::string s = es.readString(len);
            strings.push_back(s);

            // Skip 4-byte ID/Hash
            stream.ignore(4);
        }
        return true;
    }

    bool save(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        es.writeU32(version);
        es.writeU32(static_cast<uint32_t>(strings.size()));

        uint32_t id = 100; // Generic starting ID
        for (const auto& s : strings) {
            es.writeU16(static_cast<uint16_t>(s.size()));
            stream.write(s.data(), s.size());
            es.writeU32(id++);
        }
        return true;
    }
};

} // namespace DLC

#endif // LOC_FORMAT_H
