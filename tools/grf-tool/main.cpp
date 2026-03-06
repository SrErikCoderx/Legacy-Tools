#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <zlib.h>
#include "../common/grf_format.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <input> <output>" << std::endl;
        std::cerr << "Modes:" << std::endl;
        std::cerr << "  unpack <file.grf> <output_dir>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "unpack") {
        std::fstream file(argv[2], std::ios::in | std::ios::binary);
        DLC::GrfTable grf;
        if (grf.load(file)) {
            std::string outDir = argv[3];
            std::filesystem::create_directories(outDir);

            std::ofstream log(outDir + "/info.txt");
            log << "Version: " << grf.version << "\n";
            log << "String Lookup:\n";
            for (const auto& s : grf.stringLookup) log << "  " << s << "\n";

            for (const auto& f : grf.files) {
                std::string outPath = outDir + "/" + f.name;
                std::filesystem::create_directories(std::filesystem::path(outPath).parent_path());
                std::ofstream outFile(outPath, std::ios::binary);
                outFile.write(reinterpret_cast<const char*>(f.data.data()), f.data.size());
                std::cout << "Extracted: " << f.name << std::endl;
            }
        }
    }

    return 0;
}
