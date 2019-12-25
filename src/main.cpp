#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <limits.h>
#include "lexer.h"
#include "parser.h"

using namespace Lexer;

using std::cout;
using std::endl;
using std::string;
using std::vector;

vector<string> get_file_input(const char *path)
{
  vector<string> content;
  std::ifstream input(path);
  for (std::string line; getline(input, line);)
  {
    content.push_back(line);
  }
  return content;
}

void print_tokens(vector<Token> tokens)
{
  for (int i = 0; i < tokens.size(); i++)
  {
    cout << "[ " << tokens[i].type << ":"
         << " '" << tokens[i].value << "' ]"
         << " - ln:" << tokens[i].line_number << " pos:" << tokens[i].line_position << endl;
  }
}

int main(int argc, char **argv)
{
  char path[PATH_MAX];

  if (argc > 1)
  {
    realpath(argv[1], path);
  }
  else
  {
    std::cerr << "Please supply input file" << endl;
    return 0;
  }

  vector<string> input = get_file_input(path);
  vector<Token> tokens = generate_tokens(input);

  // print_tokens(tokens);

  generate_ast(tokens);

  return 0;
}
