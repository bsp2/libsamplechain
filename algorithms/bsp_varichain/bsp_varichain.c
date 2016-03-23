/* ----
 * ---- file   : bsp_varichain.c
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../algorithm_interface_proposal.h"


typedef struct {
   int32_t orig_sz;
   int32_t cur_sz;
   int32_t pad_sz;

   void *user_data;

} element_t;


typedef struct {

   element_t *elements;

   uint32_t num_elements;
   uint32_t max_elements;

   uint32_t num_slices; // 120 for AR

   int32_t extra_padding;
   int32_t min_padding;

   float32_t cur_sta; // tmp when building output chain

   bool_t b_output_valid;

} sc_t;


// Helper fxns:
static int32_t loc_get_total_smp_sz(sc_t *_sc) {
   int32_t ret = 0;
   uint32_t elementIdx;

   for(elementIdx = 0; elementIdx < _sc->num_elements; elementIdx++)
   {
      ret += _sc->elements[elementIdx].cur_sz;
   }

   return ret;
}

static int32_t loc_get_max_smp_sz(sc_t *_sc) {
   int32_t ret = 0;
   uint32_t elementIdx;

   for(elementIdx = 0; elementIdx < _sc->num_elements; elementIdx++)
   {
      int32_t sz = _sc->elements[elementIdx].cur_sz;

      if(sz > ret)
      {
         ret = sz;
      }
   }

   return ret;
}

static void loc_align_sizes_to(sc_t *_sc, int32_t _sz) {

   uint32_t elementIdx;

   for(elementIdx = 0; elementIdx < _sc->num_elements; elementIdx++)
   {
      element_t *el = &_sc->elements[elementIdx];

      int32_t chSz = el->cur_sz;

      if(chSz != ((chSz / _sz) * _sz))
      {
         chSz = ((chSz / _sz) + 1) * _sz;

         el->pad_sz = chSz - el->orig_sz;

         el->cur_sz = chSz;

         printf("[trc] STA=%6.2f origSz=%10u padSz=%8d chSz=%10d stepSz=%d\n",
                _sc->cur_sta,
                el->orig_sz,
                el->pad_sz,
                el->cur_sz,
                _sz
                );

         _sc->cur_sta += ((float32_t)chSz) / _sz;
      }
   }
}

static void loc_align_padded_sizes_to(sc_t *_sc, int32_t _sz) {

   uint32_t elementIdx;

   for(elementIdx = 0; elementIdx < _sc->num_elements; elementIdx++)
   {
      element_t *el = &_sc->elements[elementIdx];

      int32_t chSz = el->cur_sz;

      if(chSz != ((chSz / _sz) * _sz))
      {
         chSz = ((chSz / _sz) + 0) * _sz;

         el->pad_sz = chSz - el->orig_sz;

         el->cur_sz = chSz;

         printf("[trc] STA=%6.2f origSz=%10u padSz=%8d chSz=%10d stepSz=%d\n",
                _sc->cur_sta,
                el->orig_sz,
                el->pad_sz,
                el->cur_sz,
                _sz
                );

         _sc->cur_sta += ((float32_t)chSz) / _sz;
      }
   }
}

static bool_t loc_are_pad_sizes_greater_than(sc_t *_sc, int32_t _sz) {
   bool_t ret = SC_TRUE;

   uint32_t elementIdx;

   for(elementIdx = 0; ret && (elementIdx < _sc->num_elements); elementIdx++)
   {
      element_t *el = &_sc->elements[elementIdx];

      ret = ret && (el->pad_sz >= _sz);
   }

   return ret;
}

static void loc_restore_orig_sizes(sc_t *_sc) {
   uint32_t elementIdx;

   for(elementIdx = 0; (elementIdx < _sc->num_elements); elementIdx++)
   {
      element_t *el = &_sc->elements[elementIdx];

      el->cur_sz = el->orig_sz;
   }
}

static float32_t loc_calc_average_slice_padding(sc_t *_sc) {
   uint32_t elementIdx;
   int32_t padSum = 0;

   for(elementIdx = 0; (elementIdx < _sc->num_elements); elementIdx++)
   {
      element_t *el = &_sc->elements[elementIdx];

      padSum += el->pad_sz;
   }

   return ((float32_t)padSum) / _sc->num_elements;
}

// Interface impl:

static const char *loc_query_algorithm_name(void) {
   return "VariChain (bsp)";
}

static void loc_init(samplechain_t *_retSc, uint32_t _numSlices/*120 for AR*/) {
   
   if(NULL != _retSc)
   {
      if(_numSlices > 0)
      {
         sc_t *sc = malloc(sizeof(sc_t) + sizeof(element_t) * (_numSlices + 1));
         
         if(NULL != sc)
         {
            sc->elements       = (element_t*) (sc + 1);
            sc->num_elements   = 0;
            sc->max_elements   = _numSlices + 1;
            sc->num_slices     = _numSlices;
            sc->extra_padding  = 2000;
            sc->min_padding    = 1000;
            sc->cur_sta        = 0.0f;
            sc->b_output_valid = SC_FALSE;

            *_retSc = sc;
         }
         else
         {
            *_retSc = NULL;
         }
      }
      else
      {
         *_retSc = NULL;
      }
   }
}

