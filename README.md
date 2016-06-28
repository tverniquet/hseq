# hseq
unpolished faster implementation of seq

I was using seq instead of a file with the idea that a small program dumping out text must be faster than reading from a file in memory. That was not the case!

My concept of the cost of dealing with characters was a bit off. It turned out to be a bit harder to saturate memory bandwidth than I thought.

My first attempt for hseq was a function for printing incrementing numbers that I had used with earlier versions of hprime.

To get more speed I dealt with batches of 1000 numbers, where the bottom pattern will repeat until the size of the number increments, and where the top part can simply be copied 1000 times.

Note: It seems that the program isn't so useful as a "generator" anyway as piping to programs is still quite slow (slower than reading from a file primed to memory anyway).

In any case, this is me playing with dumping sequential numbers out really fast.

    gcc -O3 -Wall -Wextra -march=native hseq.c -o hseq
   

    time ./hseq 1 1e9 >/dev/null  # 0.734s 12.5GB/s
    time seq 1 1e9 >/dev/null     # 4.9s    1.9GB/s
    time cat seq.1e9 >/dev/null   # 0.929s  9.9GB/s


