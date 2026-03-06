#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../common/col_format.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <input> <output>" << std::endl;
        std::cerr << "Modes: " << std::endl;
        std::cerr << "  export <file.col> <file.txt>" << std::endl;
        std::cerr << "  import <file.txt> <file.col>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "export") {
        std::fstream file(argv[2], std::ios::in | std::ios::binary);
        DLC::ColourTable col;
        if (col.load(file)) {
            std::ofstream out(argv[3]);
            out << "VERSION=" << col.version << "\n";
            for (const auto& e : col.entries) {
                out << e.name << "=" << std::hex << e.value << std::dec << "\n";
            }
        }
    } else if (mode == "import") {
        std::ifstream in(argv[2]);
        DLC::ColourTable col;
        std::string line;
        while (std::getline(in, line)) {
            size_t sep = line.find('=');
            if (sep == std::string::npos) continue;
            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 1);
            if (key == "VERSION") col.version = std::stoul(val);
            else col.entries.push_back({key, static_cast<uint32_t>(std::stoul(val, nullptr, 16))});
        }
        std::fstream out(argv[3], std::ios::out | std::ios::binary);
        col.save(out);
    }

    return 0;
}
