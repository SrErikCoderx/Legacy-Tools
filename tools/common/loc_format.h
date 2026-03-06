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
    uint32_t version = 0;
    std::vector<std::string> strings;

    bool load(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        if (!stream.read(reinterpret_cast<char*>(&version), 4)) return false;
        version = swap32(version);

        strings.clear();
        while (!stream.eof()) {
            uint32_t count = 0;
            if (!stream.read(reinterpret_cast<char*>(&count), 4)) break;
            count = swap32(count);

            if (count == 0 || count > 100000) break;

            for (uint32_t i = 0; i < count; ++i) {
                uint16_t len = 0;
                if (!stream.read(reinterpret_cast<char*>(&len), 2)) break;
                len = swap16(len);

                std::string s = es.readString(len);
                strings.push_back(s);

                // Skip 4-byte ID/Hash
                stream.ignore(4);
            }
            // Check for more blocks
            stream.peek();
        }
        return true;
    }

    bool save(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        es.writeU32(version);
        es.writeU32(static_cast<uint32_t>(strings.size()));

        uint32_t id = 100;
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
