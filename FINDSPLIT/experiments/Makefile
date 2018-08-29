.SUFFIXES:
.SUFFIXES: .tango .pdf .performance
.PHONY: all clean findsplit

all: findsplit WatersDSE.performance BBS_HIBE.performance BlindAutoSIG.performance AHO_GSZK.performance TraceGroupEnc.performance

findsplit:
	cd .. && make findsplit

pdf: WatersDSE.pdf BBS_HIBE.pdf BlindAutoSIG.pdf AHO_GSZK.pdf TraceGroupEnc.pdf

.tango.performance:
	@echo == $* ====================================
	@../findsplit $< 2>&1 | grep '^performance time'
	@grep '^num_vertices=' G.dot
	@grep '^num_pairings=' G.dot
	@rm -f G.dot G0.dot G1.dot G.lp G.sol gurobi.log

.tango.pdf:
	../findsplit $< -b$*
	test $* = WatersDSE -o $* = BBS_HIBE && ( \
	dot2tex --usepdflatex -f tikz --autosize -c $*.g0  > $*.g0.tex && \
	dot2tex --usepdflatex -f tikz --autosize -c $*.g1  > $*.g1.tex && \
	dot2tex --usepdflatex -f tikz --autosize -c $*.dot > $*.tex       \
	) || ( \
	dot2tex --usepdflatex -f tikz -c --preproc $*.g0  | sfdp -Txdot | dot2tex -f tikz -c > $*.g0.raw && \
	dot2tex --usepdflatex -f tikz -c --preproc $*.g1  | sfdp -Txdot | dot2tex -f tikz -c > $*.g1.raw && \
	dot2tex --usepdflatex -f tikz -c --preproc $*.dot | sfdp -Txdot | dot2tex -f tikz -c > $*.raw    && \
	sed 's/join=bevel,/join=bevel,scale=10/' $*.g0.raw > $*.g0.tex && \
	sed 's/join=bevel,/join=bevel,scale=10/' $*.g1.raw > $*.g1.tex && \
	sed 's/join=bevel,/join=bevel,scale=10/' $*.raw    > $*.tex       \
	)
	pdflatex -interaction=nonstopmode $*.g0.tex
	pdflatex -interaction=nonstopmode $*.g1.tex
	pdflatex -interaction=nonstopmode $*.tex
	rm -f $*.lp $*.sol $*.dot $*.g0 $*.g1 $*.g0.raw $*.g1.raw $*.raw $*.g0.tex $*.g1.tex $*.tex $*.g0.aux $*.g1.aux $*.aux $*.g0.log $*.g1.log $*.log

clean:
	rm -f *.lp *.sol *.dot *.g0 *.g1 *.raw *.tex *.aux *.log
