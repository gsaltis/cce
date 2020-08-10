/*******************************************************************************
 * FILE		: MemoryManager.h
 * DATE		: May 14 2018
 * PROJECT	: CAN Device Simulator
 *******************************************************************************/
#ifndef _memorymanager_h_
#define _memorymanager_h_

/*******************************************************************************
 * Exported Type
 *******************************************************************************/
typedef struct _MMMemoryStats {
  size_t				BytesAllocated;
  size_t				BytesFreed;
} MMMemoryStats;

/*******************************************************************************
 * Exported Functions
 *******************************************************************************/
void*
GetMemory(size_t InSize);

void
FreeMemory(void* InPtr);

void
GetMemoryStats
(MMMemoryStats* InStats);

#endif // _memorymanager_h_

