#include <inttypes.h>

#include "cpkdriver.h"
#include "vfs.h"
#include "../log.h"

#include "../../vendor/include/flat_hash_map.hpp"

namespace Impacto {

enum CpkColumnFlags {
  STORAGE_MASK = 0xf0,
  STORAGE_NONE = 0x00,
  STORAGE_ZERO = 0x10,
  STORAGE_CONSTANT = 0x30,
  STORAGE_PERROW = 0x50,

  TYPE_MASK = 0x0f,
  TYPE_DATA = 0x0b,
  TYPE_STRING = 0x0a,
  TYPE_FLOAT = 0x08,
  TYPE_8BYTE2 = 0x07,
  TYPE_8BYTE = 0x06,
  TYPE_4BYTE2 = 0x05,
  TYPE_4BYTE = 0x04,
  TYPE_2BYTE2 = 0x03,
  TYPE_2BYTE = 0x02,
  TYPE_1BYTE2 = 0x01,
  TYPE_1BYTE = 0x00,
};

struct CpkTocEntry {
  uint32_t Id;
  std::string Name;
  uint64_t Offset;
  uint64_t UncompressedSize;
  uint64_t CompressedSize;
};

struct CpkCell {
  uint8_t Type;
  uint8_t Uint8Val;
  uint16_t Uint16Val;
  uint32_t Uint32Val;
  uint64_t Uint64Val;
  float FloatVal;
  std::string StringVal;
  std::vector<uint8_t> DataVal;
};

struct CpkColumn {
  uint32_t Flags;
  std::string Name;
  std::vector<CpkCell> Cells;
};

class CpkArchive : public VfsArchive {
 public:
  uint16_t Version;
  uint16_t Revision;

  uint32_t FileCount;
  ska::flat_hash_map<int, CpkTocEntry> TOC;

  ska::flat_hash_map<std::string, uint32_t> NamesToIds;

  IoError GetName(uint32_t id, char* outName) override;
  IoError GetId(const char* path, uint32_t* outId) override;

  IoError EnumerateStart(uint32_t* outIterator,
                         VfsFileInfo* outFileInfo) override;
  IoError EnumerateNext(uint32_t* inoutIterator,
                        VfsFileInfo* outFileInfo) override;

  ~CpkArchive() override;

 protected:
  IoError DriverOpen(uint32_t id, SDL_RWops** outHandle) override;
  IoError DriverGetSize(uint32_t id, int64_t* outSize) override;
  IoError DriverCanStream(uint32_t id, bool* outResult) override;
  IoError DriverSlurp(uint32_t id, void* outBuffer) override;
};

struct CpkOpenFile {
  CpkTocEntry* Entry;
  CpkArchive* Archive;

  int64_t Position;
};

static bool registered = false;

int64_t SDLCALL CpkEntrySize(SDL_RWops* context) {
  CpkOpenFile* file = (CpkOpenFile*)(context->hidden.unknown.data1);
  return file->Entry->UncompressedSize;
}

size_t SDLCALL CpkEntryUncompressedRead(SDL_RWops* context, void* ptr,
                                        size_t size, size_t maxnum) {
  int64_t bytes = size * maxnum;
  CpkOpenFile* file = (CpkOpenFile*)context->hidden.unknown.data1;
  int64_t fileCur = file->Entry->Offset + file->Position;
  int64_t fileEnd = file->Entry->Offset + file->Entry->UncompressedSize;
  int64_t availBytes = std::min(fileEnd - fileCur, bytes);
  int64_t availNum = availBytes / size;
  if (SDL_RWseek(file->Archive->BaseStream, fileCur, RW_SEEK_SET) == -1) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " bytes at %" PRId64
           " from open archive \"%s\" failed (seek error): %s\n",
           bytes, fileCur, file->Archive->MountPoint, SDL_GetError());
    return 0;
  }
  ImpLogSlow(LL_Trace, LC_IO, "read size=%" PRId64 " num=%" PRId64 "\n", size,
             availNum);
  size_t result = SDL_RWread(file->Archive->BaseStream, ptr, size, availNum);
  if (result == 0) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " bytes at %" PRId64
           " from open archive \"%s\" failed (read error): %s\n",
           bytes, fileCur, file->Archive->MountPoint, SDL_GetError());
  }
  return result;
}

