ADDITIONAL_CFLAGS=-D_BSD_SOURCE -D_POSIX_SOURCE

CTAGS=ctags
YACC=yacc
LEX=lex
CC=cc
CXX=c++
LD=c++

ifeq ($(OS),Windows_NT)
    CC=gcc
    CXX=g++
    LD=g++
#    CC=x86_64-w64-mingw32-gcc
#    CXX=x86_64-w64-mingw32-g++
#    LD=x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CC=gcc
        CXX=g++
        LD=g++
    endif
    ifeq ($(UNAME_S),Darwin)

    endif
endif

include Makefile