static bool_t loc_set_parameter_i(samplechain_t _sc, const char *_paramName, int32_t _paramValue) {
   bool_t ret = SC_FALSE;

   return ret;
}

static bool_t loc_set_parameter_f(samplechain_t _sc, const char *_paramName, float32_t _paramValue) {
   bool_t ret = SC_FALSE;

   return ret;
}

static bool_t loc_add(samplechain_t _sc, size_t _numSampleFrames, void *_userData) {
   bool_t ret = SC_FALSE;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(sc->num_elements < sc->max_elements)
      {
         element_t *el = &sc->elements[sc->num_elements++];

         el->orig_sz   = _numSampleFrames;
         el->cur_sz    = _numSampleFrames;
         el->pad_sz    = 0;
         el->user_data = _userData;

         // Succeeded
         ret = SC_TRUE;
      }
   }

   return ret;
}

static void loc_calc(samplechain_t _sc) {

   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      sc->b_output_valid = SC_FALSE;

      if(sc->num_elements > 0)
      {
         uint32_t elementIdx;
         int32_t totalSmpSz;
         int32_t origTotalSmpSz;
         int32_t origPadTotalSmpSz;
         int32_t maxSmpSz;
         float32_t maxPct;
         int32_t maxNumSlices;
         float32_t slcSz;
         int32_t iter = 1;
         float32_t newNumSlices;
         int32_t extraPadding = sc->extra_padding;
         float32_t padNewNumSlices;

         totalSmpSz = loc_get_total_smp_sz(sc);

         origTotalSmpSz = totalSmpSz;

         for(;;)
         {
            // Add padding to all chain elements
            for(elementIdx = 0; elementIdx < sc->num_elements; elementIdx++)
            {
               sc->elements[elementIdx].cur_sz += extraPadding;
            }

            totalSmpSz = loc_get_total_smp_sz(sc);

            origPadTotalSmpSz = totalSmpSz;

            maxSmpSz = loc_get_max_smp_sz(sc);

            maxPct = ((float32_t)maxSmpSz) / totalSmpSz;
            maxNumSlices = (int32_t)((sc->num_slices * maxPct) + 0.5f);

            slcSz = maxSmpSz / maxNumSlices;

            sc->cur_sta = 0.0f;
            loc_align_sizes_to(sc, slcSz);

            totalSmpSz = loc_get_total_smp_sz(sc);

            printf("[...] newTotalSmpSz=%d\n", totalSmpSz);
            newNumSlices = totalSmpSz / slcSz;
            printf("[...] newNumSlices=%f int=%d\n", newNumSlices, (int32_t)(newNumSlices+0.5f));

            slcSz = totalSmpSz / 120.0;
            printf("[...] newSlcSz=%f int=%d\n", slcSz, (int32_t)(slcSz+0.5f));

            slcSz = (int32_t)(slcSz+0.5f);

            sc->cur_sta = 0.0f;
            loc_align_padded_sizes_to(sc, slcSz);

            if(loc_are_pad_sizes_greater_than(sc, sc->min_padding))
            {
               break;
            }
            else
            {
               loc_restore_orig_sizes(sc);
               extraPadding += 100;
               iter++;
            }
         }

         totalSmpSz = loc_get_total_smp_sz(sc);
         padNewNumSlices = totalSmpSz / slcSz;
         printf("[...] padNewNumSlices=%f int=%d\n", padNewNumSlices, (int32_t)(padNewNumSlices+0.5f));

         // Add pad entry
         {
            element_t *el = &sc->elements[sc->num_elements++];

            int32_t padSz = (int32_t) ((sc->num_slices - (int32_t)padNewNumSlices) * slcSz);

            el->orig_sz   = 0;
            el->cur_sz    = padSz;
            el->pad_sz    = padSz;
            el->user_data = NULL;
         }

         // Output some stats
         //  (todo) do this in the generic part of the source (?)
         {
            float32_t finalNumSlices;

            totalSmpSz = loc_get_total_smp_sz(sc);

            finalNumSlices = totalSmpSz / slcSz;

            printf("[...] finalNumSlices=%f int=%d\n", finalNumSlices, (int32_t)(finalNumSlices+0.5f));
            printf("[...] origTotalSmpSz=%d\n", origTotalSmpSz);

            printf("[...] total padding:%d ratio to unpadded orig=%f%%\n",
                   (totalSmpSz - origTotalSmpSz),
                   (((float32_t)(totalSmpSz)/origTotalSmpSz)*100.0f)
                   );
            printf("[...] ratio to padded orig=%f%%\n", (((float32_t)(totalSmpSz)/origPadTotalSmpSz)*100.0f));

            printf("[...] avg slice padding=%f\n", loc_calc_average_slice_padding(sc));

            printf("[...] totalSmpSz=%d  /120=%f\n", totalSmpSz, ((float32_t)(totalSmpSz)/120.0f));

            printf("[...] (%d bytes)\n", (totalSmpSz*2));

            printf("[dbg] num iterations=%d\n", iter);
         }

         sc->b_output_valid = SC_TRUE;
      }
   }
}

