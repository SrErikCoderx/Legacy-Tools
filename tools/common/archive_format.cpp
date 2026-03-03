#include "archive_format.h"
#include <iostream>

namespace DLC {

bool Archive::isArchive(std::iostream& stream) {
    auto pos = stream.tellg();
    EndianStream es(stream);
    es.setBigEndian(true);
    uint32_t count = es.readU32();

    // Heuristic: file count should be reasonable and first entry name length should be sane
    bool result = (count > 0 && count < 100000);
    if (result) {
        uint16_t nameLen = es.readU16();
        if (nameLen == 0 || nameLen > 1024) result = false;
    }

    stream.seekg(pos);
    return result;
}

bool Archive::load(std::iostream& stream) {
    entries.clear();
    EndianStream es(stream);
    es.setBigEndian(true);

    uint32_t count = es.readU32();
    for (uint32_t i = 0; i < count; ++i) {
        ArchiveEntry entry;
        uint16_t nameLen = es.readU16();
        entry.name = es.readString(nameLen);
        entry.size = es.readU32();
        entry.offset = es.readU32();
        entries.push_back(entry);
    }
    return true;
}

bool Archive::save(std::iostream& stream, const std::vector<std::string>& fileNames, const std::vector<std::vector<uint8_t>>& fileData) {
    EndianStream es(stream);
    es.setBigEndian(true);

    uint32_t count = static_cast<uint32_t>(fileNames.size());
    es.writeU32(count);

    uint32_t currentOffset = 4; // Start with count
    for (const auto& name : fileNames) {
        currentOffset += 2 + static_cast<uint32_t>(name.size()) + 4 + 4;
    }

    // Write headers
    for (size_t i = 0; i < fileNames.size(); ++i) {
        es.writeU16(static_cast<uint16_t>(fileNames[i].size()));
        es.writeString(fileNames[i]);
        es.writeU32(static_cast<uint32_t>(fileData[i].size()));
        es.writeU32(currentOffset);
        currentOffset += static_cast<uint32_t>(fileData[i].size());
    }

    // Write data
    for (const auto& data : fileData) {
        stream.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    return true;
}

} // namespace DLC
