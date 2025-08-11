#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>

class SymbolTable {
public:
  typedef enum {
    NOTYP, SCTN
  } Type;
  typedef enum {
    LOC, GLOB
  } Bind;

  SymbolTable();

  void printTable();

  typedef struct {
    uint32_t num;
    uint32_t value;
    uint32_t size;
    Type type;
    Bind bind;
    std::string section;
    std::uint32_t ndx;
    bool is_extern;
    bool is_defined;
  } Entry;
  
  typedef std::unordered_map <std::string, Entry> Map;
  Map table;

  std::vector <std::string> sections;
  std::vector <std::string> symbols;

  // static Status incLC(uint32_t val);
  // static uint32_t getLC();
  
  // static Status end();
  // bool sectionOpened() const;
  bool doesSymbolExist(std::string* name) const;
  // static bool closeSection(
  //   std::string prev_section, 
  //   uint32_t prev_section_sz);
private:
  static const char* type_str[];
  static const char* bind_str[];
  
};

// Num Value Size Type Bind Ndx Name
// 0: 00000000 0 NOTYP LOC UND
// 1: 00000000 0 SCTN LOC 1 .text
// 2: 00000000 0 SCTN LOC 2 .data
// 3: 00000000 0 SCTN LOC 3 .bss
// 4: 00000008 0 NOTYP LOC 2 e
// 5: 00000014 0 NOTYP LOC 1 d
// 6: 00000014 0 NOTYP GLOB 2 a
// 7: 00000000 0 NOTYP GLOB 3 c
// 8: 00000000 0 NOTYP GLOB UND b