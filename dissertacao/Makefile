all: clean dissertacao.pdf
	
#dissertacao.pdf: dissertacao.ps
#	ps2pdf14 dissertacao.ps

#dissertacao.ps: dissertacao.dvi
#	dvips dissertacao.dvi -o dissertacao.ps

dissertacao.pdf: dissertacao.bbl
	pdflatex dissertacao.tex
	pdflatex dissertacao.tex

dissertacao.bbl: dissertacao.aux
	bibtex dissertacao

dissertacao.aux: dissertacao.tex references.bib
	pdflatex dissertacao.tex

run: all 
	evince dissertacao.pdf &

clean:
	rm -f *.log *.aux *.bbl *.blg *.dvi *.ps *.pdf *.lot *.toc *.lof *.out *.bak *.loa *.new *.backup *~
