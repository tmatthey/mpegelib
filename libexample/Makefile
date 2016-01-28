# File: Makefile		-*- Makefile -*- 
# Created by: Alex Knowles (alex@ed.ac.uk) Sat Dec  2 22:01:12 1995
# Last Modified: Time-stamp: <15 Aug 96 1618 Alex Knowles> 
# RCS $Id: Makefile,v 1.1 1995/12/05 14:41:17 ark Exp $ 

# make this point to where we can find mpege.h & im.h
INCLUDE=-I$$HOME/include
# I compiled this on a sun so I needed these (you may not)
# EXTRA_LIBS=-lsocket -lnsl
# make this point to where we can find libmpege.a 
LIBS=-L$$HOME/lib -lmpege $(EXTRA_LIBS)  -lm
CFLAGS= -ansi -Wall -pedantic $(INCLUDE)
CC=gcc

all: simple_encode

simple_encode: simple_encode.o
	$(CC) $(LDFLAGS) -o $@ simple_encode.o $(LIBS)

again: clean all

clean:
	$(RM) *.o *~ core simple_encode
