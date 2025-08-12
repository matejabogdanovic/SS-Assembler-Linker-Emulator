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
  typedef enum : uint8_t  {
    DEFINED = 0b00000001,
    EXTERN = 0b00000010,
    ABSOLUTE = 0b00000100
  } Flags;
  
  SymbolTable();

  void printTable();

   typedef struct Entry {
    uint32_t offset;
    Bind bind;
    std::string section;
    uint8_t flags;
    Type type;

    std::uint32_t ndx;
    uint32_t size;
    uint32_t num;
    Entry(){

    }
    
    Entry(
    uint32_t offset,
    Bind bind,
    std::string section,
    uint8_t flags = 0,
    Type type = NOTYP,
    std::uint32_t ndx = 0,
    uint32_t size = 0,
    uint32_t num = 0
  ) : offset(offset), bind(bind), section(section), flags(flags),
    type(type), ndx(ndx),size(size), num(num)
    {

    }

  } Entry;


  static bool isDefined(uint8_t flags) ;
  static bool isExtern(uint8_t flags) ;
  static bool isAbsolute(uint8_t flags) ;

  bool sectionOpened() const;

  std::string getCurrentSectionName() const;
  std::string getUndefinedSectionName() const;
  Entry* getCurrentSection();

  bool doesSymbolExist(std::string* name) const;
  void addSymbol(std::string* name, Entry e);
  Entry* getSymbol(std::string* name);
  Entry* getSection(std::string* name);

  bool doesSectionExist(std::string* name) const;
  void addSection(std::string* name, Entry e);

  typedef std::unordered_map <std::string, Entry> Map;
  Map symbols;
  Map sections;

  uint32_t current_section = 0;
  std::vector <std::string> section_names;
  std::vector <std::string> symbol_names;



  

private:
  static const char* type_str[];
  static const char* bind_str[];

  void printTablePart(std::string* name, Entry* e) const;
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