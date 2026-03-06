#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../common/loc_format.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <input> <output>" << std::endl;
        std::cerr << "Modes: " << std::endl;
        std::cerr << "  export <file.loc> <file.txt>" << std::endl;
        std::cerr << "  import <file.txt> <file.loc>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "export") {
        std::fstream file(argv[2], std::ios::in | std::ios::binary);
        DLC::LocTable loc;
        if (loc.load(file)) {
            std::ofstream out(argv[3]);
            out << "VERSION=" << loc.version << "\n";
            for (size_t i = 0; i < loc.strings.size(); ++i) {
                out << i << "=" << loc.strings[i] << "\n";
            }
        }
    } else if (mode == "import") {
        std::ifstream in(argv[2]);
        DLC::LocTable loc;
        std::string line;
        while (std::getline(in, line)) {
            size_t sep = line.find('=');
            if (sep == std::string::npos) continue;
            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 1);
            if (key == "VERSION") loc.version = std::stoul(val);
            else loc.strings.push_back(val);
        }
        std::fstream out(argv[3], std::ios::out | std::ios::binary);
        loc.save(out);
    }

    return 0;
}
