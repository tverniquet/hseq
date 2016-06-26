/*
 * Author: Tristan Verniquet
 * Date: June 2016
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


/*
 * Help memcpy out by copying the buffer according to a vector size.
 *
 * Use fewer vectors if possible (ie gcd)
 */
int num_vec[] = {0,0,0,0,0,5,3,7,1,9,5,11,3,13,7,15,1,17,9,19,5,21,11};


char vecbuf[32*32] __attribute__((aligned(32)));
char outbuf[1024*1024]__attribute__((aligned(32)));
char *o = outbuf;


static void
flush()
{
   if (write(STDOUT_FILENO, outbuf, o-outbuf) < 0)
     exit(EXIT_FAILURE);
   o = outbuf;
}


static void
add(char *p, size_t cnt)
{
   memcpy(o, p, cnt);
   if ( (o += cnt) > outbuf + 32*1024)
      flush();
}


char lower_1000[21*1000*2];
int lower_gap = 0;

static void
set_lower_1000(int size)
{
   uint64_t *p, *q;
   int i;
   int j;
   int k;
   if (size != lower_gap) {
      lower_gap = size;

      bzero(lower_1000, sizeof lower_1000);
      for (i = 0; i < 10; i++) {
         for (j = 0; j < 10; j++) {
            for (k = 0; k < 10; k++) {
               lower_1000[(i*100+j*10 + k +1)*size - 2] = '0' + k;
               lower_1000[(i*100+j*10 + k +1)*size - 3] = '0' + j;
               lower_1000[(i*100+j*10 + k +1)*size - 4] = '0' + i;
            }
         }
      }
   }

   p = (uint64_t *)o;
   q = (uint64_t *)lower_1000;

   i = (size * 1000 + 7) / 8;
   while (i--)
      *p++ |= *q++;
}


static inline void __attribute__((always_inline))
copy_set_x(char *buf, int size)
{
   int i,j,k;
   int x = num_vec[size];
   char *p = o;
   k = 0;
   for (i = 0; i < x; i++)
      for (j = 0; j < 32; j++, k++)
         vecbuf[k] = buf[k%size];

   i = (size*1000 + 32*x - 1)/(32*x);

   while (i--) {
      memcpy(p, vecbuf, 32*x);
      p += 32*x;
   }
}

static void
copy_set(char *buf, int size)
{
#if 0
   cope_set_x(buf, size)
#else
   switch(size) {
      case 5: copy_set_x(buf, 5); break;
      case 6: copy_set_x(buf, 6); break;
      case 7: copy_set_x(buf, 7); break;
      case 8: copy_set_x(buf, 8); break;
      case 9: copy_set_x(buf, 9); break;
      case 10: copy_set_x(buf, 10); break;
      case 11: copy_set_x(buf, 11); break;
      case 12: copy_set_x(buf, 12); break;
      case 13: copy_set_x(buf, 13); break;
      case 14: copy_set_x(buf, 14); break;
      case 15: copy_set_x(buf, 15); break;
      case 16: copy_set_x(buf, 16); break;
      case 17: copy_set_x(buf, 17); break;
      case 18: copy_set_x(buf, 18); break;
      case 19: copy_set_x(buf, 19); break;
      case 20: copy_set_x(buf, 20); break;
      case 21: copy_set_x(buf, 21); break;
      case 22: copy_set_x(buf, 22); break;
   }
#endif
}


/*
 * This was my original print function from earlier versions of hprime
 */
static void
add_range_slow(uint64_t start, uint64_t end, uint64_t skip)
{
   char buf[22] = "00000000000000000000\n", *b, *bmax = &buf[20];
   uint64_t t;
   uint64_t vo = 0;
   uint64_t i;

   for (i = start; i <= end && i >= start; i+=skip) {
      t = i - vo;
      b = &buf[20];
      do {
         t += *--b - '0';
         *b = (t%10) + '0';
      } while ( t/=10 );
      if (b < bmax)
         bmax = b;
      add(bmax, &buf[21]-bmax);
      vo = i;
   }
   flush();
}


static uint64_t
read_num (char *num)
{
   double f;
   uint64_t i;
   char *lp;

   i = strtoull(num, &lp, 0);

   if (*lp != '\0') {
      sscanf(num, "%lf", &f);
      i = f;
   }
   return i;
}


int
main(int argc, char *argv[])
{
   uint64_t start;
   uint64_t end;
   uint64_t skip = argc > 3 ? strtol(argv[2], NULL, 0) : 1;
   int size;
   char buf[22];

   start = read_num(argv[1]);
   end = read_num(argc > 3 ? argv[3] : argv[2]);

   if (skip != 1 || end - start < 1000 || start > UINT64_MAX - 1000) {
      add_range_slow(start, end, skip);
      exit(EXIT_SUCCESS);
   }


   /* Currently only skip == 1 */

   if ((start % 1000) != 0) {
      add_range_slow(start, start/1000*1000 + 999, skip);
      start = start/1000*1000 + 1000;
   }


   for (; start <= end - 1000; start += 1000) {

      /* print the next number - Don't worry about this cost atm */
      size = sprintf(buf, "%lu\n", start);

      /* Copy the above 1000 times */
      copy_set(buf, size);

      /* Set the lower parts for those 1000 numbers */
      set_lower_1000(size);

      /* Flush the buffer */
      o += 1000 * size;
      flush();
   }

   if (start <= end)
      add_range_slow(start, end, skip);

   flush();
}
