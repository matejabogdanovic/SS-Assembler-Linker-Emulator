#pragma once
#include <string>
#include <cstdint>
#include <map>
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

    ABSOLUTE = 0b00000001
  } Flags;
  
  SymbolTable(bool init = true);

  void print(std::ostream& stream);
  void printBinary(std::ostream& os);
  void loadFromFile(std::istream& is);
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
  static bool isDefined(Entry* e) ;
  static bool isExtern(Entry* e) ;
  static bool isAbsolute(uint8_t flags) ;
  bool doesSymbolExist(std::string* name) const;
  bool sectionOpened() const; 
  bool doesSectionExist(std::string* name) const;
  
  // getters
  std::string getCurrentSectionName() const;
  std::string getUndefinedSectionName() const;
  Entry* getCurrentSection();
  uint32_t getSectionStart(uint32_t ndx);
  inline size_t getNumOfSections(){
    return section_names.size();
  }
  inline size_t getNumOfSymbols(){
    return symbol_names.size();
  }

  // gets first symbol/section not all duplicates
  Entry* getSection(std::string* name);
  Entry* getSymbol(std::string* name);
  Entry* getSection(uint32_t num_or_ndx);
  // gets specific symbol with that number even if there are duplicates
  Entry* getSymbol(uint32_t num);
  

  std::string getSymbolName(Entry* e) const;
  std::string getSectionName(Entry* e) const;
  std::string getSymbolName(uint32_t num) const;
  std::string getSectionName(uint32_t num_or_ndx) const;
  
  void addSymbolNoDuplicates(std::string* name, Entry e);
  void addSymbol(std::string* name, Entry e);
  void addSection(std::string* name, Entry e);

  
  typedef std::multimap <std::string, Entry> Map;
  Map symbols;
  Map sections;

  uint32_t current_section = 0;
  static const uint32_t UNDEFINED_SECTION = 0;

  

  void printEntry(std::string* name, Entry* e, std::ostream& os) const;
  private:
  static const char* type_str[];
  static const char* bind_str[];
  
  
  void printPartBinary(std::string* name, Entry* e, std::ostream& os) const;

  private:
    std::vector <std::string> section_names;
  std::vector <std::string> symbol_names;

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