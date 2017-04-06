#!/gnuplot

set terminal wxt 0 size 1400, 1000 enhanced font "Verdana,18" persist
set output
set border 31 front lt black linewidth 2.000 dashtype solid
set format x "%L"
set format y "%.0f%%"
set grid
unset label
set style line 1  linecolor rgb "#0060ad"  linewidth 2.000 dashtype 1 pointtype 7 pointsize 1.500 pointinterval -1
set style line 2  linecolor rgb "black"  linewidth 4.000 dashtype 5 pointtype 5 pointsize default pointinterval 0
set logscale x 2
set pointsize 1
set pointintervalbox 3
set xtics   (2.00000, 16.0000, 256.000, 2048.00, 32768.0, 262144., 2.09715e+006)
set title ""
set title  font "" norotate
set xlabel ""
set xlabel  font "" textcolor lt -1 norotate
set ylabel ""
set ylabel  font "" textcolor lt -1 rotate by -270
set yrange [ 0.00000 : 50.0000 ] noreverse nowriteback
plot "plt.10000.4.txt" using ($1):($2) with linespoints ls 1 lc 1 title "MAX deviation ppn=4", "plt.10000.8.txt" using ($1):($2) with linespoints ls 1 lc 2 title "MAX deviation ppn=8", "plt.10000.16.txt" using ($1):($2) with linespoints ls 1 lc 3 title "MAX deviation ppn=16", "plt.10000.4.txt" using ($1):($4) with linespoints ls 1 lc 4  title "Standard deviation ppn=4", "plt.10000.8.txt" using ($1):($4) with linespoints ls 1 lc 5 title "Standard deviation ppn=8", "plt.10000.16.txt" using ($1):($4) with linespoints ls 1 lc 6 title "Standard deviation ppn=16", 3 with lines ls 2 notitle

