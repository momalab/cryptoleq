set logscale y

plot "o_factorial_e3.out" using 1:($2+$3) w l lw 3
replot "o_fib_e3.out" using 1:($2+$3) w l lw 3
replot "o_insert.out" using 1:($2+$3) w l lw 3
replot "o_jenkins.out" using 1:($2+$3) w l lw 3
replot "o_matrix.out" using 1:($2+$3) w l lw 3
replot "o_sieve.out" using 1:($2+$3) w l lw 3
replot "o_speck.out" using 1:($2+$3) w l lw 3

pause 1000
