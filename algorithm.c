/* ----
 * ---- file   : test_bsp_varichain.c
 * ---- author : bsp
 * ---- legal  : Distributed under terms of the MIT LICENSE (MIT).
 * ----
 * ---- Permission is hereby granted, free of charge, to any person obtaining a copy
 * ---- of this software and associated documentation files (the "Software"), to deal
 * ---- in the Software without restriction, including without limitation the rights
 * ---- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * ---- copies of the Software, and to permit persons to whom the Software is
 * ---- furnished to do so, subject to the following conditions:
 * ----
 * ---- The above copyright notice and this permission notice shall be included in
 * ---- all copies or substantial portions of the Software.
 * ----
 * ---- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * ---- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * ---- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * ---- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * ---- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * ---- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * ---- THE SOFTWARE.
 * ----
 * ---- info   : This is part of the "libsamplechain" package.
 * ----
 * ---- changed: 23Mar2016, 25Mar2016
 * ----
 * ----
 */

#include <stdio.h>
#include <stdint.h>

#include "algorithm_interface_proposal.h"


extern void bsp_varichain_select   (samplechain_algorithm_t *_algorithm);
extern void bsp_samplechain_select (samplechain_algorithm_t *_algorithm);


uint32_t samplechain_get_num_algorithms(void) {
   // (todo) increase this number when adding more algorithms
   return 2;
}

bool_t samplechain_select_algorithm(uint32_t _algorithmIdx, samplechain_algorithm_t *_retAlgorithm) {
   bool_t ret = SC_FALSE;

   if(NULL != _retAlgorithm)
   {
      switch(_algorithmIdx)
      {
         default:
            break;

         case 0:
            bsp_varichain_select(_retAlgorithm);
            ret = SC_TRUE;
            break;

         case 1:
            bsp_samplechain_select(_retAlgorithm);
            ret = SC_TRUE;
            break;

         // (todo) add more algorithms here
      }
   }

   return ret;
}
