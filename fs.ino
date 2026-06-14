#include <EEPROM.h>
void R_GFName(int findex, char* out) { 
    int index = 256 * findex;
    for(int i = 0; i < MAX_SIZE; i++) {
        out[i] = EEPROM[index + i];
    }
    out[8] = '\0';
}

void R_GFData(int findex, char* out) { 
    int index = (256 * findex) + 10;
    for(int i = 0; i < MAX_FILE_SIZE; i++) {
        out[i] = EEPROM[index + i];
    }
}

int R_GFSize(int findex) { 
    int index = (256 * findex) + 9;
    return EEPROM[index];
    
}

int GetFileNameIndex(char *name) {
  for(int i = 0; i < 4; i++) {
    char fname[MAX_SIZE];
    R_GFName(i,fname);
    if(!strcmp(name, fname)) {
      return i;
    }
  }
  return -1;
}

File GetFileFromIndex(int index) {
  File Ret;
  R_GFName(index,Ret.name);
  Ret.size = R_GFSize(index);
  R_GFData(index,Ret.data);

  return Ret;
}

File GetFile(char *name) {
  int index = GetFileNameIndex(name);
  return GetFileFromIndex(index);
}

void WriteFileToEEPROM(char *name, uint8_t size, char *data, int findex) {
  int index = (256 * findex);

  for(int i = 0; i < MAX_SIZE; i++) {
    EEPROM.update(index + i, name[i]);
  }
  EEPROM.update(index + 9, size);
  for(int i = 0; i < MAX_FILE_SIZE; i++) {
    EEPROM.update(index + 10 + i, data[i]);
  }
}

bool FileExistsAtIndex(int findex) {
  int index = (256 * findex);
  if(EEPROM[index] == 255 || EEPROM[index] == 0) return false;
  return true;
}

bool FileExists(char *name) {
  int index = GetFileNameIndex(name);
  if(index < 0) return false;
  return FileExistsAtIndex(index);
}

int NextFreeFileIndex() {
  for(int i = 0; i < 4; i++) {
    if(!FileExistsAtIndex(i)) return i;
  }
  return -1;
}

bool CreateFile(char *name) {
  int index = NextFreeFileIndex();
  if(index >= 0) {
    for(int i = 0; i < MAX_SIZE; i++) {
      EEPROM.update(index + i, name[i]);
    }
    EEPROM.update(index + 9, 0);

    return true;
  }
  return false;
}

bool WriteFile(char *name, char *data, uint8_t size) {
  int index = GetFileNameIndex(name);
  if(size < MAX_FILE_SIZE) return false;
  if(!FileExists(name)) return false;
  if(index >= 0) {
    for(int i = 0; i < size; i++) {
      EEPROM.update(index + 10 + i, data[i]);
    }
    EEPROM.update(index + 9, size);

    return true;
  }
  return false;
}

bool ReadFile(char *name, char* out) {
  int index = GetFileNameIndex(name);
  if(!FileExists(name)) return false;
  if(index >= 0) {
    int size = R_GFSize(index);
    for(int i = 0; i < size; i++) {
      out[i] = EEPROM[index + i];
    }
    return true;
  }
  return false;
}
