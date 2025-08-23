#include "../../inc/linker/Linker.hpp"
std::map<std::string, SymbolTable::Entry*> Linker::defined_syms;

std::list<Linker::FileState> Linker::files;
std::vector<std::string> Linker::file_names;
std::list <Linker::SectionsUnion> Linker::map;
int Linker::parseArguments(int argc, char* argv[]){
  for (size_t i = 1; i < argc; i++){
    file_names.push_back(std::string(argv[i])+std::string(".bin"));
  }
  return 0;
}

int Linker::start(int argc, char* argv[]){
  
  if(parseArguments(argc, argv)<0){
    std::cerr << "linker: error: invalid arguments.\n";
    return -1;
  };
  

  if(loadData() < 0)return -2;
  if(processing() < 0)return -3;

  return 0;

}

int Linker::loadData(){
  for (size_t i = 0; i < file_names.size(); i++)
  {
    SymbolTable symtab(false);
    RelTable rel;
    Memory memory;
    
    std::ifstream inputBinary(file_names[i], std::ios::binary); // otvara fajl za pisanje

    if (!inputBinary.is_open()) {
      std::cerr << "assembler: error: can't open input file\n";
      return -1;
    }
    // read data
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    std::cout << "From file: " << file_names[i] << std::endl;

    symtab.loadFromFile(inputBinary);
    rel.loadFromFile(inputBinary, &symtab);
    memory.loadFromFile(inputBinary);
    
    inputBinary.close(); 
    
    // store data
    files.push_back({symtab, rel, memory});
    symtab.print(std::cout);
    rel.print(std::cout, &symtab);
    memory.printCode(std::cout, &symtab);

  }
  return 0;
}

int Linker::processing(){
  std::cout << "Start processing.\n";
  createMap();
  linking();
  return 0;
}

void Linker::createMap(){
  std::vector<std::string> extern_syms;
  for(FileState& file: files){
    
    Memory* memory = &file.memory;
    RelTable* rel = &file.rel;
    SymbolTable* symtab = &file.symtab;
    // go through sections
    for (size_t i = 0; i < file.symtab.section_names.size(); i++){
        uint32_t start_address = 0; // todo
      
        std::string* section_name = &symtab->section_names[i];
        SymbolTable::Entry* section = symtab->getSection(section_name);

        bool found = false;
         
  
        for (SectionsUnion& sec_union: map){ // find your section union
          if(sec_union.name == symtab->getSectionName(section)){
            // check for size todo
            found = true;
            sec_union.size += section->size;
            sec_union.sections.push_back({memory, section, symtab});
            sec_union.rels.push_back(rel);

            std::cout << "To union: " << sec_union.name
            << " added new section: "; 
            symtab->printEntry(section_name, section, std::cout);  
            break;
          }
        }
        
        if(!found){
          SectionsUnion sec_union;
          //(*section_name, start_address, section->size)
          sec_union.name = *section_name;
          sec_union.start_address = start_address;
          sec_union.size = section->size;
          sec_union.sections.push_back({memory, section, symtab});
          sec_union.rels.push_back(rel);

          map.emplace_back(sec_union);

          std::cout << "Created union: " << sec_union.name
            << " added new section: "; 
            symtab->printEntry(section_name, section, std::cout); 
        }
      }
    
    // 
   
  }
   for (SectionsUnion& sec_union: map){ 
    uint64_t curr_sz = 0;
    for (Section& section: sec_union.sections){
      SymbolTable* symtab = section.symtab;
      for (size_t i = 0; i < symtab->symbol_names.size(); i++){
        std::string* sym_name = &symtab->symbol_names[i];
        SymbolTable::Entry* symbol = &symtab->symbols[*sym_name];
        // symbol in this section
        if(symtab->section_names[symbol->ndx] != sec_union.name ||
          symbol->bind == SymbolTable::Bind::LOC)
          continue;
        if(SymbolTable::isDefined(symbol->flags)){
          if(defined_syms.count(*sym_name)>0){
            std::cerr << "linker: symbol already exported." << std::endl;
            return;
          }
          std::cout << "(+)d\n";
          symbol->offset += sec_union.start_address + curr_sz;
          defined_syms[*sym_name] = symbol;
        }else if(SymbolTable::isExtern(symbol->flags)){
          std::cout << "(+)e\n";
          
          extern_syms.push_back(*sym_name);
        } // if absolute todo
          
      }
    
      curr_sz += section.section->size;
    }
      
  }

  for(std::string extern_sym_name: extern_syms){
    if(defined_syms.count(extern_sym_name) == 0){
      std::cerr << "linker: symbol '" << extern_sym_name << "' not defined in input files." << std::endl;
      return;
    }
  }


  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  for(SectionsUnion& sec_union: map){
    std::cout << "Union: " << sec_union.name 
    << " start: "<< sec_union.start_address
    << " size: " << sec_union.size
    << " " << sec_union.sections.size() << " section(s) included." << std::endl; 
  }
}

void Linker::linking(){


  uint32_t addr = 0;
  for(SectionsUnion& sec_union: map){
    
    int i = 0;
   
    uint32_t offset = 0;
    for(Section& section: sec_union.sections){
      RelTable* rel =  sec_union.rels[i++]; // rel table for this section in section union
      
      for(RelTable::Entry& record: rel->table){
        // case for different rel types
        if(record.section->ndx == section.section->ndx){ 
          uint32_t addr_to_fix = sec_union.start_address + offset + record.offset;
          std::cout << "addr abs to fix is: 0x" << std::hex << addr_to_fix 
          << " using rel to fix: 0x" << record.symbol->offset ;
          std::cout<< " using address: 0x" << 
          defined_syms[
            section.symtab->symbol_names[record.symbol->num]
          ]->offset
          <<std::dec << std::endl;
        }

      }
      offset += section.section->size;
    }   
  } 
  

  
}