#ifndef DLC_PACK_FORMAT_H
#define DLC_PACK_FORMAT_H

#include <string>
#include <vector>
#include "io_helpers.h"
#include "dlc_format.h"

namespace DLC {

class DLCPackHandler {
public:
    static bool isDLCPack(std::iostream& stream);

    bool load(std::iostream& stream, DLCPack& pack);
    bool save(std::iostream& stream, const DLCPack& pack);
};

} // namespace DLC

#endif // DLC_PACK_FORMAT_H