static uint32_t loc_query_num_elements(samplechain_t _sc) {
   uint32_t ret = 0;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      ret = sc->num_elements;
   }

   return ret;
}

static size_t loc_query_total_size(samplechain_t _sc) {
   size_t ret = 0;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(sc->b_output_valid)
      {
         uint32_t elementIdx;

         for(elementIdx = 0; elementIdx < sc->num_elements; elementIdx++)
         {
            ret += (size_t) (sc->elements[elementIdx].cur_sz);
         }
      }
   }

   return ret;
}

static size_t loc_query_element_offset(samplechain_t _sc, uint32_t _elementIdx) {
   size_t ret = 0;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(sc->b_output_valid)
      {
         if(_elementIdx < sc->num_elements)
         {
            uint32_t elementIdx;
            
            for(elementIdx = 0; elementIdx < _elementIdx; elementIdx++)
            {
               ret += (size_t) (sc->elements[elementIdx].cur_sz);
            }
         }
      }
   }

   return ret;
}

static size_t loc_query_element_total_size(samplechain_t _sc, uint32_t _elementIdx) {
   size_t ret = 0;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(sc->b_output_valid)
      {
         if(_elementIdx < sc->num_elements)
         {
            ret = (size_t) (sc->elements[_elementIdx].cur_sz);
         }
      }
   }

   return ret;
}

static size_t loc_query_element_original_size(samplechain_t _sc, uint32_t _elementIdx) {
   size_t ret = 0;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(_elementIdx < sc->num_elements)
      {
         if(_elementIdx < sc->num_elements)
         {
            ret = (size_t) (sc->elements[_elementIdx].orig_sz);
         }
      }
   }

   return ret;
}

static void *loc_query_element_user_data(samplechain_t _sc, uint32_t _elementIdx) {
   void *ret = NULL;
   sc_t *sc = (sc_t*)_sc;

   if(NULL != sc)
   {
      if(_elementIdx < sc->num_elements)
      {
         if(_elementIdx < sc->num_elements)
         {
            ret = sc->elements[_elementIdx].user_data;
         }
      }
   }

   return ret;
}

static void loc_exit(samplechain_t *_sc) {

   if(NULL != _sc)
   {
      sc_t *sc = (sc_t*)*_sc;

      if(NULL != sc)
      {
         free(sc);
         *_sc = NULL;
      }
   }
}

void bsp_varichain_select(samplechain_algorithm_t *_algorithm) {

   _algorithm->query_algorithm_name        = &loc_query_algorithm_name;
   _algorithm->init                        = &loc_init;
   _algorithm->set_parameter_i             = &loc_set_parameter_i;
   _algorithm->set_parameter_f             = &loc_set_parameter_f;
   _algorithm->add                         = &loc_add;
   _algorithm->calc                        = &loc_calc;
   _algorithm->query_num_elements          = &loc_query_num_elements;
   _algorithm->query_total_size            = &loc_query_total_size;
   _algorithm->query_element_offset        = &loc_query_element_offset;
   _algorithm->query_element_total_size    = &loc_query_element_total_size;
   _algorithm->query_element_original_size = &loc_query_element_original_size;
   _algorithm->query_element_user_data     = &loc_query_element_user_data;
   _algorithm->exit                        = &loc_exit;
}
