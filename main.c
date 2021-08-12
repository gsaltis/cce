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
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <StringUtils.h>
#include <MemoryManager.h>
#include <ascii.h>
#include <ANSIColors.h>
#include <FileUtils.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
/*****************************************************************************!
 * Local Macros
 *****************************************************************************/
#define MAIN_DEFAULT_PROJECT_NAME                       "NONE"
#define MAIN_DEFAULT_COPYRIGHT_HOLDER_NAME      "Gregory R Saltis"

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
 * Local Type : FunctionType
 *****************************************************************************/
enum _FunctionType
{
  FunctionTypeNone                                      = 0,
  FunctionTypeC,
  FunctionTypeJavascript
};
typedef enum _FunctionType FunctionType;

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static bool
MainCreateMainFile = false;

bool
MainReplaceCodeLine = false;

string
MainCodeLine = NULL;

int
MainCodeLineNumber = 0;

StringList*
MainMakefileObjects = NULL;

string
MainMakefileName = "Makefile";

static bool
MainCreateMakefile = false;

static string
MainTarget = NULL;

static string
MainInitializeDataValue = NULL;

static FunctionType
MainFunctionType = FunctionTypeC;

static bool
MainAddStructElements = false;

static string
MainProjectName = NULL;

static string
MainCopyrightHolderName = NULL;

static bool
MainOverwriteFunctionFile;

static bool
MainCreateModuleDirectory = false;

static string
MainProgramName = "cce";

static string
MainFilename;

static string
MainReturnType;

static string
MainSourceSuffix = ".c";

static string
MainJavascriptSuffix = ".js";

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
MainMonths[] = {
  "January", "February", "March",     "April",   "May",      "June",
  "July",    "August",   "September", "October", "November", "December"
};

static string
MainGlobalHeaders[] = {
  "stdio.h", "stdlib.h", "string.h", "stdbool.h", "stdint.h"
};

static string
MainStructName = NULL;

static string
FunctionHeaderTemplate =
  "/*****************************************************************************!\n"
  " * Function : %s\n"
  " *****************************************************************************/\n";

static string
StructHeaderTemplate =
  "/*****************************************************************************!\n"
  " * Exported Type : %s\n"
  " *****************************************************************************/\n";

ElementScope
MainElementScope = ElementScopeGlobal;

string
MainDataName = NULL;

string
MainDataType = NULL;

string
MainNewModuleName = NULL;

string
MainVersion = "1.1.2";

bool
MainAddGlobalHeaders = false;

bool
MainAddLocalHeaders = false;

string
MainGlobalHeaderName = NULL;

string
MainLocalHeaderName = NULL;

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
CreateMainFile
(void);

void
MainCreateBlock
(FILE* InFile, string InBlockText);

void
MainCreateFileHeader
(string InFilename, FILE* InFile);

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
MainWriteJavascriptFunctionFile
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

void
MainAddNewModuleItem
();

void
MainAddNewStructure
();

void
MainAddNewStructElements
();

void
MainHandleAddGlobalHeader
();

void
MainHandleAddLocalHeader
();

void
CreateMakefile
();

void
ReplaceCodeLine
();

