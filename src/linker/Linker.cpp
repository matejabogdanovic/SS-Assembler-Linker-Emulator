#include "../../inc/linker/Linker.hpp"

std::unordered_map<std::string, uint32_t> Linker::fixed_sections_address;
std::list<FileState> Linker::files;
std::vector<std::string> Linker::file_names;
std::string Linker::output = "out.hex";
Sections Linker::sections;

SymbolTable Linker::global_symtab(false);
RelTable Linker::global_rel;

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
        throw LinkerException("no output file");
      }
      output = std::string(argv[++i]);
LOG(std::cout << " output " << output << std::endl;)
    }else if(!options_end && command == "-hex"){
      if(hex_defined){
        throw LinkerException("option -hex repeated");
      }
      hex_defined = true;

    }else if(!options_end && std::regex_match(command, match ,place_regex)){
      fixed_sections_address[match[1]] = std::stoul(match[2], nullptr, 16);
    }else {
      options_end = true;
      file_names.push_back(std::string(argv[i])+std::string(".bin"));
    }

    

  }
  if(!hex_defined){
    throw LinkerException("option -hex not given");
    
  }


  return 0;
}

int Linker::start(int argc, char* argv[]){
  
  if(parseArguments(argc, argv)<0){
    throw LinkerException("invalid arguments");
    // return -1;
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
      throw LinkerException("can't open input file -> " + file_names[i]);
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
    throw LinkerException("can't open output file");
    return -1;
  }

  sections.printHex(outputFile);

  outputFile.close(); 

  std::ofstream outputFileBinary(output+".bin", std::ios::binary);

  if (!outputFileBinary.is_open()) {
    throw LinkerException("can't open binary output file");
    return -1;
  }

  sections.printBinary(outputFileBinary);

  outputFileBinary.close(); 



  return 0;
}


// when relocatable sections are starting from 0 and offsets are in sections
void Linker::findDefinedSymbols(){
  std::vector<std::string> extern_syms;

  

  for (const Sections::SectionsUnion& sec_union: sections.map){ 
    uint64_t subsection_local_offset = 0;
    std::string sec_union_name = sec_union.name;

    global_symtab.addSection(&sec_union_name, 
      SymbolTable::Entry(sec_union.start_address, 
        SymbolTable::Bind::LOC,
        0, 0, 
        SymbolTable::Type::SCTN, 
        sec_union.size, 
        0
    ));

    for (const Sections::Section& section: sec_union.sections){
      SymbolTable* symtab = &section.file->symtab;


      for (size_t i = 0; i < symtab->getNumOfSymbols(); i++){
        std::string sym_name = symtab->getSymbolName(i);
        SymbolTable::Entry* symbol = symtab->getSymbol(&sym_name);
        
        if(symtab->getSectionName(symbol->ndx) != sec_union_name)continue;
        if(symbol->bind == SymbolTable::Bind::LOC){
          // todo when relocatable => check if defined and then add local symbol, if already extern, then error... assembler checks.
          continue;
        }


        if(SymbolTable::isDefined(symbol)){
          
          if(global_symtab.doesSymbolExist(&sym_name) && 
          SymbolTable::isDefined(global_symtab.getSymbol(&sym_name))){
            
            throw LinkerException("multiple symbol definition -> " + sym_name);
          }
LOG(std::cout << "(+)d\n";)
          // calculate final address
   
          global_symtab.addSymbol(&sym_name, 
          SymbolTable::Entry(symbol->offset + sec_union.start_address + subsection_local_offset, 
          SymbolTable::Bind::GLOB,
          global_symtab.getCurrentSection()->ndx, 
          symbol->flags, 
          symbol->type, 
          symbol->size, 
          0
           ));
        }else if(SymbolTable::isExtern(symbol)){
LOG(std::cout << "(+)e\n";)

          extern_syms.push_back(sym_name);

           global_symtab.addSymbol(&sym_name, 
            SymbolTable::Entry(symbol->offset, 
            SymbolTable::Bind::GLOB,
            global_symtab.getCurrentSection()->ndx, 
            symbol->flags, 
            symbol->type, 
            symbol->size, 
            0
           ));

        } // if absolute todo
          
      }
    
      subsection_local_offset += section.section->size;
    }
      
  }
  // todo maybe check sections?
  for(std::string extern_sym_name: extern_syms){

    if(!global_symtab.doesSymbolExist(&extern_sym_name) || 
    global_symtab.doesSymbolExist(&extern_sym_name) &&  !SymbolTable::isDefined(global_symtab.getSymbol(&extern_sym_name)
    )){
      // if relocatable it's okay
      throw LinkerException("unresolved symbol -> " + extern_sym_name);
    }
  }

  // creating global relocation table
  // todo if relocatable offset needs to be record.offset + union offset + local section offset
  for(FileState& file: files){
    for(RelTable::Entry& record: file.rel.table){
        std::string sym_name = record.type == RelTable::RelType::T_LOC?
        file.symtab.getSectionName(record.symbol):
        file.symtab.getSymbolName(record.symbol);
        std::string sec_name = file.symtab.getSectionName(record.section);

        LOG(std::cout << sym_name << " " << std::hex << 
          record.offset << 
          " " << record.offset + sections.getSectionsUnion(&sec_name) ->start_address +
           sections.getSubsectionLocalOffset(&sec_name, &file)
          << std::dec << std::endl;)

        global_rel.put({
          record.offset, 
          record.type == RelTable::RelType::T_LOC ?
           global_symtab.getSection(&sym_name) : 
          global_symtab.getSymbol(&sym_name),

          global_symtab.getSection(&sec_name),
          record.type,
          record.addend
        });
    }
  }
}

