# Makefile
include		../Makefile.ini

#PARAMETERS= -treino pequenosTestes/treino -validacao pequenosTestes/validacao -teste pequenosTestes/teste -dontsave -g 2 graphs/citation.txt graphs/authorship.txt
#PARAMETERS= -treino pequenosTestes/treino1 -validacao pequenosTestes/validacao -teste pequenosTestes/teste1 -dontsave -g 2 graphs/citation.txt graphs/authorship.txt
PARAMETERS= -treino pequenosTestes/treino1 -validacao pequenosTestes/validacao -teste pequenosTestes/teste1 -dontsave -g 1 graphs/citation.txt
#authorship.txt
#PARAMETERS= -treino pequenosTestes/treino1 -validacao pequenosTestes/validacao1 -teste pequenosTestes/teste1 -dontsave
#PARAMETERS= -treino pequenosTestes/treino -validacao pequenosTestes/validacao -teste pequenosTestes/teste -dontsave -seed 0
#PARAMETERS= -treino pequenosTestes/treino -validacao pequenosTestes/validacao -teste pequenosTestes/teste -dontsave -g 1 graphs/authorship2.txt

TARGET	= gp-cred

OBJS = TreeEvaluator.o NormalEstimator.o Graph.o IGraph.o Statistics.o MyGP.o IGP.o gpcpp.o NaiveBayes.o KNN.o InOut.o Examples.o Tokenizer.o Main.o 

#Use igraph library installed by apt-get 
CPPFLAGS = -O2 $(OVERALLCPPFLAGS) -I$(GPINCLUDE)  -Wno-deprecated -g -L/usr/lib -ligraph -I/usr/include/igraph/ -I/usr/local/include/igraph -std=c++0x -Wall
#CPPFLAGS = -O2 $(OVERALLCPPFLAGS) -I$(GPINCLUDE)  -Wno-deprecated -g -L/usr/lib -std=c++0x -Wall
#Use igraph library installed in local 
#CPPFLAGS = -O2 $(OVERALLCPPFLAGS) -I$(GPINCLUDE)  -Wno-deprecated -g -L/usr/local/lib -ligraph -I/usr/local/include/igraph -std=c++0x -Wall 

LDFLAGS	 = $(OVERALLCPPFLAGS) -L$(GPLIB) -lgp -lm  

#paranoid: $(OBJS) $(TARGET)
#CPPFLAGS += -Wextra -Wfloat-equal -Wundef -Wunsafe-loop-optimizations \
	   -Wredundant-decls -Wpointer-arith -Winline -Wcast-qual \
	   -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare \
	   -Wmissing-field-initializers -Wmissing-format-attribute -Wnormalized=nfc \
	   -Wlong-long

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<


all: $(OBJS) $(TARGET)

$(TARGET) : $(OBJS) $(GPLIBFILE)
	$(CCC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(CPPFLAGS) $(LIBS)
	@echo 'Lembrar de rodar versao com valgrind'

clean:
	rm -f *~ *.o $(OBJS)

superclean: clean
	rm -f $(TARGET) *pop.dat* *.data.*

valgrind:
	valgrind --leak-check=full $(TARGET) $(PARAMETERS) 

valgrindall:
	valgrind --leak-check=full --show-reachable=yes $(TARGET) $(PARAMETERS) 

# File dependencies
#cred.o: cred.cc statistics.o tradNB.o ../include/gpconfig.h ../include/gp.h
#avaliaGP.o: avaliaGP.cc statistics.o tradNB.o ../include/gpconfig.h ../include/gp.h
#comite.o: comite.cc statistics.o tradNB.o ../include/gpconfig.h ../include/gp.h
#avaliacao.o: avaliacao.cc statistics.o tradNB.o ../include/gpconfig.h ../include/gp.h

#igraph:
#	@echo "usando regra $@"
#	gcc $(FILE_IGRAPH) -I/usr/local/include/igraph -L/usr/local/lib -ligraph -o igraph

run:
	./$(TARGET) $(PARAMETERS)


