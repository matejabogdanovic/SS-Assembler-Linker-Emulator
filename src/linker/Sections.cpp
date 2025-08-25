#include "../../inc/linker/Sections.hpp"


bool Sections::put(FileState* file,std::string* section_name, SymbolTable::Entry* section, uint32_t* start_address){
  Memory* memory = &file->memory;
  RelTable* rel = &file->rel;
  SymbolTable* symtab = &file->symtab;
  
  Sections::SectionsUnion sec_union;
  bool found = false;

  uint64_t last_address = 0;
  for (auto iter = map.begin(); iter != map.end() ;iter++){ // find your section union
    last_address = iter->start_address + iter->size;   
    if(iter->name == symtab->getSectionName(section)){
        // check for size todo
        found = true;
        sec_union = *iter;
        map.erase(iter);

        std::cout << "To union: " << sec_union.name
        << " adding new section: "; 
        symtab->printEntry(section_name, section, std::cout);  
        break;
    }
    
  }

  if(!found)sec_union.order = globalOrder++;
  sec_union.name = *section_name;
  sec_union.start_address = (start_address?*start_address:last_address);
  sec_union.fixed = start_address != nullptr; 
  sec_union.size += section->size;
  sec_union.sections.push_back({file, section});
  map.insert(sec_union);

  if(!found){
    //defined_syms[*section_name] = start_address;
    std::cout << "Created union: " << sec_union.name
      << " added new section: "; 
      symtab->printEntry(section_name, section, std::cout); 
  }
    

  reorderSections();

  return !found;

}

void Sections::reorderSections(){
   for (auto iter = map.begin(); iter != map.end() ;iter++){
    if(iter != map.begin()){
      auto prev = std::prev(iter);
      if(prev->start_address + prev->size <= iter->start_address)
        continue;
      // this section needs to move
      if(iter->fixed){ // can't move sorry
        std::cerr << "Sections "<< prev->name << " and " << iter->name  <<" are overlapping." << std::endl;
        exit(-1);
      }
      SectionsUnion updated = *iter;
      updated.start_address = prev->start_address + prev->size;
      map.erase(iter);
      map.insert(updated);
    }
  }
}