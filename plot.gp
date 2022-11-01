INFILE=ARG1


a0 = -1
b0 = 0.1

f(x, a, b) = a*x + b

fit_min = 5.5
fit_max = 8.5

fit [fit_min:fit_max] f(x, a0, b0) INFILE using 1:2 via a0,b0

set xlabel 'log(Q)'
set ylabel 'log(I)'

plot INFILE using 1:2 pt 7 ps 1 t '', \
     [fit_min:fit_max] f(x, a0, b0) dashtype 2 lw 2 lc rgb 'red' t sprintf("a = %.2f +- %.2f\nb = %.2f +- %.2f", a0, a0_err, b0, b0_err)

pause -1