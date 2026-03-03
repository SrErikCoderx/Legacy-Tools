#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <zlib.h>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"
#include "../common/dlc_pack_format.h"
#include "../common/archive_format.h"

namespace fs = std::filesystem;

void decompress_and_save(const std::string& name, const std::vector<uint8_t>& data, const fs::path& outputDir) {
    fs::path outPath = outputDir / name;
    fs::create_directories(outPath.parent_path());

    std::vector<uint8_t> dataToSave;
    // Enhanced check for zlib: 0x78 0x9C, 0x78 0x01, 0x78 0xDA, or 0x78 0x5E
    if (data.size() > 2 && data[0] == 0x78 && (data[1] == 0x9C || data[1] == 0x01 || data[1] == 0xDA || data[1] == 0x5E)) {
        std::cout << "  Decompressing " << name << "..." << std::endl;
        z_stream strm;
        strm.zalloc = Z_NULL; strm.zfree = Z_NULL; strm.opaque = Z_NULL;
        strm.avail_in = static_cast<uInt>(data.size());
        strm.next_in = const_cast<Bytef*>(data.data());

        if (inflateInit(&strm) == Z_OK) {
            uint8_t out[65536];
            do {
                strm.avail_out = sizeof(out);
                strm.next_out = out;
                int ret = inflate(&strm, Z_NO_FLUSH);
                if (ret < 0 && ret != Z_BUF_ERROR) break;
                dataToSave.insert(dataToSave.end(), out, out + (sizeof(out) - strm.avail_out));
            } while (strm.avail_out == 0);
            inflateEnd(&strm);
        } else {
            dataToSave = data;
        }
    } else {
        dataToSave = data;
    }

    std::ofstream outFile(outPath, std::ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(dataToSave.data()), dataToSave.size());
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file> [output_dir]" << std::endl;
        return 1;
    }

    std::string outputDirStr = (argc >= 3) ? argv[2] : "extracted";
    fs::path outputDir(outputDirStr);

    std::fstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    if (DLC::DLCPackHandler::isDLCPack(file)) {
        std::cout << "Extracting Legacy Console DLC Pack..." << std::endl;
        DLC::DLCPack pack;
        DLC::DLCPackHandler handler;
        if (handler.load(file, pack)) {
            for (const auto& entry : pack.files) {
                std::string name = DLC::utf16_to_utf8(entry.name);
                std::cout << " - " << name << " (" << entry.fileSize << " bytes)" << std::endl;
                decompress_and_save(name, entry.data, outputDir);
            }
        }
    } else if (DLC::Archive::isArchive(file)) {
        std::cout << "Extracting Legacy Console Archive..." << std::endl;
        DLC::Archive arc;
        if (arc.load(file)) {
            for (const auto& entry : arc.getEntries()) {
                std::cout << " - " << entry.name << " (" << entry.size << " bytes)" << std::endl;
                std::vector<uint8_t> data(entry.size);
                file.seekg(entry.offset);
                file.read(reinterpret_cast<char*>(data.data()), entry.size);
                decompress_and_save(entry.name, data, outputDir);
            }
        }
    } else {
        std::cerr << "Error: Unsupported file format." << std::endl;
        return 1;
    }

    std::cout << "Extraction complete." << std::endl;
    return 0;
}
