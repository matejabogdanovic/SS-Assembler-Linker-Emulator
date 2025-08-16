#pragma once
#include "../common/Memory.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <list>
class LiteralPool{
public:


  void put(uint32_t value, uint32_t location_to_patch, bool invalid = false);

  bool isInvalid(uint32_t location);


  typedef struct {
    uint32_t location;
    uint32_t literal; 
  }LiteralPatch;
  std::list <LiteralPatch> patches; 

  std::unordered_map <uint32_t, bool> isInvalidLocation; // locations that are invalid

   
};