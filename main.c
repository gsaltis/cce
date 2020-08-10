/*****************************************************************************
 * FILE NAME    : main.c
 * DATE         : August 01 2020
 * PROJECT      :  
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "String.h"
#include "MemoryManager.h"
#include "ascii.h"
#include "ANSIColors.h"
#include "FileUtils.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Type : Parameter
 *****************************************************************************/
struct _Parameter
{
  string                                type;
  string                                name;
  struct _Parameter*                    next;
};
typedef struct _Parameter Parameter;

/*****************************************************************************!
 * Local Type : ElementScope
 *****************************************************************************/
enum _ElementScope
{
  ElementScopeLocal = 1,
  ElementScopeGlobal
};
typedef enum _ElementScope ElementScope;

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static bool
MainOverwriteFunctionFile;

static string
MainProgramName = "cce";

static string
MainFilename;

static string
MainReturnType;

static string
MainSourceSuffix = ".c";

static string
MainHeaderSuffix = ".h";

static string
DirSeparator = "/";

static string
MainHeaderName;

static string
MainSourceName;

static string
MainModuleName;

static string
MainFunctionName;

static Parameter*
MainParameters;

static string
FunctionHeaderTemplate =
  "/*****************************************************************************!\n"
  " * Function : %s\n"
  " *****************************************************************************/\n";
 
ElementScope
MainElementScope = ElementScopeGlobal;

string
MainDataName = NULL;

string
MainDataType = NULL;
 
/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
InsertDataDefinition
(char*** InLines, int* InLinesCount, int InInsertPoint, bool IsExtern);

void
MainInsertFunctionInclude
(char*** InLines, int* InLinesCount);

void
MainAddFunctionInclude
();

void
MainAddFunctionDeclaration
();

void
MainAddFunctionItem
();

void
MainAddDataItem
();

void
ResaveFile
(string InFilename, char** InLines, int InLinesCount);

void
InsertFunctionDeclaration
(char*** InLines, int* InLinesCount, int InInsertPoint);

void
MainAddFunctionToSource
();

void
MainAddFunctionToHeader
();

void
VerifyCommandLine
();

void
ProcessCommandLine
(int argc, char** argv);

void
MainInitialize
();

Parameter*
ParameterCreate
(string InType, string InName);

Parameter*
ParameterAppend
(Parameter* InHead, Parameter* InParameter);

void
MainDisplayHelp
();

void
MainWriteFunctionFile
();

void
CreateFileBackupCopy
(string InFilename);

void
MainAddDataDeclaration
();

void
MainAddDataDefinition
();

/*****************************************************************************!
 * Function : main
 *****************************************************************************/
