OPT=-O3
# OPT=-O0 -g

# =====================================================================

.PHONY: all debug samples clean zip
.SUFFIXES: .md .pdf

all: findsplit
debug: lexical syntactic semantic decide optimizer naive crypto2014 bruteforce backtrack montecarlo linear driver
# samples: findsplit
# 	cd samples && make
# test: findsplit
# 	cd test && make
clean:
	rm -f findsplit driver linear bruteforce backtrack naive crypto2014 montecarlo optimizer decide semantic syntactic lexical *.o ast_node_def.* *.output syntactic.h y.tab.* *.dot *.svg *.lp *.sol syntactic.c lexical.c syntactic_debug.c lexical_debug.c tags *.scip
#	cd samples && make clean
#	cd test && make clean
tags:
	${CTAGS} -R
zip: clean
	cd .. &&  \
	zip -9r --filesync findsplit.zip findsplit && \
	( mkdir -p backup ; \
	  cp findsplit.zip backup/findsplit-`date "+%Y-%m-%d-%H-%M-%S"`.zip )
doc: README.pdf LICENSE.pdf

# =====================================================================

CPPFLAGS=-I/usr/local/include
CFLAGS=${CPPFLAGS} ${OPT} -std=c99 ${ADDITIONAL_CFLAGS}
CXXFLAGS=${CPPFLAGS} ${OPT} -std=c++11
LDFLAGS=

# =====================================================================

LEXICAL_O=
LEXICAL_H=syntactic.h
SYNTACTIC_O=ast_node_def.o ast.o lexical.o
SYNTACTIC_H=ast_node_def.h ast.h
SEMANTIC_O=${SYNTACTIC_O} syntactic.o pairing_graph.o
SEMANTIC_H=${SYNTACTIC_H} pairing_graph.h semantic.h
DECIDE_O=${SEMANTIC_O} semantic.o
DECIDE_H=${SEMANTIC_H} decide.h
OPTIMIZER_O=${DECIDE_O} decide.o mysignal.o
OPTIMIZER_H=${DECIDE_H} optimizer.h
MONTECARLO_O=${OPTIMIZER_O} optimizer.o
MONTECARLO_H=${OPTIMIZER_H} montecarlo.h mysignal.h
NAIVE_O=${OPTIMIZER_O} optimizer.o
NAIVE_H=${OPTIMIZER_H} naive.h mysignal.h
CRYPTO2014_O=${OPTIMIZER_O} optimizer.o
CRYPTO2014_H=${OPTIMIZER_H} crypto2014.h mysignal.h
BRUTEFORCE_O=${OPTIMIZER_O} optimizer.o
BRUTEFORCE_H=${OPTIMIZER_H} bruteforce.h mysignal.h
BACKTRACK_O=${OPTIMIZER_O} optimizer.o
BACKTRACK_H=${OPTIMIZER_H} backtrack.h mysignal.h
LINEAR_O=${OPTIMIZER_O} optimizer.o
LINEAR_H=${OPTIMIZER_H} linear.h
DRIVER_O=${MONTECARLO_O} montecarlo.o bruteforce.o backtrack.o naive.o crypto2014.o linear.o
DRIVER_H=${MONTECARLO_H} bruteforce.h backtrack.h naive.h crypto2014.h linear.h driver.h
FINDSPLIT_O=${DRIVER_O} driver.o
FINDSPLIT_H=${DRIVER_H}

# =====================================================================

.y.o:
.l.o:
.y.h:
	${YACC} -v -d $<
	rm -f y.tab.c y.output 
	mv y.tab.h $*.h
.y.c:
	${YACC} -v -d $<
	rm -f y.tab.h y.output
	mv y.tab.c $*.c
.l.c:
	${LEX} -o$*.c $<
.md.pdf:
	pandoc -f markdown -t latex $< -o $*.pdf

# =====================================================================

findsplit: Makefile ${FINDSPLIT_O} findsplit.o
	${LD} ${LDFLAGS} ${FINDSPLIT_O} $@.o -o $@
driver: Makefile ${DRIVER_O} driver_debug.o
	${LD} ${LDFLAGS} ${DRIVER_O} $@_debug.o -o $@
linear: Makefile ${LINEAR_O} linear_debug.o
	${LD} ${LDFLAGS} ${LINEAR_O} $@_debug.o -o $@
bruteforce: Makefile ${BRUTEFORCE_O} bruteforce_debug.o
	${LD} ${LDFLAGS} ${BRUTEFORCE_O} $@_debug.o -o $@
backtrack: Makefile ${BACKTRACK_O} backtrack_debug.o
	${LD} ${LDFLAGS} ${BACKTRACK_O} $@_debug.o -o $@
naive: Makefile ${NAIVE_O} naive_debug.o
	${LD} ${LDFLAGS} ${NAIVE_O} $@_debug.o -o $@
crypto2014: Makefile ${CRYPTO2014_O} crypto2014_debug.o
	${LD} ${LDFLAGS} ${CRYPTO2014_O} $@_debug.o -o $@
montecarlo: Makefile ${MONTECARLO_O} montecarlo_debug.o
	${LD} ${LDFLAGS} ${MONTECARLO_O} $@_debug.o -o $@
