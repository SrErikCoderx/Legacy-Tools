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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <output_file> <input_dir> [--compress] [--dlc]" << std::endl;
        return 1;
    }

    std::string outputFileName = argv[1];
    std::string inputDir = argv[2];
    bool doCompress = false;
    bool forceDLC = false;

    for (int i = 3; i < argc; ++i) {
        if (std::string(argv[i]) == "--compress") doCompress = true;
        if (std::string(argv[i]) == "--dlc") forceDLC = true;
    }

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Error: Input directory does not exist." << std::endl;
        return 1;
    }

    std::vector<fs::path> inputFiles;
    for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if (entry.is_regular_file()) inputFiles.push_back(entry.path());
    }

    std::fstream file(outputFileName, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    bool isPck = (!forceDLC && (outputFileName.size() >= 4 && (outputFileName.substr(outputFileName.size() - 4) == ".pck" || outputFileName.substr(outputFileName.size() - 4) == ".arc")));

    if (isPck) {
        std::cout << "Creating Legacy Console Archive (.pck/.arc)..." << std::endl;
        std::vector<std::string> names;
        std::vector<std::vector<uint8_t>> data;
        for (const auto& p : inputFiles) {
            std::string rel = fs::relative(p, inputDir).string();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            names.push_back(rel);

            std::ifstream inFile(p, std::ios::binary);
            std::vector<uint8_t> content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

            if (doCompress) {
                uLongf cSize = compressBound(content.size());
                std::vector<uint8_t> compressed(cSize);
                if (compress(compressed.data(), &cSize, content.data(), content.size()) == Z_OK) {
                    compressed.resize(cSize);
                    data.push_back(compressed);
                } else data.push_back(content);
            } else data.push_back(content);
        }
        DLC::Archive arc;
        arc.save(file, names, data);
    } else {
        std::cout << "Creating Legacy Console DLC Pack (.dlc)..." << std::endl;
        DLC::DLCPack pack;
        pack.version = 3;

        DLC::FileParam fp;
        fp.type = DLC::PARAM_DISPLAY_NAME;
        fp.data = DLC::utf8_to_utf16le("Custom Pack");
        pack.globalParameters.push_back(fp);

        for (const auto& p : inputFiles) {
            DLC::FileEntry entry;
            std::string rel = fs::relative(p, inputDir).string();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            entry.name = DLC::utf8_to_utf16le(rel);
            entry.type = DLC::TYPE_TEXTURE;

            std::ifstream inFile(p, std::ios::binary);
            std::vector<uint8_t> content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

            if (doCompress) {
                uLongf cSize = compressBound(content.size());
                entry.data.resize(cSize);
                if (compress(entry.data.data(), &cSize, content.data(), content.size()) == Z_OK) {
                    entry.data.resize(cSize);
                } else entry.data = content;
            } else entry.data = content;

            entry.fileSize = static_cast<uint32_t>(entry.data.size());
            pack.files.push_back(entry);
        }
        DLC::DLCPackHandler handler;
        handler.save(file, pack);
    }

    std::cout << "Successfully created " << outputFileName << std::endl;
    return 0;
}
