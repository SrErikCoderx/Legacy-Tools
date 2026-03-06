#ifndef COL_FORMAT_H
#define COL_FORMAT_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include "io_helpers.h"

namespace DLC {

struct ColourEntry {
    std::string name;
    uint32_t value;
};

class ColourTable {
public:
    uint32_t version;
    std::vector<ColourEntry> entries;

    bool load(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        version = es.readU32();
        uint32_t count = es.readU32();
        entries.clear();
        for (uint32_t i = 0; i < count; ++i) {
            ColourEntry entry;
            entry.name = es.readUTF();
            entry.value = es.readU32();
            entries.push_back(entry);
        }
        return true;
    }

    bool save(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);
        es.writeU32(version);
        es.writeU32(static_cast<uint32_t>(entries.size()));
        for (const auto& entry : entries) {
            es.writeUTF(entry.name);
            es.writeU32(entry.value);
        }
        return true;
    }
};

} // namespace DLC

#endif // COL_FORMAT_H
