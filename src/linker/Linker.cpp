#include "../../inc/linker/Linker.hpp"
std::map<std::string, uint32_t> Linker::defined_syms;
std::unordered_map<std::string, uint32_t> Linker::section_starts;
std::list<FileState> Linker::files;
std::vector<std::string> Linker::file_names;
Sections Linker::sections;

int Linker::parseArguments(int argc, char* argv[]){
  for (size_t i = 1; i < argc; i++){
    file_names.push_back(std::string(argv[i])+std::string(".bin"));
  }

  section_starts["a"] = 0x30;
  //section_starts["b"] = 0x3f;
  

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
    files.push_back({});
    FileState& file = files.back();

    std::ifstream inputBinary(file_names[i], std::ios::binary); // otvara fajl za pisanje

    if (!inputBinary.is_open()) {
      std::cerr << "assembler: error: can't open input file\n";
      return -1;
    }
    // read data
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    std::cout << "From file: " << file_names[i] << std::endl;

    file.symtab.loadFromFile(inputBinary);
    file.rel.loadFromFile(inputBinary, &file.symtab);
    file.memory.loadFromFile(inputBinary);
    
    inputBinary.close(); 
    
    // store data
    
    file.symtab.print(std::cout);
     file.rel.print(std::cout, &file.symtab);
    file.memory.printCode(std::cout, &file.symtab);

  }
  return 0;
}

int Linker::processing(){
  std::cout << "Start processing.\n";
  createMap();
  findDefinedSymbols();
  linking();
  return 0;
}

void Linker::findDefinedSymbols(){
  std::vector<std::string> extern_syms;

  for (const Sections::SectionsUnion& sec_union: sections.map){ 
    uint64_t curr_sz = 0;

    for (const Sections::Section& section: sec_union.sections){
      SymbolTable* symtab = &section.file->symtab;

      for (size_t i = 0; i < symtab->symbol_names.size(); i++){
        std::string* sym_name = &symtab->symbol_names[i];
        SymbolTable::Entry* symbol = &symtab->symbols[*sym_name];
        
        if(symbol->bind == SymbolTable::Bind::LOC ||
          symtab->section_names[symbol->ndx] != sec_union.name)continue;

        if(SymbolTable::isDefined(symbol->flags)){
          if(defined_syms.count(*sym_name)>0){
            std::cerr << "linker: symbol already exported." << std::endl;
            return;
          }
          std::cout << "(+)d\n";
          // calculate final address
          defined_syms[*sym_name] = symbol->offset + sec_union.start_address + curr_sz;
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

}

void Linker::createMap(){

  for(FileState& file: files){
    // go through sections and make section union
    for (size_t i = 0; i < file.symtab.section_names.size(); i++){
      std::string* section_name = &file.symtab.section_names[i];
      bool section_fixed = section_starts.count(*section_name)>0;
      uint32_t start_address = 
      (section_fixed?section_starts[*section_name]:0); 
      SymbolTable::Entry* section = file.symtab.getSection(section_name);

      if(sections.put(&file, section_name, section, section_fixed?&start_address:nullptr)){
        // add new section to defined symbols
        defined_syms[*section_name] = start_address;
      }
    }
   
  }

  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  for(const Sections::SectionsUnion& sec_union: sections.map){
    std::cout << "Union: " << sec_union.name 
    << std::hex<< " start: 0x"<< sec_union.start_address << std::dec
    << " size: " << sec_union.size
    << " " << sec_union.sections.size() << " section(s) included." << std::endl; 
  }
}

void Linker::linking(){
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  std::cout << "LINKING\n";
  uint32_t addr = 0;
  for(const Sections::SectionsUnion& sec_union: sections.map){

    uint32_t offset = 0;
    std::cout <<"Section union: " << sec_union.name << std::endl;
    for(const Sections::Section& section: sec_union.sections){
      RelTable* rel =  &section.file->rel; // rel table for this section in section union
      
      for(RelTable::Entry& record: rel->table){
        // case for different rel types
        if(record.section->ndx == section.section->ndx){ 
          uint32_t addr_to_fix = sec_union.start_address + offset + record.offset;
          uint32_t addr_to_put;
          switch (record.type)
          {
          case RelTable::T_GLOB:
             addr_to_put = defined_syms[section.file->symtab.symbol_names[record.symbol->num]];
          break;
          case RelTable::T_LOC:
           addr_to_put = defined_syms[section.file->symtab.section_names[record.symbol->num]]
            + record.addend;
          break;
          default:
            std::cerr << "Invalid relocation type." << std::endl;
            return;  

          }
   


          std::cout << "addr abs to fix is: 0x" << std::hex << addr_to_fix 
          << " addr relative to fix: 0x" << record.offset ;
          std::cout<< " using address: 0x" << 
          addr_to_put
          <<std::dec << std::endl;

          section.file->memory.changeWord(
            addr_to_put, 
            
            record.offset + section.file->symtab.getSectionStart(section.section->ndx));
        }

      }
     
      if(section.section->size != 0)
        section.file->memory.print(std::cout,
           section.file->symtab.getSectionStart(section.section->ndx), 
        section.section->size, 
        offset+sec_union.start_address-section.file->symtab.getSectionStart(section.section->ndx));
      offset += section.section->size;
    }   
  } 
  

  
}