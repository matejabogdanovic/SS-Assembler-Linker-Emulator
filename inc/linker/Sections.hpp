#include "./FileState.hpp"
#include <list>
class Sections{
public:
  struct Section{
    FileState* file;
    SymbolTable::Entry* section;
  };

  struct SectionsUnion{
    std::string name = ""; 
    uint32_t start_address = 0;
    uint64_t size = 0;
    std::vector <Section> sections; // aggregate    
  };

  std::list <SectionsUnion> map;

  // returns true if new section union is made
  bool put(FileState* file,std::string* section_name, SymbolTable::Entry* section,  uint32_t start_address);
};