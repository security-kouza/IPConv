.PHONY: all doc clean zip
.SUFFIXES: .md .pdf .tex .txt .csdl .result

all: Makefile findsplit gsconv gsconv3
doc: Makefile pdf txt

pdf: Makefile README.pdf LICENSE.pdf
tex: Makefile README.tex LICENSE.tex
txt: Makefile README.txt LICENSE.txt
clean: Makefile
	rm -f findsplit gsconv gsconv3 *.dot *.g0 *.g1 *.gsconv *.lp *.result *.sol *.tango gurobi.log
	rm -f *.dot *.g0 *.g1 *.gsconv *.lp *.result *.sol *.tango gurobi.log *.tex *.aux *.log *.raw
	cd src/findsplit3 && make clean
	cd src/gsconv && make clean
	cd src/gsconv3 && make clean
zip: Makefile clean
	cd .. &&  \
	zip -9r \
	  FINDSPLIT-`date "+%Y-%m-%d-%H-%M-%S"`.zip FINDSPLIT \
          --exclude 'FINDSPLIT/.git/*'
test: Makefile all test1.result test2.result test3.result test4.result test5.result test6.result test7.result
test_clean:
	rm -f *.dot *.g0 *.g1 *.gsconv *.lp *.result *.sol *.tango gurobi.log *.tex *.aux *.log *.raw

findsplit: Makefile src/findsplit3
	cd src/findsplit3 && make
	cp src/findsplit3/findsplit .
gsconv: Makefile src/gsconv
	cd src/gsconv && make
	cp src/gsconv/gsconv .
gsconv3: Makefile src/gsconv3
	cd src/gsconv3 && make
	cp src/gsconv3/gsconv3 .

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
.md.pdf: template.latex 
	pandoc -f markdown -t latex --template="template.latex" $< -o $*.pdf
.md.tex: template.latex 
	pandoc -t latex --template="template.latex" $< -o $*.tex
.md.txt:
	pandoc -t plain $< -o $*.txt
.csdl.result: all
	./gsconv3 < $< > $*.gsconv
	./gsconv < $*.gsconv > $*.tango
	./findsplit --ratio=4 -b$* $*.tango
	./gsconv3 $*.dot < $*.csdl > $*.result

LICENSE.md: clean LICENSE.sh
	./LICENSE.sh > LICENSE.md
CSDL.pdf: CSDL.md
	pandoc --latex-engine=lualatex -f markdown -t latex --template="template-j.latex" CSDL.md -o CSDL.pdf
