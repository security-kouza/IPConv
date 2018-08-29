OPT=-O3
#OPT=-O0 -g

# =====================================================================

CPPFLAGS=-I/usr/local/include
CFLAGS=${CPPFLAGS} ${OPT} -std=c99 ${ADDITIONAL_CFLAGS}
CXXFLAGS=${CPPFLAGS} ${OPT} -std=c++11 -Wno-write-strings
LDFLAGS=
MAKEFILES=BSDmakefile GNUmakefile Makefile

# =====================================================================

.PHONY: all debug samples clean zip d

all: gsconv
# all: semantic
# all: operation
# all: syntactic
# all: lexical 

clean:
	rm -f gsconv semantic syntactic lexical *.o ast.* *.output syntactic.h y.tab.* syntactic.c lexical.c syntactic_debug.c lexical_debug.c tags lex.yy.c dump preamble.cpp type_define.txt *.d
tags:
	${CTAGS} -R
zip: clean
	cd .. &&  \
	zip -9r --filesync gsconv.zip gsconv && \
	( mkdir -p backup ; \
	  cp gsconv.zip backup/gsconv-`date "+%Y-%m-%d-%H-%M-%S"`.zip )
d: all
	${CC} -E -MMD *.c >& /dev/null
	${CXX} -E -MMD *.cpp >& /dev/null

.y.o:
.l.o:
.y.h:
	${YACC} -t -v -d $<
	rm -f y.tab.c y.output 
	mv y.tab.h $*.h
.y.c:
	${YACC} -t -v -d $<
	rm -f y.tab.h y.output
	mv y.tab.c $*.c
.l.c:
	${LEX} -o$*.c $<

# =====================================================================
LEXICAL_O=octet_string.o symbol_stack.o
LEXICAL_H=syntactic.h octet_string.h symbol_stack.h operation.h type_define.txt symbol_define.txt
SYNTACTIC_O=${LEXICAL_O} lexical.o lisp.o ast.o
SYNTACTIC_H=${LEXICAL_H} lisp.h ast.h
OPERATION_O=${SYNTACTIC_O} syntactic.o
OPERATION_H=${SYNTACTIC_H} operation.h semantic.h
DUMP_O=${OPERATION_O}
DUMP_H=${OPERATION_H}
SEMANTIC_O=${DUMP_O} operation.o preamble.o
SEMANTIC_H=${DUMP_H} preamble.h
GSCONV_O=${SEMANTIC_O} semantic.o
# =====================================================================
gsconv:    ${MAKEFILES} ${GSCONV_O} gsconv.o
	   ${LD} ${LDFLAGS} ${GSCONV_O} $@.o -o $@
	   if [ "opt${OPT}" = "opt-O3" ] ; then strip -s $@ ; fi
semantic:  ${MAKEFILES} ${SEMANTIC_O} semantic_debug.o
	   ${LD} ${LDFLAGS} ${SEMANTIC_O} $@_debug.o -o $@
dump:      ${MAKEFILES} ${DUMP_O} dump.o
	   ${LD} ${LDFLAGS} ${DUMP_O} $@.o -o $@
operation: ${MAKEFILES} ${SEMANTIC_O} operation_debug.o
	   ${LD} ${LDFLAGS} ${OPERATION_O} $@_debug.o -o $@
syntactic: ${MAKEFILES} ${SYNTACTIC_O} syntactic_debug.o
	   ${LD} ${LDFLAGS} ${SYNTACTIC_O} $@_debug.o -o $@
lexical:   ${MAKEFILES} ${LEXICAL_O} lexical_debug.o
	   ${LD} ${LDFLAGS} ${LEXICAL_O} $@_debug.o -o $@

# =====================================================================

lexical_debug.o: ${MAKEFILES} lexical.o
	${CC} ${CFLAGS} -c lexical.c -o $@ -DLEXICAL_DEBUG
syntactic_debug.o: ${MAKEFILES} syntactic.o
	${CC} ${CFLAGS} -c syntactic.c -o $@ -DSYNTACTIC_DEBUG
operation_debug.o: ${MAKEFILES} operation.o
	${CXX} ${CXXFLAGS} -c operation.cpp -o $@ -DOPERATION_DEBUG
semantic_debug.o: ${MAKEFILES} semantic.o
	${CXX} ${CXXFLAGS} -c semantic.cpp -o $@ -DSEMANTIC_DEBUG

# =====================================================================

lisp.o:         ${MAKEFILES} lisp.c lisp.h
octet_string.o: ${MAKEFILES} octet_string.c octet_string.h
symbol_stack.o: ${MAKEFILES} symbol_stack.cpp symbol_stack.h syntactic.h
ast.o:          ${MAKEFILES} ast.c ast.h
lexical.o:      ${MAKEFILES} lexical.c syntactic.h octet_string.h \
                symbol_stack.h type_define.txt symbol_define.txt
syntactic.o:    ${MAKEFILES} syntactic.c lisp.h ast.h syntactic.h \
                type_define.txt symbol_define.txt
operation.o:    ${MAKEFILES} operation.cpp operation.h lisp.h semantic.h \
                type_define.txt
dump.o:         ${MAKEFILES} dump.cpp operation.h lisp.h ast.h \
                symbol_stack.h syntactic.h type_define.txt symbol_define.txt
preamble.o:     ${MAKEFILES} preamble.cpp lisp.h ast.h preamble.h
semantic.o:     ${MAKEFILES} semantic.cpp lisp.h ast.h symbol_stack.h \
                semantic.h operation.h preamble.h syntactic.h \
                type_define.txt symbol_define.txt
gsconv.o:       ${MAKEFILES} gsconv.cpp lisp.h ast.h symbol_stack.h \
                semantic.h operation.h preamble.h syntactic.h \
                type_define.txt symbol_define.txt

# =====================================================================

lexical.c: ${MAKEFILES} lexical.l
syntactic.c: ${MAKEFILES} syntactic.y
syntactic.h: ${MAKEFILES} syntactic.y
preamble.cpp: ${MAKEFILES} dump preamble.txt
	./dump < preamble.txt > $@

# =====================================================================

ast.c: ${MAKEFILES} ast_c.sh ast.txt
	chmod 755 ./ast_c.sh
	./ast_c.sh ast.txt > ast.c
ast.h: ${MAKEFILES} ast_h.sh ast.txt
	chmod 755 ./ast_h.sh
	./ast_h.sh ast.txt > ast.h
ast.txt: ${MAKEFILES} ast_txt.sh syntactic.y
	chmod 755 ./ast_txt.sh
	./ast_txt.sh syntactic.y > ast.txt
type_define.txt: ${MAKEFILES} type_define_txt.sh operation.h
	chmod 755 ./type_define_txt.sh
	./type_define_txt.sh operation.h > type_define.txt

# =====================================================================