int64_t SDLCALL CpkEntryUncompressedSeek(SDL_RWops* context, int64_t offset,
                                         int whence) {
  CpkOpenFile* file = (CpkOpenFile*)context->hidden.unknown.data1;
  int64_t relOffset;
  switch (whence) {
    case RW_SEEK_SET: {
      relOffset = offset - file->Position;
      break;
    }

    case RW_SEEK_CUR: {
      relOffset = offset;
      break;
    }

    case RW_SEEK_END: {
      int64_t invOffset =
          file->Entry->UncompressedSize - offset;  // equivalent for SEEK_SET
      relOffset = invOffset - file->Position;
      break;
    }

    default: {
      ImpLog(LL_Error, LC_IO,
             "Invalid seek type %d in file \"%s\" in archive \"%s\"\n", whence,
             file->Entry->Name.c_str(), file->Archive->MountPoint);
      return SDL_SetError("Invalid seek type");
      break;
    }
  }
  if (file->Position + relOffset > file->Entry->UncompressedSize ||
      file->Position + relOffset < 0) {
    ImpLog(LL_Error, LC_IO,
           "Invalid seek (mode %d, offset %" PRId64
           ") in file \"%s\" in archive \"%s\"\n",
           whence, offset, file->Entry->Name.c_str(),
           file->Archive->MountPoint);
    return SDL_SetError("Out-of-bounds seek");
  } else {
    file->Position += relOffset;
    return file->Position;
  }
}

size_t SDLCALL CpkEntryWrite(SDL_RWops* context, const void* ptr, size_t size,
                             size_t num) {
  const char error[] = "Tried to write to a CPK entry";
  SDL_SetError(error);
  ImpLog(LL_Error, LC_IO, "%s\n", error);
  return 0;
}

int SDLCALL CpkEntryClose(SDL_RWops* context) {
  CpkOpenFile* file = (CpkOpenFile*)(context->hidden.unknown.data1);
  ImpLog(LL_Debug, LC_IO, "Closing file \"%s\" in \"%s\"\n",
         file->Entry->Name.c_str(), file->Archive->MountPoint);
  if (file->Archive->OpenHandles == 0) {
    ImpLog(LL_Error, LC_IO,
           "Closed file \"%s\" in \"%s\"\n while no file handles were "
           "open\n",
           file->Entry->Name.c_str(), file->Archive->MountPoint);
  } else {
    file->Archive->OpenHandles--;
  }

  free(context);
  free(file);
  return 0;
}

std::string CpkReadString(SDL_RWops* stream, long stringsOffset) {
  int stringAddr = stringsOffset + SDL_ReadBE32(stream);

  long retAddr = SDL_RWtell(stream);

  SDL_RWseek(stream, stringAddr, RW_SEEK_SET);

  std::string str = "";
  char ch;
  while ((ch = SDL_ReadU8(stream)) != 0x00) {
    str += ch;
  }
  SDL_RWseek(stream, retAddr, RW_SEEK_SET);

  return str;
}

bool CpkReadUtfBlock(
    SDL_RWops* stream,
    std::vector<ska::flat_hash_map<std::string, CpkCell>>* rows) {
  long utfBeginOffset = SDL_RWtell(stream);

  const uint32_t utfMagic = 0x40555446;
  if (SDL_ReadBE32(stream) != utfMagic) {
    ImpLog(LL_Trace, LC_IO, "Error reading CPK UTF table\n");
    return false;
  }

  int tableSize = SDL_ReadBE32(stream);
  int rowsOffset = SDL_ReadBE32(stream);
  int stringsOffset = SDL_ReadBE32(stream);
  int dataOffset = SDL_ReadBE32(stream);

  rowsOffset += (utfBeginOffset + 8);
  stringsOffset += (utfBeginOffset + 8);
  dataOffset += (utfBeginOffset + 8);

  int tableNameOffset = SDL_ReadBE32(stream);
  int numColumns = SDL_ReadBE16(stream);
  int rowLength = SDL_ReadBE16(stream);
  int numRows = SDL_ReadBE32(stream);

  std::vector<CpkColumn> columns;

  for (int i = 0; i < numColumns; i++) {
    CpkColumn column;
    column.Flags = SDL_ReadU8(stream);
    if (column.Flags == 0) column.Flags = SDL_ReadBE32(stream);

    column.Name = CpkReadString(stream, stringsOffset);

    columns.push_back(column);
  }

  for (int i = 0; i < numRows; i++) {
    SDL_RWseek(stream, rowsOffset + (i * rowLength), SEEK_SET);
    ska::flat_hash_map<std::string, CpkCell> row;
    for (auto& column : columns) {
      CpkCell cell;
      cell.Type = 0;
      cell.Uint8Val = 0;
      cell.Uint16Val = 0;
      cell.Uint32Val = 0;
      cell.Uint64Val = 0;
      cell.FloatVal = 0.0f;
      int storageFlag = column.Flags & CpkColumnFlags::STORAGE_MASK;
      if (storageFlag == 0x50) {
        cell.Type = column.Flags & CpkColumnFlags::TYPE_MASK;
        switch (cell.Type) {
          case 0:
          case 1:
            cell.Uint8Val = SDL_ReadU8(stream);
            break;
          case 2:
          case 3:
            cell.Uint16Val = SDL_ReadBE16(stream);
            break;
          case 4:
          case 5:
            cell.Uint32Val = SDL_ReadBE32(stream);
            break;
          case 6:
          case 7:
            cell.Uint64Val = SDL_ReadBE64(stream);
            break;
          case 8:
            cell.FloatVal = (float)SDL_ReadBE32(stream);
            break;
          case 0xA:
            cell.StringVal = CpkReadString(stream, stringsOffset);
            break;
          case 0xB:
            int dataPos = SDL_ReadBE32(stream) + dataOffset;
            cell.Uint64Val = dataPos;
            int dataSize = SDL_ReadBE32(stream);
            cell.DataVal.reserve(dataSize);
            long retAddr = SDL_RWtell(stream);
            SDL_RWseek(stream, dataPos, SEEK_SET);
            for (int i = 0; i < dataSize; i++) {
              cell.DataVal.push_back(SDL_ReadU8(stream));
            }
            SDL_RWseek(stream, retAddr, SEEK_SET);
            break;
        }
      }
      row[column.Name] = cell;
    }
    rows->push_back(row);
  }
}

