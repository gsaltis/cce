/*****************************************************************************
 * FILE NAME    : String.c
 * DATE         : January 15 2019
 * PROJECT      : Bay Simulator
 * COPYRIGHT    : Copyright (C) 2019 by Vertiv Company
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "String.h"
#include "MemoryManager.h"
#include "ascii.h"

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
 * Function : StringCopy
 *****************************************************************************/
string
StringCopy
(
 string                         InString
)
{
    uint32_t                    n;
    string                      returnString;
    if ( NULL == InString ) {
        return NULL;
    }

    n = strlen(InString);
    returnString = GetMemory(n + 1);
    strncpy(returnString, InString, n);
    returnString[n] = NUL;

    return returnString;
}

/*****************************************************************************!
 * Function : StringCopy
 *****************************************************************************/
string
StringNCopy
(
 string                         InString,
 uint32_t                       InLength
)
{
  uint32_t                    n;
  string                      returnString;
  if ( NULL == InString ) {
    return NULL;
  }

  n = strlen(InString);
  if ( n > InLength ) {
    n = InLength;  
  }
  returnString = GetMemory(n + 1);
  strncpy(returnString, InString, n);
  returnString[n] = NUL;

  return returnString;
}
/*****************************************************************************!
 * Function : StringBeginsWith
 * Purpose  : Check if String1 begins with String2
 *****************************************************************************/
bool
StringBeginsWith
(
 string                         InString1,
 string                         InString2
)
{
    uint32_t                    t1, t2;
    if ( NULL == InString1 || NULL == InString2 ) {
        return false;
    }
    t1 = strlen(InString1);
    t2 = strlen(InString2);
    
    if ( t1 == 0 || t2 == 0 ) {
        return false;
    }
    
    if ( t2 > t1 ) {
        return false;
    }
    return strncmp(InString1, InString2, t2) == 0;
}

/*****************************************************************************!
 * Function : StringEqual
 *****************************************************************************/
bool
StringEqual
(
 string                         InString1,
 string                         InString2
)
{
    uint32_t                            t1;
    if ( NULL == InString1 || NULL == InString2 ) {
        return false;
    }
    t1 = strlen(InString1);
    if ( t1 != strlen(InString2) ) {
        return false;
    }
    return strcmp(InString1, InString2) == 0;
}

/*****************************************************************************!
 * Function : StringEqualNoCase
 *****************************************************************************/
bool
StringEqualNoCase
(
 string                         InString1,
 string                         InString2
)
{
    uint32_t                            t1, i;
    
    if ( NULL == InString1 || NULL == InString2 ) {
        return false;
    }
    t1 = strlen(InString1);
    if ( t1 != strlen(InString2) ) {
        return false;
    }
    for (i = 0; i < t1; i++) {
        if ( tolower(InString1[i]) != tolower(InString2[i]) ) {
            return false;
        }
    }
    return true;
}

/*****************************************************************************!
 * Function : StringTrim
 *****************************************************************************/
string
StringTrim
(
 string                         InString
)
{
    string                      start;
    string                      end;
    string                      s;
    uint32_t                    n;
    
    if ( NULL == InString ) {
        return NULL;
    }

    // Trim off the leading spaces
    start = InString;
    while ( isspace(*start) && *start ) {
        start++;
    }

    // It looks like the string is all spaces
    if ( *start == NUL ) {
        return NULL;
    }

    // Trim off the trailing spaces
    n = strlen(InString);
    end = InString + (n - 1);
    while ( isspace(*end) && end != start ) {
        end--;
    }

    // Copy the string portion
    n = (end+1) - start;
    s = (string)GetMemory(n + 1);
    memcpy(s, start, n);
    s[n] = NUL;
    return s;
}

/*****************************************************************************!
 * Function : StringCapitalize
 *****************************************************************************/
void
StringCapitalize
(
 string                         InString
)
{
  if ( InString == NULL || InString[0] == NUL ) {
    return;
  }

  if ( !isalpha(InString[0]) ) {
    return;
  }

  InString[0] = toupper(InString[0]);
}

/*****************************************************************************!
 * Function : StringIsInteger
 *****************************************************************************/
bool
StringIsInteger
(
 string                         InString
)
{
    string                      s;
    if ( NULL == InString ) {
        return false;
    }
    for (s = InString; *s; s++) {
        if ( !isdigit(*s) ) {
            return false;
        }
    }
    return true;
}

/*****************************************************************************!
 * Function : StringToU32
 *****************************************************************************/
