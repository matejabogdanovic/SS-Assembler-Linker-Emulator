#include "../../inc/linker/Linker.hpp"
std::map<std::string, uint32_t> Linker::defined_syms;
std::unordered_map<std::string, uint32_t> Linker::section_starts;
std::list<FileState> Linker::files;
std::vector<std::string> Linker::file_names;
std::string Linker::output = "out.hex";
Sections Linker::sections;


#include <regex>
int Linker::parseArguments(int argc, char* argv[]){
  std::regex place_regex(R"(^-place=([\.a-zA-Z0-9_]+)@0x([0-9A-Fa-f]+)$)");
  bool hex_defined = false;
  bool options_end = false;
  for (size_t i = 1; i < argc; i++){
    std::string command = argv[i];
    std::smatch match;


    if(!options_end && command == "-o"){
      if(argc < i+1){
        std::cerr << "No output file." << std::endl;
        return -1;
      }
      output = std::string(argv[++i]);
LOG(std::cout << " output " << output << std::endl;)
    }else if(!options_end && command == "-hex"){
      if(hex_defined){
        std::cerr << "Option -hex already given.\n";
        return -2;
      }
      hex_defined = true;

    }else if(!options_end && std::regex_match(command, match ,place_regex)){
      section_starts[match[1]] = std::stoul(match[2], nullptr, 16);
    }else {
      options_end = true;
      file_names.push_back(std::string(argv[i])+std::string(".bin"));
    }

    

  }
  if(!hex_defined){
    std::cerr << "Option -hex not defined." << std::endl;
    exit(0);
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
    files.push_back({});
    FileState& file = files.back();

    std::ifstream inputBinary(file_names[i], std::ios::binary); // otvara fajl za pisanje

    if (!inputBinary.is_open()) {
      std::cerr << "assembler: error: can't open input file\n";
      return -1;
    }
    // read data
LOG(std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";)

LOG(std::cout << "From file: " << file_names[i] << std::endl;)

    file.symtab.loadFromFile(inputBinary);
    file.rel.loadFromFile(inputBinary, &file.symtab);
    file.memory.loadFromFile(inputBinary);
    
    inputBinary.close(); 
    
    // store data
    
LOG(file.symtab.print(std::cout);)
LOG(file.rel.print(std::cout, &file.symtab);)
LOG(file.memory.printCode(std::cout, &file.symtab);)

  }
  return 0;
}

int Linker::processing(){
LOG(std::cout << "Start processing.\n";)
  createSectionOrder();
  findDefinedSymbols();
  linking();

LOG(sections.printHex(std::cout);)

  std::ofstream outputFile(output);

  if (!outputFile.is_open()) {
    std::cerr << "assembler: error: can't open output file\n";
    return -1;
  }

  sections.printHex(outputFile);

  outputFile.close(); 

  std::ofstream outputFileBinary(output+".bin", std::ios::binary);

  if (!outputFileBinary.is_open()) {
    std::cerr << "assembler: error: can't open output file\n";
    return -1;
  }

  sections.printBinary(outputFileBinary);

  outputFileBinary.close(); 



  return 0;
}

void Linker::findDefinedSymbols(){
  std::vector<std::string> extern_syms;

  for (const Sections::SectionsUnion& sec_union: sections.map){ 
    uint64_t curr_sz = 0;
    // add section union to defined symbols
    defined_syms[sec_union.name] = sec_union.start_address;

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
LOG(std::cout << "(+)d\n";)
          // calculate final address
          defined_syms[*sym_name] = symbol->offset + sec_union.start_address + curr_sz;
        }else if(SymbolTable::isExtern(symbol->flags)){
LOG(std::cout << "(+)e\n";)
          
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

void Linker::createSectionOrder(){

  for(FileState& file: files){
    // go through sections and make section union
    for (size_t i = 0; i < file.symtab.section_names.size(); i++){
      std::string* section_name = &file.symtab.section_names[i];
      bool section_fixed = section_starts.count(*section_name)>0;
      uint32_t start_address = 
      (section_fixed?section_starts[*section_name]:0); 
      SymbolTable::Entry* section = file.symtab.getSection(section_name);

      sections.put(&file, section_name, section, section_fixed?&start_address:nullptr);
        
    }
   
  }

LOG(std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  for(const Sections::SectionsUnion& sec_union: sections.map){

    std::cout << "Union: " << sec_union.name 
    << std::hex<< " start: 0x"<< sec_union.start_address << std::dec
    << " size: " << sec_union.size
    << " " << sec_union.sections.size() << " section(s) included." << std::endl; 
  }
)
}

void Linker::linking(){
LOG(std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";)
LOG(std::cout << "LINKING\n";)
  uint32_t addr = 0;
  for(const Sections::SectionsUnion& sec_union: sections.map){

    uint32_t offset = 0;
LOG(std::cout <<"Section union: " << sec_union.name << std::endl;)
    for(const Sections::Section& section: sec_union.sections){
      uint32_t offs_inside_union = 0;
      RelTable* rel =  &section.file->rel; // rel table for this section in section union
      uint32_t section_start_in_file = section.file->symtab.getSectionStart(section.section->ndx);
      
      for(RelTable::Entry& record: rel->table){
        // case for different rel types
        if(record.section->ndx != section.section->ndx)continue;

        uint32_t addr_to_fix = sec_union.start_address + offset + record.offset;
        uint32_t addr_to_put;
        switch (record.type)
        {
        case RelTable::T_GLOB:
           addr_to_put = defined_syms[section.file->symtab.symbol_names[record.symbol->num]];
        break;
        case RelTable::T_LOC:
        // NEED TO CHANGE IN SPECIFIC SUBSECTION, NOT JUST IN SECTION UNION BECAUSE ADDEND IS LOCAL
        // todo, fix this 
        for(const Sections::SectionsUnion& sec_union2: sections.map){
          if(sec_union2.name != section.file->symtab.section_names[record.symbol->num])continue;
          for(const Sections::Section& section2: sec_union2.sections){
            if(section2.file == section.file)break;
            offs_inside_union += section2.section->size;
          }
        }
         addr_to_put = defined_syms[section.file->symtab.section_names[record.symbol->num]]
          + record.addend + offs_inside_union;
        break;
        default:
          std::cerr << "Invalid relocation type." << std::endl;
          return;  
        }
  
        LOG(std::cout << "addr abs to fix is: 0x" << std::hex << addr_to_fix 
        << " addr relative to fix: 0x" << record.offset ;)
        LOG(std::cout<< " using address: 0x" << 
        addr_to_put
        <<std::dec << std::endl;  
        section.file->memory.changeWord(
          addr_to_put, 
          record.offset + section_start_in_file);)

      }
      LOG(if(sec_union.name != "")
        section.file->memory.print(std::cout, section_start_in_file, section.section->size,
      offset+sec_union.start_address-section_start_in_file);)


      offset += section.section->size;
    }   
  } 
  

  
}


