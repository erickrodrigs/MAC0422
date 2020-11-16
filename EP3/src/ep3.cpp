#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <ctime>
using namespace std;

void createFileSystem(string path) {
  FILE *disk;
  time_t now;
  char *date;
  disk = fopen(path.c_str(), "w");
  
  for (int i = 0; i < 1000; i++)
    fprintf(disk, "%d", 1);
  
  for (int i = 1000; i < 2000; i++)
    fprintf(disk, "%d", -1);
  
  fprintf(disk, "%s", "/|");
  fprintf(disk, "%s", "DIR|");

  now = time(0);
  date = ctime(&now);
  date[24] = '|';

  for (int i = 0; i < 3; i++)
    fprintf(disk, "%s", date);

  fclose(disk);
}

void debug(FILE *disk) {

  while (true) {
    int head = ftell(disk);
    int mode;
    string text;
    char c;
    cout << "cabeça: "<< head << endl;
    cout << "Modo (1 cabeça, 2 escrita, 3 leitura, 4 sair): ";
    cin >> mode;

    if (mode == 1) {
      cout << "Digite a nova cabeça: ";
      cin >> head;
      fseek(disk, head, 0);
    }
    else if (mode == 2) {
      cout << "Digite a string que vai ser escrita: ";
      cin >> text;
      fprintf(disk, "%s", text.c_str());
    }
    else if (mode == 3) {
      cout << "Leitura: ";
      fscanf(disk, "%c", &c);
      cout << "Saída: " << c << endl;
    }
    else break;
  }
}

int main() {
  bool exit = false;
  string command, path;
  FILE *disk = NULL;
  
  while (!exit) {
    cout << "[ep3]: ";
    cin >> command;
  
    if (command == "mount") {
      cin >> path;

      if (disk != NULL)
        fclose(disk);

      if (access(path.c_str(), F_OK) == -1)
        createFileSystem(path);

      disk = fopen(path.c_str(), "r+");      
    }
    else if (command == "cp") {}
    else if (command == "mkdir") {}
    else if (command == "rmdir") {}
    else if (command == "cat") {}
    else if (command == "touch") {}
    else if (command == "rm") {}
    else if (command == "ls") {}
    else if (command == "find") {}
    else if (command == "df") {}
    else if (command == "unmount") {
      if (disk != NULL)
        fclose(disk);
    }
    else if (command == "sai") {
      if (disk != NULL)
        fclose(disk);
      exit = true;
    }
    else if (command == "debug") {
      debug(disk);
    }
    else {}
 
  
  }
  return 0;
}