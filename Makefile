#
# jmlist v1.0
# Copyright (C) 2009 Jean Mousinho
#
# This file is part of jmlist.
#
# jmlist is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# jmlist is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with jmlist.  If not, see <http://www.gnu.org/licenses/>.
#

CC		:= gcc
CFLAGS	:= -O -Wall -pedantic -std=c99 -Wuninitialized -Wsign-compare -Wno-pointer-sign -Wformat-security
LFLAGS	:=
LIBS	:= -lm -lz `mysql_config --cflags --libs`
OBJS	:= wstrainer.o jmlist.o jmlist_test.o
.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -c $<

all: wstrainer

wstrainer: $(OBJS)
	$(CC) $(LFLAGS) -o wstrainer $(OBJS) $(LIBS)

%.o: %.c
	$(CC) -g $(CFLAGS) -o $@ $<

#status.o: ../shared/status.cpp ../shared/status.h
#	$(CC) $(CFLAGS) -o status.o ../shared/status.cpp

jmlist: jmlist.c jmlist.h jmlist_test.c
	$(CC) -g -DALLOW_NULL_PTR -DWITH_MAIN_ROUTINE -DWITH_ASSOC_LIST $(CFLAGS) -o jmlist jmlist.c jmlist.h jmlist_test.c

benchmark: jmlist_benchmark

jmlist_benchmark: jmlist.c jmlist.h jmlist_benchmark.c
	$(CC) -DALLOW_NULL_PTR -DWITH_ASSOC_LIST $(CFLAGS) -o jmlist_benchmark jmlist.c jmlist_benchmark.c

clean:
	rm *.o

