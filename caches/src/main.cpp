#include <iostream>
#include <fstream>

#include "cache.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "ERROR: command usage - ./cache-sim input_trace.txt output.txt\n";
    exit(EXIT_FAILURE);
  }

  std::ifstream input_trace(argv[1]);
  std::ofstream output(argv[2]);

  unsigned int data;
  char op;
  int lines = 0;
  DirectCache caches[] = {DirectCache(1),
                          DirectCache(4),
                          DirectCache(16),
                          DirectCache(32)};
  while (input_trace >> op >> std::hex >> data) {
    for (auto& c: caches) {
      c.get_addr(data);
    }
    lines++;
  }
  input_trace.close();

  for (auto& c: caches) {
    std::cout << c.get_hits() << ", " << lines << "; ";
  }

  output.close();

  return 0;
}
