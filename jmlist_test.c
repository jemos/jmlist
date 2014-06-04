/*
	This file is part of jmlist.

	The MIT License (MIT)
	Copyright (c) 2014 Jean-François Mousinho

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "jmlist_test.h"
#include "jmlist.h"

void jmlist_test_print_status(char *func,jmlist_status status)
{
	char status_str[64];
	jmlist_status_to_string(status, status_str, sizeof(status_str));
	printf("%s() = %s\n",func,status_str);
}

jmlist_status
find_routine(void *ptr,void *param,jmlist_lookup_result *result)
{
	printf("find_routine PTR=%p result=%p param=%p\n",ptr,(void*)result,param);

	if( ptr == param )
		*result = jmlist_entry_found;

	return JMLIST_ERROR_SUCCESS;
}

int main(int argc,char *argv[])
{
	jmlist_status status;
	jmlist_lookup_result result;
	char *data[] = { "line 1", "line 2", "line 3" };
	char buffer[64];
	
	/* initialize jmlist engine */
	struct _jmlist_init_params init_params = { .fdump = stdout, .fdebug = stdout, .fverbose = stdout, .flags = JMLIST_FLAG_DEBUG};
	status = jmlist_initialize(&init_params);
	jmlist_test_print_status("jmlist_initialize",status);
	
	/*
	 * test 1: create indexed list without shifts, try to insert and remove some entries
	 * then dump the list to output.
	 */
	
	printf(	"\n  TEST #1 ------------------------------------------------------- \n"
			"    create indexed list without shifts, try to insert and remove some\n"
			"    entries then dump the list to output.\n\n");

	jmlist jml;
	struct _jmlist_params params = { .flags = JMLIST_INDEXED };
	status = jmlist_create(&jml,&params);

	status = jmlist_insert(jml,data[0]);
	status = jmlist_insert(jml,data[1]);
	status = jmlist_insert(jml,data[2]);
	status = jmlist_dump(jml);
	
	status = jmlist_remove_by_ptr(jml,data[1]);
	status = jmlist_dump(jml);
	
	bool fragmented;
	jmlist_is_fragmented(jml, false, &fragmented);
	
	status = jmlist_free(jml);
	
	/*
	 * test 2: create indexed list WITHOUT shifts, try to insert and remove in a list bigger
	 * than JMLIST_IDXLIST_DEF_MALLOC_INC entries.
	 */
	
	printf(	"\n  TEST #2 ------------------------------------------------------- \n"
			"    Create indexed list WITHOUT shifts, try to insert and remove \n"
			"    in a list bigger than JMLIST_IDXLIST_DEF_MALLOC_INC entries.\n\n");

	params.flags = JMLIST_INDEXED;
	params.idx_list.malloc_inc = JMLIST_IDXLIST_DEF_MALLOC_INC/2;
	
	jmlist_create(&jml,&params);
	
	for( int i = 0 ; i < JMLIST_IDXLIST_DEF_MALLOC_INC ; i++ )
	{
		sprintf(buffer,"line %u",i);
		jmlist_insert(jml,strdup(buffer));
	}
	jmlist_dump(jml);
	
	void *ptr;
	
	while( jmlist_pop(jml,&ptr) == JMLIST_ERROR_SUCCESS )
	{
		free(ptr);
	}
	jmlist_free(jml);
	
	/*
	 * test 3: create indexed list WITH shifts, try to insert and remove some entries..
	 */
	
	printf(	"\n  TEST #3 ------------------------------------------------------- \n"
			"    Create indexed list with shifts, try to insert and remove some\n"
			"    entries..\n\n");

	params.flags = JMLIST_INDEXED | JMLIST_IDX_USE_SHIFT;
	params.idx_list.malloc_inc = JMLIST_IDXLIST_DEF_MALLOC_INC/2;

	jmlist_create(&jml,&params);

	status = jmlist_insert(jml,data[0]);
	status = jmlist_insert(jml,data[1]);
	status = jmlist_insert(jml,data[2]);
	status = jmlist_dump(jml);

	jmlist_index count;
	jmlist_entry_count(jml,&count);

	status = jmlist_remove_by_ptr(jml,data[1]);
	jmlist_entry_count(jml,&count);
	
	status = jmlist_dump(jml);

	status = jmlist_insert(jml,data[1]);
	jmlist_dump(jml);

	// remove middle element
	jmlist_remove_by_index(jml,1);
	jmlist_dump(jml);

	// remove tail element
	jmlist_remove_by_index(jml,1);
	jmlist_dump(jml);

	// remove head element
	jmlist_remove_by_index(jml,0);
	jmlist_dump(jml);
	
	/* in this case its expected that the list is NOT fragmented due to the shifts,
	 tho, remove action is slower with worst case O(N) */
	jmlist_is_fragmented(jml, false, &fragmented);
	
	status = jmlist_free(jml);
	
	/*
	 * test 4: verify that jmlist_is_fragmented doesn't seek the list when JMLIST_IDX_USE_FRAG_FLAG
	 * is set. create indexed list with JMLIST_IDX_USE_FRAG_FLAG, inserts and removes
	 * should flag the list as fragmented whenever that happens. jmlist_is_fragmented
	 * should not need to seek throughout the list, being O(k) then.
	 */
	
	printf(	"\n  TEST #4 ------------------------------------------------------- \n"
			"    Verify that jmlist_is_fragmented doesn't seek the list when JMLIST_IDX_USE_FRAG_FLAG"
			"    is set. create indexed list with JMLIST_IDX_USE_FRAG_FLAG, inserts and removes"
			"    should flag the list as fragmented whenever that happens. jmlist_is_fragmented"
			"    should not need to seek throughout the list, being O(k) then.\n\n");

	params.flags = JMLIST_INDEXED | JMLIST_IDX_USE_FRAG_FLAG;
	params.idx_list.malloc_inc = JMLIST_IDXLIST_DEF_MALLOC_INC;
	
	jmlist_create(&jml,&params);
	
	status = jmlist_insert(jml,data[0]);
	status = jmlist_insert(jml,data[1]);
	status = jmlist_insert(jml,data[2]);
	status = jmlist_dump(jml);
	
	status = jmlist_remove_by_ptr(jml,data[1]);
	status = jmlist_dump(jml);
	
	/* in this case its expected that the list is fragmented due to the entry
	 removed in the middle of the list, jmlist_is_fragmented runs in O(k). */
	jmlist_is_fragmented(jml, false, &fragmented);
	
	jmlist_free(jml);
	
	/*
	 * test 5: test the linked list basic operations, create a linked list,
	 * insert and remove various entries.
	 */

	printf(	"\n  TEST #5 ------------------------------------------------------- \n"
			"    Test the linked list basic operations, create a linked list,\n"
			"    insert and remove various entries..\n\n");

	params.flags = JMLIST_LINKED;
	jmlist_create(&jml,&params);
	
	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);
	jmlist_dump(jml);

	// remove middle element using ptr.	
	jmlist_remove_by_ptr(jml,data[1]);
	jmlist_dump(jml);

	jmlist_push(jml,data[1]);
	jmlist_dump(jml);

	// remove middle element
	jmlist_remove_by_index(jml,1);
	jmlist_dump(jml);

	// remove tail element
	jmlist_remove_by_index(jml,1);
	jmlist_dump(jml);

	// remove head element
	jmlist_remove_by_index(jml,0);
	jmlist_dump(jml);

	jmlist_free(jml);
	
	jmlist_uninitialize();

	/*
	 * test 6: test the internal list, create various lists and check for memory leaks.
	 */
	printf(	"\n  TEST #6 ------------------------------------------------------- \n"
			"    test the internal list, create various lists and check for\n"
			"    memory leaks..\n\n");

	struct _jmlist_memory_info jml_mem;
	
	jmlist_memory(&jml_mem);	
	printf("idx_list.total=%u  idx_list.used=%u\nlnk_list.total=%u  lnk_list.used=%u\nJMLIST TOTAL: %u  USED:%u\n",
		   jml_mem.idx_list.total, jml_mem.idx_list.used, jml_mem.lnk_list.total, jml_mem.lnk_list.used,
		   jml_mem.total, jml_mem.used);
	
	init_params.flags |= JMLIST_FLAG_INTERNAL_LIST; 
	status = jmlist_initialize(&init_params);
	jmlist_test_print_status("jmlist_initialize",status);
	
	jmlist_memory(&jml_mem);	
	printf("idx_list.total=%u  idx_list.used=%u\nlnk_list.total=%u  lnk_list.used=%u\nJMLIST TOTAL: %u  USED:%u\n",
		   jml_mem.idx_list.total, jml_mem.idx_list.used, jml_mem.lnk_list.total, jml_mem.lnk_list.used,
		   jml_mem.total, jml_mem.used);
	
	params.flags = JMLIST_INDEXED;
	jmlist jml_list[5];
	for( int i = 0 ; i < 5 ; i++ )
	{
		sprintf(params.tag,"idx%u",i);
		jmlist_create(&jml_list[i],&params);
	}
	
	jmlist_memory(&jml_mem);	
	printf("idx_list.total=%u  idx_list.used=%u\nlnk_list.total=%u  lnk_list.used=%u\nJMLIST TOTAL: %u  USED:%u\n",
		   jml_mem.idx_list.total, jml_mem.idx_list.used, jml_mem.lnk_list.total, jml_mem.lnk_list.used,
		   jml_mem.total, jml_mem.used);
	
	/* this will cause a leak, one of the lists will not be freed */
	for( int i = 0 ; i < 4 ; i++ )
		jmlist_free(jml_list[i]);

	jmlist_memory(&jml_mem);	
	printf("idx_list.total=%u  idx_list.used=%u\nlnk_list.total=%u  lnk_list.used=%u\nJMLIST TOTAL: %u  USED:%u\n",
		   jml_mem.idx_list.total, jml_mem.idx_list.used, jml_mem.lnk_list.total, jml_mem.lnk_list.used,
		   jml_mem.total, jml_mem.used);
	
	jmlist_free_all();
	
	jmlist_memory(&jml_mem);	
	printf("idx_list.total=%u  idx_list.used=%u\nlnk_list.total=%u  lnk_list.used=%u\nJMLIST TOTAL: %u  USED:%u\n",
		   jml_mem.idx_list.total, jml_mem.idx_list.used, jml_mem.lnk_list.total, jml_mem.lnk_list.used,
		   jml_mem.total, jml_mem.used);

	/*
	 * TEST 7: Test the associative list type. 
	 */
	printf(	"\n  TEST #7 ------------------------------------------------------- \n"
			"    Test the associative list type. \n\n");

	memset(&params,0,sizeof(params));
	params.flags = JMLIST_ASSOCIATIVE;
	jmlist jmlass;
	jmlist_key key1 = "um";
	void *value1 = (void*)1;
	jmlist_key key2 = "dois";
	void *value2 = (void*)2;
	jmlist_key key3 = "tres";
	void *value3 = (void*)3;
	jmlist_key key4 = "quatro";
	void *value4 = (void*)4;
	void *value = 0;

	jmlist_create(&jmlass,&params);

	jmlist_insert_with_key(jmlass,key1,strlen(key1),(void*)value1);
	jmlist_insert_with_key(jmlass,key2,strlen(key2),(void*)value2);
	jmlist_insert_with_key(jmlass,key3,strlen(key3),(void*)value3);
	jmlist_insert_with_key(jmlass,key4,strlen(key4),(void*)value4);

	jmlist_dump(jmlass);

	jmlist_get_by_key(jmlass,key2,strlen(key2),(void*)&value);
	printf("key %s has value %p\n",(char*)key2,value);

	jmlist_remove_by_key(jmlass,key2,strlen(key2));
	jmlist_dump(jmlass);

	// remove middle element
	jmlist_remove_by_index(jmlass,1);
	jmlist_dump(jmlass);

	// remove tail element
	jmlist_remove_by_index(jmlass,1);
	jmlist_dump(jmlass);

	// remove head element
	jmlist_remove_by_index(jmlass,0);
	jmlist_dump(jmlass);

	jmlist_free(jmlass);

	/*
	 * TEST 8: Test replace operation on all list types.
	 */
	printf(	"\n  TEST #8 ------------------------------------------------------- \n"
			"    Test the replace operation on all list types.\n\n");
	
	params.flags = JMLIST_INDEXED;
	jmlist_create(&jml,&params);

	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);

	jmlist_dump(jml);

	jmlist_replace_by_index(jml,1,"REPLACED 1!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,0,"REPLACED 0!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,2,"REPLACED 2!");
	jmlist_dump(jml);

	jmlist_free(jml);

	params.flags = JMLIST_LINKED;
	jmlist_create(&jml,&params);

	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);

	jmlist_dump(jml);

	jmlist_replace_by_index(jml,1,"REPLACED 1!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,0,"REPLACED 0!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,2,"REPLACED 2!");
	jmlist_dump(jml);

	jmlist_free(jml);

	params.flags = JMLIST_ASSOCIATIVE;
	jmlist_create(&jml,&params);

	jmlist_insert_with_key(jml,"key1",4,data[0]);
	jmlist_insert_with_key(jml,"key2",4,data[1]);
	jmlist_insert_with_key(jml,"key3",4,data[2]);

	jmlist_dump(jml);

	jmlist_replace_by_index(jml,1,"REPLACED 1!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,0,"REPLACED 0!");
	jmlist_dump(jml);
	jmlist_replace_by_index(jml,2,"REPLACED 2!");
	jmlist_dump(jml);

	jmlist_free(jml);

	/*
	 * TEST 9: Test jmlist seeking functions.
	 */
	printf(	"\n  TEST #9 ------------------------------------------------------- \n"
			"    Test jmlist seeking functions on all list types.\n\n");

	jmlist_seek_handle shandle;
	
	params.flags = JMLIST_INDEXED;
	jmlist_create(&jml,&params);
	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);
	jmlist_dump(jml);
	jmlist_seek_start(jml,&shandle);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	jmlist_dump(jml);
	status = jmlist_seek_end(jml,&shandle);
	jmlist_test_print_status("jmlist_seek_end",status);
	jmlist_free(jml);

	params.flags = JMLIST_LINKED;
	jmlist_create(&jml,&params);
	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);
	jmlist_dump(jml);
	jmlist_seek_start(jml,&shandle);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	jmlist_dump(jml);
	status = jmlist_seek_end(jml,&shandle);
	jmlist_test_print_status("jmlist_seek_end",status);
	jmlist_free(jml);

	params.flags = JMLIST_INDEXED;
	jmlist_create(&jml,&params);
	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);
	jmlist_remove_by_index(jml,1);
	jmlist_dump(jml);
	jmlist_seek_start(jml,&shandle);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	status = jmlist_seek_next(jml,&shandle,&ptr);
	jmlist_test_print_status("jmlist_seek_next",status);
	jmlist_dump(jml);
	status = jmlist_seek_end(jml,&shandle);
	jmlist_test_print_status("jmlist_seek_end",status);
	jmlist_free(jml);


	/*
	 * TEST 10: Test jmlist find function. This function parses the list and for each
	 * entry it calls the callback function, in this case the callback function defined
	 * in this code only compares ptr with param, if equal returns entry found and
	 * the lookup should finish there.
	 */
	printf(	"\n  TEST #10 ------------------------------------------------------- \n"
			"    Test jmlist find function. This function parses the list and for each\n"
	 		"    entry it calls the callback function, in this case the callback function defined\n"
	 		"    in this code only compares ptr with param, if equal returns entry found and\n"
	 		"    the lookup should finish there.\n\n");

	params.flags = JMLIST_LINKED;
	jmlist_create(&jml,&params);
	jmlist_insert(jml,data[0]);
	jmlist_insert(jml,data[1]);
	jmlist_insert(jml,data[2]);
	jmlist_dump(jml);
	
	status = jmlist_find(jml,find_routine,data[2],&result,&ptr);
	if( (status == JMLIST_ERROR_SUCCESS) &&
			(result == jmlist_entry_found) &&
			(ptr == data[2]) ) 
		printf("  TEST #10.1 OK\n");
	else
		printf("  TEST #10.1 NOT OK\n");

	status = jmlist_find(jml,find_routine,"missing string",&result,&ptr);
	if( (status == JMLIST_ERROR_SUCCESS) &&
			(result == jmlist_entry_not_found) ) 
		printf("  TEST #10.2 OK\n");
	else
		printf("  TEST #10.2 NOT OK\n");

	jmlist_free(jml);

	printf(	"\n  TEST #11 ------------------------------------------------------- \n"
			"    Test jmlist associative function. This function parses the list and for each\n"
	 		"    entry it calls the callback function, in this case the callback function defined\n"
	 		"    in this code only compares ptr with param, if equal returns entry found and\n"
	 		"    the lookup should finish there.\n\n");

	/* END OF TESTS */
	status = jmlist_uninitialize();
	jmlist_test_print_status("jmlist_uninitialize",status);

	return EXIT_SUCCESS;
}

double jmlist_test_time(bool stop)
{
	static struct timeval ts,te;

	if( stop )
	{
		gettimeofday(&te,0);
		return (te.tv_usec - ts.tv_usec);
	}

	gettimeofday(&ts,0);

	return 0;
}

