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

  typedef enum {
    OK,
    ERROR
  } Status;

  static void printTable();

  static Status addSymbol(uint32_t value,
    Type type,
    Bind bind,
    std::string name);

  typedef struct {
    uint32_t num;
    uint32_t value;
    uint32_t size;
    Type type;
    Bind bind;
    uint32_t ndx;
  } Entry;
  
  typedef std::unordered_map <std::string, Entry> Map;
  static Map table;
  static uint32_t current_section;
  static std::vector <std::string> sym_names;

  static Status incLC(uint32_t val);
  static uint32_t getLC();
  
  static Status end();
  
private:
  static const char* type_str[];
  static const char* bind_str[];
  static uint32_t LC;
  static Map initMap();

  
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