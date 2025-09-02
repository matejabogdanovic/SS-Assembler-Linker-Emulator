#include "../../inc/common/SymbolTable.hpp"
#include <iostream>
#include <iomanip>
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


const char* SymbolTable::type_str[] = {"NOTYP", "SCTN"};
const char* SymbolTable::bind_str[] = {"LOC", "GLOB"};  



SymbolTable::SymbolTable(bool init){
  if(init){
    this->sections.emplace("", Entry (0, Bind::LOC, 0, 0, Type::SCTN));
   // this->sections[""] = Entry (0, Bind::LOC, 0, 0, Type::SCTN);
    this->section_names.push_back("");
  }

}
bool SymbolTable::isDefined(Entry* e) {
  return e->ndx != UNDEFINED_SECTION;
}
bool SymbolTable::isExtern(Entry* e){
  return (e->ndx == UNDEFINED_SECTION) && (e->bind == SymbolTable::Bind::GLOB );
}
bool SymbolTable::isAbsolute(uint8_t flags){
  return flags & (Flags::ABSOLUTE);
}

bool SymbolTable::doesSymbolExist(std::string* name) const{
  return symbols.count(*name) > 0;
};

bool SymbolTable::doesSectionExist(std::string* name) const{
  return sections.count(*name) > 0;
};

// no duplicates
void SymbolTable::addSymbol(std::string* name, Entry e){
  // if(doesSymbolExist(name)){
  //   auto sym = this->getSymbol(name);
  //   auto num = sym->num;
  //   *sym = e;
  //   sym->num = num;
  //   return; 
  // }
  e.num = symbol_names.size(); // to get my name
  
  this->symbols.emplace(*name, e);
  //symbols[*name] = e;
  symbol_names.push_back(*name);
}

void SymbolTable::addSymbolNoDuplicates(std::string* name, Entry e){
  if(doesSymbolExist(name)){
    auto sym = this->getSymbol(name);
    auto num = sym->num;
    *sym = e;
    sym->num = num;
    return; 
  }
  addSymbol(name, e);
}

// won't be duplicates since sections are aggregated
void SymbolTable::addSection(std::string* name, Entry e){
  current_section = section_names.size();
  e.num = current_section; 
  e.ndx = current_section;
  this->sections.emplace(*name, e);
  //sections[*name] = e;
  section_names.push_back(*name);
}

// gets first
SymbolTable::Entry* SymbolTable::getSymbol(std::string* name){
  // return &symbols[*name];
  auto it = symbols.find(*name);
  return (it != symbols.end())?&it->second:nullptr;
 
}
// gets first
SymbolTable::Entry* SymbolTable::getSection(std::string* name){
  // return &sections[*name];
  auto it = sections.find(*name);
  return (it != sections.end())?&it->second:nullptr;
}
// gets first, won't be duplicates
SymbolTable::Entry* SymbolTable::getSection(uint32_t num_or_ndx){
  // return &sections[section_names[num_or_ndx]];
  auto it = sections.find(section_names[num_or_ndx]);
  return (it != sections.end())?&it->second:nullptr;
}

SymbolTable::Entry* SymbolTable::getSymbol(uint32_t num){
  // return &symbols[symbol_names[num]];
  auto range = symbols.equal_range(symbol_names[num]);
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second.num == num) {  
     
        return &it->second;
 
    }
  }
  return nullptr;
}



std::string SymbolTable::getSymbolName(Entry* e) const{
  return symbol_names[e->num];
}

std::string SymbolTable::getSectionName(Entry* e) const{
  return section_names[e->ndx];
}

std::string SymbolTable::getSymbolName(uint32_t num) const{

  return symbol_names[num];
}
std::string SymbolTable::getSectionName(uint32_t num_or_ndx) const{

  return section_names[num_or_ndx];
}

// one section
uint32_t SymbolTable::getSectionStart(uint32_t ndx){
  
  uint32_t sz = 0;
  SymbolTable::Entry* curr;
  for(int i = 0; i < section_names.size() ; i++){
    curr =  getSection(i);
    if(curr->ndx == ndx){
      
        return sz;
    }
    sz += curr->size;

  }
  return 0;
}

// for assembler, linker won't need -----