int
main
(int argc, char** argv)
{
  MainInitialize();
  ProcessCommandLine(argc, argv);
  VerifyCommandLine();
  if ( MainFunctionName ) {
    MainAddFunctionItem();
  } else if ( MainDataName ) {
    MainAddDataItem();
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************!
 * Function : MainAddFunctionItem
 *****************************************************************************/
void
MainAddFunctionItem
()
{
  if ( MainSourceName ) {
    CreateFileBackupCopy(MainSourceName);
  }
  if ( MainHeaderName && MainElementScope == ElementScopeGlobal ) {
    CreateFileBackupCopy(MainHeaderName);
  }
  MainWriteFunctionFile();
  MainAddFunctionDeclaration();
  MainAddFunctionInclude();
}

/*****************************************************************************!
 * Function : MainAddDataItem
 *****************************************************************************/
void
MainAddDataItem
()
{
  if ( MainSourceName ) {
    CreateFileBackupCopy(MainSourceName);
  }
  if ( MainHeaderName && MainElementScope == ElementScopeGlobal ) {
    CreateFileBackupCopy(MainHeaderName);
  }
  MainAddDataDeclaration();
  MainAddDataDefinition();
}

/*****************************************************************************!
 * Function : MainAddDataDefinition
 *****************************************************************************/
void
MainAddDataDefinition
()
{
  char*                                 buffer;
  int                                   bufferSize;
  char**                                lines;
  int                                   linesCount; 
  int                                   i;
  string                                line;

  if ( MainSourceName == NULL ) {
    return;
  }

  GetFileBuffer(MainSourceName, &buffer, &bufferSize);
  GetFileLines(buffer, bufferSize, &lines, &linesCount);
  for (i = 0; i < linesCount; i++) {
    line = StringTrim(lines[i]);
    if ( StringEqual(line, "* Local Data") ) {
      i += 2;
      InsertDataDefinition(&lines, &linesCount, i, false);
      ResaveFile(MainSourceName, lines, linesCount);
    }
    FreeMemory(line);
  }
}

/*****************************************************************************!
 * Function : InsertDataDefinition
 *****************************************************************************/
void
InsertDataDefinition
(char*** InLines, int* InLinesCount, int InInsertPoint, bool InIsExtern)
{
  int                                   j;
  char**                                newLines;
  int                                   newLinesCount;
  bool                                  needNewLine;
  int                                   requiredLines;
  int                                   insertPoint;
  string                                s;
  int                                   i;
  int                                   linesCount;
  char**                                lines;

  lines = *InLines;
  linesCount = *InLinesCount;

  insertPoint = InInsertPoint;
  requiredLines = 2;
  s = StringTrim(lines[insertPoint]);
  needNewLine = false;
  if ( s && *s != NUL ) {
    requiredLines++;
    needNewLine = true;
  }
  FreeMemory(s);

  newLinesCount = linesCount + requiredLines;
  newLines = (char**)GetMemory(sizeof(char*) * newLinesCount);

  for (i = 0; i < InInsertPoint; i++) {
    newLines[i] = lines[i];
  }
  if ( InIsExtern ) {
    newLines[i++] = StringConcat("extern ", MainDataType);
  } else {
    newLines[i++] = StringCopy(MainDataType);
  }
  newLines[i++] = StringConcat(MainDataName, ";");
  if ( needNewLine ) {
    newLines[i++] = StringCopy("");
  }

  for ( j = insertPoint; j < linesCount; j++, i++) {
    newLines[i] = lines[j];
  }
  
  FreeMemory(lines);
  *InLines = newLines;
  *InLinesCount = newLinesCount;
}

/*****************************************************************************!
 * Function : MainAddDataDeclaration
 *****************************************************************************/
void
MainAddDataDeclaration()
{
  string                                buffer;
  int                                   bufferSize;
  char**                                lines;
  int                                   linesCount;
  int                                   i;
  string                                line;

  if ( MainHeaderName == NULL ) {
    return;
  }
  if ( !GetFileBuffer(MainHeaderName, &buffer, &bufferSize) ) {
    return;
  }

  GetFileLines(buffer, bufferSize, &lines, &linesCount);
  for ( i = 0 ; i < linesCount ; i++ ) {
    line = StringTrim(lines[i]);

    if ( StringEqual(line, "* Exported Data") ) {
      i += 2;
      InsertDataDefinition(&lines, &linesCount, i, true);
      ResaveFile(MainHeaderName, lines, linesCount);
    } 
  }

  for ( i = 0 ; i < linesCount ; i++ ) {
    FreeMemory(lines[i]);
  }
  FreeMemory(lines);
  FreeMemory(buffer);
}

/*****************************************************************************!
 * Function : MainInitialize
 *****************************************************************************/
void
MainInitialize
()
{
  MainOverwriteFunctionFile     = false;
  MainFilename                  = NULL;
  MainFunctionName              = NULL;
  MainModuleName                = NULL;
  MainHeaderName                = NULL;
  MainSourceName                = NULL;
  MainReturnType                = StringCopy("void");
  MainParameters                = NULL;
  MainDataType                  = StringCopy("int");
}

/*****************************************************************************!
 * Function : MainAddFunctionInclude 
 *****************************************************************************/
void
MainAddFunctionInclude
()
{
  int                                   i;
  int                                   linesCount;
  char**                                lines;
  int                                   bufferSize;
  string                                buffer;

  if ( NULL == MainSourceName ) {
    return;
  }

  if ( !FileExists(MainSourceName) ) {
    fprintf(stderr, "Could not find %s\n", MainSourceName);
    exit(EXIT_FAILURE);
  }

  if ( !GetFileBuffer(MainSourceName, &buffer, &bufferSize) ) {
    fprintf(stderr, "Could not read %s\n", MainSourceName);
    exit(EXIT_FAILURE);
  }

  GetFileLines(buffer, bufferSize, &lines, &linesCount);

  MainInsertFunctionInclude(&lines, &linesCount);
  ResaveFile(MainSourceName, lines, linesCount);

  for (i = 0; i < linesCount; i++ ) {
    FreeMemory(lines[i]);
  }

  FreeMemory(lines);
  FreeMemory(buffer);
}

/*****************************************************************************!
 * Function : ProcessCommandLine
 *****************************************************************************/
void
ProcessCommandLine
(int argc, char** argv)
{
  string                                type, name;
  int                                   i;
  string                                command;
  Parameter*                            parameter;
  
  for ( i = 1; i < argc; i++ ) {
    command = argv[i];
    if ( StringEqualsOneOf(command, "-h", "--help", NULL) ) {
      MainDisplayHelp();
      exit(EXIT_SUCCESS);
    }

    //
    if ( StringEqualsOneOf(command, "-f", "--function", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a function name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainFunctionName ) {
        FreeMemory(MainFunctionName);
      }
      MainFunctionName = StringCopy(argv[i]);
      continue;
    }

    //! Specify the data name
    if ( StringEqualsOneOf(command, "-d", "--data", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a data name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainDataName ) {
        FreeMemory(MainDataName);
      }
      MainDataName = StringCopy(argv[i]);
      continue;
    }

    //! Specify the type (for the data name)
    if ( StringEqualsOneOf(command, "-t", "--datatype", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a type name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainDataType ) {
        FreeMemory(MainDataType);
      }
      MainDataType = StringCopy(argv[i]);
      continue;
    }

    //!
    if ( StringEqualsOneOf(command, "-l", "--local", NULL) ) {
      MainElementScope = ElementScopeLocal;
      continue;
    }

    //!
    if ( StringEqualsOneOf(command, "-g", "--global", NULL) ) {
      MainElementScope = ElementScopeGlobal;
      continue;
    }

    //! Specify the module
    if ( StringEqualsOneOf(command, "-m", "--module", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a module name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainModuleName ) {
        FreeMemory(MainModuleName);
      }
      MainModuleName = StringCopy(argv[i]);
      continue;
    }

    //!
    if ( StringEqualsOneOf(command, "-o", "--overwrite", NULL) ) {
      MainOverwriteFunctionFile = true;
      continue;
    }
    
    //!
    if ( StringEqualsOneOf(command, "-r", "--returntype", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a return type%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainReturnType ) {
        FreeMemory(MainReturnType);
      }
      MainReturnType = StringCopy(argv[i]);
      continue;
    }

    //!
    if ( StringEqualsOneOf(command, "-p", "--parameters", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s requires parameters%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      while (i < argc) {
        type = argv[i];
        if ( StringEqual("...", type) ) {
          if ( MainParameters && i + 1 == argc ) {
            parameter = ParameterCreate(type, NULL);
            MainParameters = ParameterAppend(MainParameters, parameter);
            i++;
            break;
          } else {
            fprintf(stderr, "Variadic with not leading parameters\n");
            exit(EXIT_FAILURE);
          }
        }

        i++;
        if ( i == argc ) {
          fprintf(stderr, "%sMissing parameter name%s\n", ColorBrightRed, ColorReset);
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
        name = argv[i];
        i++;
        parameter = ParameterCreate(type, name);
        MainParameters = ParameterAppend(MainParameters, parameter);
      }
      break;
    }
    fprintf(stderr, "%s is an invalid option\n", command);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : VerifyCommandLine
 *****************************************************************************/
void
VerifyCommandLine
()
{
  //! Must specifiy either funcdtion nammd or data name
  if ( MainFunctionName == NULL && MainDataName == NULL ) {
    fprintf(stderr, "%sEither a function name or data name must be specified%s\n",
            ColorBrightRed, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }

  //! But can't specify both
  if ( MainFunctionName && MainDataName ) {
    fprintf(stderr, "%sOnly a function name or data name can be specified at one time%s\n",
            ColorBrightRed, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }

  //! The Module name is the base name of the header and source file name
  if ( MainModuleName ) {
    MainHeaderName = StringConcat(MainModuleName, MainHeaderSuffix);
    MainSourceName = StringConcat(MainModuleName, MainSourceSuffix);
  }  
 
  if ( MainFunctionName ) {
    //! If we have a module name and it's a directory, prepend the directory name to the output filename
    if ( MainModuleName && FileExists(MainModuleName) ) {
      MainFilename = StringMultiConcat(MainModuleName, DirSeparator, MainFunctionName, MainSourceSuffix, NULL);
    } else {
      MainFilename = StringMultiConcat(MainFunctionName, MainSourceSuffix, NULL);
    }

    //! Don't overwrite the old file unless told to
    if ( FileExists(MainFilename) && !MainOverwriteFunctionFile) {
      fprintf(stderr, "%s%s exists%s\n", ColorBrightRed, MainFilename, ColorReset);
      exit(EXIT_FAILURE);
    }
  }
}

/*****************************************************************************!
 * Function : ParameterCreate
 *****************************************************************************/
Parameter*
ParameterCreate
(string InType, string InName)
{
  Parameter*                            parameter;

  if ( NULL == InType ) {
    return NULL;
  }
  
  if ( StringEqual("...", InType) && NULL != InName ) {
    return NULL;
  }

  parameter = (Parameter*)GetMemory(sizeof(Parameter));
  parameter->type = StringCopy(InType);
  if ( InName ) {
    parameter->name = StringCopy(InName);
  }
  parameter->next = NULL;
  return parameter;
}

/*****************************************************************************!
 * Function : ParameterAppend
 *****************************************************************************/
Parameter*
ParameterAppend
(Parameter* InHead, Parameter* InParameter)
{
  Parameter*                           parameter;
  if ( NULL == InParameter ) {
    return InHead;
  }

  if ( NULL == InHead ) {
    return InParameter;
  }

  for ( parameter = InHead; parameter->next ; parameter = parameter->next ) {
  }
  parameter->next = InParameter;
  return InHead;
}

/*****************************************************************************!
 * Function : MainDisplayHelp
 *****************************************************************************/
void
MainDisplayHelp
()
{
  fprintf(stdout, "Usage : %s options\n", MainProgramName);
  fprintf(stdout, "         -h, --help                    : Display this message\n");
  fprintf(stdout, "         -f, --function function-name  : Specifiy the function name\n");
  fprintf(stdout, "         -m, --module module-name      : Specifiy the module name (if any)\n");
  fprintf(stdout, "         -H, --header header-name      : Specify the header name (if function is exported)\n");
  fprintf(stdout, "         -o, --overwrite               : Specify to overwrite function file if it exists\n");
  fprintf(stdout, "         -p, --parameters {type name}* : Specify the function parameters (must be last option)\n");
  fprintf(stdout, "         -l, --local                   : Specify that the element is local\n");
  fprintf(stdout, "         -g, --global                  : Specify that the element is global\n");
  fprintf(stdout, "         -d, --data dataname           : Specify the data name\n");
  fprintf(stdout, "         -t, --datatype datatype       : Specify the type of a new data item\n");
  fprintf(stdout, "         -r, --returntype type         : Specify the return type of a function\n");
}

/*****************************************************************************!
 * Function : MainWriteFunctionFile
 *****************************************************************************/
void
MainWriteFunctionFile
()
{
  Parameter*                            parameter;
  FILE*                                 file;

  file = fopen(MainFilename, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open file %s : %s\n", MainFilename, strerror(errno));
    exit(EXIT_FAILURE);
  }
  fprintf(file,  FunctionHeaderTemplate, MainFunctionName);
  fprintf(file, "%s\n", MainReturnType);
  fprintf(file, "%s\n", MainFunctionName);
  fprintf(file, "(");
  for ( parameter = MainParameters; parameter; parameter = parameter->next ) {
    fprintf(file, "%s", parameter->type);
    if ( parameter->name ) {
      fprintf(file, " %s", parameter->name);
    }
    if ( parameter->next ) {
      fprintf(file, ", ");
    }
  }
  fprintf(file, ")\n");
  fprintf(file, "{\n");
  fprintf(file, "}\n");
  fclose(file);
}

/*****************************************************************************!
 * Function : MainAddFunctionDeclaration
 *****************************************************************************/
void
MainAddFunctionDeclaration
()
{
  if ( MainElementScope == ElementScopeGlobal ) {
    if ( FileExists(MainHeaderName) ) {
      MainAddFunctionToHeader();
    } else {
      fprintf(stderr, "%sCan not add global %s to %s - %s does not exist%s\n",
              ColorBrightRed, MainFunctionName, MainHeaderName, MainHeaderName, ColorReset);
    }
  } else if ( MainElementScope == ElementScopeLocal ) {
    if ( FileExists(MainSourceName) ) {
      MainAddFunctionToSource();
    } else {
      fprintf(stderr, "%sCan not add global %s to %s - %s does not exist%s\n",
              ColorBrightRed, MainFunctionName, MainHeaderName, MainSourceName, ColorReset);
    }
  }
}

/*****************************************************************************!
 * Function : MainAddFunctionToSource 
 *****************************************************************************/
void
MainAddFunctionToSource
()
{
  bool                                  found;
  string                                s;
  int                                   i;
  int                                   linesCount;
  char**                                lines;
  int                                   bufferSize;
  string                                buffer;

  if ( NULL == MainSourceName ) {
    return;
  }

  if ( !FileExists(MainSourceName) ) {
    fprintf(stderr, "Could not find %s\n", MainSourceName);
    exit(EXIT_FAILURE);
  }

  if ( !GetFileBuffer(MainSourceName, &buffer, &bufferSize) ) {
    fprintf(stderr, "Could not read %s\n", MainSourceName);
    exit(EXIT_FAILURE);
  }

  GetFileLines(buffer, bufferSize, &lines, &linesCount);

  found = false;
  for (i = 0; i < linesCount && !found; i++) {
    s = StringTrim(lines[i]);
    if ( StringEqual(s, "* Local Functions") ) {
      i += 2;
      InsertFunctionDeclaration(&lines, &linesCount, i);
      ResaveFile(MainSourceName, lines, linesCount);
      found = true;
    }
    FreeMemory(s);
  }

  if ( !found ) {
    fprintf(stderr, "Could not find a Local Functions section in %s\n", MainSourceName);
  }
  for (i = 0; i < linesCount; i++ ) {
    FreeMemory(lines[i]);
  }

  FreeMemory(lines);
  FreeMemory(buffer);
}

/*****************************************************************************!
 * Function : MainAddFunctionToHeader
 *****************************************************************************/
void
MainAddFunctionToHeader
()
{
  bool                                  found;
  string                                s;
  int                                   i;
  int                                   linesCount;
  char**                                lines;
  int                                   bufferSize;
  string                                buffer;
  if ( NULL == MainHeaderName ) {
    return;
  }

  if ( !FileExists(MainHeaderName) ) {
    fprintf(stderr, "Could not find %s\n", MainHeaderName);
    exit(EXIT_FAILURE);
  }

  if ( !GetFileBuffer(MainHeaderName, &buffer, &bufferSize) ) {
    fprintf(stderr, "Could not read %s\n", MainHeaderName);
    exit(EXIT_FAILURE);
  }

  GetFileLines(buffer, bufferSize, &lines, &linesCount);

  found = false;
  for (i = 0; i < linesCount; i++) {
    s = StringTrim(lines[i]);
    if ( StringEqual(s, "* Exported Functions") ) {
      i += 2;
      InsertFunctionDeclaration(&lines, &linesCount, i);
      ResaveFile(MainHeaderName, lines, linesCount);
      found = true;
    }
    FreeMemory(s);
  }

  if ( !found ) {
    fprintf(stderr, "Could not find a Exported Functions section in %s\n", MainHeaderName);
  }
  for (i = 0; i < linesCount; i++ ) {
    FreeMemory(lines[i]);
  }

  FreeMemory(lines);
  FreeMemory(buffer);
}

/*****************************************************************************!
 * Function : InsertFunctionDeclaration
 *****************************************************************************/
void
InsertFunctionDeclaration
(char*** InLines, int* InLinesCount, int InInsertPoint)
{
  int                                   j;
  Parameter*                            parameter;
  char**                                newLines;
  int                                   newLinesCount;
  bool                                  needNewLine;
  int                                   requiredLines;
  int                                   insertPoint;
  string                                s;
  int                                   i;
  int                                   linesCount;
  char**                                lines;

  lines = *InLines;
  linesCount = *InLinesCount;

  insertPoint = InInsertPoint;
  requiredLines = 3;
  s = StringTrim(lines[insertPoint]);
  needNewLine = false;
  if ( s && *s != NUL ) {
    requiredLines++;
    needNewLine = true;
  }
  FreeMemory(s);

  newLinesCount = linesCount + requiredLines;
  newLines = (char**)GetMemory(sizeof(char*) * newLinesCount);

  for (i = 0; i < InInsertPoint; i++) {
    newLines[i] = lines[i];
  }
  newLines[i++] = StringCopy(MainReturnType);
  newLines[i++] = StringCopy(MainFunctionName);
  s = StringCopy("(");
  for ( parameter = MainParameters; parameter; parameter = parameter->next ) {
    s = StringConcatTo(s, parameter->type);
    if ( parameter->name ) {
      s = StringConcatTo(s, " ");
      s = StringConcatTo(s, parameter->name);
    }
    if ( parameter->next ) {
      s = StringConcatTo(s, ", ");
    }
  }
  s = StringConcatTo(s, ");");
  newLines[i++] = s;
  if ( needNewLine ) {
    newLines[i++] = StringCopy("");
  }

  for ( j = insertPoint; j < linesCount; j++, i++) {
    newLines[i] = lines[j];
  }
  
  FreeMemory(lines);
  *InLines = newLines;
  *InLinesCount = newLinesCount;
}

/*****************************************************************************!
 * Function : ResaveFile
 *****************************************************************************/
void
ResaveFile
(string InFilename, char** InLines, int InLinesCount)
{
  int                                   n;
  string                                line;
  FILE*                                 file;
  int                                   i;

  file = fopen(InFilename, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open %s\n", InFilename);
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < InLinesCount; i++) {
    line = StringCopy(InLines[i]);
    n = strlen(line);
    while (--n && n != 0) {
      if ( !isspace(line[n]) ) {
        break;
      }
    }
    line[n+1] = NUL;
    fprintf(file, "%s\n", line);
    FreeMemory(line);
  }
  fclose(file);
}

/*****************************************************************************!
 * Function : MainInsertFunctionInclude
 *****************************************************************************/
void
MainInsertFunctionInclude
(char*** InLines, int* InLinesCount)
{
  string                                s;
  char**                                lines;
  char**                                newLines;
  int                                   linesCount, newLinesCount;
  int                                   i;
  string                                filename;

  if ( MainModuleName && FileExists(MainModuleName) ) {
    filename = StringMultiConcat(MainModuleName, DirSeparator, MainSourceName, NULL);
  } else {
    filename = StringCopy(MainSourceName);
  }
  s = StringMultiConcat("#include \"", filename, "\"\n", NULL);
  
  lines = *InLines;
  linesCount = *InLinesCount;
  
  newLinesCount = linesCount + 1;

  newLines = (char**)GetMemory(newLinesCount * sizeof(char*));
  for ( i = 0 ; i < linesCount; i++ ) {
    newLines[i] = lines[i];
  }

  FreeMemory(lines);
  newLines[i] = s;
  *InLines = newLines;
  *InLinesCount = newLinesCount;
}

/*****************************************************************************!
 * Function : CreateFileBackupCopy
 *****************************************************************************/
void
CreateFileBackupCopy
(string InFilename)
{
  char*                                 buffer;
  int                                   bufferSize;
  string                                filename;
  FILE*                                 file;

  if ( NULL == InFilename ) {
    return;
  }

  if ( !FileExists(InFilename) ) {
    return;
  }

  GetFileBuffer(InFilename, &buffer, &bufferSize);
  filename = StringConcat(InFilename, "-bak");
 
  file = fopen(filename, "wb");
  fwrite(buffer, 1, bufferSize, file);
  fclose(file);

  FreeMemory(buffer);
  FreeMemory(filename);
}

