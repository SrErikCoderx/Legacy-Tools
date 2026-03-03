#include "archive_format.h"
#include <iostream>

namespace DLC {

bool Archive::isArchive(std::iostream& stream) {
    auto pos = stream.tellg();
    EndianStream es(stream);
    es.setBigEndian(true);

    // Read count
    uint32_t count = es.readU32();
    if (count == 0 || count > 0x10000) { stream.seekg(pos); return false; }

    // Read first entry name length
    uint16_t nameLen = es.readU16();
    if (nameLen == 0 || nameLen > 256) { stream.seekg(pos); return false; }

    // Structured DLC starts with Version (2 or 3) and then global param count.
    // If it was structured DLC, the second U32 would be global param count.
    // In media.arc, the second field is a U16 name length followed by ASCII.

    stream.seekg(pos);
    return true;
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

    uint32_t headerSize = 4;
    for (const auto& name : fileNames) {
        headerSize += 2 + static_cast<uint32_t>(name.size()) + 4 + 4;
    }

    uint32_t currentOffset = headerSize;
    for (size_t i = 0; i < fileNames.size(); ++i) {
        es.writeU16(static_cast<uint16_t>(fileNames[i].size()));
        es.writeString(fileNames[i]);
        es.writeU32(static_cast<uint32_t>(fileData[i].size()));
        es.writeU32(currentOffset);
        currentOffset += static_cast<uint32_t>(fileData[i].size());
    }

    for (const auto& data : fileData) {
        stream.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    return true;
}

} // namespace DLC
