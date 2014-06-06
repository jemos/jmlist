jmlist v0.1

Implementation of Dynamic Lists in C

INTRODUCTION
============

Tired of using/creating/modifying my code on different projects to
use dynamic lists I decided to create this code for future reuse.
This is something well implemented in most of higher level languages,
although I didn't found many code of C implementations.

The objective was to abstract most of the structure behind the
dynamic list while allowing to implement basic list types (indexed,
linked, associative) with basic routines available for all types.

This means you can index' access the linked or associative list,
push or pop from the indexed lists, etc.

Of course, each list type has specific access characteristics, for
some types of access is better one list instead of the other.

FEATURES
--------

 * Three basic lists supported: indexed, linked and key based.
 * Memory usage numbers

INSTALATION
===========

This is not a package, it's just source code, link it with your code
and thats it.

Build Options
-------------

If the preprocessor variable `JMLDEBUG` is defined, jmlist_debug will
be active and debugging output will be generated for all jmlist
functions. The `jmlist` object requires -std=c99 to be used when
compiling it. 

Included Programs
-----------------

The source code includes a simple benchmark tool (`jmlist_benchmark`) to have some speed
numbers with this functions. These are some sample results (CPU Core 2 Duo 2Ghz).

| List Type   | Insert Time (k entry/s) | Access Time (k entry/s) |
|:-----------:|:-----------------------:|:-----------------------:|
|  Indexed    |            17.55        |     49600               |
|   Linked    |            10780        |     7.95                |
| Associative |            6345         |     18.26               |

We would expect indexed to be faster in access time and slower to
insert, the contrary in the linked list type. Note that "indexed"
access is done on the linked list.

Besides the benchmark tool there's a test tool which was used to
test code implementation of some functionality (`jmlist_test`).


USAGE TUTORIAL
==============

The jmlist module has some internal flags that can be used before you start using it. For
the moment it has the following flags:
 * `JMLIST_FLAG_DEBUG` activates debugging messages (only works if code
 is compiled with `JMLDEBUG`).
 * `JMLIST_FLAG_INTERNAL_LIST` makes jmlist use an internal list of
 allocated `jmlist` objects, it can be used in the end to check if all
 of them were freed.

> Before starting with code, all `jmlist_*` functions return a status
> code, which you can look for success with `JMLIST_ERROR_SUCCESS`.
> If it returns `JMLIST_ERROR_FAILURE`, look for jmlist_errno variable
> contents, it's also a status code but more specific. The function
> `jmlist_status_to_string` converts this code into a string.

Let's now show a full example.

	#include <stdio.h>
	#include <string.h>

	#define JMLDEBUG 1
	#include "jmlist.h"

	int main(int argc,char *argv)
	{
		jmlist jl;
		jmlist_params p;

		// Create new associative list
		p.flags = JMLIST_ASSOCIATIVE;
		jmlist_create(&jl,&p);

		// Associative lists use jmlist_insert_with_key while others
		// use jmlist_insert function.
		char *key = "my_key";
		char *value = "this is my_key value";
		jmlist_insert_with_key(jl,key,strlen(key),value);

		// You could use jmlist_get_by_index here also.
		void *ptr;
		jmlist_get_by_key(jl,key,strlen(key),&ptr);
		printf("%s = %s\n",key,(char*)ptr);

		// Outside of bounds access test
		jmlist_enable_debug();
		jmlist_status s = jmlist_get_by_index(jl,100,&ptr);
		jmlist_disable_debug();

		char buffer[128];
		jmlist_status_to_string(s,buffer,sizeof(buffer));
		printf("Returned status = %s\n",buffer);

		jmlist_free(jl);
	}

Compile with `gcc -DJMLDEBUG x.c jmlist.c -o x` and expected output is:

	$ ./x
	my_key = this is my_key value
	jmlist_get_by_index: called with jml=0x100100080, index=100
	jmlist_get_by_index: passing control to the associative list get_by_index routine.
	ijmlist_ass_get_by_index: called with jml=0x100100080, index=100
	ijmlist_ass_get_by_index: index 100 is out of bounds with list jml=0x100100080 which has 1 entries
	ijmlist_ass_get_by_index: returning with failure
	Returned status = JMLIST_ERROR_FAILURE

This example shows basic usage of jmlist. 

Using Linked Lists
------------------

*TODO*


Using Associative Lists
-----------------------

Set the flag to JMLIST_ASSOCIATIVE and then you can use `jmlist_get_by_key`,
`jmlist_insert_with_key`, `jmlist_remove_by_key`. The key is considered binary
data so you can use both a string or anything else.

For example:

	...
	char *key = "my_key";
	void *data = "this is my_key value";
	s = jmlist_insert_with_key(jml,key,strlen(key),data);
	s = jmlist_get_by_key(jml,key,strlen(key),&data);
	...

The new data pointer should be equal to the old one. If you insert two entries
with the same key, jmlist will ignore that fact, you'll have duplicate key in
the list. If you try to get by key a duplicate key it will return the first it
finds.

Seeking the Lists
-----------------

For seeking the lists there are several functions: `jmlist_parse`, `jmlist_find`,
and `jmlist_seek_*` functions. First example is with `jmlist_parse`. Basically,
you pass a processing function which will be called for each entry in the list.

	...
	char *prefix = "This will be passed to parser function."
	s = jmlist_parse(jml,parser,prefix);
	...
	void parser(void *entry, void *param) {
		static int i=0;
		printf("parser entry %d\n",i++);
		// do something with *entry..
	}
	...

Second example is similar, but the processing function can stop the seeking loop.

	...
	s = jmlist_insert(jml,"AAA");
	s = jmlist_insert(jml,"BBB");

	void *ptr;
	jmlist_lookup_result res;
	s = jmlist_find(jml,parser,"BBB",&res,&ptr);
	if( res == jmlist_entry_found ) {
		printf("FOUND ! %s\n",ptr);
	}
	...
	jmlist_status parser(void *entry, void *param,jmlist_lookup_result *result) {
		if( !strcmp(param,entry) ) {
			*result = jmlist_entry_found;
			return JMLIST_ERROR_SUCCESS;
		}
		return JMLIST_ERROR_SUCCESS;
	}
	...


This ends the examples for this version.

--------------------------------------------------
Copyright (C) 2014 Jean-François Mousinho

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

