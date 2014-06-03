#
# jmlist v1.0
#
# The MIT License (MIT)
# Copyright (c) 2014 Jean-François Mousinho
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

CC		:= gcc
CFLAGS	:= -Wall -pedantic -std=c99 -O
LFLAGS	:=
LIBS	:= 
OBJS	:= jmlist_test jmlist_benchmark

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: jmlist_test jmlist_benchmark

jmlist_test: jmlist_test.c jmlist.c jmlist.h
	$(CC) $(CFLAGS) -g -DJMLDEBUG -o jmlist_test jmlist.c jmlist.h jmlist_test.c

jmlist_benchmark: jmlist_benchmark.c jmlist.c jmlist.h
	$(CC) $(CFLAGS) -o jmlist_benchmark jmlist.c jmlist_benchmark.c

clean:
	rm -f *.o jmlist_benchmark jmlist_test