bool SymbolTable::sectionOpened() const{
    return current_section != 0;
}

std::string SymbolTable::getCurrentSectionName() const{
  return section_names[current_section];
}

std::string SymbolTable::getUndefinedSectionName() const{
  return section_names[0];
}

SymbolTable::Entry* SymbolTable::getCurrentSection(){
  auto name = getCurrentSectionName();
  return getSection(&name);
}
// --------------------------------------


void SymbolTable::printPartBinary(std::string* name, Entry* e, std::ostream& os) const{
    os.write(reinterpret_cast<const char*>(e), sizeof(Entry));

    auto namesz = name->size() +1;
    os.write(reinterpret_cast<const char*>(&namesz), sizeof(namesz));

    os.write(reinterpret_cast<const char*>(name->c_str()), namesz);

}



void SymbolTable::printBinary(std::ostream& os){
  auto num_of_sections = section_names.size();
  
  os.write(reinterpret_cast<const char*>(& num_of_sections), sizeof(num_of_sections));
  for(int i = 0; i < num_of_sections; i++){
    Entry* e =  getSection(i);
    printPartBinary(&section_names[i],  e, os) ;
  }

  auto num_of_symbols = symbol_names.size();
 
  os.write(reinterpret_cast<const char*>(& num_of_symbols), sizeof(num_of_symbols));
  for(int i = 0; i < num_of_symbols; i++){
    Entry* e =  getSymbol(i);
    printPartBinary(&symbol_names[i],  e, os) ;
  }
}

void SymbolTable::loadFromFile(std::istream& is){
  // SymbolTable symtab(false);
  size_t num_of_sections;
  is.read(reinterpret_cast<char*>(& num_of_sections), sizeof(num_of_sections));
  
  for(int i = 0; i < num_of_sections; i++){
    Entry e = {};
    is.read(reinterpret_cast <char*> (&e), sizeof(Entry));

    size_t namesz;
    is.read(reinterpret_cast< char*>(&namesz), sizeof(namesz ));

    char section_name_char[namesz];

    is.read(reinterpret_cast< char*>(&section_name_char), namesz);
    std::string section_name = std::string(section_name_char);

    addSection(&section_name, e);
    
  }


  size_t num_of_symbols;
  is.read(reinterpret_cast<char*>(& num_of_symbols), sizeof(num_of_symbols));
  for(int i = 0; i < num_of_symbols; i++){
    Entry e = {};
    is.read(reinterpret_cast <char*> (&e), sizeof(Entry));

    size_t namesz;
    is.read(reinterpret_cast< char*>(&namesz), sizeof(namesz ));
    char symbol_name_char[namesz];

    is.read(reinterpret_cast< char*>(&symbol_name_char), namesz);
    std::string symbol_name = std::string(symbol_name_char);

    addSymbol(&symbol_name, e);
    
  }

}

void SymbolTable::print(std::ostream& os){
  
  os << 
    "|  Num  " << 
    "|  Type  "<< 
    "|  Bind  " << 
    "|  Offset  " << 
    "|  Size  " << 
    "|  Ndx  "<< 
    "|  Flags  " << 
    "|  Name  |" << 
    std::endl;

    
  for(int i = 0; i < section_names.size(); i++){
    Entry* e =  getSection(i);

    printEntry(&section_names[i], e, os);
  }

  for(int i = 0; i < symbol_names.size(); i++){
    Entry e =  *getSymbol(i);
    e.num = section_names.size() + i; // correct index (from sections)
    printEntry(&symbol_names[i], &e, os);
  }

}


void SymbolTable::printEntry(std::string* name, Entry* e, std::ostream& os) const {
  
      os<< 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " 0x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->offset << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)os<< "UND";
    else os<< e->ndx;

     os << 
   
    " " << std::left << std::setw(1) << std::setfill(' ') << (isDefined(e) ? "d":" ") << 
    std::left << std::setw(1) << std::setfill(' ') << (isExtern(e) ? "e":" ") <<  
    std::left << std::setw(7) << std::setfill(' ') << (isAbsolute(e->flags) ? "a":" ")<< 
    " " << std::left << std::setw(8) << std::setfill(' ') << *name << 
    std::endl;
}
