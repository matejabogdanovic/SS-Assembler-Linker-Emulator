#include "../../inc/linker/Sections.hpp"


bool Sections::put(FileState* file,std::string* section_name, SymbolTable::Entry* section, uint32_t start_address){
  Memory* memory = &file->memory;
  RelTable* rel = &file->rel;
  SymbolTable* symtab = &file->symtab;
  
  Sections::SectionsUnion* sec_union;
  bool found = false;
  for (Sections::SectionsUnion& secu: map){ // find your section union
      
    if(secu.name == symtab->getSectionName(section)){
      // check for size todo
      found = true;
      sec_union = &secu;

      std::cout << "To union: " << secu.name
      << " added new section: "; 
      symtab->printEntry(section_name, section, std::cout);  
      
      break;
    }
  }
  if(!found){
    Sections::SectionsUnion new_union;
    sec_union = &new_union;
  }

  sec_union->name = *section_name;
  sec_union->start_address = start_address;
  sec_union->size += section->size;
  sec_union->sections.push_back({file, section});

  if(!found){
    map.emplace_back(*sec_union);
    //defined_syms[*section_name] = start_address;
    std::cout << "Created union: " << sec_union->name
      << " added new section: "; 
      symtab->printEntry(section_name, section, std::cout); 
  }
    
  return !found;

}