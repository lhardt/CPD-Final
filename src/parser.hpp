#ifndef PARSER_H
#define PARSER_H


#include "types.hpp"

class CSVParser {
public:
  CSVParser(std::string filename);

  Player consumePlayerLine();
};

#endif /* PARSER_H */
