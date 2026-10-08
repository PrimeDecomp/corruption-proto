#ifndef _CSTRINGTOKENIZER
#define _CSTRINGTOKENIZER

#include "rstl/string.hpp"

// Guessed names. The CStringTokenizer.cpp split name is itself inferred. The tokenizer only
// holds a cursor into the input text and skips whitespace between tokens.
class CStringTokenizer {
public:
  CStringTokenizer(const char* input);

  // Reads the next token; a token starting with quote runs to the closing quote and sets
  // *quoted to 1.
  rstl::string ReadToken(int* quoted, char quote);
  bool IsExhausted() const;
  const char* GetRemainingText() const;

private:
  const char* mInput;
};

#endif // _CSTRINGTOKENIZER
