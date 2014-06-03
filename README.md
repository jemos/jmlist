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


INSTALATION
===========

This is not a package, it's just source code, link it with your code
and thats it.


USAGE TUTORIAL
==============

Lets start with a simple example of indexed lists. jmlist must be
initialized before it can be used, that will allow one to specify
the debugging output file descriptor, internal list usage or not,
besides others.

Before starting with code, all `jmlist_*` functions return a status
code, which you can look for success with `JMLIST_ERROR_SUCCESS`.
If it returns `JMLIST_ERROR_FAILURE`, look for jmlist_errno variable
contents, its also a status code but more specific.

	jmlist_status status;
	struct _jmlist_init_params init_params = {
		.fdump = stdout, .fdebug = stdout,
		.fverbose = stdout, .flags = JMLIST_FLAG_DEBUG
	};

	status = jmlist_initialize(&init_params);

	/*
	   If everything went OK status should be JMLIST_ERROR_SUCCESS.
	   Now lets create the list:
	*/

	struct _jmlist_params params = { .flags = JMLIST_INDEXED };
	status = jmlist_create(&jml,&params);

	/* We can now add entries to the list.. */
	status = jmlist_insert(jml,"ABC");
	status = jmlist_insert(jml,"DEF");
	status = jmlist_insert(jml,"GHI\n");

	/* Now lets get the entries and print them.. */
	jmlist_index max_i;
	jmlist_get_count(jml,&i);

	for( int jmlist_index i = 0 ; i < max_i < i++ )
	{
		char *ptr;
		jmlist_get_by_index(jml,i,ptr);
		printf("%s",ptr);
	}

	/* now clear the list */
	status = jmlist_free(jml);

This example shows basic usage of jmlist.

--------------------------------------------------
Copyright (C) 2014 Jean-François Mousinho

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

