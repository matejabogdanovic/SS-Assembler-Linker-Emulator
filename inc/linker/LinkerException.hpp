#pragma once
#include <exception>   
#include <string>

class LinkerException : public std::exception {
    std::string message;
public:


  LinkerException(const std::string& message) : message("linker exception: "+message) {}

  const char* what() const noexcept override {
    return (message).c_str();
  }


};