#include <iostream>

#include <string.h>


static int parseArguments(int argc, char* argv[], char** input, char** output){
  if(argc < 2)return -1; 

  if(!strcmp("-o", argv[1])){ 
    if(argc != 4)return -1; 
    *output = argv[2];
    *input = argv[3];
    return 0;
  }

  if(argc == 2){
    *input = argv[1];
    *output = nullptr;
    return 0;
  }
  
  return -1;
}

extern void yyerror(const char*); // error function
extern int yyparse(); // parsing function
extern FILE* yyin; // input file

static int processing(const char* input, const char* output){
  FILE* inputFile = fopen(input, "rw+");
  if(!inputFile){
    std::cerr << "asembler: error: input file doesn't exist.\n";
  	return -1;
  }
  // check if file ends with \n
  if (fseek(inputFile, -1, SEEK_END) != 0) {
      std::cerr <<"asembler: error: fseek";
      fclose(inputFile);
      return -1;
  }
  // if file doesn't end with \n then append \n 
  if(((char)fgetc(inputFile))!='\n'){
    std::cout << "asembler: warning: input file doesn't end with \\n. Inserting new line.\n";
    fseek(inputFile, 0, SEEK_END);
    fputc('\n', inputFile);
  }
  fseek(inputFile, 0, SEEK_SET);

  // parsing
  yyin = inputFile;
  yyparse();
    
  fclose(inputFile);

      
  return 0;
}

int main(int argc, char* argv[]){
  char* input;
  char* output;

  if(parseArguments(argc, argv, &input, &output)<0){
    std::cerr << "asembler: error: invalid arguments.\n";
    return -1;
  };
  
  std::cout << "Input file: " 
            << (input ? input : "no input file.") 
            << std::endl;
  std::cout << "Output file: " 
            << (output != nullptr ? output : "a.out") 
            << std::endl;

  if(processing(input, output) < 0)return -1;
  
  return 0;
}