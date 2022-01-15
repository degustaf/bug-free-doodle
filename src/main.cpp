#include <fstream>
#include <iostream>
#include <sstream>

#include <cassert>

#include "vm.hpp"

using namespace std;

static void repl(VM &vm) {                       
  string line;

  while(true) {
    cout << "> ";
    getline(cin, line);
    vm.interpret(line.c_str());                       
  }                                         
}

static void runFile(VM &vm, const char* path) {          
  assert(path != NULL);
  ifstream source(path);
  string data((istreambuf_iterator<char>(source)), istreambuf_iterator<char>());
  VM::InterpretResult result = vm.interpret(data.c_str());    
  if(result == VM::InterpretResult::INTERPRET_COMPILE_ERROR) exit(65);
  if(result == VM::InterpretResult::INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
  VM vm;
  if (argc == 1) {                          
    repl(vm);                                
  } else if (argc == 2) {                  
    runFile(vm, argv[1]);
  } else {                                 
    cerr << "Usage: clox [path]\n";
    return 64;                               
  }
  return 0;                             
}
