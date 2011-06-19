set terminal postscript landscape color "Helvetica" 19 
set output 'ratings.eps' 
set style data boxes
#set xtics (" " 1) 
set xtics 1
#set yrange[0:]  
set xrange[-1:11]  
set grid       

set xlabel "Nota"
set ylabel "Numero de livros"

plot 'ratings_graf.txt' using ($1):($2) title "" with imp lw 60

