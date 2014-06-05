/*
	This file is part of jmlist.

	jmlist is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	jmlist is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with jmlist.  If not, see <http://www.gnu.org/licenses/>.

	Copyright (C) 2009 Jean Mousinho
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "jmlist.h"

void jmlist_benchmark_print_status(char *func,jmlist_status status);
int jmlist_benchmark(int argc,char *argv[]);

#define INDEXED_SIZE 10000
#define INDEXED_SIZE_FLOAT (double)INDEXED_SIZE

int main(int argc,char *argv[])
{
	return jmlist_benchmark(argc,argv);
}


void jmlist_benchmark_print_status(char *func,jmlist_status status)
{
	char status_str[64];
	jmlist_status_to_string(status, status_str, sizeof(status_str));
	printf("%s() = %s\n",func,status_str);
}

int jmlist_benchmark(int argc,char *argv[])
{
	jmlist_status status;
	struct timeval tv_s,tv_e;
	double idx_insert_time = 0.0;
	double lnk_insert_time = 0.0;
	double ass_insert_time = 0.0;
	double idx_access_time = 0.0;
	double lnk_access_time = 0.0;
	double ass_access_time = 0.0;
	unsigned int i;
	jmlist_index *idx_list;
	jmlist_status s;
	
	/* initialize jmlist engine */
	status = jmlist_set_internal_flags(0);
	jmlist_benchmark_print_status("jmlist_initialize",status);

	printf("jmlist initialized, benchmarking indexed list...\n");

	jmlist jml;
	struct _jmlist_params params;
	
	printf("creating random index numbers for access benchmark...\n");
	idx_list = (jmlist_index*)malloc(INDEXED_SIZE*sizeof(jmlist_index));
	srand(time(0));
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
		idx_list[i] = (unsigned int)(((double)rand()/RAND_MAX)*INDEXED_SIZE_FLOAT);
	printf("%u random index numbers created successfuly.\n",INDEXED_SIZE);

	// JMLIST INDEXED

	memset(&params,0,sizeof(params));
	params.flags = JMLIST_INDEXED | JMLIST_IDX_USE_SHIFT;
	jmlist_create(&jml,&params);

	printf("\n ------------------------------------------- \n");
	printf(	" Benchmarking jmlist indexed.\n"
			"    ... inserting %u items in the list...\n",INDEXED_SIZE);

	gettimeofday(&tv_s,0);
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
	{
		jmlist_insert(jml,(void*)i);
	}
	gettimeofday(&tv_e,0);

	printf("    ... insert finished.\n");
	idx_insert_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);
	
	/* apply access time test */
	printf("    ... accessing to %u random items in the list...\n",INDEXED_SIZE);
	gettimeofday(&tv_s,0);
	void *ptr;
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
	{
		jmlist_get_by_index(jml,idx_list[i],&ptr);
	}
	gettimeofday(&tv_e,0);
	printf("    ... access finished.\n");
	idx_access_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);

	
	jmlist_free(jml);

	// JMLIST LINKED

	params.flags = JMLIST_LINKED;
	jmlist_create(&jml,&params);

	printf("\n ------------------------------------------- \n");
	printf(	" Benchmarking jmlist linked.\n"
			"    ... inserting %u items in the list...\n",INDEXED_SIZE);

	gettimeofday(&tv_s,0);
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
	{
		jmlist_insert(jml,(void*)i);
	}
	gettimeofday(&tv_e,0);

	printf("    ... insert finished.\n");
	lnk_insert_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);

	/* apply access time test */
	printf("    ... accessing to %u random items in the list...\n",INDEXED_SIZE);
	gettimeofday(&tv_s,0);
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
	{
		jmlist_get_by_index(jml,idx_list[i],&ptr);
	}
	gettimeofday(&tv_e,0);
	printf("    ... access finished.\n");
	lnk_access_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);
	
	jmlist_free(jml);

	// JMLIST ASSOC

	params.flags = JMLIST_ASSOCIATIVE;
	s = jmlist_create(&jml,&params);
	assert(s == JMLIST_ERROR_SUCCESS);

	printf("\n ------------------------------------------- \n");
	printf(	" Benchmarking jmlist associative.\n"
			"    ... creating %u keys...\n",INDEXED_SIZE);

	char buffer[64];
	void **key_list = (void**)malloc(sizeof(void*)*INDEXED_SIZE);
	for( i = 0 ; i < INDEXED_SIZE ; i++ ) {
		sprintf(buffer,"key %d",i);
		key_list[i] = strdup(buffer);
	}
	printf("    ... created %u keys.\n",INDEXED_SIZE);

	printf("    ... inserting %u items in the list...\n",INDEXED_SIZE);
	gettimeofday(&tv_s,0);
	for( i = 0 ; i < INDEXED_SIZE ; i++ ) {
		s = jmlist_insert_with_key(jml,key_list[i],strlen(key_list[i]),key_list[i]);
		assert(s == JMLIST_ERROR_SUCCESS);
	}
	gettimeofday(&tv_e,0);

	printf("    ... insert finished.\n");
	ass_insert_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);

	/* apply access time test */
	printf("    ... accessing to %u random items in the list...\n",INDEXED_SIZE);
	gettimeofday(&tv_s,0);
	for( i = 0 ; i < INDEXED_SIZE ; i++ )
	{
		s = jmlist_get_by_key(jml,key_list[i],strlen(key_list[i]),&ptr);
		assert(s == JMLIST_ERROR_SUCCESS);
		assert(ptr == (void*) key_list[i]);
	}
	gettimeofday(&tv_e,0);
	printf("    ... access finished.\n");
	ass_access_time = (double)((tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec)*1e-6);
	
	jmlist_free(jml);

	printf("\n list type     | insert time (k/s) | access time (k/s)\n");
	printf(" indexed       | %17.3e | %15.3e \n",
			INDEXED_SIZE_FLOAT/idx_insert_time*1e-3,
			INDEXED_SIZE_FLOAT/idx_access_time*1e-3);
	printf(" linked        | %17.3e | %15.3e \n",
			INDEXED_SIZE_FLOAT/lnk_insert_time*1e-3,
			INDEXED_SIZE_FLOAT/lnk_access_time*1e-3);
	printf(" associative   | %17.3e | %15.3e \n",
			INDEXED_SIZE_FLOAT/ass_insert_time*1e-3,
			INDEXED_SIZE_FLOAT/ass_access_time*1e-3);

	jmlist_cleanup();

	return EXIT_SUCCESS;
}