uint32_t
StringToU32
(
 string                         InString
)
{
    if ( NULL == InString ) {
        return 0;
    }
    return (uint32_t)atoi(InString);
}

/*****************************************************************************!
 * Function : StringToU16
 *****************************************************************************/
uint16_t
StringToU16
(
 string                         InString
)
{
    if ( NULL == InString ) {
        return 0;
    }
    return (uint16_t)atoi(InString);

}

/*****************************************************************************!
 * Function : StringToU8
 *****************************************************************************/
uint8_t
StringToU8
(
 string                         InString
)
{
    if ( NULL == InString ) {
        return 0;
    }
    return (uint8_t)atoi(InString);
}

/*****************************************************************************!
 * Function : StringConcat
 *****************************************************************************/
string
StringConcat
(
 string                         InString1,
 string                         InString2
)
{
    uint32_t                            n;
    string                              returnString;
    if ( NULL == InString1 || NULL == InString2 ) {
        return NULL;
    }
    n = strlen(InString1) + strlen(InString2);
    returnString = (string)GetMemory(n + 1);
    sprintf(returnString, "%s%s", InString1, InString2);
    return returnString;
}

/*****************************************************************************!
 * Function : StringConcatTo
 * Purpose  : Same functionality as StringConcat except InString1 is assumed
 *            to a pointer to a string and is freed.
 *****************************************************************************/
string
StringConcatTo
(
 string                         InString1,
 string                         InString2
)
{
    uint32_t                            n;
    string                              returnString;
    if ( NULL == InString1 || NULL == InString2 ) {
        return NULL;
    }
    n = strlen(InString1) + strlen(InString2);
    returnString = (string)GetMemory(n + 1);
    sprintf(returnString, "%s%s", InString1, InString2);
    FreeMemory(InString1);
    return returnString;
}

/*****************************************************************************!
 * Function : StringContainsChar 
 *****************************************************************************/
bool
StringContainsChar
(
 string                         InString,
 char                           InChar
)
{
    if ( NULL == InString ) {
        return false;
    }
    
    while (*InString) {
        if ( InChar == *InString ) {
            return true;
        }
        InString++;
    }
    return false;
}

/*****************************************************************************!
 * Function : StringReplaceChar
 *****************************************************************************/
string
StringReplaceChar
(
 string                         InString,
 char                           InChar,
 char                           InReplacementChar
)
{
    string                      s;

    if ( NULL == InString ) {
        return NULL;
    }
    
    for (s = InString; *s; s++) {
        if ( *s == InChar ) {
            *s = InReplacementChar;
        }
    }
    return InString;
}

/*****************************************************************************!
 * Function : StringSplit
 *****************************************************************************/
StringList*
StringSplit
(
 string                         InString,
 string                         InDividers,
 bool                           InSkipEmpty
)
{
    string                              s;
    uint32_t                            n;
    string                              start;
    string                              end;
    StringList*                         list;
    
    if ( NULL == InString || NULL == InDividers ) {
        return NULL;
    }
    (void)InSkipEmpty;
    start = InString;

    list = StringListCreate();
    while (true) {
        // Skip dividers
        while (StringContainsChar(InDividers, *start) && *start) {
            start++;
        }

        // We hit the end of the string
        if (*start == NUL ) {
            break;
        }
        end = start;

        // Skip to the end or next dividers
        while (!StringContainsChar(InDividers, *end) && *end) {
            end++;
        }

        // Copy the string portion
        n = end - start;
        s = GetMemory(n + 1);
        memcpy(s, start, n);
        s[n] = NUL;

        // Append it to the list we are building
        StringListAppend(list, s);
        if ( *end != NUL ) {
            end++;
        }
        start = end;
    }

    return list;
}

/*****************************************************************************!
 * Function : StringListCreate
 *****************************************************************************/
StringList*
StringListCreate
()
{
    StringList*                 returnList;

    returnList = (StringList*)GetMemory(sizeof(StringList));
    memset(returnList, 0x00, sizeof(StringList));
    return returnList;
}

/*****************************************************************************!
 * Function : StringListAppend
 *****************************************************************************/
void
StringListAppend
(
 StringList*                            InStringList,
 string                                 InString
)
{
    uint32_t                            i;
    string*                             newList;
    uint32_t                            newCount;
    if ( NULL == InStringList || NULL == InString ) {
        return;
    }
    newCount = InStringList->stringCount + 1;
    newList = (string*)GetMemory((sizeof(string)) * newCount);
    for ( i = 0; i < InStringList->stringCount; i++ ) {
        newList[i] = InStringList->strings[i];
    }
    if ( InStringList->stringCount > 0 ) {
        FreeMemory(InStringList->strings);
    }
    newList[InStringList->stringCount] = InString;
    InStringList->strings = newList;
    InStringList->stringCount = newCount;
}

