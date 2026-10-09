#ifndef _RSTL_STRINGEXTRAS
#define _RSTL_STRINGEXTRAS

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

class CStringExtras {
public:
  static int IndexOfSubstring(const rstl::string&, const rstl::string&);
  static int CompareCaseInsensitive(const rstl::string&, const rstl::string&);
  static char ConvertToUpperCase(char c);
  static int ConvertToLowerCase(int c);
  static rstl::string ConvertToLowerCase(const rstl::string& str);
  static rstl::string CreatePrefix(const rstl::string& str, int count);
  static rstl::string CreateFromInteger(int v);
  // 0x80505D30, named by the demo map (CreateFromReal__13CStringExtrasFfi): "%.<precision>f"
  // with the precision clamped to 0..12.
  static rstl::string CreateFromReal(float v, int precision);
  // Guessed name: the inverse of CreateFromInteger; asserts kException_NotInt on bad input.
  static int ConvertToInteger(const rstl::string& str);
  static rstl::string ConvertToANSI(const rstl::wstring& str);
  static rstl::wstring ConvertToUNICODE(const rstl::string& str);
  static rstl::string ReadString(CInputStream& in);
  // Guessed name and class (its neighbours in RstlExtras.cpp are CStringExtras). 0x80505428
  // formats through CBasics' va_list Stringize and returns the result as a string.
  static rstl::string Format(const char* fmt, ...);
  static rstl::vector< rstl::string > TokenizeString(const rstl::string&, const char*, int);
};

#endif // _RSTL_STRINGEXTRAS
