#include "string.hpp"

#include <iostream>

int main() {
  const String name_request{"Enter your name:\n"};
  std::cout << name_request;

  String name;
  std::cin >> name;

  String greeting;
  greeting += "Hello, ";
  greeting += name;
  greeting.PushBack('.');
  greeting.PushBack(' ');
  greeting += "Have a nice day!";
  greeting.PushBack('\n');
  std::cout << greeting;
}