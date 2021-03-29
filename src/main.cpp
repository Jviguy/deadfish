#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

enum OpCodes {
  // I for increment
  i,
  // D for decrement
  d,
  // S for square
  s,
  // O for output
  o,
  // U for unknown
  u
};

int opcode(char c) {
  if (c == 'i') {
    return OpCodes::i;
  } else if (c == 'd') {
    return OpCodes::d;
  } else if (c == 's') {
    return OpCodes::s;
  } else if (c == 'o') {
    return OpCodes::o;
  } else {
    return OpCodes::u;
  }
}

class Program {
public:
  void handle(std::string ins) {
    for (const char &c : ins) {
      if (val == 256 || val < 0) {
        val = 0;
      }
      switch (opcode(c)) {
      case OpCodes::i:
        ++val;
        break;
      case OpCodes::d:
        --val;
        break;
      case OpCodes::s:
        val *= val;
        break;
      case OpCodes::o:
        std::cout << val << std::endl;
        break;
      case OpCodes::u:
        std::cout << "Unknown Instruction: " << c << std::endl;
        break;
      }
    }
  }

private:
  int val = 0;
};

int main(int argc, char **argv) {
  // SexFish Interactive mode
  Program *prg = new Program();
  if (argc <= 1) {
    while (true) {
      std::cout << ">>";
      std::string ins;
      std::cin >> ins;
      prg->handle(ins);
    }
    // Compiler Mode
  } else if (strcmp("-c", argv[1]) == 0) {
    delete prg;
    std::string asms("global main\nextern printf\nsection .text\nmain: ");
    std::ifstream fs;
    fs.open(argv[2], std::fstream::in | std::fstream::out | std::fstream::app);
    std::stringstream buffer;
    buffer << fs.rdbuf();
    std::string s;
    while (getline(buffer, s, '\n')) {
      for (const char &c : s) {
        switch (opcode(c)) {
        case OpCodes::i:
	  asms += "inc qword [value]\n";
          break;
	case OpCodes::d:
	  asms += "dec qword [value]\n";
	  break;
	case OpCodes::s:
	  asms += "mov rax, qword [value]\nmul qword [value]\nmov qword [value], rax\n";
	  break;
	case OpCodes::o:
	  asms += "push rax\nmov rdi, fmt\nmov rsi, qword [value]\nxor rax, rax\ncall printf\n";
	  break;
    	}
      }
    }
    fs.close();
    std::string asme(
        "mov rax, 60\nxor rdi, rdi\nsyscall\nsection .data\nvalue: dq 0\nfmt: db \"%d\", 10, 0");
    std::ofstream output("tmp.s", std::ios::ate|std::ios::app);
    output << asms << asme << std::endl;
    output.close();
    system("nasm -felf64 tmp.s");
    system("gcc -Wall -no-pie tmp.o");
    std::remove("tmp.o");
    std::remove("tmp.s");
    // File mode
  } else {
    std::ifstream fs;
    fs.open(argv[1], std::fstream::in | std::fstream::out | std::fstream::app);
    std::stringstream buffer;
    buffer << fs.rdbuf();
    std::string s;
    while (getline(buffer, s, '\n')) {
      prg->handle(s);
    }
    fs.close();
    delete prg;
  }
}
