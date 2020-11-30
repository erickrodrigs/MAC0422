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

void freeBlocks(int currentBlock) {
  int aux;

  if (currentBlock != -1) {
    bitMap[currentBlock] = 1;
    aux = currentBlock;
    currentBlock = FAT[currentBlock];
    FAT[aux] = - 1;
    freeBlocks(currentBlock);
  }
}

bool findFile(string file, char type) {
  bool found = false;
  char metaData[20];
  int jump = 95;
  int blockAddress, currentBlock;

  blockAddress = ftell(disk);
  currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
  
  if (file == "") {
    fseek(disk, 178957, 0);
    return true;
  }

  fscanf(disk, "%s", metaData);
  fscanf(disk, "%s", metaData);

  while (!found) {
    string name(metaData);

    if (name != ">") {
      fscanf(disk, "%s", metaData);
      fscanf(disk, "%s", metaData);

      if (metaData[0] == type && name == file)
        found = true;  
      else {
        fseek(disk, ftell(disk) + jump, 0);

        if (ftell(disk) - blockAddress + 119 > SIZEOFBLOCK) {
          currentBlock = FAT[currentBlock];

          if (currentBlock != -1) {
            blockAddress = currentBlock*SIZEOFBLOCK + BEGIN;
            fseek(disk, blockAddress, 0);
            fscanf(disk, "%s", metaData);
          }
        }

        fscanf(disk, "%s", metaData);
      }
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
      fseek(disk, ftell(disk) + 89, 0);
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

void mkdir(string path) { //um  ou dois blocos livres
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  time_t now;
  char *date;

  directories = parse(path);
    
  if (directories.size() > 0 && findDirectory(directories)) {

    if (!findFile(directories.back(), 'D')) {
      fseek(disk, ftell(disk) - 1, 0);

      if ((ftell(disk) - BEGIN)%SIZEOFBLOCK + 1 + 119 > SIZEOFBLOCK) {
        blockAddress = ftell(disk);
        currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
        FAT[currentBlock] = searchFreeBlock(); //depois fazer tratamento do erro
        currentBlock = FAT[currentBlock];
        bitMap[currentBlock] = 0;
        blockAddress = currentBlock*SIZEOFBLOCK + BEGIN;
        fseek(disk, blockAddress, 0);
        fprintf(disk, "< ");
      }
      cout << directories.back();
      fprintf(disk, "%14s%s", directories.back().c_str(), " | ");
      fprintf(disk, "%s", "DIR | ");
      fprintf(disk, "%9d | ", 0);

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

int touch(string path) {//zero, um  ou dois blocos livres
  vector<string> directories;
  int blockAddress = 0;
  int sizeAddress = -1;
  int currentBlock;
  time_t now;
  char *date, cstring[50];

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {

    if (!findFile(directories.back(), 'A')) {
      fseek(disk, ftell(disk) - 1, 0);

      if ((ftell(disk) - BEGIN) % SIZEOFBLOCK + 1 + 119 > SIZEOFBLOCK) {
        blockAddress = ftell(disk);
        currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
        FAT[currentBlock] = searchFreeBlock(); //depois fazer tratamento do erro
        currentBlock = FAT[currentBlock];
        bitMap[currentBlock] = 0;
        blockAddress = currentBlock*SIZEOFBLOCK + BEGIN;
        fseek(disk, blockAddress, 0);
        fprintf(disk, "< ");
      }

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
  int fileSize = 0, write;
  int blockAddress;
  int sizeAddress = touch(destiny);

  if (sizeAddress == -1) {
    cout << "Digite um caminho de destino válido!\n";
    return;
  }

  originFile = fopen(origin.c_str(), "r");

  fseek(disk, sizeAddress + 86, 0);
  fscanf(disk, "%d", &blockAddress);
  freeBlocks(FAT[blockAddress]);
  FAT[blockAddress] = -1;
  fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN + 2, 0);

  write = SIZEOFBLOCK - 2;
  while ((character = getc(originFile)) != EOF) {
    if (write == 0) {
      FAT[blockAddress] = searchFreeBlock();
      blockAddress = FAT[blockAddress];
      bitMap[blockAddress] = 0;
      write = SIZEOFBLOCK;
    }

    fprintf(disk, "%c", character);
    fileSize++;
    write--;
  }

  if (write >= 2)
    fprintf(disk, "> ");
  else if (write == 1) {
    fprintf(disk, ">");
    FAT[blockAddress] = searchFreeBlock();
    blockAddress = FAT[blockAddress];
    bitMap[blockAddress] = 0;
    fprintf(disk, " ");
  }
  else {
    FAT[blockAddress] = searchFreeBlock();
    blockAddress = FAT[blockAddress];
    bitMap[blockAddress] = 0;
    fprintf(disk, "> ");
  }

  fseek(disk, sizeAddress, 0);
  fprintf(disk, "%9d", fileSize);
  
  fclose(originFile);
}

void cat(string path) {
  vector<string> directories;
  int blockAddress = 0;
  int fileSize, numberOfBlocks, read;
  time_t now;
  char *date, cstring[50];

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {
    if (findFile(directories.back(), 'A')) {
      fscanf(disk, "%s" , cstring);
      fscanf(disk, "%d", &fileSize);
      fscanf(disk, "%s", cstring);
      fprintf(disk, " ");

      fprintf(disk, "%s", date);

      fseek(disk, ftell(disk) + 50, 0);

      fscanf(disk, "%d", &blockAddress);
      fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN + 2, 0);

      numberOfBlocks = (fileSize + 4)/SIZEOFBLOCK;
      numberOfBlocks += (fileSize + 4)%SIZEOFBLOCK != 0;
      read = fileSize > SIZEOFBLOCK - 2 ? SIZEOFBLOCK - 2 : fileSize;

      for (int i = 0; i < numberOfBlocks; i++) {
        for (int j = 0; j < read; j++) {
          cout << (char) getc(disk);
        }

        fileSize -= read;
        read = fileSize > SIZEOFBLOCK ? SIZEOFBLOCK : fileSize;
        blockAddress = FAT[blockAddress];
        fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN, 0);
      }
    }
    else {
      cout << "Arquivo não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void ls(string path) {
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  int fileSize;
  time_t now;
  char *date, cstring[50];
  char typeFile;

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {
    
    if (findFile(directories.back(), 'D')) {
      fscanf(disk, "%s" , cstring);
      fscanf(disk, "%s", cstring);
      fscanf(disk, "%s", cstring);
      fprintf(disk, " ");
      fprintf(disk, "%s", date);

      fseek(disk, ftell(disk) + 50, 0);

      fscanf(disk, "%d", &blockAddress);
      fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN + 1, 0);

      fscanf(disk, "%s", cstring);

      while (cstring[0] != '>') {
        cout << cstring << " ";
        fscanf(disk, "%s", cstring);
        fscanf(disk, "%s", cstring);

        typeFile = cstring[0];
        fscanf(disk, "%s", cstring);
        fscanf(disk, "%d", &fileSize);

        if (typeFile == 'D') {
          cout << "(diretório)";
        }
        else {
          cout << " | Tamanho: " << fileSize << "B";
        }

        fscanf(disk, "%s", cstring);

        for (int i = 0; i < 5; i++)
          fscanf(disk, "%s", cstring);

        cout << " | Data de modificação: ";
        for (int i = 0; i < 5; i++) {
          fscanf(disk, "%s", cstring);
          cout << cstring << " ";
        }

        cout << endl;
        for (int i = 0; i < 6; i++)
          fscanf(disk, "%s", cstring);

        if ((ftell(disk) - BEGIN)%SIZEOFBLOCK + 1 + 119 > SIZEOFBLOCK) {
          blockAddress = ftell(disk);
          currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
          currentBlock = FAT[currentBlock];
          blockAddress = currentBlock*SIZEOFBLOCK + BEGIN;
          fseek(disk, blockAddress, 0);
          fscanf(disk, "%s", cstring);
        }
        fscanf(disk, "%s", cstring);
      }
    }
    else {
      cout << "Arquivo não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void rm(string path) {
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  int fileSize, numberOfBlocks, read;
  time_t now;
  char *date, cstring[50];
  char content[5000];
  int removedFilePosition;
  int endPosition = 3946;
  bool firstBlock = true;

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {
    if (findFile(directories.back(), 'A')) {
      currentBlock = (ftell(disk) - BEGIN) / SIZEOFBLOCK;

      removedFilePosition = ftell(disk) - 21;
      fseek(disk, ftell(disk) + 89, 0);
      fscanf(disk, "%d", &blockAddress);
      freeBlocks(blockAddress);

      fgets(content, SIZEOFBLOCK - (ftell(disk) - BEGIN) % SIZEOFBLOCK, disk);

      fseek(disk, removedFilePosition, 0);

      fputs(content, disk);

      while (FAT[currentBlock] != -1) {
        fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 2, 0);

        fgets(content, 116, disk);

        fseek(disk, currentBlock * SIZEOFBLOCK + BEGIN + endPosition, 0);
        
        fputs(content, disk);

        fprintf(disk, " > ");

        fseek(disk, currentBlock * SIZEOFBLOCK + BEGIN + 117, 0);
        fscanf(disk, "%s", cstring);

        // se nao tem mais outro elemento na pasta
        if (cstring[0] == '>') {
          bitMap[FAT[currentBlock]] = 1;
          FAT[currentBlock] = -1;
        }
        else {
          fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 117, 0);
          fgets(content, SIZEOFBLOCK - (ftell(disk) - BEGIN) % SIZEOFBLOCK, disk);
          fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 1, 0);
          fputs(content, disk);
          currentBlock = FAT[currentBlock];
        }
      }
    }
    else {
      cout << "Arquivo não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void listOfBlocks(string path, int block, vector<int> & blocks, vector<string> & eliminated) {
  int blockAddress = 0, currentBlock;
  int fileSize;
  int position;
  char cstring[50];
  char typeFile;

  fseek(disk, block * SIZEOFBLOCK + BEGIN + 1, 0);
  fscanf(disk, "%s", cstring);

  while (cstring[0] != '>') {
    eliminated.push_back(path + '/' + cstring);
    fscanf(disk, "%s", cstring);
    fscanf(disk, "%s", cstring);

    typeFile = cstring[0];
    fseek(disk, ftell(disk) + 89, 0);
    fscanf(disk, "%d", &blockAddress);

    position = ftell(disk);

    if (typeFile == 'D') {
      listOfBlocks(eliminated.back(), blockAddress, blocks, eliminated);
    }

    blocks.push_back(blockAddress);

    fseek(disk, position, 0);

    if ((ftell(disk) - BEGIN) % SIZEOFBLOCK + 1 + 119 > SIZEOFBLOCK) {
      blockAddress = ftell(disk);
      currentBlock = (blockAddress - BEGIN) / SIZEOFBLOCK;
      currentBlock = FAT[currentBlock];
      blockAddress = currentBlock * SIZEOFBLOCK + BEGIN;
      fseek(disk, blockAddress, 0);
      fscanf(disk, "%s", cstring);
    }

    fscanf(disk, "%s", cstring);
  }
}

void searchFiles(string path, string fileName, int block, vector<string> & foundFiles) {
  int blockAddress = 0, currentBlock;
  int fileSize;
  int position;
  char cstring[50];
  char typeFile;

  fseek(disk, block * SIZEOFBLOCK + BEGIN + 1, 0);
  fscanf(disk, "%s", cstring);

  while (cstring[0] != '>') {
    string currentFileName(cstring);

    fscanf(disk, "%s", cstring);
    fscanf(disk, "%s", cstring);

    typeFile = cstring[0];
    fseek(disk, ftell(disk) + 89, 0);
    fscanf(disk, "%d", &blockAddress);

    position = ftell(disk);

    if (typeFile == 'D') {
      searchFiles(path + '/' + currentFileName, fileName, blockAddress, foundFiles);
    }
    else {
      if (fileName == currentFileName)//Funciona?
        foundFiles.push_back(path + '/' + currentFileName);
    }

    fseek(disk, position, 0);

    if ((ftell(disk) - BEGIN) % SIZEOFBLOCK + 1 + 119 > SIZEOFBLOCK) {
      blockAddress = ftell(disk);
      currentBlock = (blockAddress - BEGIN) / SIZEOFBLOCK;
      currentBlock = FAT[currentBlock];
      blockAddress = currentBlock * SIZEOFBLOCK + BEGIN;
      fseek(disk, blockAddress, 0);
      fscanf(disk, "%s", cstring);
    }

    fscanf(disk, "%s", cstring);
  }

}

void rmdir(string path) {
  vector<string> directories, eliminated;
  vector<int> blocks;
  int blockAddress = 0, currentBlock;
  int removedFilePosition = 0;
  int endPosition = 3946;
  char cstring[50];
  char content[5000];
  char typeFile;

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {
    if (findFile(directories.back(), 'D')) {
      removedFilePosition = ftell(disk) - 21;
      fseek(disk, ftell(disk) + 89, 0);
      fscanf(disk, "%d", &blockAddress);
      fgets(content, SIZEOFBLOCK - (ftell(disk) - BEGIN) % SIZEOFBLOCK, disk);

      eliminated.push_back(path);
      listOfBlocks(path, blockAddress, blocks, eliminated);
      blocks.push_back(blockAddress);

      cout << "Os seguintes arquivos/diretórios serão removidos: " << endl;
      for (int i = 0; i < (int) eliminated.size(); i++) {
        cout << eliminated[i] << endl;
      }

      for (int i = 0; i < (int) blocks.size(); i++) {
        freeBlocks(blocks[i]);
      }

      fseek(disk, removedFilePosition, 0);

      fputs(content, disk);

      while (FAT[currentBlock] != -1) {
        fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 2, 0);

        fgets(content, 116, disk);

        fseek(disk, currentBlock * SIZEOFBLOCK + BEGIN + endPosition, 0);
        
        fputs(content, disk);

        fprintf(disk, " > ");

        fseek(disk, currentBlock * SIZEOFBLOCK + BEGIN + 117, 0);
        fscanf(disk, "%s", cstring);

        // se nao tem mais outro elemento na pasta
        if (cstring[0] == '>') {
          bitMap[FAT[currentBlock]] = 1;
          FAT[currentBlock] = -1;
        }
        else {
          fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 117, 0);
          fgets(content, SIZEOFBLOCK - (ftell(disk) - BEGIN) % SIZEOFBLOCK, disk);
          fseek(disk, FAT[currentBlock] * SIZEOFBLOCK + BEGIN + 1, 0);
          fputs(content, disk);
          currentBlock = FAT[currentBlock];
        }
      }
    }
    else {
      cout << "Diretório não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void find(string path, string fileName) {
  vector<string> directories, foundFiles;
  int blockAddress = 0, currentBlock;
  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories)) {
    if (findFile(directories.back(), 'D')) {
      fseek(disk, ftell(disk) + 89, 0);
      fscanf(disk, "%d", &blockAddress);
      if (path == "/")
        path = "";
      searchFiles(path, fileName, blockAddress, foundFiles);
      cout << "Arquivos encontrados:" << endl;

      for (int i = 0; i < foundFiles.size(); i++) {
        cout << foundFiles[i] << endl;
      }
      cout << endl;
    }
    else {
      cout << "Diretório não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void df() {
  //a partir da raiz, usa os metadados (número de bytes) e o FAT pra determinar os blocos desperdiçados.
  //As pastas deve ser incluídas e sobre os espaços a mais?
}

void debug() {
  while (true) {
    int head = ftell(disk);
    int mode;
    string text;
    char c;
    cout << "cabeça: "<< head << endl;
    cout << "Modo (1 cabeça, 2 escrita, 3 leitura, 4 bitmap/FAT, 5 sair): ";
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
    else if (mode == 4) {
      cout << "BITMAP: " << endl;
      for (int i = 0; i < BLOCKS; i++) {
        cout << bitMap[i] << " ";
      }

      cout << endl << "FAT: " << endl;
      for (int i = 0; i < BLOCKS; i++) {
        cout << FAT[i] << " ";
      }

      cout << endl;
    }
    else break;
  }
}

int main() {
  bool exit = false;
  string command, path, origin, fileName;
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
    else if (command == "rmdir") {
      cin >> path;
      rmdir(path);
    }
    else if (command == "cat") {
      cin >> path;
      cat(path);
    }
    else if (command == "touch") {
      cin >> path;
      touch(path);
    }
    else if (command == "rm") {
      cin >> path;
      rm(path);
    }
    else if (command == "ls") {
      cin >> path;
      ls(path);
    }
    else if (command == "find") {
      cin >> path >> fileName;
      find(path, fileName);
    }
    else if (command == "df") {
      df();
    }
    else if (command == "unmount") {
      if (disk != NULL) {
        saveBITMAP();
        saveFAT();
        fclose(disk);
        disk = NULL;
      }
    }
    else if (command == "sai") {
      if (disk != NULL) {
        saveBITMAP();
        saveFAT();
        fclose(disk);
        disk = NULL;
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