.SUFFIXES:
.SUFFIXES: .csdl .result

test: Makefile test1.result test2.result test3.result test4.result test5.result test6.result test7.result test8.result test9.result
clean:
	rm -f *.dot *.g0 *.g1 *.gsconv *.lp *.result *.sol *.tango gurobi.log *.tex *.aux *.log *.raw

.csdl.result:
	../gsconv3 < $< > $*.gsconv
	../gsconv < $*.gsconv > $*.tango
	../findsplit --ratio=4 -b$* $*.tango
	../gsconv3 $*.dot < $*.csdl > $*.result
