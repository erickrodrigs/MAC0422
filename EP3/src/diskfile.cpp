#include <unistd.h>
#include "diskfile.hpp"

void DiskFile::mount(string path) {
  if (isMounted())
    fclose(disk);

  if (access(path.c_str(), F_OK) == -1)
    createFileSystem(path);

  disk = fopen(path.c_str(), "r+");
  openBITMAP();
  openFAT();
}

void DiskFile::unmount() {
  if (isMounted()) {
    saveBITMAP();
    saveFAT();
    fclose(disk);
    disk = NULL;
  }
}

bool DiskFile::isMounted() {
  return disk != NULL;
}

void DiskFile::openBITMAP() {
  fseek(disk, BITMAPBEGIN, 0);
  for (int i = 0; i < BLOCKS; i++)
    bitMap.push_back(getc(disk) - '0');
}

void DiskFile::openFAT() {
  fseek(disk, FATBEGIN, 0);
  int address;
  for (int i = 0; i < BLOCKS; i++) {
    fscanf(disk, "%d", &address);
    FAT.push_back(address);
  }
}

void DiskFile::saveBITMAP() {
  fseek(disk, BITMAPBEGIN, 0);
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%d", bitMap[i]);
}

void DiskFile::saveFAT() {
  fseek(disk, FATBEGIN, 0);
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%6d", FAT[i]);
}

void DiskFile::createFileSystem(string path) {  
  time_t now;
  char *date;
  disk = fopen(path.c_str(), "w");


  fseek(disk, BITMAPBEGIN, 0);
  fprintf(disk, "%d", 0);
  for (int i = 1; i < BLOCKS; i++)
    fprintf(disk, "%d", 1);
  
  fseek(disk, FATBEGIN, 0);
  for (int i = 0; i < BLOCKS; i++)
    fprintf(disk, "%6d", -1);


  fseek(disk, ROOTBEGIN, 0);
  fprintf(disk, "< ");
  
  fprintf(disk, "%17s", "/ | ");
  fprintf(disk, "%s", "DIR | ");
  fprintf(disk, "%9d | ", 0);

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  for (int i = 0; i < 3; i++)
    fprintf(disk, "%s", date);
  
  fprintf(disk, "%5d > ", 0);

  fseek(disk, BEGIN, 0);
  fprintf(disk, "< > ");
  fclose(disk);
}

int DiskFile::searchFreeBlock() {
  int i = 0;
  
  while (bitMap[i] != 1 && i < BLOCKS)
    i++;

  return i == BLOCKS ? -1 : i;  
}

vector<int> DiskFile::searchFreeBlocks(int numberOfBlocks) {
  vector<int> blocks;
  
  for (int i = 0; numberOfBlocks != 0 && i < (int) bitMap.size(); i++) {
    if (bitMap[i]) {
      blocks.push_back(i);
      numberOfBlocks--;
    }
  }
  return blocks;
}

void DiskFile::freeBlocks(int currentBlock) {
  int aux;

  if (currentBlock != -1) {
    bitMap[currentBlock] = 1;
    aux = currentBlock;
    currentBlock = FAT[currentBlock];
    FAT[aux] = - 1;
    freeBlocks(currentBlock);
  }
}

bool DiskFile::findFile(string file, char type, int & parentSizePosition) {
  bool found = false;
  char name[20], fileType[20];
  int jump = 95;
  int numberOfFiles;
  int blockAddress, currentBlock;

  blockAddress = ftell(disk);
  currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
  
  if (file == "") {
    fseek(disk, ROOTBEGIN + 22, 0);
    return true;
  }

  fseek(disk, parentSizePosition, 0);
  fscanf(disk, "%d", &numberOfFiles);
  fseek(disk, blockAddress + 1, 0);

  for (int i = 0; i < numberOfFiles && !found; i++) {
    fscanf(disk, "%s", name);
    fseek(disk, ftell(disk) + 3, 0);
    fscanf(disk, "%s", fileType);

    if (fileType[0] == type && name == file) {
      found = true;
    }
    else if ((i + 1) % 34 == 0 && currentBlock >= 0 && FAT[currentBlock] != -1) {
      currentBlock = FAT[currentBlock];
      blockAddress = currentBlock * SIZEOFBLOCK + BEGIN;
      fseek(disk, blockAddress + 1, 0);
    }
    else {
      fseek(disk, ftell(disk) + jump, 0);
    }
  }

  if (!found) {
    fseek(disk, ftell(disk) + 2, 0);
  }

  return found;
}

bool DiskFile::findDirectory(vector<string> directories, int & parentSizePosition) {
  int blockAddress = 0 + BEGIN;

  parentSizePosition = ROOTBEGIN + 25;

  fseek(disk, blockAddress, 0);
  for (int i = 0; i < (int) directories.size() - 1; i++) {
    if (findFile(directories[i], 'D', parentSizePosition)) {
      parentSizePosition = ftell(disk) + 3;
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

vector<string> DiskFile::parse(string path) {
  vector<string> directories;

  if (path[0] == '/') { 
    directories.push_back("");
    for (int i = 1; i < (int) path.length(); i++) {
      if (path[i] == '/')
        directories.push_back("");
      else 
        directories.back().push_back(path[i]);
    }
  }

  return directories;
}

void DiskFile::debug() {
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
