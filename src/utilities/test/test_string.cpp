/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   test_string.cpp
 * @author Bruce Palmer
 * @date   2015-05-20 07:38:59 d3g096
 * 
 * @brief  A simple test of the string utility class
 * 
 * 
 */

#include <cstdio>
#include "string_utils.hpp"

int main(int argc, char **argv)
{
  gridpack::utility::StringUtils util;
  std::string str = "  Test string   ";
  printf("Testing trim function\n");
  util.trim(str);
  printf("Expecting: (Test string) found: (%s)\n",str.c_str());
  str = "upper case";
  printf("\nTesting toUpper function\n");
  util.toUpper(str);
  printf("Expecting: (UPPER CASE) found: (%s)\n",str.c_str());
  str = "LOWER CASE";
  printf("\nTesting toLower function\n");
  util.toLower(str);
  printf("Expecting: (lower case) found: (%s)\n",str.c_str());
  printf("\nTesting blank tokenizer\n");
  str = " token1 token2 \" quoted token3 \" token4 \' quoted token5\' ";
  std::vector<std::string> tokens = util.blankTokenizer(str);
  if (tokens.size() != 5) {
    printf("Error: expected 5 tokens, got %d\n",static_cast<int>(tokens.size()));
  } else {
    printf("\nNumber of tokens from first string ok\n");
  }
  printf("\nListing tokens\n");
  for (int i=0; i<tokens.size(); i++) {
    if (i==0) {
      printf("expected: (token1) found: (%s)\n",tokens[0].c_str());
    } else if (i==1) {
      printf("expected: (token2) found: (%s)\n",tokens[1].c_str());
    } else if (i==2) {
      printf("expected: ( quoted token3 ) found: (%s)\n",tokens[2].c_str());
    } else if (i==3) {
      printf("expected: (token4) found: (%s)\n",tokens[3].c_str());
    } else if (i==4) {
      printf("expected: ( quoted token5) found: (%s)\n",tokens[4].c_str());
    }
  }
  std::string tag;
  printf("\nTesting clean2Char function\n");
  str = "1";
  tag = util.clean2Char(str);
  printf("expected: ( 1) found: (%s)\n",tag.c_str());
  str = " 2 ";
  tag = util.clean2Char(str);
  printf("expected: ( 2) found: (%s)\n",tag.c_str());
  str = " BL ";
  tag = util.clean2Char(str);
  printf("expected: (BL) found: (%s)\n",tag.c_str());
  str = "\' SQ \'";
  tag = util.clean2Char(str);
  printf("expected: (SQ) found: (%s)\n",tag.c_str());
  str = "\" DQ \"";
  tag = util.clean2Char(str);
  printf("expected: (DQ) found: (%s)\n",tag.c_str());

  str = "\" Test String 1 \"";
  tag = util.trimQuotes(str);
  printf("expected: (Test String 1) found: (%s)\n",tag.c_str());
  str = "\'Test String 2 \'";
  tag = util.trimQuotes(str);
  printf("expected: (Test String 2) found: (%s)\n",tag.c_str());
  str = " Test String 3 ";
  tag = util.trimQuotes(str);
  printf("expected: (Test String 3) found: (%s)\n",tag.c_str());
}