optimizer: Makefile ${OPTIMIZER_O} optimizer_debug.o
	${LD} ${LDFLAGS} ${OPTIMIZER_O} $@_debug.o -o $@
decide: Makefile ${DECIDE_O} decide_debug.o
	${LD} ${LDFLAGS} ${DECIDE_O} $@_debug.o -o $@
semantic: Makefile ${SEMANTIC_O} semantic_debug.o
	${LD} ${LDFLAGS} ${SEMANTIC_O} $@_debug.o -o $@
syntactic: Makefile ${SYNTACTIC_O} syntactic_debug.o
	${LD} ${LDFLAGS} ${SYNTACTIC_O} $@_debug.o -o $@
lexical: Makefile ${LEXICAL_O} lexical_debug.o
	${LD} ${LDFLAGS} ${LEXICAL_O} $@_debug.o -o $@

# =====================================================================

lexical_debug.o: Makefile lexical.c ${LEXICAL_H}
	${CC} ${CFLAGS} -c lexical.c -o $@ -DLEXICAL_DEBUG
syntactic_debug.o: Makefile syntactic.c ${SYNTACTIC_H}
	${CC} ${CFLAGS} -c syntactic.c -o $@ -DSYNTACTIC_DEBUG
semantic_debug.o: Makefile semantic.cpp ${SEMANTIC_H}
	${CXX} ${CXXFLAGS} -c semantic.cpp -o $@ -DSEMANTIC_DEBUG
decide_debug.o: Makefile decide.cpp ${DECIDE_H}
	${CXX} ${CXXFLAGS} -c decide.cpp -o $@ -DDECIDE_DEBUG
optimizer_debug.o: Makefile optimizer.cpp ${OPTIMIZER_H}
	${CXX} ${CXXFLAGS} -c optimizer.cpp -o $@ -DOPTIMIZER_DEBUG
montecarlo_debug.o: Makefile montecarlo.cpp ${MONTECARLO_H}
	${CXX} ${CXXFLAGS} -c montecarlo.cpp -o $@ -DMONTECARLO_DEBUG
naive_debug.o: Makefile naive.cpp ${NAIVE_H}
	${CXX} ${CXXFLAGS} -c naive.cpp -o $@ -DNAIVE_DEBUG
crypto2014_debug.o: Makefile crypto2014.cpp ${CRYPTO2014_H}
	${CXX} ${CXXFLAGS} -c crypto2014.cpp -o $@ -DCRYPTO2014_DEBUG
bruteforce_debug.o: Makefile bruteforce.cpp ${BRUTEFORCE_H}
	${CXX} ${CXXFLAGS} -c bruteforce.cpp -o $@ -DBRUTEFORCE_DEBUG
backtrack_debug.o: Makefile backtrack.cpp ${BACKTRACK_H}
	${CXX} ${CXXFLAGS} -c backtrack.cpp -o $@ -DBACKTRACK_DEBUG
linear_debug.o: Makefile linear.cpp ${LINEAR_H}
	${CXX} ${CXXFLAGS} -c linear.cpp -o $@ -DLINEAR_DEBUG
driver_debug.o: Makefile driver.cpp ${DRIVER_H}
	${CXX} ${CXXFLAGS} -c driver.cpp -o $@ -DDRIVER_DEBUG
# =====================================================================

lexical.c: Makefile lexical.l
syntactic.c: Makefile syntactic.y

# =====================================================================

mysignal.o: Makefile mysignal.cpp mysignal.h
ast.o: Makefile ast.c ast.h
ast_node_def.o: Makefile ast_node_def.c ast_node_def.h
lexical.o: Makefile lexical.c ${LEXICAL_H}
syntactic.o: Makefile syntactic.c ${SYNTACTIC_H}
pairing_graph.o: Makefile pairing_graph.cpp pairing_graph.h
semantic.o: Makefile semantic.cpp ${SEMANTIC_H}
decide.o: Makefile decide.cpp ${DECIDE_H}
optimizer.o: Makefile optimizer.cpp ${OPTIMIZER_H}
montecarlo.o: Makefile montecarlo.cpp ${MONTECARLO_H}
naive.o: Makefile naive.cpp ${NAIVE_H}
crypto2014.o: Makefile crypto2014.cpp ${CRYPTO2014_H}
bruteforce.o: Makefile bruteforce.cpp ${BRUTEFORCE_H}
backtrack.o: Makefile backtrack.cpp ${BACKTRACK_H}
linear.o: Makefile linear.cpp ${LINEAR_H}
driver.o: Makefile driver.cpp ${DRIVER_H}
findsplit.o: Makefile findsplit.cpp ${FINDSPLIT_H}

# =====================================================================

ast_node_def.c: Makefile ast_node_def_c.sh ast_node_def.txt
	chmod 755 ./ast_node_def_c.sh
	./ast_node_def_c.sh ast_node_def.txt > ast_node_def.c
ast_node_def.h: Makefile ast_node_def_h.sh ast_node_def.txt
	chmod 755 ./ast_node_def_h.sh
	./ast_node_def_h.sh ast_node_def.txt > ast_node_def.h
ast_node_def.txt: Makefile ast_node_def_txt.sh syntactic.y
	chmod 755 ./ast_node_def_txt.sh
	./ast_node_def_txt.sh syntactic.y > ast_node_def.txt

# =====================================================================
