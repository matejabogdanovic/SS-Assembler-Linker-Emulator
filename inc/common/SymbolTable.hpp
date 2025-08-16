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

  void printTable(std::ostream& stream);

   typedef struct Entry {
    uint32_t offset;
    Bind bind;
    std::uint32_t ndx;
   
    uint8_t flags;
    Type type;

    uint32_t size;
    uint32_t num;
    Entry(){

    }
    
   // Entry(const Entry&) = default;

    Entry(
    uint32_t offset,
    Bind bind,
    std::uint32_t ndx = 0,
    uint8_t flags = 0,
    Type type = NOTYP,
    uint32_t size = 0,
    uint32_t num = 0
  ) : offset(offset), bind(bind),  flags(flags),
    type(type), ndx(ndx),size(size), num(num)
    {

    }

  } Entry;

  // checks
  static bool isDefined(uint8_t flags) ;
  static bool isExtern(uint8_t flags) ;
  static bool isAbsolute(uint8_t flags) ;
  bool doesSymbolExist(std::string* name) const;
  bool sectionOpened() const; 
  bool doesSectionExist(std::string* name) const;
  
  // getters
  std::string getCurrentSectionName() const;
  std::string getUndefinedSectionName() const;
  Entry* getCurrentSection();
  uint32_t getSectionStart(uint32_t ndx);

  Entry* getSection(std::string* name);
  Entry* getSymbol(std::string* name);
  std::string getSymbolName(Entry* e) const;
  std::string getSectionName(Entry* e) const;

  
  void addSymbol(std::string* name, Entry e);
  void addSection(std::string* name, Entry e);

  
  typedef std::unordered_map <std::string, Entry> Map;
  Map symbols;
  Map sections;

  uint32_t current_section = 0;
  static const uint32_t UNDEFINED_SECTION = 0;
  std::vector <std::string> section_names;
  std::vector <std::string> symbol_names;
  


  private:
  static const char* type_str[];
  static const char* bind_str[];
  
  void printTablePart(std::string* name, Entry* e, std::ostream& os) const;
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