#ifndef VFLOAT_H
#define VFLOAT_H

#include "sse_func.h"
#include <iostream>
using namespace std;

// Vector float. 
class vfloat {
	public:
		__SSE val;
		vfloat(){}
		vfloat(__SSE _val):val(_val){}
		vfloat(float _val):val(_mm_set1x_ps(_val)){}
		operator __SSE(){return val;}

		inline float& operator[](int i) {
			return ((float *)&val)[i];
		}

		// Binary operators.
		inline vfloat& operator = (const vfloat& b) {
			val = b.val;
			return *this;
		}

		inline vfloat& operator = (float& b) {
			val = _mm_set1x_ps(b);
			return *this;
		}

		inline vfloat& operator += (const vfloat& b) {
			val = _mm_add_ps(val, b.val);
			return *this;
		}

		inline vfloat& operator -= (const vfloat& b) {
			val = _mm_sub_ps(val, b.val);		
			return *this;
		}

		inline vfloat& operator *= (const vfloat& b) {
			val = _mm_mul_ps(val, b.val);	
			return *this;
		}

		inline vfloat& operator /= (const vfloat& b) {
			val = _mm_div_ps(val, b.val);	
			return *this;
		}

		vfloat& operator + (const vfloat& b) {
			operator += (b);	
			return *this;
		}

		vfloat& operator - (const vfloat& b) {
			operator-=(b);	
			return *this;
		}

		vfloat& operator * (const vfloat& b) {
			operator*=(b);	
			return *this;
		}

		vfloat& operator / (const vfloat& b) {
			operator/=(b);	
			return *this;
		}

		// Load/Store Operations.
		void load(const vfloat& src) {
			val = _mm_load_ps((float*)&(src.val));	
		}

		void store(vfloat& dst) {
			_mm_store_ps((float*)&(dst.val), val);	
		}

#ifdef MIC
    // Mask operations.
		inline void set(mask m, float v) {
			val = _mm_mask_mov_ps(val, m, vfloat(v));
		}
#endif

    void print() {
      for (int i = 0; i < 16; ++i) {
        cout << *((float*)&val + i) << ", ";
      }
      cout << "***********************************************";
      cout << endl;
    }
};

// Comparison functions.
vfloat vmin(vfloat& a, vfloat& b) {
	return _mm_min_ps(a.val, b.val);
}

vfloat vmax(vfloat& a, vfloat& b) {
	return _mm_max_ps(a.val, b.val);
}

#endif
