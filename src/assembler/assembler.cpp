#include <iostream>
#include <fstream>
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

static int processing(const char* input, const char* output){
  std::ifstream inputFile(input);
  if(!inputFile.is_open()){
    std::cerr << "asembler: error: input file doesn't exist.\n";
    return -1;
  }
  std::cout << "Start processing.\n";
  std::string line;
  while (getline(inputFile, line)) {
    std::cout << line << std::endl;
  }

  inputFile.close();

  std::ofstream outputFile(output, std::ofstream::out | std::ofstream::binary ); 
  if(!outputFile.is_open()){
    std::cerr << "asembler: error: output file can't be open.\n";
    return -1;
  }
  outputFile << "Output file working.";
  outputFile.close();
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