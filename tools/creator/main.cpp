#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <zlib.h>
#include <algorithm>
#include <map>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"
#include "../common/dlc_pack_format.h"
#include "../common/archive_format.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <manifest_file> [output_file]" << std::endl;
        return 1;
    }

    std::ifstream manifest(argv[1]);
    if (!manifest) {
        std::cerr << "Error: Could not open manifest file." << std::endl;
        return 1;
    }

    std::string outputFileName = (argc >= 3) ? argv[2] : "out.pck";

    DLC::DLCPack pack;
    pack.version = 3;

    std::string line;
    DLC::FileEntry* currentFile = nullptr;

    while (std::getline(manifest, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "VERSION") {
            ss >> pack.version;
        } else if (cmd == "GLOBAL") {
            uint32_t id;
            std::string value;
            ss >> id;
            std::getline(ss, value);
            size_t start = value.find_first_of('"');
            size_t end = value.find_last_of('"');
            if (start != std::string::npos && end != std::string::npos && start < end) {
                value = value.substr(start + 1, end - start - 1);
            } else {
                if (value.find_first_not_of(" \t") != std::string::npos)
                    value = value.substr(value.find_first_not_of(" \t"));
            }
            pack.globalParameters.push_back({id, DLC::utf8_to_utf16(value)});
        } else if (cmd == "FILE") {
            std::string pckPath, sourcePath;
            uint32_t type;
            ss >> pckPath >> type >> sourcePath;

            DLC::FileEntry entry;
            entry.name = DLC::utf8_to_utf16(pckPath);
            entry.type = type;

            std::ifstream inFile(sourcePath, std::ios::binary);
            if (!inFile) {
                std::cerr << "Warning: Could not open source file " << sourcePath << std::endl;
            } else {
                entry.data.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            }
            entry.fileSize = static_cast<uint32_t>(entry.data.size());
            pack.files.push_back(entry);
            currentFile = &pack.files.back();
        } else if (cmd == "PARAM") {
            if (!currentFile) {
                std::cerr << "Error: PARAM outside of FILE context." << std::endl;
                continue;
            }
            uint32_t id;
            std::string value;
            ss >> id;
            std::getline(ss, value);
            size_t start = value.find_first_of('"');
            size_t end = value.find_last_of('"');
            if (start != std::string::npos && end != std::string::npos && start < end) {
                value = value.substr(start + 1, end - start - 1);
            } else {
                if (value.find_first_not_of(" \t") != std::string::npos)
                    value = value.substr(value.find_first_not_of(" \t"));
            }
            currentFile->parameters.push_back({id, DLC::utf8_to_utf16(value)});
        }
    }

    std::fstream file(outputFileName, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    DLC::DLCPackHandler handler;
    handler.save(file, pack);

    std::cout << "Successfully created structured DLC: " << outputFileName << std::endl;
    return 0;
}