void Linker::createSectionOrder(){

  for(FileState& file: files){
    // go through sections and make section union
    for (size_t i = 0; i < file.symtab.getNumOfSections(); i++){
      std::string section_name = file.symtab.getSectionName(i);
      bool section_fixed = fixed_sections_address.count(section_name)>0;
      uint32_t start_address = 
      (section_fixed?fixed_sections_address[section_name]:0); 
      SymbolTable::Entry* section = file.symtab.getSection(&section_name);

      sections.put(&file, &section_name, section, section_fixed?&start_address:nullptr);
        
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
  // go in every section union
  for(const Sections::SectionsUnion& sec_union: sections.map){
    uint32_t fixing_subsection_local_offset = 0;
    LOG(std::cout <<"Section union: " << sec_union.name << std::endl;)
    // go in every section/subsection in section union
    for(const Sections::Section& section: sec_union.sections){
      uint32_t fixer_subsection_local_offset = 0;
      RelTable* rel =  &section.file->rel; // rel table for this section in section union
      uint32_t section_start_in_obj_file = section.file->symtab.getSectionStart(section.section->ndx);
      // go through all relocations in the file and find sections to fix
      for(RelTable::Entry& record: rel->table){
        if(record.section->ndx != section.section->ndx)continue;
        // case for different rel types
        std::string sym_name;
        uint32_t addr_to_put;
        switch (record.type)
        {
        case RelTable::T_GLOB:
          sym_name = section.file->symtab.getSymbolName(record.symbol->num);
          addr_to_put = global_symtab.getSymbol(&sym_name)->offset;
        break;
        // symbol which address we need to put is local and section is given as symbol in reltable
        // that section is in union and we know union start but we don't know offset of that
        // section inside the union, so calculate that
        case RelTable::T_LOC:
        sym_name = section.file->symtab.getSectionName(record.symbol->num);
        fixer_subsection_local_offset = sections.getSubsectionLocalOffset(&sym_name, section.file);

         addr_to_put = global_symtab.getSection(&sym_name)->offset
          + record.addend + fixer_subsection_local_offset;
        break;
        default:
          throw LinkerException("invalid relocation type");

        }
  
        LOG(std::cout << "addr abs to fix is: 0x" << std::hex << (sec_union.start_address + fixing_subsection_local_offset + record.offset) 
        << " addr relative to fix: 0x" << record.offset ;)
        LOG(std::cout<< " using address: 0x" << 
        addr_to_put
        <<std::dec << std::endl; ) 
        
        section.file->memory.changeWord(
          addr_to_put, 
          record.offset + section_start_in_obj_file);


        
      }
      LOG(if(sec_union.name != "")
        section.file->memory.print(std::cout, section_start_in_obj_file, section.section->size,
      fixing_subsection_local_offset+sec_union.start_address-section_start_in_obj_file);)


      fixing_subsection_local_offset += section.section->size;
    }   
  } 


  LOG(global_symtab.print(std::cout);)
  LOG(global_rel.print(std::cout, &global_symtab));
}


