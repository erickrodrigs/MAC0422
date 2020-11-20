#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <ctime>
using namespace std;

#define BLOCKS 25562
#define BEGIN 179040
#define SIZEOFBLOCK 4096

FILE *disk;
vector<int> FAT;
vector<int> bitMap;

void openBITMAP() {
  for (int i = 0; i < BLOCKS; i++)
    bitMap.push_back(getc(disk) - '0');
}

void openFAT() {
  int address;
  for (int i = 0; i < BLOCKS; i++) {
    fscanf(disk, "%d", &address);
    FAT.push_back(address);
  }

}

void createFileSystem(string path) {
  
  time_t now;
  char *date;
  disk = fopen(path.c_str(), "w");
  
  fprintf(disk, "%d", 0);
  for (int i = 1; i < BLOCKS; i++)
    fprintf(disk, "%d", 1);
  
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%6d", -1);

  fprintf(disk, "%s", " | ");
  
  fprintf(disk, "%17s", "/ | ");
  fprintf(disk, "%s", "DIR | ");

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  for (int i = 0; i < 3; i++)
    fprintf(disk, "%s", date);

  fprintf(disk, "%5d< >", 0);
  fclose(disk);
}

int searchFreeBlock() {
  int i = 0;
  
  while (bitMap[i] != 1 && i < BLOCKS)
    i++;

  return i == BLOCKS ? -1 : i;  
}

void mkdir(string path) {
  vector<string> directories;
  int blockAddress = 0 + BEGIN;
  char metaData[20];
  int currentPosition, size, namePosition;
  bool equal, found;
  time_t now;
  char *date;

  if (path[0] == '/') {
    directories.push_back("");
    
    for (int i = 1; i < path.length(); i++) {
      if (path[i] == '/')
        directories.push_back("");
      else 
        directories.back().push_back(path[i]);
    }
    
    fseek(disk, blockAddress, 0);
    for (int i = 0; i < directories.size() - 1; i++) {
      currentPosition = 1;
      
      found = false;

      fscanf(disk, "%s", metaData);
      while (metaData[0] != '>' && !found) {

        fscanf(disk, "%s", metaData);
        string name(metaData);
        fscanf(disk, "%s", metaData);
        fscanf(disk, "%s", metaData);

        if (name == directories[i] && metaData[0] == 'D') {
          found = true;
          //cout << ftell(disk);
        }
        else {
          fseek(disk, ftell(disk) + 83, 0); //se invadir outro bloco???
          //fscanf(disk, "%s", metaData);
        }
      }

      if (found) {
        //fseek para o bloco do diretório
        fseek(disk, ftell(disk) + 78, 0);
        fscanf(disk, "%d", &blockAddress);
        blockAddress *= SIZEOFBLOCK;
        blockAddress += BEGIN;
        fseek(disk, blockAddress, 0);
        //blockAdress recebe o bloco do diretório
      }
      else {
        cout << "Digite um caminho válido!\n";
        return;
      }
    }
    
    getc(disk);
    char character;
    int i = 0;
    while ((character = getc(disk)) != '>')
      cout << character; //funciona mas é ineficiente
    
    fseek(disk, ftell(disk) - 1, 0);
    cout << directories.back();
    fprintf(disk, "%14s%s", directories.back().c_str(), " | ");
    fprintf(disk, "%s", "DIR | ");

    now = time(0);
    date = ctime(&now);
    date[24] = ' ';

    for (int i = 0; i < 3; i++)
      fprintf(disk, "%s", date);

    
    blockAddress = searchFreeBlock();
    if (blockAddress > 0) {
      bitMap[blockAddress] = 0;
      fprintf(disk, "%5d >", blockAddress);

      fseek(disk, blockAddress*SIZEOFBLOCK + BEGIN, 0);
    
      fprintf(disk, "< >");
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void debug() {

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
  disk = NULL;
  
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
      openBITMAP();
      openFAT();    
    }
    else if (command == "cp") {}
    else if (command == "mkdir") {
      cin >> path;
      mkdir(path);
    }
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
      debug();
    }
    else {}
 
  
  }
  return 0;
}