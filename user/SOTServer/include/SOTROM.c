// thie file is the basic lib of SOTROM based on ESP_ROTS_SDK v 1.4.0
SOTROM_file *SOTROM_fopen(char* fileName) {
  SOTROM_file *openedFile;
  openedFile = malloc(70);
  openedFile->location = 0;
  openedFile->offset = 0;
  openedFile->fileSize = 0;
  openedFile->fileExsit = false;

  // scan FAT to get file
  char *pointerFilename = (char *)zalloc(64);
  uint32 currentFATPointer = SOT_ROM_ORG;
  uint32 maxFATPointer = SOT_ROM_ORG + SOT_ROM_FAT_SIZE;
  SpiFlashOpResult res;
  while(currentFATPointer < maxFATPointer) {
    // read filename
    res = spi_flash_read(currentFATPointer, (uint32* )pointerFilename, 64);
    if(res == SPI_FLASH_RESULT_OK) {
      if(strlen(pointerFilename) > 0) {
        if(strcmp(fileName, pointerFilename) == 0) {
          char *pointerFilename = (char *)zalloc(56);
          uint32 fileSize;
          uint32 location;
          res |= spi_flash_read(currentFATPointer + 64, (uint32* )pointerFilename, 56);
          res |= spi_flash_read(currentFATPointer + 120, (uint32* )&fileSize, 4);
          res |= spi_flash_read(currentFATPointer + 124, (uint32* )&location, 4);
          if(res == SPI_FLASH_RESULT_OK) {
            openedFile->fileExsit = true;
            openedFile->mime = pointerFilename;
            openedFile->fileSize = fileSize;
            openedFile->location = location;
            openedFile->location += maxFATPointer;
            break;
          }
        }
        currentFATPointer += 128;
      } else {
        break;
      }
    } else {
      break;
    }

  }

  // printf("file found: %d\n", openedFile->fileExsit);
  // printf("file mime: %s\n", openedFile->mime);
  // printf("file length: %d\n", openedFile->fileSize);
  // printf("file location: %d\n", openedFile->location);
  // printf("file offset: %d\n", openedFile->offset);

  return openedFile;
}

bool SOTROM_fread(SOTROM_file *file, uint32 *data, int32 datalength) {

  // check file exsite
  if(!file->fileExsit) {
    return false;
  }

  int32 fileLength = file->fileSize;
  int32 currentOffset = file->offset;
  int32 startReadLocation = file->location + currentOffset;

  // offset reach the end
  if(currentOffset >= fileLength) {
    return false;
  }

  if(currentOffset + datalength > fileLength) {
    datalength = fileLength - currentOffset;
  }

  SpiFlashOpResult res;
  res = spi_flash_read(startReadLocation, data, datalength);
  if(res == SPI_FLASH_RESULT_OK) {
    file->offset = currentOffset + datalength;
    char *tmpDataPtr = (char *)data;
    tmpDataPtr[datalength] = 0;
    return true;
  } else {
    return false;
  }

}
