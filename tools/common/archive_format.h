#ifndef ARCHIVE_FORMAT_H
#define ARCHIVE_FORMAT_H

#include <string>
#include <vector>
#include "io_helpers.h"

namespace DLC {

struct ArchiveEntry {
    std::string name;
    uint32_t size;
    uint32_t offset;
};

class Archive {
public:
    static bool isArchive(std::iostream& stream);

    bool load(std::iostream& stream);
    bool save(std::iostream& stream, const std::vector<std::string>& fileNames, const std::vector<std::vector<uint8_t>>& fileData);

    const std::vector<ArchiveEntry>& getEntries() const { return entries; }

private:
    std::vector<ArchiveEntry> entries;
};

} // namespace DLC

#endif // ARCHIVE_FORMAT_H
