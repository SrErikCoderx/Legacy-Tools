#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "../common/xzp_format.h"
#include "../common/dlc_utils.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.xzp> [output_dir]" << std::endl;
        return 1;
    }

    std::fstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    DLC::XzpTable xzp;
    if (xzp.load(file)) {
        std::cout << "Magic: XUIZ, Version: " << xzp.version << ", TotalSize: " << xzp.totalSize << std::endl;
        std::cout << "Entries found: " << xzp.entries.size() << std::endl;

        if (argc >= 3) {
            std::string outDir = argv[2];
            std::filesystem::create_directories(outDir);
            for (const auto& e : xzp.entries) {
                std::string name = DLC::utf16_to_utf8(e.name);
                std::cout << " - " << name << " (" << e.size << " bytes)" << std::endl;

                std::vector<uint8_t> data(e.size);
                file.seekg(e.offset);
                file.read(reinterpret_cast<char*>(data.data()), e.size);

                std::filesystem::path outPath = std::filesystem::path(outDir) / name;
                std::filesystem::create_directories(outPath.parent_path());
                std::ofstream outFile(outPath, std::ios::binary);
                outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
            }
        } else {
            for (const auto& e : xzp.entries) {
                std::cout << " - " << DLC::utf16_to_utf8(e.name) << " (Size: " << e.size << ", Offset: 0x" << std::hex << e.offset << std::dec << ")" << std::endl;
            }
        }
    } else {
        std::cerr << "Error: Could not parse .xzp file." << std::endl;
    }

    return 0;
}
