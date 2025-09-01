#pragma once
#include <exception>   
#include <string>

class EmulatorException : public std::exception {
    std::string message;
public:


  EmulatorException(const std::string& message) : message("emulator exception: "+message) {}

  const char* what() const noexcept override {
    return (message).c_str();
  }


};