#include "../../inc/common/RelTable.hpp"
#include "../../inc/common/Memory.hpp"
struct FileState{
    SymbolTable symtab{false};
    RelTable rel;
    Memory memory;

};