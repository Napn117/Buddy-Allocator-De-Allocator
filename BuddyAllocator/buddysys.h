//////////////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Buddy System Algorithm
//
//   Description:  Buddy System Algorithm
//
//   Submitted by: Jesse Easton-Harris 12013205, Luke Roycroft 22013719
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __BUDDYSYS_H__
#define __BUDDYSYS_H__

#include "auxiliary.h"

extern long long int MEMORYSIZE;

typedef unsigned char byte; // shorter, replace cast to (char *) with cast to (byte *)

struct llist
{
  long long int size;     // size of the block (ONLY for data, this size does not consider the Node size! (so it is same as s[k])
  int alloc;              // 0 is free, 1 means allocated
  struct llist *next;     // next component
  struct llist *previous; // previous component
};

typedef struct llist Node;

// Declaration for buddysys.cpp to access the structured memory block created in main by - wholememory=(Node*) VirtualAlloc(NULL, MEMORYSIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);.
extern Node *wholememory;

void *buddyMalloc(int request_memory);
int buddyFree(void *p);
void createFreeList();
#endif