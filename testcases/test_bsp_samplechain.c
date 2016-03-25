/* ----
 * ---- file   : test_bsp_samplechain.c
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
 * ---- changed: 25Mar2016
 * ----
 * ----
 */

#include <stdio.h>
#include <stdint.h>

#include "../algorithm_interface_proposal.h"


void test_bsp_samplechain(void) {

   samplechain_algorithm_t alg;
   samplechain_t sc;

   samplechain_select_algorithm(1, &alg);

   alg.init(&sc, 120);

   alg.set_parameter_i(sc, "extra_padding", 2000);

   alg.add(sc, 16980, NULL/*userData*/);
   alg.add(sc,  5878, NULL/*userData*/);
   alg.add(sc, 19156, NULL/*userData*/);
   alg.add(sc, 17850, NULL/*userData*/);
   alg.add(sc,  2395, NULL/*userData*/);
   alg.add(sc,  6531, NULL/*userData*/);
   alg.add(sc,  7401, NULL/*userData*/);
   alg.add(sc,  7619, NULL/*userData*/);
   alg.add(sc, 16980, NULL/*userData*/);
   alg.add(sc, 21551, NULL/*userData*/);
   alg.add(sc,  2830, NULL/*userData*/);

   // (note) let algorithm figure out the exact chain size
   alg.set_parameter_i(sc, "chain_size",  (int32_t)alg.query_num_elements(sc));

   alg.calc(sc);

   printf("total samplechain size is %u sample frames\n", alg.query_total_size(sc));

   alg.exit(&sc);

}
