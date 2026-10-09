#ifndef _COBJECTTAGTOFILENAMEMAPPING
#define _COBJECTTAGTOFILENAMEMAPPING

#include "types.h"

#include "Kyoto/CAssetId.hpp"
#include "Kyoto/SObjectTag.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Minimal view of the prototype's tag-to-filename table (CObjectTagToFilenameMapping.cpp,
// 0x80281D10..0x80282420; the class name follows the file name). It holds one entry per tag of
// the vector it is built from, filled in from the PAK contents lists. The scan-text debug manager
// keeps one, and main's "Dump Simple Pool" and "Dump Loaded Textures" options build one over the
// simple pool's referenced tags; its implicit destructor is emitted in main.cpp (0x8000BD8C).
// Neither Echoes nor Prime has it.
class CObjectTagToFilenameMapping {
public:
  struct SEntry {
    CAssetId mAssetId;
    // Guessed name. Printed in the "UncompressedSize" column of SimplePool.txt.
    uint mSize;
    rstl::string mFilename;
  };

  // 0x80281E14: one default entry per tag, then the PAK contents lists are read (0x80281EAC).
  explicit CObjectTagToFilenameMapping(const rstl::vector< SObjectTag >& tags);

  // Guessed name. 0x80281D70: the entry built for tags[index].
  const SEntry& GetEntry(int index) const;

private:
  rstl::vector< SEntry > mEntries;
};
CHECK_SIZEOF(CObjectTagToFilenameMapping, 0x10)

#endif // _COBJECTTAGTOFILENAMEMAPPING
