/*
 * G2MEAB IObj.cpp translation-unit scaffold (NonMatching).
 * .text: 0x80508574..0x80508A74 (end exclusive), 8 native functions.
 * Includes the 64-bit asset-ID API, private decimal parser, FourCC converter,
 * SObjectTag::Type2Text and shared invalid-ID/object-tag initializer.
 * Shared initialization supports this inferred original-file extension.
 * Remaining: the FourCC converter, SObjectTag::Type2Text and the static initializer.
 */

#include "Kyoto/CAssetId.hpp"

#include "Kyoto/Streams/CInputStream.hpp"
#include "Kyoto/Streams/COutputStream.hpp"

#include <stdio.h>

rstl::string CAssetId::ToHexString() const {
  char buf[20];
  sprintf(buf, "0x%8.8X%8.8X", static_cast< uint >(mId >> 32), static_cast< uint >(mId));
  return rstl::string(buf);
}

void CAssetId::PutTo(COutputStream& out) const {
  unsigned long long id = mId;
  out.Put(&id, sizeof(id));
}

CAssetId::CAssetId(CInputStream& in) : mId(in.ReadInt64()) {}

CAssetId::CAssetId(const char* str) {
  if (str[0] != '\0' && str[1] != '\0' && str[0] == '0' && str[1] == 'x') {
    unsigned long long id = 0;
    for (const char* c = str + 2; *c != '\0'; ++c) {
      if (*c >= '0' && *c <= '9') {
        id = (id << 4) | (*c - '0');
      } else if (*c >= 'A' && *c <= 'F') {
        id = (id << 4) | (*c - 'A' + 10);
      } else if (*c >= 'a' && *c <= 'f') {
        id = (id << 4) | (*c - 'a' + 10);
      } else {
        break;
      }
    }
    mId = id;
  } else {
    mId = ParseDecimalString(str);
  }
}

long long CAssetId::ParseDecimalString(const char* str) {
  long long value = 0;
  int sign = 1;
  while (*str == ' ' || *str == '\t') {
    ++str;
  }
  if (*str == '-') {
    sign = -1;
  }
  while (*str != '\0') {
    if (*str >= '0' && *str <= '9') {
      value = value * 10 + *str++ - '0';
    } else {
      ++str;
    }
  }
  return sign * value;
}
