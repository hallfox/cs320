#include "cache.hpp"
#include <cmath>

/** Cache implementations **/

DirectCache::DirectCache(int size) {
  // Kilobytes is 2^10, so set # = size * 32
  sets = size * 32;
  // Construct a "cold" cache
  blocks = std::vector<unsigned int>(sets, CACHE_COLD);
  hits = 0;
}

bool DirectCache::get_addr(unsigned int addr) {
  int index_bits = static_cast<int>(std::log2(sets));
  unsigned int index, tag;

  index = (addr >> 5) & ~(-1 << index_bits);
  tag = (addr >> (5+index_bits)) & ~(-1 << (27-index_bits));

  if (blocks[index] != tag) {
    blocks[index] = tag;
    return false;
  }
  hits++;
  return true;
}

int DirectCache::get_hits() {
  return hits;
}
