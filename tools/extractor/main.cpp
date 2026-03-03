#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <zlib.h>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dlc_file> [output_dir]" << std::endl;
        return 1;
    }

    std::string outputDir = (argc >= 3) ? argv[2] : "extracted_dlc";
    fs::create_directories(outputDir);

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version < 2) {
        std::cerr << "Error: Unsupported DLC version" << std::endl;
        return 1;
    }

    uint32_t paramCount;
    file.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));
    for (uint32_t i = 0; i < paramCount; ++i) {
        uint32_t type, wchCount;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));
        file.read(reinterpret_cast<char*>(&wchCount), sizeof(wchCount));
        file.seekg(wchCount * sizeof(char16_t), std::ios::cur);
    }

    uint32_t fileCount;
    file.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));

    struct InternalFileDetails {
        uint32_t fileSize;
        uint32_t type;
        uint32_t wchCount;
        std::u16string name;
    };
    std::vector<InternalFileDetails> fileDetails(fileCount);

    for (uint32_t i = 0; i < fileCount; ++i) {
        file.read(reinterpret_cast<char*>(&fileDetails[i].fileSize), sizeof(fileDetails[i].fileSize));
        file.read(reinterpret_cast<char*>(&fileDetails[i].type), sizeof(fileDetails[i].type));
        file.read(reinterpret_cast<char*>(&fileDetails[i].wchCount), sizeof(fileDetails[i].wchCount));
        std::vector<char16_t> buffer(fileDetails[i].wchCount);
        file.read(reinterpret_cast<char*>(buffer.data()), fileDetails[i].wchCount * sizeof(char16_t));
        fileDetails[i].name = std::u16string(buffer.data(), fileDetails[i].wchCount);
    }

    for (uint32_t i = 0; i < fileCount; ++i) {
        uint32_t fileParamCount;
        file.read(reinterpret_cast<char*>(&fileParamCount), sizeof(fileParamCount));
        for (uint32_t j = 0; j < fileParamCount; ++j) {
            uint32_t type, wchCount;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            file.read(reinterpret_cast<char*>(&wchCount), sizeof(wchCount));
            file.seekg(wchCount * sizeof(char16_t), std::ios::cur);
        }

        std::string fileName = DLC::utf16le_to_utf8(fileDetails[i].name);
        std::cout << "Extracting: " << fileName << " (" << fileDetails[i].fileSize << " bytes)..." << std::endl;

        std::vector<uint8_t> buffer(fileDetails[i].fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileDetails[i].fileSize);

        bool isCompressed = (buffer.size() > 2 && buffer[0] == 0x78 && buffer[1] == 0x9C);

        std::vector<uint8_t> dataToSave;
        if (isCompressed) {
            std::cout << "  Detected Zlib compression. Decompressing..." << std::endl;
            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            strm.avail_in = buffer.size();
            strm.next_in = buffer.data();

            if (inflateInit(&strm) == Z_OK) {
                uint8_t out[32768];
                do {
                    strm.avail_out = sizeof(out);
                    strm.next_out = out;
                    int ret = inflate(&strm, Z_NO_FLUSH);
                    if (ret < 0 && ret != Z_BUF_ERROR) {
                        std::cerr << "  Inflate error: " << ret << std::endl;
                        break;
                    }
                    dataToSave.insert(dataToSave.end(), out, out + (sizeof(out) - strm.avail_out));
                } while (strm.avail_out == 0);
                inflateEnd(&strm);
            }
        } else {
            dataToSave = buffer;
        }

        fs::path outPath = fs::path(outputDir) / fileName;
        fs::create_directories(outPath.parent_path());

        std::ofstream outFile(outPath, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error: Could not create output file " << outPath << std::endl;
            continue;
        }
        outFile.write(reinterpret_cast<const char*>(dataToSave.data()), dataToSave.size());
    }

    std::cout << "Extraction complete." << std::endl;
    return 0;
}
