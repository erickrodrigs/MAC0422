#include <iostream>
#include <vector>
using namespace std;

int main() {
  bool exit = false;
  string command;
  
  while (!exit) {
    cout << "[ep3]: ";
    cin >> command;

    if (command == "mount") {}
    else if (command == "cp") {}
    else if (command == "mkdir") {}
    else if (command == "rmdir") {}
    else if (command == "cat") {}
    else if (command == "touch") {}
    else if (command == "rm") {}
    else if (command == "ls") {}
    else if (command == "find") {}
    else if (command == "df") {}
    else if (command == "unmount") {}
    else if (command == "sai") {
      exit = true;
    }
    else {}
 
  
  }
  return 0;
}