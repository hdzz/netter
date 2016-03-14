/*
 * util.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "util.h"

uint64_t get_tick_count()
{
	struct timeval tval;
	uint64_t ret_tick;
    
	gettimeofday(&tval, NULL);
    
	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
}

