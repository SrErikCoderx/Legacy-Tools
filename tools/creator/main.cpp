#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <zlib.h>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"

namespace fs = std::filesystem;

void write_u16string(std::ofstream& file, const std::u16string& str) {
    uint32_t wchCount = static_cast<uint32_t>(str.size());
    file.write(reinterpret_cast<const char*>(&wchCount), sizeof(wchCount));
    file.write(reinterpret_cast<const char*>(str.data()), wchCount * sizeof(char16_t));
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <output_dlc> <input_dir> [--compress]" << std::endl;
        return 1;
    }

    std::string outputFileName = argv[1];
    std::string inputDir = argv[2];
    bool compressFiles = (argc >= 4 && std::string(argv[3]) == "--compress");

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Error: Input directory does not exist." << std::endl;
        return 1;
    }

    std::ofstream file(outputFileName, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not create output file " << outputFileName << std::endl;
        return 1;
    }

    uint32_t version = DLC::CURRENT_DLC_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    uint32_t globalParamCount = 1;
    file.write(reinterpret_cast<const char*>(&globalParamCount), sizeof(globalParamCount));
    uint32_t paramType = DLC::PARAM_DISPLAY_NAME;
    file.write(reinterpret_cast<const char*>(&paramType), sizeof(paramType));
    write_u16string(file, DLC::utf8_to_utf16le("Custom DLC Pack"));

    std::vector<fs::path> inputFiles;
    for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            inputFiles.push_back(entry.path());
        }
    }

    uint32_t fileCount = static_cast<uint32_t>(inputFiles.size());
    file.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

    struct PreparedFile {
        std::vector<uint8_t> data;
        uint32_t originalSize;
        uint32_t type;
        std::u16string u16Path;
    };
    std::vector<PreparedFile> preparedFiles;

    for (const auto& path : inputFiles) {
        std::ifstream inFile(path, std::ios::binary);
        std::vector<uint8_t> rawData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

        PreparedFile pf;
        pf.originalSize = rawData.size();
        pf.type = DLC::TYPE_TEXTURE;
        std::string relativePath = fs::relative(path, inputDir).string();
        pf.u16Path = DLC::utf8_to_utf16le(relativePath);

        if (compressFiles) {
            uLongf compressedSize = compressBound(rawData.size());
            pf.data.resize(compressedSize);
            if (compress(pf.data.data(), &compressedSize, rawData.data(), rawData.size()) == Z_OK) {
                pf.data.resize(compressedSize);
            } else {
                pf.data = rawData;
            }
        } else {
            pf.data = rawData;
        }
        preparedFiles.push_back(pf);
    }

    for (const auto& pf : preparedFiles) {
        uint32_t sizeToWrite = static_cast<uint32_t>(pf.data.size());
        file.write(reinterpret_cast<const char*>(&sizeToWrite), sizeof(sizeToWrite));
        file.write(reinterpret_cast<const char*>(&pf.type), sizeof(pf.type));
        write_u16string(file, pf.u16Path);
    }

    for (const auto& pf : preparedFiles) {
        uint32_t fileParamCount = 0;
        file.write(reinterpret_cast<const char*>(&fileParamCount), sizeof(fileParamCount));
        file.write(reinterpret_cast<const char*>(pf.data.data()), pf.data.size());
    }

    std::cout << "DLC created successfully: " << outputFileName << (compressFiles ? " (Compressed)" : "") << std::endl;
    return 0;
}
