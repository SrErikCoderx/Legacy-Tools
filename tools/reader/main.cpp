#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"

const char* get_type_name(uint32_t type) {
    switch (type) {
        case DLC::TYPE_SKIN: return "Skin";
        case DLC::TYPE_CAPE: return "Cape";
        case DLC::TYPE_TEXTURE: return "Texture";
        case DLC::TYPE_UI_DATA: return "UI Data";
        case DLC::TYPE_PACK_CONFIG: return "Pack Config";
        case DLC::TYPE_TEXTURE_PACK: return "Texture Pack";
        case DLC::TYPE_LOCALISATION_DATA: return "Localisation Data";
        case DLC::TYPE_GAME_RULES: return "Game Rules";
        case DLC::TYPE_AUDIO: return "Audio";
        case DLC::TYPE_COLOUR_TABLE: return "Colour Table";
        case DLC::TYPE_GAME_RULES_HEADER: return "Game Rules Header";
        default: return "Unknown";
    }
}

const char* get_param_name(uint32_t type) {
    switch (type) {
        case DLC::PARAM_DISPLAY_NAME: return "DisplayName";
        case DLC::PARAM_THEME_NAME: return "ThemeName";
        case DLC::PARAM_FREE: return "Free";
        case DLC::PARAM_CREDIT: return "Credit";
        case DLC::PARAM_CAPE: return "Cape";
        case DLC::PARAM_BOX: return "Box";
        case DLC::PARAM_ANIM: return "Anim";
        case DLC::PARAM_PACK_ID: return "PackId";
        case DLC::PARAM_NETHER_PARTICLE_COLOUR: return "NetherParticleColour";
        case DLC::PARAM_ENCHANTMENT_TEXT_COLOUR: return "EnchantmentTextColour";
        case DLC::PARAM_ENCHANTMENT_TEXT_FOCUS_COLOUR: return "EnchantmentTextFocusColour";
        case DLC::PARAM_DATA_PATH: return "DataPath";
        case DLC::PARAM_PACK_VERSION: return "PackVersion";
        default: return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dlc_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    std::cout << "DLC Version: " << version << std::endl;

    if (version < 2) {
        std::cerr << "Error: Unsupported DLC version" << std::endl;
        return 1;
    }

    uint32_t paramCount;
    file.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));
    std::cout << "Global Parameters: " << paramCount << std::endl;

    for (uint32_t i = 0; i < paramCount; ++i) {
        uint32_t type, wchCount;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));
        file.read(reinterpret_cast<char*>(&wchCount), sizeof(wchCount));
        std::vector<char16_t> buffer(wchCount);
        file.read(reinterpret_cast<char*>(buffer.data()), wchCount * sizeof(char16_t));
        std::u16string data(buffer.data(), wchCount);
        std::cout << "  [" << type << "] " << get_param_name(type) << ": " << DLC::utf16le_to_utf8(data) << std::endl;
    }

    uint32_t fileCount;
    file.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    std::cout << "Files: " << fileCount << std::endl;

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
        std::cout << "File " << i << ": " << DLC::utf16le_to_utf8(fileDetails[i].name) << std::endl;
        std::cout << "  Type: " << get_type_name(fileDetails[i].type) << std::endl;
        std::cout << "  Size: " << fileDetails[i].fileSize << " bytes" << std::endl;

        uint32_t fileParamCount;
        file.read(reinterpret_cast<char*>(&fileParamCount), sizeof(fileParamCount));
        std::cout << "  Parameters: " << fileParamCount << std::endl;

        for (uint32_t j = 0; j < fileParamCount; ++j) {
            uint32_t type, wchCount;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            file.read(reinterpret_cast<char*>(&wchCount), sizeof(wchCount));
            std::vector<char16_t> buffer(wchCount);
            file.read(reinterpret_cast<char*>(buffer.data()), wchCount * sizeof(char16_t));
            std::u16string data(buffer.data(), wchCount);
            std::cout << "    [" << type << "] " << get_param_name(type) << ": " << DLC::utf16le_to_utf8(data) << std::endl;
        }

        auto pos = file.tellg();
        uint8_t magic[2];
        file.read(reinterpret_cast<char*>(magic), 2);
        if (magic[0] == 0x78 && magic[1] == 0x9C) {
            std::cout << "  Compression: Zlib (0x789C)" << std::endl;
        } else {
            std::cout << "  Compression: None" << std::endl;
        }
        file.seekg(pos);

        file.seekg(fileDetails[i].fileSize, std::ios::cur);
    }

    return 0;
}
