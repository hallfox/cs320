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
  DirectCache part_1[] = {
    DirectCache(1),
    DirectCache(4),
    DirectCache(16),
    DirectCache(32),
  };
  Cache part_2[] = {
    Cache(16, 2),
    Cache(16, 4),
    Cache(16, 8),
    Cache(16, 16),
  };
  FullCache part_3[] = {
    FullCache(16),
    FullCache(16, CACHE_CONFIG_HOTCOLD),
  };
  Cache part_4[] = {
    Cache(16, 2, CACHE_CONFIG_WRITETHRU),
    Cache(16, 4, CACHE_CONFIG_WRITETHRU),
    Cache(16, 8, CACHE_CONFIG_WRITETHRU),
    Cache(16, 16, CACHE_CONFIG_WRITETHRU),
  };
  Cache part_5[] = {
    Cache(16, 2, CACHE_CONFIG_PREFETCH),
    Cache(16, 4, CACHE_CONFIG_PREFETCH),
    Cache(16, 8, CACHE_CONFIG_PREFETCH),
    Cache(16, 16, CACHE_CONFIG_PREFETCH),
  };
  Cache part_6[] = {
    Cache(16, 2, CACHE_CONFIG_PREFETCH_MISS),
    Cache(16, 4, CACHE_CONFIG_PREFETCH_MISS),
    Cache(16, 8, CACHE_CONFIG_PREFETCH_MISS),
    Cache(16, 16, CACHE_CONFIG_PREFETCH_MISS),
  };
  std::vector<Cache*> tests = { part_1, part_2, part_3, part_4, part_5, part_6 };
  int test_sizes[] = {4, 4, 2, 4, 4, 4};
  while (input_trace >> op >> std::hex >> data) {
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < test_sizes[i]; j++) {
        tests[i][j].put_addr(op, data);
      }
    }
    lines++;
  }
  input_trace.close();

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < test_sizes[i]; j++) {
      std::cout << tests[i][j].get_hits() << ", " << lines << "; ";
      if (i == 2 && j == 0) {
        std::cout << "\n";
      }
    }
    std::cout << "\n";
  }

  output.close();

  return 0;
}
