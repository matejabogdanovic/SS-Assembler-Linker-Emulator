#include "../../inc/linker/Sections.hpp"


bool Sections::put(FileState* file,std::string* section_name, SymbolTable::Entry* section, uint32_t* start_address, bool can_overlap){
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

        LOG(std::cout << "To union: " << sec_union.name
        << " adding new section: "; )
LOG(symtab->printEntry(section_name, section, std::cout);)
        break;
    }
    
  }

  if(!found){
    sec_union.order = globalOrder++;
    if(!start_address && !can_overlap)
      free_address = getFreeAddress(section->size);
    sec_union.start_address = (start_address?*start_address:free_address);
    sec_union.name = *section_name;
    
    sec_union.fixed = start_address != nullptr; 
  }


  sec_union.size += section->size;
  sec_union.sections.push_back({file, section});
  map.insert(sec_union);

  if(!found){
    //defined_syms[*section_name] = start_address;
   LOG( std::cout << "Created union: " << sec_union.name
      << " added new section: "; )
LOG(symtab->printEntry(section_name, section, std::cout);)
  }
    
  if(!can_overlap)
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
  

  
  curr = std::prev(curr);
  if(0xffffffff-(curr->start_address+curr->size)  >= section_sz)
      return curr->start_address+curr->size;
  else  
    {
  
      throw LinkerException("no space");
        
    }
  
  
}

uint32_t Sections::getSubsectionLocalOffset(std::string* section, FileState* file){
  uint32_t offs_inside_union = 0;
  for(const Sections::SectionsUnion& sec_union: map){

    if(sec_union.name != *section)continue;

    for(const Sections::Section& sec: sec_union.sections){
      if(sec.file == file)break;
      offs_inside_union += sec.section->size;
    }
  }
  return offs_inside_union;
}

const Sections::SectionsUnion* Sections::getSectionsUnion(std::string* section) const{
  for(const SectionsUnion& sec_union: map){
    if(sec_union.name == *section){
      return &sec_union;
    }

  }
  return nullptr;
  
}


void Sections::reorderSections(){
   for (auto curr = map.begin(); curr != map.end() ;curr++){
    if(curr == map.begin())continue;
    auto prev = std::prev(curr);
    if(prev->start_address + prev->size <= curr->start_address)
      continue;
    // this section needs to move
    if(curr->fixed && prev->fixed){ // can't move sorry
      throw LinkerException("sections " + prev->name + " and " + curr->name  +" are overlapping");
    }
    if(!curr->fixed){ // curr not fixed or both not fixed
      SectionsUnion updated = *curr;
      updated.start_address = prev->start_address + prev->size;
      map.erase(curr);
      map.insert(updated);
    }else { //if(!prev->fixed) // curr is fixed but previous is not 
      SectionsUnion updated = *prev;
      updated.start_address = curr->start_address + curr->size;
      map.erase(prev);
      map.insert(updated);
    }

    
  }
}


void Sections::printHex(std::ostream& os){
  for(const Sections::SectionsUnion& sec_union: map){
    uint32_t offset = 0;
    os <<"=================Section " << sec_union.name <<" ("<<sec_union.sections.size()<<")=================\n";
    if(sec_union.size == 0)continue;
    // Memory section_union_memory;
    // auto section_union_memory_vector = section_union_memory.getMemoryVector();
    for(const Sections::Section& section: sec_union.sections){
      uint32_t section_start_in_file = section.file->symtab.getSectionStart(section.section->ndx);
      // auto memory = section.file->memory.getMemoryVector();
       
      //section_union_memory_vector.reserve(memory.size()); 
      // section_union_memory_vector->insert(section_union_memory_vector->end(), 
      // memory->begin()+section_start_in_file, 
      // memory->begin()+section_start_in_file+section.section->size);
 

      section.file->memory.print(os, section_start_in_file, section.section->size,
      offset+sec_union.start_address-section_start_in_file);
      
      offset+=section.section->size;
    }
    // section_union_memory.print(std::cout, 0, 0, sec_union.start_address);
    // section_union_memory_vector->empty();
  }
}


void Sections::printBinary(std::ostream& os){
  for(const Sections::SectionsUnion& sec_union: map){
    if(sec_union.size == 0)continue;
    os.write(reinterpret_cast<const char*>(&sec_union.start_address), sizeof(sec_union.start_address));
    os.write(reinterpret_cast<const char*>(&sec_union.size), sizeof(sec_union.size));
    for(const Sections::Section& section: sec_union.sections){
      uint32_t section_start_in_file = section.file->symtab.getSectionStart(section.section->ndx);
      section.file->memory.printBinary(os, 
        section_start_in_file, 
        section.section->size, false);
    }
  }
}


void Sections::printBinaryRelocatable(std::ostream& os){
  uint32_t size = 0;
  for(const Sections::SectionsUnion& sec_union: map){
    size += sec_union.size;
  }
  os.write(reinterpret_cast<const char*>(&size), sizeof(size));
  for(const Sections::SectionsUnion& sec_union: map){
    if(sec_union.size == 0)continue;
     
      
      for(const Sections::Section& section: sec_union.sections){
        uint32_t section_start_in_file = 
        section.file->symtab.getSectionStart(section.section->ndx);

        section.file->memory.printBinary(os, 
          section_start_in_file, 
          section.section->size, false);
    } 
  }
}


