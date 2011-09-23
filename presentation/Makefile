include make.define

all: compila run

compila:
	latex $(t).tex
	latex $(t).tex
	latex $(t).tex
	dvips -t a4$(t) $(t).dvi -o $(t).ps
	ps2pdf $(t).ps
#	pdflatex $(t).tex	

clean:
	rm -f $(t).log *.aux $(t).dvi $(t).bbl $(t).blg 

run:  compila
	evince $(t).pdf

check:
	 aspell -c $(t).tex

send:
	scp presentation.pdf palotti@mica.dcc.ufmg.br:/www/users/pos/palotti/public_html/.
