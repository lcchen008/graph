#ifndef VINT_H
#define VINT_H

#include "sse_func.h"

// Vector int32.
class vint32 {
 public:
  __SSEi val;
  vint32(){}
  vint32(const __SSEi& _val):val(_val){}
  vint32(const int& _val):val(_mm_set1x_i(_val)){}
  operator __SSEi(){return val;}

  inline int& operator[](int i) {
    return ((int *)&val)[i];
  }

  // Binary operators.
  inline vint32& operator = (const vint32& b) {
    val = b.val;
    return *this;
  }

  inline vint32& operator = (const int& b) {
    val = _mm_set1x_i(b);
    return *this;
  }

  inline vint32& operator += (const vint32& b) {
    val = _mm_add_i(val, b.val);
    return *this;
  }

  inline vint32& operator -= (const vint32& b) {
    val = _mm_sub_i(val, b.val);		
    return *this;
  }

  inline vint32& operator *= (const vint32& b) {
    val = _mm_mul_i(val, b.val);	
    return *this;
  }

  //inline vint32& operator /= (const vint32& b) {
  //  val = _mm_div_i(val, b.val);	
  //  return *this;
  //}

  vint32& operator + (const vint32& b) {
    operator += (b);	
    return *this;
  }

  vint32& operator - (const vint32& b) {
    operator-=(b);	
    return *this;
  }

  vint32& operator * (const vint32& b) {
    operator*=(b);	
    return *this;
  }

  //vint32& operator / (const vint32& b) {
  //  operator/=(b);	
  //  return *this;
  //}

#ifdef MIC
  // Mask operations.
  inline void set(mask m, int v) {
    val = _mm_mask_mov_i(val, m, vint32(v));
  }
#endif

  void print() {
    for (int i = 0; i < 16; ++i) {
      cout << *((int*)&val + i) << ", ";
    }
    cout << endl;
    cout << "*********************************************" << endl;
  }
};

// Comparison functions.
vint32 vmin(vint32& a, vint32& b) {
  return _mm_min_i(a.val, b.val);
}

vint32 vmax(vint32& a, vint32& b) {
  return _mm_max_i(a.val, b.val);
}

#ifdef MIC

inline mask operator < (const vint32 &a, const vint32 &b) {
  return _mm_cmplt_i(a.val, b.val);
}

inline mask operator < (const vint32 &a, const int &b) {
  return _mm_cmplt_i(a.val, vint32(b).val);
}

inline mask operator < (const int &a, const vint32 &b) {
  return _mm_cmplt_i(vint32(a).val, b.val);
}

inline mask operator > (const vint32 &a, const vint32 &b) {
  return _mm_cmpgt_i(a.val, b.val);
}

inline mask operator > (const vint32 &a, const int &b) {
  return _mm_cmpgt_i(a.val, vint32(b).val);
}

inline mask operator > (const int &a, const vint32 &b) {
  return _mm_cmpgt_i(vint32(a).val, b.val);
}

inline mask operator == (const vint32 &a, const vint32 &b) {
  return _mm_cmpeq_i(a.val, b.val);
}

inline mask operator == (const vint32 &a, const int &b) {
  return _mm_cmpeq_i(a.val, vint32(b).val);
}

inline mask operator == (const int &a, const vint32 &b) {
  return _mm_cmpeq_i(vint32(a).val, b.val);
}

#endif

#endif