bool CpkReadItoc(SDL_RWops* stream, long itocOffset, long contentOffset,
                 uint16_t align, ska::flat_hash_map<int, CpkTocEntry>& toc) {
  SDL_RWseek(stream, itocOffset, RW_SEEK_SET);
  const uint32_t itocMagic = 0x49544F43;
  if (SDL_ReadBE32(stream) != itocMagic) {
    ImpLog(LL_Trace, LC_IO, "Error reading CPK ITOC\n");
    return IoError_Fail;
  }
  SDL_RWseek(stream, itocOffset + 0x10, RW_SEEK_SET);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> itocUtfTable;
  if (!CpkReadUtfBlock(stream, &itocUtfTable)) return false;

  long retAddr = SDL_RWtell(stream);
  if (itocUtfTable[0]["DataL"].Uint64Val != 0) {
    SDL_RWseek(stream, itocUtfTable[0]["DataL"].Uint64Val, RW_SEEK_SET);
    std::vector<ska::flat_hash_map<std::string, CpkCell>> dataLUtfTable;
    if (!CpkReadUtfBlock(stream, &dataLUtfTable)) return false;
    SDL_RWseek(stream, itocUtfTable[0]["DataH"].Uint64Val, RW_SEEK_SET);
    std::vector<ska::flat_hash_map<std::string, CpkCell>> dataHUtfTable;
    if (!CpkReadUtfBlock(stream, &dataHUtfTable)) return false;

    for (auto& row : dataLUtfTable) {
      int id = row["ID"].Uint16Val;
      toc[id].Id = id;
      if (row["ExtractSize"].Uint16Val) {
        toc[id].UncompressedSize = row["ExtractSize"].Uint16Val;
        toc[id].CompressedSize = row["FileSize"].Uint16Val;
      } else {
        toc[id].UncompressedSize = row["FileSize"].Uint16Val;
      }
      if (toc[id].Name.empty()) {
        char name[6];
        sprintf_s(name, "%05i", id);
        toc[id].Name = std::string(name);
      }
    }
    for (auto& row : dataHUtfTable) {
      int id = row["ID"].Uint16Val;
      toc[id].Id = id;
      if (row["ExtractSize"].Uint32Val) {
        toc[id].UncompressedSize = row["ExtractSize"].Uint32Val;
        toc[id].CompressedSize = row["FileSize"].Uint32Val;
      } else {
        toc[id].UncompressedSize = row["FileSize"].Uint32Val;
      }
      if (toc[id].Name.empty()) {
        char name[6];
        sprintf_s(name, "%05i", id);
        toc[id].Name = std::string(name);
      }
    }
  }

  long offset = contentOffset;
  std::vector<uint32_t> ids;
  for (const auto& kv : toc) ids.push_back(kv.first);
  std::sort(ids.begin(), ids.end());
  for (const auto id : ids) {
    uint32_t size;
    if (toc[id].CompressedSize > 0) {
      size = toc[id].CompressedSize;
    } else {
      size = toc[id].UncompressedSize;
    }
    toc[id].Offset = offset;
    offset += size;
    if (size % align) offset += align - (size % align);
  }
}

