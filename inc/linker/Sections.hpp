#include "./FileState.hpp"
#include "../common/Macros.hpp"
#include "./LinkerException.hpp"
#include <set>

class Sections{
public:
  struct Section{
    FileState* file;
    SymbolTable::Entry* section;
    
  };
  int globalOrder = 0;
  struct SectionsUnion{
    int order;
    std::string name = ""; 
    uint32_t start_address = 0;
    bool fixed = false;
    uint32_t size = 0;
    std::vector <Section> sections; // aggregate    

    
  };
  struct SortedByStartAddress {
    bool operator()(const SectionsUnion& a, const SectionsUnion& b) const {
      if(a.start_address == b.start_address)
        return a.order < b.order;
      return a.start_address < b.start_address;
    }
  };
  std::multiset <SectionsUnion, SortedByStartAddress> map;

  // returns true if new section union is made
  bool put(FileState* file,std::string* section_name, SymbolTable::Entry* section, 
     uint32_t* start_address=nullptr, bool can_overlap = false);
  void printHex(std::ostream& os);
  void printBinary(std::ostream& os);
  void printBinaryRelocatable(std::ostream& os) ;

  uint32_t getSubsectionLocalOffset(std::string* section, FileState* file);
  const SectionsUnion* getSectionsUnion(std::string* section) const;
  
private:
void reorderSections();
uint32_t getFreeAddress(uint32_t section_sz);

};