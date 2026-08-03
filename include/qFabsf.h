#ifndef _QFABSF_H_
#define _QFABSF_H_

inline float qFabsf(float f) {
  int *_f = (int *)&f;
  *_f &= ~0x80000000;
  return f;
}

#endif // !_QFABSF_H_
