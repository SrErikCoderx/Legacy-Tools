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
        uint32_t headerEntries = es.readU32(); // It says 1 in header, but maybe it is directory count?

        // Let's try to parse entries sequentially
        entries.clear();
        while (stream.tellg() < totalSize) {
            XzpEntry entry;
            entry.size = es.readU32();
            if (entry.size == 0 && entries.size() > 0) break; // Heuristic end

            entry.offset = es.readU32();
            uint16_t nameLen = es.readU16();
            if (nameLen > 1024) break;

            entry.name = es.readUTF16(nameLen);
            entries.push_back(entry);
        }
        return true;
    }
};

} // namespace DLC

#endif // XZP_FORMAT_H
