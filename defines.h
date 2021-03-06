/*
 * defines.h
 *
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "limits.h"

#define MAX_STRING_LENGTH 256	/*maximum string length*/
#define INTEGER_SIZE 4			/*integer size*/

#define PERSON_PROPERTIES_NUM 4
#define PERSON_REL_PROPERTIES_NUM 2

#define SUCCESS 0
#define INFINITY_REACH_NODE INT_MAX 
#define SILENCE(x) (void)(x)

#define M 2
#define C 3

#endif /* DEFINES_H_ */
