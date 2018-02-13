#ifndef __TREND__
#define __TREND__

#include "defines.h"
#include "graphLib.h"

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

#endif /* defined(__TREND__) */
