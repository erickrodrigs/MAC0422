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

void saveBITMAP() {
  fseek(disk, 0, 0);
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%d", bitMap[i]);
}

void saveFAT() {
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%6d", FAT[i]);
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

  fprintf(disk, "%5d< > ", 0);
  fclose(disk);
}

int searchFreeBlock() {
  int i = 0;
  
  while (bitMap[i] != 1 && i < BLOCKS)
    i++;

  return i == BLOCKS ? -1 : i;  
}

bool findFile(string file, char type) {
  bool found = false;
  char metaData[20];
  int jump;

  fscanf(disk, "%s", metaData);
  while (!found) {

    fscanf(disk, "%s", metaData);
    string name(metaData);

    if (name != ">") {
      fscanf(disk, "%s", metaData);
      fscanf(disk, "%s", metaData);

      jump = metaData[0] == 'D' ? 83 : 95;

      if (metaData[0] == type && name == file)
        found = true;
      else
        fseek(disk, ftell(disk) + jump, 0);
    }
    else break;
  }

  return found;
}

bool findDirectory(vector<string> directories) {
  int blockAddress = 0 + BEGIN;
  char metaData[20];
  int currentPosition, size, namePosition;
  bool equal;
  time_t now;
  char *date;

  
  fseek(disk, blockAddress, 0);
  for (int i = 0; i < directories.size() - 1; i++) {
    currentPosition = 1;

    if (findFile(directories[i], 'D')) {
      //fseek para o bloco do diretório
      fseek(disk, ftell(disk) + 78, 0);
      fscanf(disk, "%d", &blockAddress);
      blockAddress *= SIZEOFBLOCK;
      blockAddress += BEGIN;
      fseek(disk, blockAddress, 0);
      //blockAdress recebe o bloco do diretório
    }
    else {
      return false;
    }
  }
  return true;
}

vector<string> parse(string path) {
  vector<string> directories;

  if (path[0] == '/') { 
    directories.push_back("");
    for (int i = 1; i < path.length(); i++) {
      if (path[i] == '/')
        directories.push_back("");
      else 
        directories.back().push_back(path[i]);
    }
  }

  return directories;
}

void mkdir(string path) {
  vector<string> directories;
  int blockAddress = 0;
  time_t now;
  char *date;

  directories = parse(path);
    
  if (directories.size() > 0 && findDirectory(directories)) {

    if (!findFile(directories.back(), 'D')) {
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
        fprintf(disk, "%5d > ", blockAddress);

        fseek(disk, blockAddress*SIZEOFBLOCK + BEGIN, 0);
      
        fprintf(disk, "< > ");
      }
    }
    else cout << "Já existe um diretório com esse nome\n";
  }
  else 
    cout << "Digite um caminho válido!\n";
}

int touch(string path) {
  vector<string> directories;
  int blockAddress = 0;
  int sizeAddress = -1;
  time_t now;
  char *date, cstring[50];

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {

    if (!findFile(directories.back(), 'A')) {
      fseek(disk, ftell(disk) - 1, 0);
      cout << directories.back();
      fprintf(disk, "%14s%s", directories.back().c_str(), " | ");
      fprintf(disk, "%s", "ARQ | ");
      sizeAddress = ftell(disk);
      fprintf(disk, "%9d | ", 0);

      for (int i = 0; i < 3; i++)
        fprintf(disk, "%s", date);

      
      blockAddress = searchFreeBlock();
      if (blockAddress > 0) {
        bitMap[blockAddress] = 0;
        fprintf(disk, "%5d > ", blockAddress);

        fseek(disk, blockAddress*SIZEOFBLOCK + BEGIN, 0);
      
        fprintf(disk, "< > ");
      }
    }
    else {
      fscanf(disk, "%s" , cstring);
      sizeAddress = ftell(disk) + 1;
      fscanf(disk, "%s", cstring);
      fscanf(disk, "%s", cstring);
      fprintf(disk, " ");

      fprintf(disk, "%s", date);

    }
  }
  else 
    cout << "Digite um caminho válido!\n";
  
  return sizeAddress;
}

void cp(string origin, string destiny) {
  // Abrir arquivo, copiar tudo de dentro dele
  FILE *originFile;
  char character;
  int fileSize = 0;
  int blockAddress;
  int sizeAddress = touch(destiny);

  if (sizeAddress == -1) {
    cout << "Digite um caminho de destino válido!\n";
    return;
  }

  originFile = fopen(origin.c_str(), "r");

  fseek(disk, sizeAddress + 86, 0);
  fscanf(disk, "%d", &blockAddress);
  fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN + 2, 0);

  while ((character = getc(originFile)) != EOF) {
    fprintf(disk, "%c", character);
    fileSize++;
  }

  fprintf(disk, " >");

  fseek(disk, sizeAddress, 0);
  fprintf(disk, "%9d", fileSize);
  
  fclose(originFile);
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
  string command, path, origin;
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
    else if (command == "cp") {
      cin >> origin >> path;
      cp(origin, path);
    }
    else if (command == "mkdir") {
      cin >> path;
      mkdir(path);
    }
    else if (command == "rmdir") {}
    else if (command == "cat") {}
    else if (command == "touch") {
      cin >> path;
      touch(path);
    }
    else if (command == "rm") {}
    else if (command == "ls") {}
    else if (command == "find") {}
    else if (command == "df") {}
    else if (command == "unmount") {
      if (disk != NULL) {
        saveBITMAP();
        saveFAT();
        fclose(disk);
      }
    }
    else if (command == "sai") {
      if (disk != NULL) {
        saveBITMAP();
        saveFAT();
        fclose(disk);
      }
      exit = true;
    }
    else if (command == "debug") {
      debug();
    }
    else {}
 
  
  }
  return 0;
}