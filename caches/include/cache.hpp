#ifndef _CACHE_H
#define _CACHE_H

#include <vector>
#include <list>

#define CACHE_COLD -1

typedef unsigned int uint;

typedef struct CacheConfig {
  bool writethru_on_miss;
  bool prefetch;
  bool prefetch_on_miss;
  bool hot_cold_sim;
  CacheConfig(bool w, bool p, bool pm, bool hc):
    writethru_on_miss(w), prefetch(p), prefetch_on_miss(pm), hot_cold_sim(hc)
  {}
} CacheConfig;

static const CacheConfig CACHE_CONFIG_DEFAULT = CacheConfig(false, false, false, false);
static const CacheConfig CACHE_CONFIG_HOTCOLD = CacheConfig(false, false, false, true);
static const CacheConfig CACHE_CONFIG_WRITETHRU = CacheConfig(true, false, false, false);
static const CacheConfig CACHE_CONFIG_PREFETCH = CacheConfig(false, true, false, false);
static const CacheConfig CACHE_CONFIG_PREFETCH_MISS = CacheConfig(false, false, true, false);

class Set {
protected:
  std::vector<uint> ways;
  CacheConfig config;
public:
  Set(int n_ways, CacheConfig conf):
    ways(std::vector<uint>(n_ways, CACHE_COLD)),
    config(conf)
  {}
  virtual ~Set() {}
  int find_block(uint addr);
  bool has_block(uint addr);
  virtual void put_block(char op, uint addr) = 0;
  void prefetch(uint addr);
};

class LRUSet: public Set {
private:
  std::list<int> lru_order;
public:
  LRUSet(int n_ways, CacheConfig conf):
    Set(n_ways, conf),
    lru_order(std::list<int>(n_ways))
  {
    for (int i = 0; i < n_ways; i++) {
      lru_order.push_front(i);
    }
  }
  virtual void put_block(char op, uint addr) override;
};

class HotColdSet: public Set {
private:
  // Hot cold structure
  std::vector<bool> hot_cold;
  void set_hot(int index);
  int get_cold();
public:
  HotColdSet(int n_ways, CacheConfig conf):
    Set(n_ways, conf),
    hot_cold(std::vector<bool>(n_ways-1))
  {}
  virtual void put_block(char op, uint addr) override;
};

class Cache {
private:
  int hits;
protected:
  std::vector<Set*> sets;
  CacheConfig config;
public:
  // Size is in kilobytes
  Cache(int size, int ways, CacheConfig conf=CACHE_CONFIG_DEFAULT);
  ~Cache();
  int get_hits() {return hits;}
  void put_addr(char, uint);
};

class DirectCache: public Cache {
public:
  // Size is in kilobytes
  DirectCache(int size, CacheConfig conf=CACHE_CONFIG_DEFAULT): Cache(size, 1, conf) {}
};

class FullCache: public Cache {
public:
  FullCache(int size, CacheConfig conf=CACHE_CONFIG_DEFAULT): Cache(size, 32*size, conf) {}
};

#endif