bool CpkReadToc(SDL_RWops* stream, long tocOffset, long contentOffset,
                ska::flat_hash_map<int, CpkTocEntry>& toc) {
  SDL_RWseek(stream, tocOffset, RW_SEEK_SET);
  const uint32_t tocMagic = 0x544F4320;
  if (SDL_ReadBE32(stream) != tocMagic) {
    ImpLog(LL_Trace, LC_IO, "Error reading CPK TOC\n");
    return IoError_Fail;
  }
  SDL_RWseek(stream, tocOffset + 0x10, RW_SEEK_SET);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> tocUtfTable;
  if (!CpkReadUtfBlock(stream, &tocUtfTable)) return false;

  for (auto& row : tocUtfTable) {
    int id = row["ID"].Uint32Val;
    toc[id].Id = id;
    toc[id].Offset = row["FileOffset"].Uint64Val;
    if (!row["DirName"].StringVal.empty()) {
      toc[id].Name = row["DirName"].StringVal + "/" + row["FileName"].StringVal;
    } else {
      toc[id].Name = row["FileName"].StringVal;
    }
    int extractedSize = row["ExtractSize"].Uint32Val;
    int fileSize = row["FileSize"].Uint32Val;
    if (extractedSize && (extractedSize != fileSize)) {
      toc[id].UncompressedSize = extractedSize;
      toc[id].CompressedSize = fileSize;
    } else {
      toc[id].UncompressedSize = fileSize;
    }
    if (toc[id].Name.empty()) toc[id].Name = "NO_NAME";
  }
}

bool CpkReadEtoc(SDL_RWops* stream, long etocOffset,
                 ska::flat_hash_map<int, CpkTocEntry>& toc) {
  SDL_RWseek(stream, etocOffset, RW_SEEK_SET);
  const uint32_t etocMagic = 0x45544F43;
  if (SDL_ReadBE32(stream) != etocMagic) {
    ImpLog(LL_Trace, LC_IO, "Error reading CPK ETOC\n");
    return IoError_Fail;
  }
  SDL_RWseek(stream, etocOffset + 0x10, RW_SEEK_SET);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> etocUtfTable;
  if (!CpkReadUtfBlock(stream, &etocUtfTable)) return false;

  // for (auto& row : etocUtfTable) {
  // TODO: This contains the LocalDir and UpdateDateTime params. Do we actually
  // need this?...
  //}
}

