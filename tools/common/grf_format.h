#ifndef GRF_FORMAT_H
#define GRF_FORMAT_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <zlib.h>
#include "io_helpers.h"

namespace DLC {

struct GrfFile {
    std::string name;
    std::vector<uint8_t> data;
};

class GrfTable {
public:
    uint16_t version;
    uint8_t compressionType;
    std::vector<std::string> stringLookup;
    std::vector<GrfFile> files;

    bool load(std::iostream& stream) {
        EndianStream es(stream);
        es.setBigEndian(true);

        version = es.readU16();
        compressionType = static_cast<uint8_t>(stream.get());
        for (int i = 0; i < 8; ++i) stream.get(); // Padding

        std::stringstream decompressed;
        std::iostream* contentStream = &stream;

        if (compressionType != 0) {
            uint32_t compressedSize = es.readU32();
            uint32_t uncompressedSize = es.readU32();

            std::vector<uint8_t> compressed(compressedSize);
            stream.read(reinterpret_cast<char*>(compressed.data()), compressedSize);

            std::vector<uint8_t> uncompressed(uncompressedSize);
            uLongf destLen = uncompressedSize;
            if (uncompress(uncompressed.data(), &destLen, compressed.data(), compressedSize) != Z_OK) {
                return false;
            }
            decompressed.write(reinterpret_cast<const char*>(uncompressed.data()), uncompressedSize);
            contentStream = &decompressed;
        }

        EndianStream ces(*contentStream);
        ces.setBigEndian(true);

        uint32_t stringCount = ces.readU32();
        stringLookup.clear();
        for (uint32_t i = 0; i < stringCount; ++i) {
            stringLookup.push_back(ces.readUTF());
        }

        uint32_t fileCount = ces.readU32();
        files.clear();
        for (uint32_t i = 0; i < fileCount; ++i) {
            GrfFile f;
            f.name = ces.readUTF();
            uint32_t size = ces.readU32();
            f.data.resize(size);
            contentStream->read(reinterpret_cast<char*>(f.data.data()), size);
            files.push_back(f);
        }

        return true;
    }
};

} // namespace DLC

#endif // GRF_FORMAT_H
