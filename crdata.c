/*****************************************************************************
 * FILE NAME    : crdata.c
 * DATE         : August 05 2020
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
 * Local Data
 *****************************************************************************/
static bool
MainOverwriteFunctionFile;

static FILE*
MainFile;

static string
MainProgramName = "crdata";

static string
MainFilename;

static string
MainReturnType;

static string
MainSuffix = ".c";

static string
DirSeparator = "\\";

static string
MainHeaderName;

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
  
/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
ResaveHeaderFile
(char** InLines, int InLinesCount);

void
InsertFunctionInHeader
(char*** InLines, int* InLinesCount, int InInsertPoint);

void
AppendToHeader
();

void
MainOpenFile
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
MainWriteFile
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
  MainOpenFile();
  MainWriteFile();
  AppendToHeader();
  fclose(MainFile);
  return EXIT_SUCCESS;
}

/*****************************************************************************!
 * Function : MainInitialize
 *****************************************************************************/
void
MainInitialize
()
{
  MainOverwriteFunctionFile     = false;
  MainFile                      = NULL;
  MainFilename                  = NULL;
  MainFunctionName              = NULL;
  MainModuleName                = NULL;
  MainHeaderName                = NULL;
  MainReturnType                = StringCopy("void");
  MainParameters                = NULL;
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

    //
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
    if ( StringEqualsOneOf(command, "-H", "--header", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s%s is missing a header name%s\n", ColorBrightRed, command, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      if ( MainHeaderName ) {
        FreeMemory(MainHeaderName);
      }
      MainHeaderName = StringCopy(argv[i]);
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
  if ( NULL == MainFunctionName ) {
    fprintf(stderr, "%sMissing function name%s\n", ColorBrightRed, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }
  if ( MainModuleName ) {
    MainFilename = StringMultiConcat(MainModuleName, DirSeparator, MainFunctionName, MainSuffix, NULL);
  } else {
    MainFilename = StringMultiConcat(MainFunctionName, MainSuffix, NULL);
  }
  if ( FileExists(MainFilename) && !MainOverwriteFunctionFile) {
    fprintf(stderr, "%s%s exists%s\n", ColorBrightRed, MainFilename, ColorReset);
    exit(EXIT_FAILURE);
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
}

/*****************************************************************************!
 * Function : MainOpenFile
 *****************************************************************************/
void
MainOpenFile
()
{
  MainFile = fopen(MainFilename, "wb");
  if ( NULL == MainFile ) {
    fprintf(stderr, "Could not open file %s : %s\n", MainFilename, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : MainWriteFile
 *****************************************************************************/
void
MainWriteFile
()
{
  Parameter*                            parameter;
  fprintf(MainFile,  FunctionHeaderTemplate, MainFunctionName);
  fprintf(MainFile, "%s\n", MainReturnType);
  fprintf(MainFile, "%s\n", MainFunctionName);
  fprintf(MainFile, "(");
  for ( parameter = MainParameters; parameter; parameter = parameter->next ) {
    fprintf(MainFile, "%s", parameter->type);
    if ( parameter->name ) {
      fprintf(MainFile, " %s", parameter->name);
    }
    if ( parameter->next ) {
      fprintf(MainFile, ", ");
    }
  }
  fprintf(MainFile, ")\n");
  fprintf(MainFile, "{\n");
  fprintf(MainFile, "}\n");
}

/*****************************************************************************!
 * Function : AppendToHeader
 *****************************************************************************/
void
AppendToHeader
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
      InsertFunctionInHeader(&lines, &linesCount, i);
      ResaveHeaderFile(lines, linesCount);
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
 * Function : InsertFunctionInHeader
 *****************************************************************************/
void
InsertFunctionInHeader
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
 * Function : ResaveHeaderFile
 *****************************************************************************/
void
ResaveHeaderFile
(char** InLines, int InLinesCount)
{
  int                                   n;
  string                                line;
  FILE*                                 file;
  int                                   i;

  file = fopen(MainHeaderName, "wb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open %s\n", MainHeaderName);
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
