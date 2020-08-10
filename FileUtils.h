/*****************************************************************************
 * FILE NAME    : FileUtils.h
 * DATE         : March 17 2020
 * PROJECT      : Bay Simulator
 * COPYRIGHT    : Copyright (C) 2020 by Vertiv Company
 *****************************************************************************/

#ifndef _fileutils_h_
#define _fileutils_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
string
FilenameExtractBase
(string InFilename);

bool
FileExists
(string InFilename);

void
GetFileLines
(char* InBuffer, int InBufferSize, char*** InLines, int* InLinesCount);

bool
GetFileBuffer
(char* InFilename, char** InBuffer, int* InBufferSize);

bool
FileCreateEmptyFile
(string InFilename);

#endif /* _fileutils_h_*/
