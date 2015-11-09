#ifndef __Software__Query3__
#define __Software__Query3__

#include "defines.h"
#include "GraphLib.h"

struct Interests
{
	int tag,noe;
	Queue * q;
};
typedef struct Interests Interests;

struct InterList
{
	Interests * iNode;
	struct InterList * next;
};
typedef struct InterList InterList;

struct InterObj
{
	int mmax,wmax,next_num;
	InterList * team;
};
typedef struct InterObj InterObj;

void findTrends(int k,Graph* g, char** womenTrends, char** menTrends);

#endif /* defined(__Software__Query3__) */