string
CreateFunctionFilename
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
  if ( MainCreateMainFile ) {
    CreateMainFile();
    return EXIT_SUCCESS;
  }
  if ( MainReplaceCodeLine ) {
        ReplaceCodeLine();
  } else if ( MainCreateMakefile ) {
        CreateMakefile();
  } else if ( MainAddGlobalHeaders ) {
        MainHandleAddGlobalHeader();
  } else if ( MainAddLocalHeaders ) {
        MainHandleAddLocalHeader();
  } else if ( MainFunctionName ) {
    MainAddFunctionItem();
  } else if ( MainDataName ) {
    MainAddDataItem();
  } else if ( MainNewModuleName ) {
        MainAddNewModuleItem();
  } else if ( MainAddStructElements ) {
        MainAddNewStructElements();
  } else if ( MainStructName ) {
        MainAddNewStructure();
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************!
 * Function : ReplaceCodeLine
 *****************************************************************************/
void
ReplaceCodeLine
()
{
  string                                filename;
  char*                                                                 buffer;
  int                                   bufferLen;
  char**                                lines;
  int                                   lineCount;
  int                                                                   n, i;
  FILE*                                                                 file;

  filename = CreateFunctionFilename();
  if ( NULL == filename ) {
        fprintf(stderr, "Could not create function filename\n");
        exit(EXIT_FAILURE);
  }
  if ( !FileExists(filename) ) {
        fprintf(stderr, "%s does not exist\n", filename);
        FreeMemory(filename);
        exit(EXIT_FAILURE);
  }
  if ( !GetFileBuffer(filename, &buffer, &bufferLen) ) {
        fprintf(stderr, "Could not read %s : %s\n", filename, strerror(errno));
        FreeMemory(filename);
        exit(EXIT_FAILURE);
  }
  GetFileLines(buffer, bufferLen, &lines, &lineCount);
  FreeMemory(buffer);

  if ( lineCount < MainCodeLineNumber ) {
        fprintf(stderr, "%d line does not exists in %s\n", MainCodeLineNumber, filename);
        exit(EXIT_FAILURE);
  }

  n = MainCodeLineNumber - 1;
  CreateFileBackupCopy(filename);
  FreeMemory(lines[n]);
  lines[n] = StringCopy(MainCodeLine);
  file = fopen(filename, "wb");
  if ( NULL == file ) {
        fprintf(stderr, "Could not open %s for writing : %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
  }
  for ( i = 0 ; i < lineCount; i++ ) {
        fprintf(file, "%s\n", lines[i]);
  }
  fclose(file);
  fprintf(stderr, "%s updated\n", filename);
}

/*****************************************************************************!
 * Function : MainHandleAddGlobalHeader
 *****************************************************************************/
void
MainHandleAddGlobalHeader
()
{
  string                                                                filename;
  string                                                                headername;
  char*                                                                 buffer;
  char**                                                                lines;
  int                                                                   bufferSize;
  int                                                                   linesCount;
  FILE*                                                                 file;
  int                                                                   i;
  bool                                                                  needExtraLine;

  if ( MainElementScope == ElementScopeGlobal ) {
        filename = StringMultiConcat(MainModuleName, MainHeaderSuffix, NULL);
  } else {
        filename = StringMultiConcat(MainModuleName, MainSourceSuffix, NULL);
  }
 
  headername = StringMultiConcat("<", MainGlobalHeaderName,  ">", NULL);

  if ( !GetFileBuffer(filename, &buffer, &bufferSize) ) {
        fprintf(stderr, "Could not read contents of %s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
  }

  CreateFileBackupCopy(filename);
  if ( unlink(filename) != 0 ) {
        fprintf(stderr, "Could not remove %s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
  }
  GetFileLines(buffer, bufferSize, &lines, &linesCount);
  file = fopen(filename, "wb");
  if ( NULL == file  ) {
    fprintf(stderr, "Could not open %s for output : %s\n", filename, strerror(errno));
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
        for (i = 0; i < linesCount; i++ ) {
          FreeMemory(lines[i]);
        }
        FreeMemory(lines);
        exit(EXIT_FAILURE);
  }
  for ( i = 0 ; i < linesCount; i++ ) {
        fprintf(file, "%s\n", lines[i]);
        if ( StringEqual(lines[i], " * Global Headers") ) {
          break;
        }
  }

  if ( i == linesCount ) {
        fprintf(stderr, "No Global Headers section found in f%s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
        for (i = 0; i < linesCount; i++ ) {
          FreeMemory(lines[i]);
        }
        FreeMemory(lines);
        return;
  }
  i++;
  needExtraLine = false;
  fprintf(file, "%s\n", lines[i]);
  for ( i++ ; i < linesCount; i++ ) {
        if ( lines[i][0] == 0x00 ) {
          break;
        }
        if ( lines[i][0] == '/' ) {
          needExtraLine = true;
          break;
        }
        fprintf(file, "%s\n", lines[i]);
  }
  fprintf(file, "#include %s\n", headername);
  if ( needExtraLine ) {
        fprintf(file, "\n");
  }
  for ( ; i < linesCount; i++ ) {
        fprintf(file, "%s\n", lines[i]);
  }
  fclose(file);

  FreeMemory(filename);
  FreeMemory(buffer);
  FreeMemory(headername);
  for ( i = 0 ; i < linesCount ; i++ ) {
        FreeMemory(lines[i]);
  }
  FreeMemory(lines);
}

/*****************************************************************************!
 * Function : MainHandleAddLocalHeader
 *****************************************************************************/
void
MainHandleAddLocalHeader
()
{
  string                                                                filename;
  string                                                                headername;
  char*                                                                 buffer;
  char**                                                                lines;
  int                                                                   bufferSize;
  int                                                                   linesCount;
  FILE*                                                                 file;
  int                                                                   i;
  bool                                                                  needExtraLine;

  if ( MainElementScope == ElementScopeGlobal ) {
        filename = StringMultiConcat(MainModuleName, MainHeaderSuffix, NULL);
  } else {
        filename = StringMultiConcat(MainModuleName, MainSourceSuffix, NULL);
  }
 
  headername = StringMultiConcat("\"", MainLocalHeaderName,  "\"", NULL);

  if ( !GetFileBuffer(filename, &buffer, &bufferSize) ) {
        fprintf(stderr, "Could not read contents of %s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
  }

  CreateFileBackupCopy(filename);
  if ( unlink(filename) != 0 ) {
        fprintf(stderr, "Could not remove %s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
  }
  GetFileLines(buffer, bufferSize, &lines, &linesCount);
  file = fopen(filename, "wb");
  if ( NULL == file  ) {
    fprintf(stderr, "Could not open %s for output : %s\n", filename, strerror(errno));
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
        for (i = 0; i < linesCount; i++ ) {
          FreeMemory(lines[i]);
        }
        FreeMemory(lines);
        exit(EXIT_FAILURE);
  }
  for ( i = 0 ; i < linesCount; i++ ) {
        fprintf(file, "%s\n", lines[i]);
        if ( StringEqual(lines[i], " * Local Headers") ) {
          break;
        }
  }

  if ( i == linesCount ) {
        fprintf(stderr, "No Local Headers section found in f%s\n", filename);
        FreeMemory(filename);
        FreeMemory(headername);
        FreeMemory(buffer);
        for (i = 0; i < linesCount; i++ ) {
          FreeMemory(lines[i]);
        }
        FreeMemory(lines);
        return;
  }
  i++;
  needExtraLine = false;
  fprintf(file, "%s\n", lines[i]);
  for ( i++ ; i < linesCount; i++ ) {
        if ( lines[i][0] == 0x00 ) {
          break;
        }
        if ( lines[i][0] == '/' ) {
          needExtraLine = true;
          break;
        }
        fprintf(file, "%s\n", lines[i]);
  }
  fprintf(file, "#include %s\n", headername);
  if ( needExtraLine ) {
        fprintf(file, "\n");
  }
  for ( ; i < linesCount; i++ ) {
        fprintf(file, "%s\n", lines[i]);
  }
  fclose(file);

  FreeMemory(filename);
  FreeMemory(buffer);
  FreeMemory(headername);
  for ( i = 0 ; i < linesCount ; i++ ) {
        FreeMemory(lines[i]);
  }
  FreeMemory(lines);
}

/*****************************************************************************!
 * Function : MainAddNewStructElements
 * Purpose  : Add new elements to an existing structure
 *****************************************************************************/
void
MainAddNewStructElements
()
{
  string                                                                filename;
  char*                                                                 buffer;
  int                                                                   bufferLength;
  char**                                                                lines;
  int                                                                   linesCount;
  FILE*                                                                 file;
  int                                                                   i;

  if ( MainModuleName == NULL ) {
        fprintf(stderr, "No module name specified\n");
        return;
  }
  if ( MainStructName == NULL ) {
        fprintf(stderr, "No structure name specified\n");
    return;
  }
  if ( MainParameters == NULL ) {
        fprintf(stderr, "No structure elements specified\n");
    return;
  }

  //! Build the structure file name
  if ( FileExists(MainModuleName) ) {
        filename = StringMultiConcat(MainModuleName, "/", MainStructName, MainHeaderSuffix, NULL);
  } else {
        filename = StringMultiConcat(MainStructName, MainHeaderSuffix);
  }

  //! Open the file and get the lines
  if ( !GetFileBuffer(filename, &buffer, &bufferLength) ) {
        fprintf(stderr, "Could not read contents of %s\n", filename);
        FreeMemory(filename);
        return;
  }
  GetFileLines(buffer, bufferLength, &lines, &linesCount);

  //! Open the header file for output
  if ( 0 != unlink(filename) ) {
        fprintf(stderr, "Could not unlink %s : %s\n", filename, strerror(errno));
        return;
  }

  //! Backup the file before we start changing it
  CreateFileBackupCopy(filename);

  file = fopen(filename, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open %s for output : %s\n", filename, strerror(errno));
    return;
  }

  //! Loop over the lines and write out the ones before the insert point
  //  (the open brace of the struct - should be first character in line
  for (i = 0; i < linesCount; i++) {
        fprintf(file, "%s\n", lines[i]);
        if ( StringEqual("{", lines[i]) ) {
          i++;
          break;
    }
  }

  if ( i == linesCount ) {
        fprintf(stderr, "Could not find the begining of the structure %s\n", MainStructName);
        return;
  }
        
  //! Insert the elements
  for ( Parameter* parameter = MainParameters; parameter; parameter = parameter->next ) {
        int                                                                     k;
        k = fprintf(file, "  %s", parameter->type);
        k = 40 - k;
        if ( k < 1 ) {
          k = 1;
    }
    fprintf(file, "%*s%s;\n", k, " ", parameter->name);
  }

  //! Write out the remainder of the structure
  for ( ; i < linesCount; i++) {
        fprintf(file, "%s\n", lines[i]);
  }

  //! Close the file
  fclose(file);

  //! Free any memory we got
  for ( i = 0 ; i < linesCount; i++ ) {
        FreeMemory(lines[i]);
  }
  FreeMemory(buffer);
  FreeMemory(lines);
  FreeMemory(filename);
}

/*****************************************************************************!
 * Function : MainAddNewStructure
 *****************************************************************************/
void
MainAddNewStructure
()
{
  string                                                                filename;
  FILE*                                                                 file;
  string                                                                fenceDefine, s;
  char**                                                                lines;
  char*                                                                 buffer;
  int                                                                   bufferLength;
  int                                                                   linesCount;
  int                                                                   i, k;
  string                                                                filename2;

  s = StringConcat(MainStructName, MainHeaderSuffix);
  fenceDefine = StringToLowerCase(s);
  FreeMemory(s);

  for (s = fenceDefine ; *s; s++ ) {
        if ( !StringContainsChar("abcdefghijklmnopqrstuvwxyz0123456789_", *s) ) {
          *s = '_';
        }
  }
  if ( FileExists(MainModuleName) ) {
        filename = StringMultiConcat(MainModuleName, "/", MainStructName, MainHeaderSuffix, NULL);
  } else {
        filename = StringMultiConcat(MainStructName, MainHeaderSuffix, NULL);
  }

  if ( FileExists(filename) && !MainOverwriteFunctionFile ) {
        fprintf(stderr, "%s exists cannot overwrite\n", filename);
  }
  file = fopen(filename, "wb");
  if ( NULL == file ) {
        fprintf(stderr, "Could not open %s : %s\n", filename, strerror(errno));
  }

  fprintf(file, StructHeaderTemplate, MainStructName);
  fprintf(file, "#ifndef _%s_\n", fenceDefine);
  fprintf(file, "#define _%s_\n", fenceDefine);
  fprintf(file, "struct _%s\n", MainStructName);
  fprintf(file, "{\n");
  if ( MainParameters ) {
    for ( Parameter* parameter = MainParameters; parameter; parameter = parameter->next ) {
          k = fprintf(file, "  %s", parameter->type);
          k = 40 - k;
          if ( k <= 0 ) {
                k = 1;
          }
          fprintf(file, "%*s%s;\n", k, " ", parameter->name);
        }
  }
  fprintf(file, "};\n");
  fprintf(file, "typedef struct _%s %s;\n", MainStructName, MainStructName);
  fprintf(file, "#endif // _%s_\n", fenceDefine);
  fclose(file);
  FreeMemory(fenceDefine);

  filename2 = StringConcat(MainModuleName, MainHeaderSuffix);
  GetFileBuffer(filename2, &buffer, &bufferLength);
  GetFileLines(buffer, bufferLength, &lines, &linesCount);
  unlink(filename2);
  file = fopen(filename2, "wb");
  for (i = 0; i < linesCount; i++) {
        fprintf(file, "%s\n", lines[i]);
        if ( StringEqual(" * Local Headers", lines[i]) ) {
          FreeMemory(lines[i]);
          i++;
          fprintf(file, "%s\n", lines[i]);
          fprintf(file, "#include \"%s\"\n", filename);
        }

        FreeMemory(lines[i]);
  }
  FreeMemory(lines);
  FreeMemory(buffer);
  FreeMemory(filename2);
  FreeMemory(filename);
  fclose(file);
        
}

/*****************************************************************************!
 * Function : MainAddNewModuleItem
 *****************************************************************************/
void
MainAddNewModuleItem
()
{
  FILE*                                                                 file;
  string                                                                s, s2;
  int                                                                   i, n;

  file = fopen(MainSourceName, "wb");
  if ( NULL == file ) {
        fprintf(stderr, "Could not open %s : %s\n", MainSourceName, strerror(errno));
        exit(EXIT_FAILURE);
  }
  MainCreateFileHeader(MainSourceName, file);
  fprintf(file, "\n");

  MainCreateBlock(file, "Global Headers");
  n = sizeof(MainGlobalHeaders) / sizeof(MainGlobalHeaders[0]);
  for ( i = 0 ; i < n ; i++ ) {
    fprintf(file, "#include <%s>\n", MainGlobalHeaders[i]);
  }
  fprintf(file, "\n");

  MainCreateBlock(file, "Local Headers");
  fprintf(file, "#include \"%s\"\n", MainHeaderName);
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Macros");
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Data");
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Functions");
  fprintf(file, "\n");
 
  fclose(file);

  file = fopen(MainHeaderName, "wb");
  if ( NULL == file ) {
        fprintf(stderr, "Could not open %s : %s\n", MainHeaderName, strerror(errno));
        exit(EXIT_FAILURE);
  }
  MainCreateFileHeader(MainHeaderName, file);
  s = StringReplaceChar(MainHeaderName, '.', '_');
  s2 = StringToLowerCase(s);
  FreeMemory(s);

  fprintf(file, "#ifndef _%s_\n", s2);
  fprintf(file, "#define _%s_\n", s2);
  fprintf(file, "\n");
  MainCreateBlock(file, "Global Headers");
  fprintf(file, "\n");

  MainCreateBlock(file, "Local Headers");
  fprintf(file, "\n");
  MainCreateBlock(file, "Exported Macros");
  fprintf(file, "\n");
  MainCreateBlock(file, "Exported Data");
  fprintf(file, "\n");
  MainCreateBlock(file, "Exported Functions");
  fprintf(file, "\n");
  fprintf(file, "#endif // _%s_\n", s2);

  fclose(file);
}

/*****************************************************************************!
 * Function : MainCreateFileHeader
 *****************************************************************************/
void
MainCreateFileHeader
(string InFilename, FILE* InFile)
{
  time_t                                                                t;
  struct tm*                                                    tm;
  char                                                                  timeString[64];

  t = time(NULL);
  tm = localtime(&t);
  sprintf(timeString, "%s %d %04d", MainMonths[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);

  fprintf(InFile, "/*****************************************************************************\n");
  fprintf(InFile, " * FILE NAME    : %s\n", InFilename);
  fprintf(InFile, " * DATE         : %s\n", timeString);
  fprintf(InFile, " * PROJECT      : %s\n", MainProjectName);
  fprintf(InFile, " * COPYRIGHT    : Copyright (C) %d by %s\n", tm->tm_year + 1900, MainCopyrightHolderName);
  fprintf(InFile, " *****************************************************************************/\n");
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
  if ( MainFunctionType == FunctionTypeC ) {
    if ( MainHeaderName && MainElementScope == ElementScopeGlobal ) {
      CreateFileBackupCopy(MainHeaderName);
    }
  }
  if ( MainFunctionType == FunctionTypeC ) {
    MainWriteFunctionFile();
    MainAddFunctionDeclaration();
    MainAddFunctionInclude();
  } else if ( MainFunctionType == FunctionTypeJavascript ) {
        MainWriteJavascriptFunctionFile();
  }
  fprintf(stdout, "%s created\n", MainFilename);
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
  fprintf(stdout, "%s created\n", MainDataName);
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
  if ( !InIsExtern && MainInitializeDataValue ) {
        newLines[i++] = StringMultiConcat(MainDataName, " = ", MainInitializeDataValue, ";", NULL);
  } else {
    newLines[i++] = StringConcat(MainDataName, ";");
  }
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
  MainProjectName                               = StringCopy(MAIN_DEFAULT_PROJECT_NAME);
  MainCopyrightHolderName               = StringCopy(MAIN_DEFAULT_COPYRIGHT_HOLDER_NAME);
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

    if ( StringEqualsOneOf(command, "-ma", "--mainfile", NULL) ) {
      MainCreateMainFile = true;
      continue;
    }
    if ( StringEqualsOneOf(command, "-se", "--structelement", NULL) ) {
          MainAddStructElements = true;
          continue;
        }

        if ( StringEqualsOneOf(command, "-M", "--createmakefile", NULL ) ) {
          MainCreateMakefile = true;
          continue;
        }

        if ( StringEqualsOneOf(command, "-C", "--codeline", NULL) ) {
          MainReplaceCodeLine = true;
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s requires a line of code\n", command);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          if ( MainCodeLine ) {
                FreeMemory(MainCodeLine);
          }
          MainCodeLine = StringCopy(argv[i]);
          continue;
        }

        if ( StringEqualsOneOf(command, "-N", "--linenumber", NULL) ) {
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s requires a line number\n", command);
            MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          MainCodeLineNumber = atoi(argv[i]); 
          continue;
        }

        if ( StringEqualsOneOf(command, "-O", "--mainobjects", NULL) ) {
          i++;
          if ( i == argc ) {
            fprintf(stderr, "%s requires a series of objects\n", command);
            MainDisplayHelp();
            exit(EXIT_FAILURE);
          }
          MainMakefileObjects = StringListCreate();
          while ( i < argc ) {
            StringListAppend(MainMakefileObjects, StringCopy(argv[i]));
            i++;
          }
          continue;
        }

        if ( StringEqualsOneOf(command, "-T", "--target", NULL) ) {
          i++;
        
          if ( i == argc  ) {
                fprintf(stderr, "%s requires a target name\n", command);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          if ( MainTarget ) {
                FreeMemory(MainTarget);
          }
          MainTarget = StringCopy(argv[i]);
          continue;
        }

        if ( StringEqualsOneOf(command, "-s", "--struct", NULL) ) {
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s requires a structure name\n", command);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          if ( MainStructName ) {
                FreeMemory(MainStructName);
          }
          MainStructName = StringCopy(argv[i]);
          continue;
        }

    if ( StringEqualsOneOf(command, "-h", "--help", NULL) ) {
      MainDisplayHelp();
      exit(EXIT_SUCCESS);
    }

        //
        if ( StringEqualsOneOf(command, "-i", "--init", NULL) ) {
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s%s is missing an initial value%s\n", ColorBrightRed, command, ColorReset);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
      if ( MainInitializeDataValue ) {
                FreeMemory(MainInitializeDataValue);
          }
          MainInitializeDataValue = StringCopy(argv[i]);
      continue;
        }

        //
        if ( StringEqualsOneOf(command, "-v", "--version", NULL) ) {
          fprintf(stdout, "%s : Version %s\n", MainProgramName, MainVersion);
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

    if ( StringEqualsOneOf(command, "-j", "--javascript", NULL) ) {
          MainFunctionType = FunctionTypeJavascript;
          continue;
    }
    if ( StringEqualsOneOf(command, "-pr", "--project", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a project name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainProjectName ) {
        FreeMemory(MainProjectName);
      }
      MainProjectName = StringCopy(argv[i]);
      continue;
    }

    if ( StringEqualsOneOf(command, "-co", "--copyrightholder", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a copyright holder name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainCopyrightHolderName ) {
        FreeMemory(MainCopyrightHolderName);
      }
      MainCopyrightHolderName = StringCopy(argv[i]);
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

    //! Specify the new module name
    if ( StringEqualsOneOf(command, "-n", "--newmodule", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a module name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainNewModuleName ) {
        FreeMemory(MainNewModuleName);
      }
      MainNewModuleName = StringCopy(argv[i]);
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

        if ( StringEqualsOneOf(command, "-L", "--addlocalheader", NULL) ) {
          MainAddLocalHeaders = true;
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s is missing a header name\n", command);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          MainLocalHeaderName = StringCopy(argv[i]);
          continue;
        }



        if ( StringEqualsOneOf(command, "-G", "--addglobalheader", NULL) ) {
          MainAddGlobalHeaders = true;
          i++;
          if ( i == argc ) {
                fprintf(stderr, "%s is missing a header name\n", command);
                MainDisplayHelp();
                exit(EXIT_FAILURE);
          }
          MainGlobalHeaderName = StringCopy(argv[i]);
          continue;
        }

        if ( StringEqualsOneOf(command, "-nd", "--newmoduledirectory", NULL) ) {
          MainCreateModuleDirectory = true;
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
        if ( StringEqualsOneOf(command, "-c", "--createmoduledir", NULL) ) {
          MainCreateModuleDirectory = true;
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

    if ( StringEqualsOneOf(command, "-P", "--typelessparameters", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s requires parameters%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      while (i < argc) {
        name = argv[i];
        i++;
        parameter = ParameterCreate(NULL, name);
        MainParameters = ParameterAppend(MainParameters, parameter);
      }
      break;
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
  if ( MainReplaceCodeLine ) {
        if ( MainModuleName == NULL ) {
          fprintf(stderr, "Missing module name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }

        if ( MainCodeLine == NULL ) {
          fprintf(stderr, "Missing code line\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }

        if ( MainCodeLineNumber == 0 ) {
          fprintf(stderr, "Missing code line number\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
        return;
  }
  if ( MainAddGlobalHeaders ) {
        if ( MainGlobalHeaderName == NULL ) {
          fprintf(stderr, "Missing global header name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
    if ( MainModuleName == NULL ) {
          fprintf(stderr, "Missing module name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
  }

  if ( MainCreateMakefile ) {
        if ( MainTarget == NULL ) {
          fprintf(stderr, "Missing target name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
        return;
  }

  if ( MainAddLocalHeaders ) {
        if ( MainLocalHeaderName == NULL ) {
          fprintf(stderr, "Missing local header name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
    if ( MainModuleName == NULL ) {
          fprintf(stderr, "Missing module name\n");
          MainDisplayHelp();
          exit(EXIT_FAILURE);
        }
  }

  //! Must specifiy either funcdtion nammd or data name
  if ( !MainCreateMainFile && !MainAddLocalHeaders && !MainAddGlobalHeaders && MainFunctionName == NULL && MainDataName == NULL && MainNewModuleName == NULL && MainStructName == NULL ) {
    fprintf(stderr, "%sEither a global header, local header, structure name, function name, new module name or data name must be specified%s\n",
            ColorBrightRed, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }

  //! But can't specify both
  if ( (MainCreateMainFile ? 1 : 0) + (MainAddLocalHeaders ? 1 : 0) + (MainAddGlobalHeaders ? 1 : 0) + (MainStructName ? 1 : 0) + ((MainFunctionName ? 1 : 0) + (MainDataName ? 1 : 0) + (MainNewModuleName ? 1 : 0)) != 1) {
    fprintf(stderr, "%sOnly a local header, global header, structure name, function name, new module name or data name can be specified at one time%s\n",
            ColorBrightRed, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }

  if ( MainStructName && MainModuleName == NULL ) {
        fprintf(stderr, "A new structre requires a module\n");
        exit(EXIT_FAILURE);
  }

  //! The Module name is the base name of the header and source file name
  if ( MainModuleName ) {
    MainHeaderName = StringConcat(MainModuleName, MainHeaderSuffix);
    MainSourceName = StringConcat(MainModuleName, MainSourceSuffix);
  }

  // Check to see if we have to create a new module directory
  if ( MainNewModuleName ) {
        if ( MainCreateModuleDirectory ) {
          if ( FileExists(MainNewModuleName) ) {
                if ( ! MainOverwriteFunctionFile ) {
                  fprintf(stderr, "Module directory %s exists\n", MainNewModuleName);
                  exit(EXIT_FAILURE);
                }
          }
#ifdef LINUX          
          mkdir(MainNewModuleName, 0755);
#else           
          mkdir(MainNewModuleName);
#endif          
        }

    MainHeaderName = StringConcat(MainNewModuleName, MainHeaderSuffix);
    MainSourceName = StringConcat(MainNewModuleName, MainSourceSuffix);
        if ( FileExists(MainSourceName) && !MainOverwriteFunctionFile ) {
          fprintf(stderr, "%s%s exists%s\n", ColorBrightRed, MainSourceName, ColorReset);
          exit(EXIT_FAILURE);
        }
        if ( FileExists(MainHeaderName) && !MainOverwriteFunctionFile ) {
          fprintf(stderr, "%s%s exists%s\n", ColorBrightRed, MainHeaderName, ColorReset);
          exit(EXIT_FAILURE);
        }
  }

  if ( MainFunctionName ) {
    //! If we have a module name and it's a directory, prepend the directory name to the output filename
    if ( MainModuleName && FileExists(MainModuleName) ) {
      MainFilename = StringMultiConcat(MainModuleName, DirSeparator, MainFunctionName, NULL);
    } else {
      MainFilename = StringMultiConcat(MainFunctionName, NULL);
    }
        if ( MainFunctionType == FunctionTypeC ) {
          MainFilename = StringConcatTo(MainFilename, MainSourceSuffix);
    } else {
          MainFilename = StringConcatTo(MainFilename, MainJavascriptSuffix);
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

  if ( NULL == InType && NULL == InName ) {
    return NULL;
  }
 
  if ( StringEqual("...", InType) && NULL != InName ) {
    return NULL;
  }

  parameter = (Parameter*)GetMemory(sizeof(Parameter));
  if ( InType ) {
    
    parameter->type = StringCopy(InType);
  } else {
        parameter->type = NULL;
  }
  if ( InName ) {
    parameter->name = StringCopy(InName);
  } else {
        parameter->name = NULL;
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
  fprintf(stdout, "         -c,  --createmoduledir          : Specify the creation of a module directory\n");
  fprintf(stdout, "         -C,  --codeline line            : Specify the code line to replace\n");
  fprintf(stdout, "                                           (requires -N option)\n");
  fprintf(stdout, "                                           (requires -m module name)\n");
  fprintf(stdout, "                                           (optionaly uses -f function name)\n");
  fprintf(stdout, "         -d,  --data dataname            : Specify the data name\n");
  fprintf(stdout, "         -f,  --function function-name   : Specify the function name\n");
  fprintf(stdout, "         -G,  --addglobalheader filename : Specify the global header name to be added\n");
  fprintf(stdout, "                                           (requires module name)\n");
  fprintf(stdout, "         -g,  --global                   : Specify that the element is global\n");
  fprintf(stdout, "         -h,  --help                     : Display this message\n");
  fprintf(stdout, "         -H,  --header header-name       : Specify the header name (if function is exported)\n");
  fprintf(stdout, "         -i,  --init value               : Specify the initial value of a data value\n");
  fprintf(stdout, "         -j,  --javascript               : Specify function is javascript function\n");
  fprintf(stdout, "         -l,  --local                    : Specify that the element is local\n");
  fprintf(stdout, "         -L,  --addlocalheader filename  : Specify the local header name to be added\n");
  fprintf(stdout, "                                           (requires module name)\n");
  fprintf(stdout, "         -m,  --module module-name       : Specifiy the module name (if any)\n");
  fprintf(stdout, "         -M,  --createmakefile           : Specify the creation of a new Makefile\n");
  fprintf(stdout, "                                           (requirestarget name)\n");
  fprintf(stdout, "                                           (optionaly uses makefile objects\n");
  fprintf(stdout, "         -ma, --mainfile                 : Create a empty 'main.c'\n");
  fprintf(stdout, "         -n,  --newmodule modulename     : Specify a new module\n");
  fprintf(stdout, "         -nd, --newmoduledirectory       : Specify a new module directory is to be created\n");
  fprintf(stdout, "         -N,  --linenumber               : Specify a line number which is to be replaces\n");
  fprintf(stdout, "         -o,  --overwrite                : Specify to overwrite function file if it exists\n");
  fprintf(stdout, "         -O,  --makefileobjects {name}*  : Specify a list of makefile objects\n");
  fprintf(stdout, "         -p,  --parameters {type name}*  : Specify the function parameters (must be last option)\n");
  fprintf(stdout, "         -P,  --typelessparameters name* : Specify the function parameters (must be last option)\n");
  fprintf(stdout, "         -r,  --returntype type          : Specify the return type of a function\n");
  fprintf(stdout, "         -s,  --struct name              : Specify a new structure type\n");
  fprintf(stdout, "         -se, --structelement name       : Specify a new structure element\n");
  fprintf(stdout, "                                           (requires module name, struct name and -p elements\n");
  fprintf(stdout, "         -t,  --datatype datatype        : Specify the type of a new data item\n");
  fprintf(stdout, "         -T,  --target name              : Specifies a makefile main target\n");
  fprintf(stdout, "         -v,  --version                  : Display the version number\n");
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
 * Function : MainWriteJavascriptFunctionFile
 *****************************************************************************/
void
MainWriteJavascriptFunctionFile
()
{
  Parameter*                            parameter;
  FILE*                                 file;
  string                                filename;
 
  filename = StringCopy(MainFilename);

  file = fopen(filename, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open file %s : %s\n", filename, strerror(errno));
    FreeMemory(filename);
    exit(EXIT_FAILURE);
  }
  fprintf(file,  FunctionHeaderTemplate, MainFunctionName);
  fprintf(file, "function\n");
  fprintf(file, "%s\n", MainFunctionName);
  fprintf(file, "(");
  for ( parameter = MainParameters; parameter; parameter = parameter->next ) {
    fprintf(file, "%s", parameter->name);
    if ( parameter->next ) {
      fprintf(file, ", ");
    }
  }
  fprintf(file, ")\n");
  fprintf(file, "{\n");
  fprintf(file, "}\n");
  fclose(file);
 
  FreeMemory(filename);
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
    filename = StringMultiConcat(MainModuleName, DirSeparator, MainFunctionName, MainSourceSuffix, NULL);
  } else {
    filename = StringMultiConcat(MainFunctionName, MainSourceSuffix, NULL);
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

/*****************************************************************************!
 * Function : MainCreateBlock
 *****************************************************************************/
void
MainCreateBlock
(FILE* InFile, string InBlockText)
{
  fprintf(InFile, "/*****************************************************************************!\n");
  fprintf(InFile, " * %s\n", InBlockText);
  fprintf(InFile, " *****************************************************************************/\n");
}


/*****************************************************************************!
 * Function : CreateMakefile
 *****************************************************************************/
void
CreateMakefile
()
{
  FILE*                                 file;
  int                                   i, m, k;

  if ( MainTarget == NULL  ) {
    return;
  }

  file = fopen(MainMakefileName, "wb");
  if ( NULL == file ) {
        fprintf(stderr, "Could not open %s for writing : %s\n", MainMakefileName, strerror(errno));
        exit(EXIT_FAILURE);
  }

  fprintf(file, "CC                                     = gcc\n");
  fprintf(file, "LINK                                   = gcc\n");
  fprintf(file, "CC_OPTS                                        = -c -g -Wall\n");
  fprintf(file, "CC_INCS                                        = \n");
  fprintf(file, "LINK_OPTS                              = -g\n");
  fprintf(file, "LINK_LIBS                              = \n");
  fprintf(file, "\n");
  fprintf(file, "TARGET                                 = %s\n", MainTarget);
  fprintf(file, "OBJS                                   = $(sort                                \\\n");
  for ( i = 0 ; i < MainMakefileObjects->stringCount; i++ ) {
    fprintf(file, "                                         %s", MainMakefileObjects->strings[i]);
        m = (44 + strlen(MainMakefileObjects->strings[i])) / 8;
        if ( m >= 10 ) {
          m = 9;
        }
        for ( k = m; k < 10; k++  ) {
          fprintf(file, "\t");
        }
        fprintf(file, "\\\n");
  }
  fprintf(file, "                                          )\n");
  fprintf(file, "LIBS                                   = \n");
  fprintf(file, "%%.o                                   : %%.c\n");
  fprintf(file, "\n");
  fprintf(file, "                                         @echo [CC] $<\n");
  fprintf(file, "                                         @$(CC) $(CC_OPTS) $(CC_INCS) $<\n");
  fprintf(file, "\n");
  fprintf(file, ".PHONY                                 : $(TARGET)\n");
  fprintf(file, "$(TARGET)                              : $(OBJS)\n");
  fprintf(file, "                                         @echo [LD] $(TARGET)\n");
  fprintf(file, "                                         @$(LINK) $(LINK_OPTS) -o $(TARGET) $(OBJS) $(LINK_LIBS) $(LIBS)\n");
  fprintf(file, "\n");
  fprintf(file, "include                                          depends.mk\n");
  fprintf(file, "\n");
  fprintf(file, ".PHONY                                 : junkclean\n");
  fprintf(file, "junkclean                              : \n");
  fprintf(file, "                                         rm -rf $(wildcard *~ *-bak);\n");
  fprintf(file, "\n");
  fprintf(file, ".PHONY                                 : clean\n");
  fprintf(file, "clean                                  : junkclean\n");
  fprintf(file, "                                         rm -rf $(wildcard $(OBJS) $(TARGET)\n");
  fclose(file);
  fprintf(stdout, "%s created\n", MainMakefileName);
}

/*****************************************************************************!
 * Function : CreateFunctionFilename
 *****************************************************************************/
string
CreateFunctionFilename
()
{
  string                                filename;

  if ( NULL == MainModuleName ) {
        return NULL;
  }

  if ( NULL == MainFunctionName ) {
        return NULL;
  }

  if ( FileExists(MainModuleName) ) {
        filename = StringMultiConcat(MainModuleName, "/", MainFunctionName, MainSourceSuffix, NULL);
  } else {
        filename = StringMultiConcat(MainFunctionName, MainSourceSuffix, NULL);
  }
  return filename;
}


/*****************************************************************************!
 * Function : CreateMainFile
 *****************************************************************************/
void
CreateMainFile
(void)
{
  int                                   i;
  int                                   n;
  FILE*                                 file;
  string                                filename = "main.c";
  
  if ( FileExists(filename) ) {
    if ( ! MainOverwriteFunctionFile ) {
      fprintf(stderr, "Could not overwrite %s.\n", filename);
      exit(EXIT_FAILURE);
    }
  }

  file = fopen(filename, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open %s : %s\n", filename, strerror(errno));
    exit(EXIT_FAILURE);
  }

  MainCreateFileHeader(filename, file);
  fprintf(file, "\n");

  MainCreateBlock(file, "Global Headers");
  n = sizeof(MainGlobalHeaders) / sizeof(MainGlobalHeaders[0]);
  for ( i = 0 ; i < n ; i++ ) {
    fprintf(file, "#include <%s>\n", MainGlobalHeaders[i]);
  }
  fprintf(file, "\n");

  MainCreateBlock(file, "Local Headers");
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Macros");
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Data");
  fprintf(file, "\n");
  MainCreateBlock(file, "Local Functions");
  fprintf(file, "\n");

  MainCreateBlock(file, "Function : main");
  fprintf(file, "int\n");
  fprintf(file, "main\n");
  fprintf(file, "(int argc, char** argv)\n");
  fprintf(file, "{\n");
  fprintf(file, "  return EXIT_SUCCESS;\n");
  fprintf(file, "}\n");
  fclose(file);
}
