#include "dlc_pack_format.h"
#include <iostream>

namespace DLC {

bool DLCPackHandler::isDLCPack(std::iostream& stream) {
    auto pos = stream.tellg();
    EndianStream es(stream);
    es.setBigEndian(true);
    uint32_t version = es.readU32();
    stream.seekg(pos);
    return (version == 2 || version == 3);
}

bool DLCPackHandler::load(std::iostream& stream, DLCPack& pack) {
    EndianStream es(stream);
    es.setBigEndian(true);

    pack.version = es.readU32();
    if (pack.version < 2 || pack.version > 3) return false;

    uint32_t paramCount = es.readU32();
    for (uint32_t i = 0; i < paramCount; ++i) {
        FileParam fp;
        fp.type = es.readU32();
        uint32_t wchCount = es.readU32();
        fp.data = es.readUTF16(wchCount);
        pack.globalParameters.push_back(fp);
    }

    uint32_t fileCount = es.readU32();
    for (uint32_t i = 0; i < fileCount; ++i) {
        FileEntry entry;
        entry.fileSize = es.readU32();
        entry.type = es.readU32();
        uint32_t wchCount = es.readU32();
        entry.name = es.readUTF16(wchCount);
        pack.files.push_back(entry);
    }

    for (uint32_t i = 0; i < fileCount; ++i) {
        uint32_t fileParamCount = es.readU32();
        for (uint32_t j = 0; j < fileParamCount; ++j) {
            FileParam fp;
            fp.type = es.readU32();
            uint32_t wchCount = es.readU32();
            fp.data = es.readUTF16(wchCount);
            pack.files[i].parameters.push_back(fp);
        }

        pack.files[i].data.resize(pack.files[i].fileSize);
        stream.read(reinterpret_cast<char*>(pack.files[i].data.data()), pack.files[i].fileSize);
    }

    return true;
}

bool DLCPackHandler::save(std::iostream& stream, const DLCPack& pack) {
    EndianStream es(stream);
    es.setBigEndian(true);

    es.writeU32(pack.version);

    es.writeU32(static_cast<uint32_t>(pack.globalParameters.size()));
    for (const auto& fp : pack.globalParameters) {
        es.writeU32(fp.type);
        es.writeU32(static_cast<uint32_t>(fp.data.size()));
        es.writeUTF16(fp.data);
    }

    es.writeU32(static_cast<uint32_t>(pack.files.size()));
    for (const auto& entry : pack.files) {
        es.writeU32(static_cast<uint32_t>(entry.data.size()));
        es.writeU32(entry.type);
        es.writeU32(static_cast<uint32_t>(entry.name.size()));
        es.writeUTF16(entry.name);
    }

    for (const auto& entry : pack.files) {
        es.writeU32(static_cast<uint32_t>(entry.parameters.size()));
        for (const auto& fp : entry.parameters) {
            es.writeU32(fp.type);
            es.writeU32(static_cast<uint32_t>(fp.data.size()));
            es.writeUTF16(fp.data);
        }
        stream.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
    }

    return true;
}

} // namespace DLC
