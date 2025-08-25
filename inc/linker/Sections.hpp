#include "./FileState.hpp"
#include <set>

class Sections{
public:
  struct Section{
    FileState* file;
    SymbolTable::Entry* section;
  };

  struct SectionsUnion{
    std::string name = ""; 
    uint32_t start_address = 0;
    bool fixed = false;
    uint64_t size = 0;
    std::vector <Section> sections; // aggregate    
  };
  struct SortedByStartAddress {
    bool operator()(const SectionsUnion& a, const SectionsUnion& b) const {
        return a.start_address < b.start_address;
    }
  };
  std::set <SectionsUnion, SortedByStartAddress> map;

  // returns true if new section union is made
  bool put(FileState* file,std::string* section_name, SymbolTable::Entry* section,  uint32_t* start_address=nullptr);
};