IoError CpkMount(SDL_RWops* stream, VfsArchive** outArchive) {
  const uint32_t magic = 0x43504B20;
  if (SDL_ReadBE32(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not a CPK\n");
    return IoError_Fail;
  }
  SDL_RWseek(stream, 0x10, RW_SEEK_SET);
  std::vector<ska::flat_hash_map<std::string, CpkCell>> headerUtfTable;
  if (!CpkReadUtfBlock(stream, &headerUtfTable)) return IoError_Fail;

  uint16_t alignVal = headerUtfTable[0]["Align"].Uint16Val;
  uint32_t fileCount = headerUtfTable[0]["Files"].Uint32Val;
  uint16_t version = headerUtfTable[0]["Version"].Uint16Val;
  uint16_t revision = headerUtfTable[0]["Revision"].Uint16Val;

  CpkArchive* result = new CpkArchive;
  result->Version = version;
  result->Revision = revision;
  result->FileCount = fileCount;

  if (headerUtfTable[0]["TocOffset"].Uint64Val != 0) {
    CpkReadToc(stream, headerUtfTable[0]["TocOffset"].Uint64Val,
               headerUtfTable[0]["ContentOffset"].Uint64Val, result->TOC);
  }

  if (headerUtfTable[0]["EtocOffset"].Uint64Val != 0) {
    CpkReadEtoc(stream, headerUtfTable[0]["EtocOffset"].Uint64Val, result->TOC);
  }

  if (headerUtfTable[0]["ItocOffset"].Uint64Val != 0) {
    CpkReadItoc(stream, headerUtfTable[0]["ItocOffset"].Uint64Val,
                headerUtfTable[0]["ContentOffset"].Uint64Val, alignVal,
                result->TOC);
  }

  // TODO: Group based TOC. Haven't actually seen anyone implement this.
  // if (headerUtfTable[0]["GtocOffset"].Uint64Val != 0) {

  //}
  for (const auto& p : result->TOC) {
    result->NamesToIds[p.second.Name] = p.second.Id;
  }

  ImpLog(LL_Debug, LC_IO, "Mounted as CPK\n");
  *outArchive = result;
  return IoError_OK;
}

CpkArchive::~CpkArchive() {}

IoError CpkArchive::DriverOpen(uint32_t id, SDL_RWops** outHandle) {
  auto it = TOC.find(id);
  if (it == TOC.end()) {
    ImpLog(LL_Error, LC_IO, "CPK open: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  }

  SDL_RWops* rwops = (SDL_RWops*)calloc(1, sizeof(SDL_RWops));
  rwops->type = SDL_RWOPS_UNKNOWN;
  rwops->size = CpkEntrySize;
  rwops->write = CpkEntryWrite;

  CpkTocEntry* entry = &TOC[id];

  if (entry->CompressedSize) {
    ImpLog(LL_Error, LC_IO,
           "CPK open: cannot open LAYLA compressed file as a stream\n");
    return IoError_Fail;
  } else {
    rwops->close = CpkEntryClose;
    rwops->read = CpkEntryUncompressedRead;
    rwops->seek = CpkEntryUncompressedSeek;

    CpkOpenFile* file = (CpkOpenFile*)calloc(1, sizeof(CpkOpenFile));
    file->Archive = this;
    file->Entry = entry;
    file->Position = 0;

    rwops->hidden.unknown.data1 = file;
  }

  *outHandle = rwops;
  return IoError_OK;
}

IoError CpkArchive::DriverGetSize(uint32_t id, int64_t* outSize) {
  auto it = TOC.find(id);
  if (it == TOC.end()) {
    ImpLog(LL_Debug, LC_IO, "CPK GetSize: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  } else {
    CpkTocEntry* entry = &TOC[id];
    ImpLogSlow(LL_Trace, LC_IO, "CPK GetSize: %d found for %d in \"%s\"\n",
               entry->UncompressedSize, id, MountPoint);
    *outSize = entry->UncompressedSize;
    return IoError_OK;
  }
}

IoError CpkArchive::GetName(uint32_t id, char* outName) {
  auto it = TOC.find(id);
  if (it == TOC.end()) {
    ImpLog(LL_Debug, LC_IO, "CPK GetName: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  } else {
    CpkTocEntry* entry = &TOC[id];
    ImpLogSlow(LL_Trace, LC_IO, "CPK GetName: %s found for %d in \"%s\"\n",
               entry->Name.c_str(), id, MountPoint);
    strncpy_s(outName, VfsMaxPath, entry->Name.c_str(), entry->Name.length());
    return IoError_OK;
  }
}

IoError CpkArchive::GetId(const char* path, uint32_t* outId) {
  auto it = NamesToIds.find(path);
  if (it == NamesToIds.end()) {
    ImpLog(LL_Debug, LC_IO, "CPK GetId: %s not found in \"%s\"\n", path,
           MountPoint);
    return IoError_NotFound;
  } else {
    *outId = it->second;
    ImpLogSlow(LL_Trace, LC_IO, "CPK GetId: %d found for %s in \"%s\"\n",
               *outId, path, MountPoint);
    return IoError_OK;
  }
}

IoError CpkArchive::EnumerateNext(uint32_t* inoutIterator,
                                  VfsFileInfo* outFileInfo) {
  if (FileCount >= *inoutIterator || *inoutIterator < 0) {
    ImpLogSlow(LL_Trace, LC_IO,
               "CPK EnumerateNext: %d out of bounds in \"%s\"\n",
               *inoutIterator, MountPoint);
    return IoError_Eof;
  }
  outFileInfo->Id = TOC[*inoutIterator].Id;
  strncpy_s(outFileInfo->Name, VfsMaxPath, TOC[*inoutIterator].Name.c_str(),
            TOC[*inoutIterator].Name.length());
  *inoutIterator++;
  return IoError_OK;
}

IoError CpkArchive::EnumerateStart(uint32_t* outIterator,
                                   VfsFileInfo* outFileInfo) {
  *outIterator = 0;
  return EnumerateNext(outIterator, outFileInfo);
}

IoError CpkArchive::DriverCanStream(uint32_t id, bool* outResult) {
  auto it = TOC.find(id);
  if (it == TOC.end()) {
    ImpLog(LL_Debug, LC_IO, "CPK CanStream: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  }
  if (it->second.CompressedSize)
    *outResult = false;
  else
    *outResult = true;
  return IoError_OK;
}

uint16_t get_next_bits(char* input, int* offset_p, uint8_t* bit_pool_p,
                       int* bits_left_p, int bit_count) {
  uint16_t out_bits = 0;
  int num_bits_produced = 0;
  int bits_this_round;

  while (num_bits_produced < bit_count) {
    if (*bits_left_p == 0) {
      *bit_pool_p = input[*offset_p];
      *bits_left_p = 8;
      *offset_p = *offset_p - 1;
    }

    if (*bits_left_p > (bit_count - num_bits_produced))
      bits_this_round = bit_count - num_bits_produced;
    else
      bits_this_round = *bits_left_p;

    out_bits <<= bits_this_round;

    out_bits |= *bit_pool_p >> (*bits_left_p - bits_this_round) &
                ((1 << bits_this_round) - 1);

    *bits_left_p -= bits_this_round;
    num_bits_produced += bits_this_round;
  }

  return out_bits;
}

IoError CpkArchive::DriverSlurp(uint32_t id, void* outBuffer) {
  auto it = TOC.find(id);
  if (it == TOC.end()) {
    ImpLog(LL_Debug, LC_IO, "CPK Slurp: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  } else {
    CpkTocEntry* entry = &TOC[id];
    int64_t fileCur = entry->Offset;
    int64_t fileEnd = entry->Offset + entry->CompressedSize;

    if (SDL_RWseek(BaseStream, fileCur, RW_SEEK_SET) == -1) {
      ImpLog(LL_Error, LC_IO,
             "Reading %" PRIu64 " decompressed bytes at %" PRId64
             " from open archive \"%s\" failed (seek error)\n",
             entry->UncompressedSize, fileCur, MountPoint);
      return IoError_Fail;
    }

    char* input = (char*)malloc(entry->CompressedSize);
    SDL_RWread(BaseStream, input, 1, entry->CompressedSize);
    SDL_RWseek(BaseStream, fileCur, RW_SEEK_SET);

    SDL_RWseek(BaseStream, 8, RW_SEEK_CUR);
    int uncompressed_size = SDL_ReadLE32(BaseStream);
    int uncompressed_header_offset = SDL_ReadLE32(BaseStream);
    SDL_RWseek(BaseStream, uncompressed_header_offset, RW_SEEK_CUR);
    SDL_RWread(BaseStream, outBuffer, 1, 0x100);

    int input_end = (entry->CompressedSize - 0x100 - 1);
    int input_offset = input_end;
    int output_end = 0x100 + uncompressed_size - 1;
    uint8_t bit_pool = 0;
    int bits_left = 0, bytes_output = 0;
    int vle_lens[4] = {2, 3, 5, 8};

    while (bytes_output < uncompressed_size) {
      if (get_next_bits(input, &input_offset, &bit_pool, &bits_left, 1) > 0) {
        int backreference_offset =
            output_end - bytes_output +
            get_next_bits(input, &input_offset, &bit_pool, &bits_left, 13) + 3;
        int backreference_length = 3;
        int vle_level;

        for (vle_level = 0; vle_level < 4; vle_level++) {
          int this_level = get_next_bits(input, &input_offset, &bit_pool,
                                         &bits_left, vle_lens[vle_level]);
          backreference_length += this_level;
          if (this_level != ((1 << vle_lens[vle_level]) - 1)) break;
        }

        if (vle_level == 4) {
          int this_level;
          do {
            this_level =
                get_next_bits(input, &input_offset, &bit_pool, &bits_left, 8);
            backreference_length += this_level;
          } while (this_level == 255);
        }

        for (int i = 0; i < backreference_length; i++) {
          ((char*)outBuffer)[output_end - bytes_output] =
              ((char*)outBuffer)[backreference_offset--];
          bytes_output++;
        }
      } else {
        // verbatim byte
        ((char*)outBuffer)[output_end - bytes_output] = (uint8_t)get_next_bits(
            input, &input_offset, &bit_pool, &bits_left, 8);
        bytes_output++;
      }
    }
    free(input);
    return IoError_OK;
  }
}

void CpkRegisterDriver() {
  if (!registered) {
    VfsArchive::RegisterDriver(CpkMount);
    registered = true;
    ImpLog(LL_Info, LC_IO, "Registered CPK VFS driver\n");
  } else {
    ImpLog(LL_Warning, LC_IO, "CPK VFS driver registered twice\n");
  }
}

}  // namespace Impacto