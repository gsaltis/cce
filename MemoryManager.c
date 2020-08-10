/*******************************************************************************
 * FILE         : MemoryManager.c
 * DATE         : May 14 2018
 * PROJECT      : CAN Device Simulator
 *******************************************************************************/

/*******************************************************************************
 * Global Headers
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*******************************************************************************
 * Local Headers
 *******************************************************************************/
#include "MemoryManager.h"

/*******************************************************************************
 * Local Data
 *******************************************************************************/
static MMMemoryStats MMStats = { 0 };

/*******************************************************************************
 * Function : GetMemory
 *******************************************************************************/
void*
GetMemory
(
 size_t                                 InSize
)
{
  int                                   n;
  void*                                 rPtr;
  int*                                  p;
  if ( InSize == 0 ) {
    return NULL;
  }
  n = sizeof(int) + InSize;
  rPtr = (void*)malloc(n);
  if ( NULL == rPtr ) {
    printf("Malloc Error - exiting\n");
    exit(EXIT_FAILURE);
  }

  p = (int*)rPtr;
  *p = InSize;
  p++;
  MMStats.BytesAllocated += InSize;
  rPtr = (void*)p;
  return rPtr;
}

/*******************************************************************************
 * Function : FreeMemory
 *******************************************************************************/
void
FreeMemory
(
 void*                                  InPtr
)
{
  int*                                  p;
  if ( NULL == InPtr ) {
    return;
  }
  p = (int*)InPtr;
  p--;
  MMStats.BytesFreed += *p;
  free(p);
}

/*******************************************************************************
 * Function : GetMemoryStats
 *******************************************************************************/
void
GetMemoryStats
(
 MMMemoryStats*                         InStats
)
{
  if ( NULL == InStats ) {
    return;
  }

  InStats->BytesAllocated = MMStats.BytesAllocated;
  InStats->BytesFreed = MMStats.BytesFreed;
}


