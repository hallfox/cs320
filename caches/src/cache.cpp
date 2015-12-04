#include "cache.hpp"
#include <cmath>
#include <iostream>

/** Set implementation **/

int Set::find_block(uint addr) {
  for (int i = 0; i < ways.size(); i++) {
    if (ways[i] == addr) {
      return i;
    }
  }
  return -1;
}

bool Set::has_block(uint addr) {
  return find_block(addr) != -1;
}

void LRUSet::put_block(char op, uint addr) {
  // for (auto it = lru_order.cbegin(); it != lru_order.cend(); it++) {
  //   std::cout << *it << ", ";
  // }
  // std::cout << "; ";
  int index = find_block(addr);
  if (index != -1) {
    // Hit, update LRU list
    lru_order.remove(index);
    lru_order.push_front(index);
  }
  else if (!config.writethru_on_miss || op != 'S') {
    // Miss, find LRU
    int deleted = lru_order.back();
    // Replace
    lru_order.pop_back();
    lru_order.push_front(deleted);
    ways[deleted] = addr;
  }
  // for (auto it = lru_order.cbegin(); it != lru_order.cend(); it++) {
  //   std::cout << *it << ", ";
  // }
  // std::cout << std::endl;
}

int HotColdSet::get_cold() {
  int size = hot_cold.size();
  int start = 0, end = size;
  int loc;
  for (loc = (start+end)/2; loc != start && loc != end; loc = (start+end)/2) {
    if (!hot_cold[loc]) {
      start = loc;
    }
    else {
      end = loc;
    }
  }
  if (hot_cold[loc]) {
    return loc;
  }
  else {
    return loc + 1;
  }
}

void HotColdSet::set_hot(int index) {
  int size = hot_cold.size();
  int start = 0, end = size;
  int loc;
  for (loc = (start+end)/2; start+1 != end; loc = (start+end)/2) {
    if (loc < index) {
      start = loc;
      hot_cold[loc] = true;
    }
    else {
      end = loc;
      hot_cold[loc] = false;
    }
  }
  if (loc < index) {
    hot_cold[loc] = true;
  }
  else {
    hot_cold[loc] = false;
  }
}

void HotColdSet::put_block(char op, uint addr) {
  int index = find_block(addr);
  if (index != -1) {
    // Hit, update LRU list
    set_hot(index);
  }
  else if (!config.writethru_on_miss || op != 'S') {
    // Miss, find LRU
    int deleted = get_cold();
    // Replace
    set_hot(deleted);
    ways[deleted] = addr;
  }
}

/** Cache implementations **/

Cache::Cache(int size, int ways, CacheConfig conf):
  hits(0),
  config(conf)
{
  // Kilobytes is 2^10, so set # = size * 2^10 / (32*ways) = 32 * size/ways
  int n_sets = 32 * size / ways;
  if (config.hot_cold_sim) {
    sets = std::vector<Set*>(n_sets);
    for (int i = 0; i < n_sets; i++) {
      sets[i] = new HotColdSet(ways, config);
    }
  }
  else {
    sets = std::vector<Set*>(n_sets);
    for (int i = 0; i < n_sets; i++) {
      sets[i] = new LRUSet(ways, config);
    }
  }
}

Cache::~Cache() {
  for (auto& s: sets) {
    if (s != nullptr) {
      delete s;
    }
  }
}

void Cache::put_addr(char op, uint addr) {
  uint index_bits = static_cast<uint>(std::log2(sets.size()));
  uint index, tag;

  index = (addr >> 5) & ~(-1 << index_bits);
  tag = (addr >> (5+index_bits)) & ~(-1 << (27-index_bits));

  bool is_hit = sets[index]->has_block(tag);
  if (is_hit) {
    hits++;
  }
  sets[index]->put_block(op, tag);
  if (config.prefetch || (!is_hit && config.prefetch_on_miss)) {
    int next_index = (index+1)%sets.size();
    sets[next_index]->put_block(op, next_index != 0 ? tag : tag+1);
 }
}
