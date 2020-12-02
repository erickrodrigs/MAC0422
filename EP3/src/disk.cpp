#include "disk.hpp"

Disk::Disk() {
  disk = NULL;
}

int Disk::getFreeSpace() {
  return freeSpace;
}

int Disk::getFiles() {
  return files;
}

int Disk::getWastedSpace() {
  return wastedSpace;
}

int Disk::getDirectories() {
  return directories;
}

void Disk::fillInfo(int blockAddress, int total) {
  int numberOfFiles, sizeOfFile, positionBackup, currentBlock;
  char type[30];

  currentBlock = (blockAddress - BEGIN)/SIZEOFBLOCK;
  fseek(disk, blockAddress + 1, 0);
  
  for (int i = 0; i < total; i++){
    fseek(disk, ftell(disk) + 18, 0);
    fscanf(disk, "%s", type);


    fseek(disk, ftell(disk) + 3, 0);

    if (type[0] == 'D') {
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, ftell(disk) + 77, 0);
      fscanf(disk, "%d", &blockAddress);

      positionBackup = ftell(disk);
      fillInfo(blockAddress*SIZEOFBLOCK + BEGIN, numberOfFiles);
      fseek(disk, positionBackup, 0);

      directoryWastedSpace(blockAddress);
      directories++;
    }
    else {
      fscanf(disk, "%d", &sizeOfFile);
      fseek(disk, ftell(disk) + 77, 0);
      fscanf(disk, "%d", &blockAddress);
      fileWastedSpace(sizeOfFile);
      files++;
    }
    
    if ((i + 1) % 34 == 0 && currentBlock >= 0 && FAT[currentBlock] != -1) {
      currentBlock = FAT[currentBlock];
      blockAddress = currentBlock*SIZEOFBLOCK + BEGIN;
      fseek(disk, blockAddress + 1, 0);
    }
  }

}

void Disk::directoryWastedSpace(int numberOfFiles) {
  wastedSpace += numberOfFiles % 34 == 0 ? 0 : 52;
  wastedSpace += 116*(numberOfFiles % 34) + 52 * (numberOfFiles / 34);
}

void Disk::fileWastedSpace(int sizeOfFile) {
  wastedSpace += (sizeOfFile + 4) % SIZEOFBLOCK;
}

void Disk::df() {
  freeSpace = 0;

  for (int i = 0; i < (int) bitMap.size(); i++) 
    freeSpace += bitMap[i];
  freeSpace *= SIZEOFBLOCK;

  directories = 0;
  wastedSpace = 0;
  files = 0;
  
  fillInfo(ROOTBEGIN, 1);
  
  cout << "INFORMAÇÕES SOBRE O DISCO:\n\n";
  cout << "Número de arquivos: " << getFiles() << endl;
  cout << "Número de diretórios: " << getDirectories() << endl;
  cout << "Espaço desperdiçado: " << getWastedSpace() << endl;
  cout << "Espaço livre: " << getFreeSpace() << endl;
}
