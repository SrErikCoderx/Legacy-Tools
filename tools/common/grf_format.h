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

        // Skip until count at offset 14
        stream.ignore(11);

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
            decompressed.seekg(0);
            contentStream = &decompressed;
        }

        EndianStream ces(*contentStream);
        ces.setBigEndian(false); // Counts in internal stream are often LE

        uint16_t stringCount = ces.readU16();
        stringLookup.clear();
        for (uint16_t i = 0; i < stringCount; ++i) {
            uint8_t len = static_cast<uint8_t>(contentStream->get());
            if (contentStream->eof()) break;
            stringLookup.push_back(ces.readString(len));
            contentStream->ignore(1); // Null terminator
        }

        uint16_t fileCount = ces.readU16();
        files.clear();
        for (uint16_t i = 0; i < fileCount; ++i) {
            GrfFile f;
            uint8_t nameLen = static_cast<uint8_t>(contentStream->get());
            if (contentStream->eof()) break;
            f.name = ces.readString(nameLen);
            contentStream->ignore(1); // Null terminator

            uint32_t size = ces.readU32();
            if (size > 0 && size < 100 * 1024 * 1024) {
                f.data.resize(size);
                contentStream->read(reinterpret_cast<char*>(f.data.data()), size);
            }
            files.push_back(f);
        }

        return true;
    }
};

} // namespace DLC

#endif // GRF_FORMAT_H
