#include "Console.h"
#include "Logger.h"
Logger logger;

void Logger::init() {
  ok = SD.begin(PIN_CS);
  
  if (ok) {
    lastSizeCheck = millis();
    console.println("SD inicializado com sucesso :-)");
    open();
  } else {
    console.println("SD nao inicializado :-(");
  }
}

void Logger::checkFileLength() {
  if (ok && file && (millis() - lastSizeCheck >= TIME_SIZE_CHECK)) {
    lastSizeCheck = millis();
    if (file.size() >= MAX_FILE_SIZE) {
      close();
      open();
    }
  }
}

void Logger::open() {
  String fileName;
  int index = 1;
  while (true) {
    fileName = String(index);
    while (fileName.length() < 6)
      fileName = "0" + fileName;
    fileName = fileName + ".LOG";
    
    char buffer[fileName.length() + 1];
    fileName.toCharArray(buffer, sizeof(buffer));
    if (!SD.exists(buffer))
      break;
    
    index++;
  }
  
  console.println("New log file: " + fileName);
  char buffer[fileName.length() + 1];
  fileName.toCharArray(buffer, sizeof(buffer));
  file = SD.open(buffer, FILE_WRITE);
}

void Logger::close() {
  if (file) file.close();
}

void Logger::clear() {
  if (!ok) return;
  close();
  clear(SD.open("/"));
  open();
}

void Logger::clear(File dir) {
  dir.rewindDirectory();
  
  while(true) {
    File entry = dir.openNextFile();
    if (!entry) {
      dir.close();
      SD.remove(dir.name());
      break;
    }
    
    if (entry.isDirectory()) {
      clear(entry);
    }
    
    entry.close();
    
    if (String(entry.name()).endsWith(".LOG")) {
      SD.remove(entry.name());
    }
  }
}

void Logger::write(byte b) {
  if (ok && file) {
    file.write(b);
  }
  
  console.write(b);
}

void Logger::print(String s) {
  if (ok && file) { 
    file.print(s);
    file.flush();
    checkFileLength();
  }
  
  console.print(s);
}

void Logger::println() {
  if (ok && file) {
    file.println();
    file.flush();
    checkFileLength();
  }
  
  console.println();
}

void Logger::println(String s) {
  if (ok && file) {
    file.println(s);
    file.flush();
    checkFileLength();
  }
  
  console.println(s);
}

void Logger::info(String s) {
  #ifdef INFO
    s.trim();
    println(" INFO: " + s);
  #endif
}

void Logger::debug(String s) {
  #ifdef DEBUG
    s.trim();
    println("DEBUG: " + s);
  #endif
}

void Logger::listFiles() {
  if (!ok) return;
  File root = SD.open("/");
  console.println(root.name());
  listFiles(root, 1);
}

void Logger::listFiles(File dir, int numTabs) {
  dir.rewindDirectory();
  
  while(true) {
    File entry = dir.openNextFile();
    if (!entry) {
      dir.close();
      break;
    }
     
    for (uint8_t i = 0; i < numTabs; i++) {
      console.print('\t');
    }
     
    console.print(entry.name());
    if (entry.isDirectory()) {
      console.println("/");
      listFiles(entry, numTabs + 1);
    } else {
      console.print("\t\t");
      console.println(String(entry.size(), DEC));
      entry.close();
    }
  }
}
