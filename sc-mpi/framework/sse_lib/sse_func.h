#ifndef SSE_FUNC_H_
#define SSE_FUNC_H_

#include "../conf.h"

#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y) 

#define WIDTH 32

/****************************for MIC ARCHITECTURE********************************/
#ifdef MIC
#define __SSE  __m512  //float
#define __SSEi __m512i //int
#define mask __mmask16 //mask

/*******************************Float Instructions**********************/
/*******Arithmetic Operations***/
#define _mm_add_ps  _mm512_add_ps
#define _mm_sub_ps  _mm512_sub_ps
#define _mm_mul_ps  _mm512_mul_ps
#define _mm_sqrt_ps _mm512_rsqrt23_ps
#define _mm_div_ps  _mm512_div_ps
#define _mm_min_ps  _mm512_min_ps
#define _mm_max_ps  _mm512_max_ps

#define _mm_mask_add_ps  _mm512_mask_add_ps
#define _mm_mask_sub_ps  _mm512_mask_sub_ps
#define _mm_mask_mul_ps  _mm512_mask_mul_ps
#define _mm_mask_sqrt_ps  _mm512_mask_rsqrt23_ps
#define _mm_mask_div_ps  _mm512_mask_div_ps
#define _mm_mask_min_ps  _mm512_mask_min_ps
#define _mm_mask_max_ps  _mm512_mask_max_ps


/*********SET/LOAD/STORE/MOV Operations*******/
#define _mm_set1x_ps _mm512_set1_ps
#define _mm_set_ps  _mm512_set_ps
#define _mm_store_ps _mm512_store_ps
#define _mm_load_ps _mm512_load_ps

// LOAD/STORE operation with mask.
#define _mm_mask_store_ps _mm512_mask_store_ps
#define _mm_mask_load_ps _mm512_mask_load_ps
#define _mm_mask_mov_ps _mm512_mask_mov_ps
#define _mm_mask_gather_ps _mm512_mask_i32gather_ps
#define _mm_mask_scatter_ps _mm512_mask_i32scatter_ps

/************Logic Operations*************/
#define _mm_cmpeq_ps _mm512_cmpeq_ps_mask
#define _mm_cmplt_ps _mm512_cmplt_ps_mask
#define _mm_cmple_ps _mm512_cmple_ps_mask
#define _mm_cmpneq_ps _mm512_cmpneq_ps_mask
#define _mm_cmpnlt_ps _mm512_cmpnlt_ps_mask
#define _mm_cmpnle_ps _mm512_cmpnle_ps_mask
#define _mm_cmpord_ps _mm512_cmpord_ps_mask
#define _mm_cmpunord_ps _mm512_cmpunord_ps_mask

/*********************Int32 Instructions***************************/
/*********SET/LOAD/STORE/MOV Operations*******/
#define _mm_set1x_i _mm512_set1_epi32
#define _mm_store_i TOKENPASTE(_mm512_store_epi,WIDTH)
#define _mm_load_i TOKENPASTE(_mm512_load_epi,WIDTH)

// LOAD/STORE with mask.
#define _mm_mask_store_i TOKENPASTE(_mm512_mask_store_epi,WIDTH)
#define _mm_mask_load_i TOKENPASTE(_mm512_mask_load_epi,WIDTH)
#define _mm_mask_mov_i TOKENPASTE(_mm512_mask_mov_epi,WIDTH)
#define _mm_mask_gather_i TOKENPASTE(_mm512_mask_i32gather_epi, WIDTH)
#define _mm_mask_scatter_i TOKENPASTE(_mm512_mask_i32scatter_epi, WIDTH)

/*******Arithmetic Operations***/
#define _mm_add_i _mm512_add_epi32
#define _mm_adds_i _mm512_adds_epi32
#define _mm_sub_i _mm512_sub_epi32
#define _mm_subs_i _mm512_subs_epi32
#define _mm_mul_i _mm512_mullo_epi32
#define _mm_div_i _mm512_div_epi32
#define _mm_min_i  _mm512_min_epi32
#define _mm_max_i  _mm512_max_epi32

#define _mm_mask_add_i _mm512_mask_add_epi32
#define _mm_mask_sub_i _mm512_mask_sub_epi32
#define _mm_mask_mul_i _mm512_mask_mullo_epi32
#define _mm_mask_div_i _mm512_mask_div_epi32
#define _mm_mask_min_i  _mm512_mask_min_epi32
#define _mm_mask_max_i  _mm512_mask_max_epi32

// Logic operators.
#define _mm_cmpeq_i TOKENPASTE(_mm512_cmpeq_epi, TOKENPASTE(WIDTH, _mask))
#define _mm_cmpgt_i TOKENPASTE(_mm512_cmpgt_epi, TOKENPASTE(WIDTH, _mask))
#define _mm_cmplt_i TOKENPASTE(_mm512_cmplt_epi, TOKENPASTE(WIDTH, _mask))

#elif defined AVX
/************************for AVX CPU ARCHITECTURE****************************/

