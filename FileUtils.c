/*****************************************************************************
 * FILE NAME    : FileUtils.c
 * DATE         : March 17 2020
 * PROJECT      : Bay Simulator
 * COPYRIGHT    : Copyright (C) 2020 by Vertiv Company
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "String.h"
#include "MemoryManager.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/

/*****************************************************************************!
 * Function : GetFileBuffer
 *****************************************************************************/
bool
GetFileBuffer
(char* InFilename, char** InBuffer, int* InBufferSize)
{
  int                                   bytesRead;
  FILE*                                 file;
  char*                                 buffer;
  int                                   filesize;
  struct stat                           statbuf;
  
  file = fopen(InFilename, "rb");
  if ( NULL == file ) {
    fprintf(stderr, "Could not open %s\n", InFilename);
    return false;
  }

  //! Get a buffer big enough to hold the contents
  stat(InFilename, &statbuf);
  filesize = statbuf.st_size;
  buffer = (char*)GetMemory(filesize + 1);
  if ( NULL == buffer ) {
    fclose(file);
    fprintf(stderr, "Could not get memory for file\n");
    return false;
  }

  //! Read the file contents
  bytesRead = fread(buffer, 1, filesize, file);
  fclose(file);
  if ( bytesRead != filesize ) {
    fprintf(stderr, "Error reading %s\n", InFilename);
    FreeMemory(buffer);
    fclose(file);
    return false;
  }
  buffer[filesize] = 0x00;
  *InBuffer = buffer;
  *InBufferSize = filesize;
  return true;
}

/*****************************************************************************!
 * Function : GetFileLines
 *****************************************************************************/
void
GetFileLines
(char* InBuffer, int InBufferSize, char*** InLines, int* InLinesCount)
{
  int                                   i;
  int                                   n;
  char*                                 record;
  char*                                 start;
  char*                                 end;
  bool                                  done = false;
  char**                                temp_lines;
  char**                                lines;
  int                                   lines_count;
  
  lines = NULL;
  lines_count = 0;
  
  start = InBuffer;
  while ( ! done ) {

    // Find the end of the line
    end = start;
    while (*end != '\n' && *end != 0x00 ) {
      end++;
    }
    n = end - start;

    // Copy the line
    record = (char*)GetMemory(n + 1);
    strncpy(record, start, n);
    record[n] = 0x00;

    // Append it to the line set
    if ( lines_count == 0 ) {
      // Start a new set
      lines = (char**)GetMemory(sizeof(char*));
      *lines = record;
      lines_count = 1;
    } else {

      // Append to the current set
      temp_lines = (char**)GetMemory(sizeof(char*) * (lines_count + 1));
      for (i = 0; i < lines_count; i++) {
        temp_lines[i] = lines[i];
      }
      FreeMemory(lines);
      temp_lines[i] = record;
      lines_count++;
      lines = temp_lines;
    }

    // Move to the start of the next line;
    if ( *end != 0x00 ) {
      end++;
    }
    start = end;

    // NUL means we are done
    if ( *start == 0x00 ) {
      done = true;
    }
  }
  *InLines = lines;
  *InLinesCount = lines_count;
}

/*****************************************************************************!
 * Function : FileExists
 *****************************************************************************/
bool
FileExists
(string InFilename)
{
  struct stat                           statbuf;
  if ( NULL == InFilename ) {
    return false;
  }
  
  return stat(InFilename, &statbuf) == 0;
}

/*****************************************************************************!
 * Function : FilenameExtractBase
 *****************************************************************************/
string
FilenameExtractBase
(string InFilename)
{
  string                                base;
  StringList*                           strings;
  if ( NULL == InFilename ) {
    return NULL;
  }

  strings = StringSplit(InFilename, ".", false);
  if ( NULL == strings ) {
    return NULL;
  }

  if ( strings->stringCount < 1 ) {
    StringListDestroy(strings);
    return NULL;
  }
  base = StringCopy(strings->strings[0]);
  StringListDestroy(strings);
  return base;
}


/*****************************************************************************!
 * Function : FileCreateEmptyFile
 *****************************************************************************/
bool
FileCreateEmptyFile
(string InFilename)
{
  FILE*					file;

  file = fopen(InFilename, "wb");
  if ( NULL == file ) {
    return false;
  }
  fclose(file);
  return true;
}
