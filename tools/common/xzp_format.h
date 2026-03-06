#ifndef XZP_FORMAT_H
#define XZP_FORMAT_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include "io_helpers.h"

namespace DLC {

struct XzpEntry {
    std::u16string name;
    uint32_t size;
    uint32_t offset;
};

class XzpTable {
public:
    uint32_t version;
    uint32_t totalSize;
    std::vector<XzpEntry> entries;

    bool load(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        std::string magic = es.readString(4);
        if (magic != "XUIZ") return false;

        version = es.readU32();
        totalSize = es.readU32();
        uint32_t numEntries = es.readU32();

        entries.clear();
        for (uint32_t i = 0; i < numEntries; ++i) {
            XzpEntry entry;
            entry.size = es.readU32();
            entry.offset = es.readU32();

            // Skip 6 bytes (often Hash + Unknown)
            stream.ignore(6);

            uint16_t nameLen;
            stream.read(reinterpret_cast<char*>(&nameLen), 2);
            // NameLen is often Little Endian in these files
            if (es.isBigEndian()) {
                if (nameLen > 0xFF) nameLen = ((nameLen & 0xFF) << 8) | (nameLen >> 8);
            }

            if (nameLen > 1024) break;

            std::u16string u16name;
            for(int j=0; j<nameLen; ++j) {
                uint16_t c;
                stream.read(reinterpret_cast<char*>(&c), 2);
                if ((c & 0xFF00) == 0 && (c & 0x00FF) != 0) {
                   // Likely LE
                } else if ((c & 0x00FF) == 0 && (c & 0xFF00) != 0) {
                   // Likely BE
                   c = (c >> 8) | (c << 8);
                }
                u16name += static_cast<char16_t>(c);
            }
            entry.name = u16name;
            entries.push_back(entry);
        }
        return true;
    }
};

} // namespace DLC

#endif // XZP_FORMAT_H
