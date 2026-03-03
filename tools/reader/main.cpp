#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../common/dlc_format.h"
#include "../common/dlc_utils.h"
#include "../common/dlc_pack_format.h"
#include "../common/archive_format.h"

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
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::fstream file(argv[1], std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    if (DLC::Archive::isArchive(file)) {
        std::cout << "Detected Legacy Console Archive (.pck/.arc)" << std::endl;
        DLC::Archive arc;
        if (arc.load(file)) {
            std::cout << "Files: " << arc.getEntries().size() << std::endl;
            for (const auto& entry : arc.getEntries()) {
                std::cout << " - " << entry.name << " (Size: " << entry.size << ", Offset: " << entry.offset << ")" << std::endl;
            }
        }
    } else if (DLC::DLCPackHandler::isDLCPack(file)) {
        std::cout << "Detected Legacy Console DLC Pack (.dlc)" << std::endl;
        DLC::DLCPack pack;
        DLC::DLCPackHandler handler;
        if (handler.load(file, pack)) {
            std::cout << "Version: " << pack.version << std::endl;
            std::cout << "Global Parameters: " << pack.globalParameters.size() << std::endl;
            for (const auto& fp : pack.globalParameters) {
                std::cout << "  [" << fp.type << "] " << get_param_name(fp.type) << ": " << DLC::utf16le_to_utf8(fp.data) << std::endl;
            }
            std::cout << "Files: " << pack.files.size() << std::endl;
            for (size_t i = 0; i < pack.files.size(); ++i) {
                const auto& entry = pack.files[i];
                std::cout << "File " << i << ": " << DLC::utf16le_to_utf8(entry.name) << std::endl;
                std::cout << "  Type: " << get_type_name(entry.type) << " (" << entry.type << ")" << std::endl;
                std::cout << "  Size: " << entry.fileSize << " bytes" << std::endl;
                std::cout << "  Parameters: " << entry.parameters.size() << std::endl;
                for (const auto& fp : entry.parameters) {
                    std::cout << "    [" << fp.type << "] " << get_param_name(fp.type) << ": " << DLC::utf16le_to_utf8(fp.data) << std::endl;
                }
                if (entry.data.size() > 2 && entry.data[0] == 0x78 && entry.data[1] == 0x9C) {
                    std::cout << "  Compression: Zlib (0x789C)" << std::endl;
                } else {
                    std::cout << "  Compression: None" << std::endl;
                }
            }
        }
    } else {
        std::cerr << "Error: Unknown or unsupported file format." << std::endl;
        return 1;
    }

    return 0;
}
