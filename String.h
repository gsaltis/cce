/*****************************************************************************
 * FILE NAME    : String.h
 * DATE         : January 15 2019
 * PROJECT      : Bay Simulator
 * COPYRIGHT    : Copyright (C) 2019 by Vertiv Company
 *****************************************************************************/
#ifndef _string_h_
#define _string_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*****************************************************************************!
 * Exported Type : string
 *****************************************************************************/
typedef char* string;

/*****************************************************************************!
 * Exported Type : StringList
 *****************************************************************************/
typedef struct _StringList
{
    string*                     strings;
    uint32_t                    stringCount;
} StringList;

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
string
StringCopy
(string InString);

string
StringNCopy
(string InString, uint32_t InStringLength);

bool
StringEqual
(string InString1, string InString2);

bool
StringEqualsOneOf
(string InString1, ...);

bool
StringBeginsWith
(string InString1, string InString2);

bool
StringEqualNoCase
(string InString1, string InString2);

bool
StringIsInteger
(string InString);

uint32_t
StringToU32
(string InString);

uint16_t
StringToU16
(string InString);

uint8_t
StringToU8
(string InString);

string
StringConcat
(string InString1, string InString2);

string
StringConcatTo
(string InString1, string InString2);

string
StringMultiConcat
(string InString1, ...);

string
StringTrim
(string InString);

void
StringCapitalize
(string InString);

string
StringReplaceChar
(string InString, char InChar, char InReplacementChar);

StringList*
StringSplit
(string InString, string InDividers, bool InSkipEmpty);

bool
StringContainsChar
(string InString, char InChar);

StringList*
StringListCreate
();

void
StringListAppend
(StringList* InStringList, string InString);

string
StringListGet
(StringList* InStringList, uint32_t InIndex);

uint32_t
StringListGetCount
(StringList* InStringList);

bool
StringListContains
(StringList* InStringList, string InString);

string
StringListConcat
(StringList* InStringList, string InSeparator);

void
StringListDestroy
(StringList* InStringList);

int
StringSizedToInteger
(string InString, int size); 

string
StringFill(char InChar, int InFillSize);

#endif /* _string_h_*/
