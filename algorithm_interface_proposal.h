/* ----
 * ---- file   : algorithm_interface_proposal.h
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
 * ---- changed: 23Mar2016
 * ----
 * ----
 */

#ifndef SAMPLECHAIN_INTERFACE_H_INCLUDED
#define SAMPLECHAIN_INTERFACE_H_INCLUDED

#include "cplusplus_begin.h"


// Opaque sample chain handle
typedef void *samplechain_t;

typedef struct {
   // Query the algorithm name
   const char *(*query_algorithm_name) (void);

   // Allocate/init samplechain / internal data structures
   //  - Prepare new output chain
   //  - Returns new samplechain handle in 'retSc'
   void (*init) (samplechain_t *_retSc, uint32_t _numSlices/*120 for AR*/);

   // Set algorithm-specific parameter value (e.g. "extra_padding")
   //  - Returns true if parameter was set successfully, false otherwise (unknown param, value out of range, ..)
   //  - Requires that init() has been called
   bool_t (*set_parameter_i) (samplechain_t _sc, const char *_paramName, int32_t _paramValue);
   bool_t (*set_parameter_f) (samplechain_t _sc, const char *_paramName, float32_t _paramValue);

   // Add a new element (waveform) to the chain
   //  - Invalidates the current output
   //  - May only be called after init() was called
   //  - Returns true if the element was added, false otherwise (e.g. max number of slices exceeded)
   bool_t (*add) (samplechain_t _sc, size_t _numSampleFrames);

   // Calculate sample chain
   //  - Layout sample chain elements and create new output state (for queries)
   void (*calc) (samplechain_t _sc);

   // Query the current number of elements in the sample chain
   uint32_t (*query_num_elements) (samplechain_t _sc);

   // Query total size of sample chain (number of sample frames)
   //  - Requires that 'calc' has been called
   //  - Returns 0 if something went terribly wrong (tm)
   size_t (*query_total_size) (samplechain_t _sc);

   // Query start offset of sample chain element (number of sample frames)
   //  - (note) this is just an utility fxn (could be implemented generically)
   //  - Requires that 'calc' has been called
   size_t (*query_element_offset) (samplechain_t _sc, uint32_t _elementIdx);

   // Query total size of sample chain element (number of sample frames)
   //  - Requires that 'calc' has been called
   size_t (*query_element_total_size) (samplechain_t _sc, uint32_t _elementIdx);

   // Query original size of sample chain element (waveform) (number of sample frames)
   //  - (note) the difference to the total size is the number of padding frames
   size_t (*query_element_original_size) (samplechain_t _sc, uint32_t _elementIdx);  

   // Free samplechain / internal datastructures
   void (*exit) (samplechain_t *_sc);
      
} samplechain_algorithm_t;


// Query the number of available algorithms
uint32_t samplechain_get_num_algorithms (void);

// Select an algorithm.
//  - Returns algorithm vtable in 'retAlgorithm' if 'algorithmIdx' is valid, NULL otherwise.
void samplechain_select_algorithm (uint32_t _algorithmIdx, samplechain_algorithm_t *_retAlgorithm);


#if 0
// Example usage:
samplechain_algorithm_t alg;
samplechain_t sc;

samplechain_select_algorithm(0, &alg);

alg.init(&sc, 120);

alg.add(sc, 34004);
alg.add(sc, 11800);
alg.add(sc, 38356);
alg.add(sc, 35744);
alg.add(sc,  4834);
alg.add(sc, 13106);
alg.add(sc, 14846);
alg.add(sc, 15282);
alg.add(sc, 34004);
alg.add(sc, 43146);
alg.add(sc,  5704);

alg.calc();

printf("total samplechain size is %lu sample frames\n", alg.query_total_size(sc));

alg.exit();
#endif


#endif // SAMPLECHAIN_INTERFACE_H_INCLUDED