#define __SSE  __m256
#define __SSEi __m256i

/*************************************Float Instructions****************************/
/*******Arithmetic Operations***/
#define _mm_add_ps  _mm256_add_ps
#define _mm_sub_ps  _mm256_sub_ps
#define _mm_mul_ps  _mm256_mul_ps
#define _mm_sqrt_ps _mm256_rsqrt23_ps
#define _mm_div_ps  _mm256_div_ps
#define _mm_min_ps  _mm256_min_ps
#define _mm_max_ps  _mm256_max_ps

#define _mm_mask_add_ps  _mm256_mask_add_ps
#define _mm_mask_sub_ps  _mm256_mask_sub_ps
#define _mm_mask_mul_ps  _mm256_mask_mul_ps
#define _mm_mask_sqrt_ps  _mm256_mask_rsqrt23_ps
#define _mm_mask_div_ps  _mm256_mask_div_ps
#define _mm_mask_min_ps  _mm256_mask_min_ps
#define _mm_mask_max_ps  _mm256_mask_max_ps

/*********SET/LOAD/STORE/MOV Operations*******/
#define _mm_set1x_ps _mm256_set1_ps
#define _mm_set_ps  _mm256_set_ps
#define _mm_store_ps _mm256_store_ps
#define _mm_load_ps _mm256_load_ps

// LOAD/STORE operation with mask.
#define _mm_mask_store_ps _mm256_mask_store_ps
#define _mm_mask_load_ps _mm256_mask_load_ps
#define _mm_mask_mov_ps _mm256_mask_mov_ps
#define _mm_mask_gather_ps _mm256_mask_i32gather_ps
#define _mm_mask_scatter_ps _mm256_mask_i32scatter_ps

/************Logic Operations*************/
#define _mm_cmpeq_ps _mm256_cmpeq_ps_mask
#define _mm_cmplt_ps _mm256_cmplt_ps_mask
#define _mm_cmple_ps _mm256_cmple_ps_mask
#define _mm_cmpneq_ps _mm256_cmpneq_ps_mask
#define _mm_cmpnlt_ps _mm256_cmpnlt_ps_mask
#define _mm_cmpnle_ps _mm256_cmpnle_ps_mask
#define _mm_cmpord_ps _mm256_cmpord_ps_mask
#define _mm_cmpunord_ps _mm256_cmpunord_ps_mask

#else

/************************for SSE CPU ARCHITECTURE****************************/
#define __SSE  __m128
#define __SSEi __m128i
#define WIDTH 32

/*************************************Integer Instructions****************************/
/************Arithmetic Intructions***********/
#define _mm_add_i TOKENPASTE(_mm_add_epi,WIDTH)
#define _mm_adds_i TOKENPASTE(_mm_adds_epi,WIDTH)
#define _mm_sub_i TOKENPASTE(_mm_sub_epi,WIDTH)
#define _mm_subs_i TOKENPASTE(_mm_subs_epi,WIDTH)
#define _mm_mul_i TOKENPASTE(_mm_mul_epi,WIDTH)
#define _mm_min_i TOKENPASTE(_mm_min_epi,WIDTH)
#define _mm_max_i TOKENPASTE(_mm_max_epi,WIDTH)

/*************SET/Load/Store Intructions**********/
#define _mm_set1x_i TOKENPASTE(_mm_set1_epi,WIDTH)
#define _mm_set_i TOKENPASTE(_mm_set_epi,WIDTH)
#define _mm_setr_i TOKENPASTE(_mm_setr_epi,WIDTH)
#define _mm_load_i TOKENPASTE(_mm_load_epi,WIDTH)
#define _mm_store_i TOKENPASTE(_mm_store_epi,WIDTH)

/************Logic Instruction**************/
#define _mm_cmpeq_i TOKENPASTE(_mm_cmpeq_epi,WIDTH)
#define _mm_cmpgt_i TOKENPASTE(_mm_cmpgt_epi,WIDTH)
#define _mm_cmplt_i TOKENPASTE(_mm_cmplt_epi,WIDTH)

/*************************************Float Instructions****************************/
/************Arithmetic Intructions***********/
#define _mm_add_ps _mm_add_ps
#define _mm_adds_ps _mm_adds_ps
#define _mm_sub_ps _mm_sub_ps
#define _mm_subs_ps _mm_subs_ps
#define _mm_mul_ps _mm_mul_ps

/*************SET/Load/Store Intructions**********/
#define _mm_set1x_ps _mm_set1_ps
#define _mm_set_ps _mm_set_ps
#define _mm_setr_ps _mm_setr_ps
#define _mm_load_ps _mm_load_ps
#define _mm_store_ps _mm_store_ps

/************Logic Instruction**************/
#define _mm_cmpeq_ps _mm_cmpeq_ps
#define _mm_cmpgt_ps _mm_cmpgt_ps
#define _mm_cmplt_ps _mm_cmplt_ps

#endif

#endif
