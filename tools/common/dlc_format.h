#ifndef DLC_FORMAT_H
#define DLC_FORMAT_H

#include <cstdint>
#include <vector>
#include <string>

namespace DLC {

const uint32_t CURRENT_DLC_VERSION = 3;

enum DLCType : uint32_t {
    TYPE_SKIN = 0,
    TYPE_CAPE,
    TYPE_TEXTURE,
    TYPE_UI_DATA,
    TYPE_PACK_CONFIG,
    TYPE_TEXTURE_PACK,
    TYPE_LOCALISATION_DATA,
    TYPE_GAME_RULES,
    TYPE_AUDIO,
    TYPE_COLOUR_TABLE,
    TYPE_GAME_RULES_HEADER,
    TYPE_MAX
};

enum DLCParameterType : int32_t {
    PARAM_INVALID = -1,
    PARAM_DISPLAY_NAME = 0,
    PARAM_THEME_NAME,
    PARAM_FREE,
    PARAM_CREDIT,
    PARAM_CAPE,
    PARAM_BOX,
    PARAM_ANIM,
    PARAM_PACK_ID,
    PARAM_NETHER_PARTICLE_COLOUR,
    PARAM_ENCHANTMENT_TEXT_COLOUR,
    PARAM_ENCHANTMENT_TEXT_FOCUS_COLOUR,
    PARAM_DATA_PATH,
    PARAM_PACK_VERSION,
    PARAM_MAX
};

struct FileParam {
    uint32_t type;
    std::u16string data;
};

// Compression header used by 4J/Minecraft Console
struct CompressionHeader {
    uint32_t magic; // e.g. 'ZLIB' or similar, but often just uncompressed size
    uint32_t compressedSize;
    uint32_t uncompressedSize;
};

struct FileEntry {
    uint32_t type;
    std::u16string name;
    uint32_t fileSize;
    bool isCompressed;
    uint32_t uncompressedSize;
    std::vector<FileParam> parameters;
    std::vector<uint8_t> data;
};

struct DLCPack {
    uint32_t version;
    std::vector<FileParam> globalParameters;
    std::vector<FileEntry> files;
};

} // namespace DLC

#endif // DLC_FORMAT_H
