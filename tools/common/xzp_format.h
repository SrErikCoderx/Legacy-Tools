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

        // numEntries at 0x14
        stream.seekg(0x14, std::ios::beg);
        uint16_t numEntries = es.readU16();

        entries.clear();
        for (uint32_t i = 0; i < numEntries; ++i) {
            XzpEntry entry;
            entry.size = es.readU32();
            entry.offset = es.readU32();

            uint16_t nameLen = 0;
            stream.read(reinterpret_cast<char*>(&nameLen), 2);
            // XUIZ nameLen is LE

            if (nameLen > 1024 || nameLen == 0) break;

            entry.name = es.readUTF16LE(nameLen);
            entries.push_back(entry);
        }
        return true;
    }
};

} // namespace DLC

#endif // XZP_FORMAT_H
