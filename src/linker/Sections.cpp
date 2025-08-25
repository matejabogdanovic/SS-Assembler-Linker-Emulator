#include "../../inc/linker/Sections.hpp"


bool Sections::put(FileState* file,std::string* section_name, SymbolTable::Entry* section, uint32_t* start_address){
  Memory* memory = &file->memory;
  RelTable* rel = &file->rel;
  SymbolTable* symtab = &file->symtab;
  
  Sections::SectionsUnion sec_union;
  bool found = false;
  uint32_t free_address = 0;
 
  for (auto iter = map.begin(); iter != map.end() ;iter++){ // find your section union
     
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

  if(!found){
    sec_union.order = globalOrder++;
    if(!start_address)
      free_address = getFreeAddress(section->size);
    
  }


  sec_union.name = *section_name;
  sec_union.start_address = (start_address?*start_address:free_address);
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

uint32_t  Sections::getFreeAddress(uint32_t section_sz){
  if(map.size()==0)return 0;
  
  auto curr = map.begin();
  for (; curr != map.end() ;curr++){
    if(curr == map.begin())continue;
    auto prev = std::prev(curr);
    if(curr->start_address - (prev->start_address+prev->size) >= section_sz ){
      return prev->start_address+prev->size;
    }
  }
  

  
  curr--;
  if(0xffffffff-(curr->start_address+curr->size)  >= section_sz)
      return curr->start_address+curr->size;
  else  
    {
        std::cerr << "No more space for new section." << std::endl;
        exit(-1);
    }
  
  
}


void Sections::reorderSections(){
   for (auto curr = map.begin(); curr != map.end() ;curr++){
    if(curr == map.begin())continue;
    auto prev = std::prev(curr);
    if(prev->start_address + prev->size <= curr->start_address)
      continue;
    // this section needs to move
    if(curr->fixed){ // can't move sorry
      std::cerr << "Sections "<< prev->name << " and " << curr->name  <<" are overlapping." << std::endl;
      exit(-1);
    }
    SectionsUnion updated = *curr;
    updated.start_address = prev->start_address + prev->size;
    map.erase(curr);
    map.insert(updated);
    
  }
}