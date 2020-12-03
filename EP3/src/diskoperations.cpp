#include "diskoperations.hpp"

void DiskOperations::mkdir(string path) { //um  ou dois blocos livres
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  int parentSizePosition, numberOfFiles;
  time_t now;
  char *date;

  directories = parse(path);
    
  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (!findFile(directories.back(), 'D', parentSizePosition)) {
      fseek(disk, ftell(disk) - 1, 0);

      if ((ftell(disk) - BEGIN) % SIZEOFBLOCK == 3946) {
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
      
      fseek(disk, parentSizePosition, 0);
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, parentSizePosition, 0);
      fprintf(disk, "%9d", numberOfFiles + 1);

      fseek(disk, ftell(disk) + 3, 0);
      fprintf(disk, "%s", date);
      fprintf(disk, "%s", date);
    }
    else cout << "Já existe um diretório com esse nome\n";
  }
  else 
    cout << "Digite um caminho válido!\n";
}

int DiskOperations::touch(string path, bool changeAccessDate) {//zero, um  ou dois blocos livres
  vector<string> directories;
  int blockAddress = 0;
  int sizeAddress = -1;
  int currentBlock;
  int parentSizePosition, numberOfFiles;
  time_t now;
  char *date;

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (!findFile(directories.back(), 'A', parentSizePosition)) {
      fseek(disk, ftell(disk) - 1, 0);

      if ((ftell(disk) - BEGIN) % SIZEOFBLOCK == 3946) {
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

      fseek(disk, parentSizePosition, 0);
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, parentSizePosition, 0);
      fprintf(disk, "%9d", numberOfFiles + 1);

      fseek(disk, ftell(disk) + 3, 0);
      fprintf(disk, "%s", date);
      fprintf(disk, "%s", date);
    }
    else {
      sizeAddress = ftell(disk) + 3;
      fseek(disk, sizeAddress + 12, 0);

      if (changeAccessDate) 
        fprintf(disk, "%s", date);
      else 
        fseek(disk, ftell(disk) + 25, 0);

      fprintf(disk, "%s", date);
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
  
  return sizeAddress;
}

void DiskOperations::cp(string origin, string destiny) {
  // Abrir arquivo, copiar tudo de dentro dele
  FILE *originFile;
  char character;
  int fileSize = 0, write;
  int blockAddress;
  int sizeAddress = touch(destiny, false);

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

void DiskOperations::cat(string path) {
  vector<string> directories;
  int blockAddress = 0;
  int fileSize, numberOfBlocks, read;
  int parentSizePosition;
  time_t now;
  char *date;

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (findFile(directories.back(), 'A', parentSizePosition)) {
      fseek(disk, ftell(disk) + 3, 0);
      fscanf(disk, "%d", &fileSize);
      fseek(disk, ftell(disk) + 3, 0);

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

void DiskOperations::ls(string path) {
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  int fileSize, parentSizePosition;
  int numberOfFiles;
  char date[26], cstring[50];
  char typeFile;

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (findFile(directories.back(), 'D', parentSizePosition)) {
      fseek(disk, ftell(disk) + 3, 0);
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, ftell(disk) + 77, 0);
      fscanf(disk, "%d", &blockAddress);

      currentBlock = blockAddress;

      fseek(disk, blockAddress * SIZEOFBLOCK + BEGIN + 1, 0);

      for (int i = 0; i < numberOfFiles; i++){
        fscanf(disk, "%s", cstring);
        cout << cstring << " ";

        fseek(disk, ftell(disk) + 3, 0);
        fscanf(disk, "%s", cstring);

        typeFile = cstring[0];

        fseek(disk, ftell(disk) + 3, 0);
        fscanf(disk, "%d", &fileSize);

        if (typeFile == 'D') {
          cout << "(diretório)";
        }
        else {
          cout << " | Tamanho: " << fileSize << "B";
        }

        fseek(disk, ftell(disk) + 28, 0);
        fgets(date, 25, disk);

        cout << " | Data de modificação: " << date << endl;

        fseek(disk, ftell(disk) + 31, 0);
        
        if ((i + 1) % 34 == 0 && currentBlock >= 0 && FAT[currentBlock] != -1) {
          currentBlock = FAT[currentBlock];
          blockAddress = currentBlock * SIZEOFBLOCK + BEGIN;
          fseek(disk, blockAddress + 1, 0);
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

void DiskOperations::listOfBlocks(string path, int block, vector<int> & blocks, vector<string> & eliminated) {
  int blockAddress = 0, currentBlock;
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

void DiskOperations::searchFiles(string path, string fileName, int block, vector<string> & foundFiles) {
  int blockAddress = 0, currentBlock;
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

void DiskOperations::rm(string path) {
  vector<string> directories;
  int blockAddress = 0, currentBlock;
  time_t now;
  char *date, cstring[50];
  char content[5000];
  int removedFilePosition;
  int endPosition = 3946;
  int parentSizePosition, numberOfFiles;

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (findFile(directories.back(), 'A', parentSizePosition)) {
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
      fseek(disk, parentSizePosition, 0);
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, parentSizePosition, 0);
      fprintf(disk, "%9d", numberOfFiles - 1);

      fseek(disk, ftell(disk) + 3, 0);
      fprintf(disk, "%s", date);
      fprintf(disk, "%s", date);
    }
    else {
      cout << "Arquivo não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void DiskOperations::rmdir(string path) {
  vector<string> directories, eliminated;
  vector<int> blocks;
  int blockAddress = 0, currentBlock;
  time_t now;
  int removedFilePosition = 0;
  int endPosition = 3946;
  int parentSizePosition, numberOfFiles;
  char *date, cstring[50];
  char content[5000];

  now = time(0);
  date = ctime(&now);
  date[24] = ' ';

  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (findFile(directories.back(), 'D', parentSizePosition)) {
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
      fseek(disk, parentSizePosition, 0);
      fscanf(disk, "%d", &numberOfFiles);
      fseek(disk, parentSizePosition, 0);
      fprintf(disk, "%9d", numberOfFiles - 1);

      fseek(disk, ftell(disk) + 3, 0);
      fprintf(disk, "%s", date);
      fprintf(disk, "%s", date);
    }
    else {
      cout << "Diretório não encontrado!\n";
    }
  }
  else 
    cout << "Digite um caminho válido!\n";
}

void DiskOperations::find(string path, string fileName) {
  vector<string> directories, foundFiles;
  int blockAddress = 0;
  int parentSizePosition;
  directories = parse(path);

  if (directories.size() > 0 && findDirectory(directories, parentSizePosition)) {
    if (findFile(directories.back(), 'D', parentSizePosition)) {
      fseek(disk, ftell(disk) + 89, 0);
      fscanf(disk, "%d", &blockAddress);
      if (path == "/")
        path = "";
      searchFiles(path, fileName, blockAddress, foundFiles);
      cout << "Arquivos encontrados:" << endl;

      for (int i = 0; i < (int) foundFiles.size(); i++) {
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
