#ifndef _CACHE_H
#define _CACHE_H

#include <vector>

#define CACHE_COLD -1

class DirectCache {
private:
  /** Cache line format: 32 bytes, address space: 32 bits
      Tag[32-log(size)-10] Index[log(size)-5] Offset[5]
   **/
  int sets;
  int hits;
  std::vector<unsigned int> blocks;

public:
  // Size is in kilobytes
  DirectCache(int size);
  bool get_addr(unsigned int addr);
  int get_hits();
};

#endif
