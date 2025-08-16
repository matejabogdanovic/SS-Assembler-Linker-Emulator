#pragma once
#include "../common/Memory.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
class LiteralPool{
public:


  void put(uint32_t value, uint32_t location_to_patch, bool invalid = false);
  // void literalBackpatch(Memory* memory, uint32_t literalPoolStartAddr);
  bool isInvalid(uint32_t location);
  size_t size() const;

  typedef struct {
    uint32_t location;
    uint32_t index_of_literal; // index in pool
  }LiteralPatch;
  std::list <LiteralPatch> literalpatch; 
  std::vector <uint32_t> pool; // how pool is in memory
  std::unordered_map <uint32_t, bool> isInvalidLocation; // locations that are invalid

  // map to check for literal repetition
  // std::unordered_map <uint32_t, uint32_t> literalTable; // key = literal, value = index in pool 
};