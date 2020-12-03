#include "disk.hpp"

int main() {
  bool exit = false;
  string command, path, origin, fileName;
  Disk disk;
  
  while (!exit) {
    cout << "[ep3]: ";
    cin >> command;
  
    if (command == "mount") {
      cin >> path;
      disk.mount(path);
    }
    else if (command == "cp") {
      cin >> origin >> path;

      if (disk.isMounted()) {
        disk.cp(origin, path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "mkdir") {
      cin >> path;

      if (disk.isMounted()) {
        disk.mkdir(path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "rmdir") {
      cin >> path;

      if (disk.isMounted()) {
        disk.rmdir(path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "cat") {
      cin >> path;

      if (disk.isMounted()) {
        disk.cat(path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "touch") {
      cin >> path;

      if (disk.isMounted()) {
        disk.touch(path, true);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "rm") {
      cin >> path;

      if (disk.isMounted()) {
        disk.rm(path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "ls") {
      cin >> path;

      if (disk.isMounted()) {
        disk.ls(path);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "find") {
      cin >> path >> fileName;

      if (disk.isMounted()) {
        disk.find(path, fileName);
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "df") {
      if (disk.isMounted()) {
        disk.df();
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else if (command == "unmount") {
      disk.unmount();
    }
    else if (command == "sai") {
      disk.unmount();
      exit = true;
    }
    else if (command == "debug") {
      if (disk.isMounted()) {
        disk.debug();
      }
      else {
        cout << "Disco ainda não foi montado!" << endl;
      }
    }
    else {
      cout << "Comando inválido!" << endl;
    }
  }

  return 0;
}