/*****************************************************************************!
 * Function : StringListGetCount
 *****************************************************************************/
uint32_t
StringListGetCount
(
 StringList*                            InStringList
)
{
    if ( NULL == InStringList ) {
        return 0;
    }
    return InStringList->stringCount;
}

/*****************************************************************************!
 * Function : StringListContains
 *****************************************************************************/
bool
StringListContains
(
 StringList*                            InStringList,
 string                                 InString
)
{
    uint32_t                            i;
    if ( NULL == InStringList || NULL == InString ) {
        return false;
    }
    for ( i = 0 ; i < InStringList->stringCount; i++) {
        if ( StringEqual(InStringList->strings[i], InString) ) {
            return true;
        }
    }
    return false;
}

/*****************************************************************************!
 * Function : StringListDestroy
 *****************************************************************************/
void
StringListDestroy
(
 StringList*                            InStringList
)
{
    uint32_t                            i;
    if ( NULL == InStringList ) {
        return;
    }

    for ( i = 0; i < InStringList->stringCount; i++ ) {
        FreeMemory(InStringList->strings[i]);
    }
    FreeMemory(InStringList);
}

/*****************************************************************************!
 * Function : StringListGet
 *****************************************************************************/
string
StringListGet
(
 StringList*                            InStringList,
 uint32_t                               InIndex
)
{
    if ( NULL == InStringList ) {
        return NULL;
    }
    
    if ( InIndex >= InStringList->stringCount ) {
        return NULL;
    }

    return InStringList->strings[InIndex];
}

/*****************************************************************************!
 * Function : StringSizedToInteger
 *****************************************************************************/
int
StringSizedToInteger
(string InString, int size)  // In Bits
{
    int                                 base;
    string                              s;
    char                                prefix[3];
    int                                 n;

    s = InString;
    base = 10;
    if ( strlen(s) > 2 ) {
        strncpy(prefix, s, 2);
        s += 2;
        prefix[2] = NUL;
        if ( StringEqualNoCase(prefix, "0x") ) {
            base = 16;
        } else if ( StringEqualNoCase(prefix, "0b") ) {
            base = 2;
        }
    }
    n = strlen(s);
    if ( base == 2 && n > size ) {
        return 0;
    } else if ( base == 16 && n > (size/4) ) {
        return 0;
    }
    return strtol(s, NULL, base);
}
 
/*****************************************************************************!
 * Function : StringEqualsOneOf
 *****************************************************************************/
bool
StringEqualsOneOf
(string InString1, ...)
{
  string                                s;
  va_list                               va;

  va_start(va, InString1);
  s = va_arg(va, string);
  while (s) {
    if ( StringEqual(InString1, s) ) {
      va_end(va);
      return true;
    }
    s = va_arg(va, string);
  }
  return false;
}

/*****************************************************************************!
 * Function : StringFill
 *****************************************************************************/
string
StringFill
(char InChar, int InSize)
{
  string                                returnString;

  if (InChar == NUL || InSize == 0) {
    return NULL;
  }
  returnString = (string)GetMemory(InSize + 1);
  memset(returnString, InChar, InSize);
  returnString[InSize] = NUL;
  return returnString;
}

/*****************************************************************************!
 * Function : StringListConcat
 *****************************************************************************/
string
StringListConcat
(StringList* InList, string InSeparators)
{
  string                                returnString;
  int                                   i;

  if ( InList == NULL ) {
    return NULL;
  }
  returnString = "";

  for (i = 0; i < InList->stringCount; i++) {
    if ( returnString ) {
      if ( InSeparators ) {
        returnString = StringConcatTo(returnString, InSeparators);
      }
      returnString = StringConcatTo(returnString, InList->strings[i]);
    } else {
      returnString = StringCopy(InList->strings[i]);
    }
  }
  return returnString;
}


/*****************************************************************************!
 * Function : StringMultiConcat
 *****************************************************************************/
string
StringMultiConcat
(string InString1, ...)
{
  va_list                               args;
  string                                returnString, s;

  if ( NULL == InString1 ) {
    return NULL;
  }

  returnString = StringCopy(InString1);
  va_start(args, InString1);
  s = va_arg(args, string);
  while (s) {
    returnString = StringConcatTo(returnString, s);
    s = va_arg(args, string);
  }
  return returnString;
} 

