#ifndef SPARTAN_FILE_MANAGER_H
#define SPARTAN_FILE_MANAGER_H

#include <Arduino.h>

String SpartanLoadFile(String InputFile);
void SpartanWriteFile(String File_to, String Input);
void SpartanRemoveFile(String File_to);

#endif
