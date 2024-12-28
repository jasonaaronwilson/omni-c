#line 2 "callable.c"
/**
 * A callable is a closure, aka a function pointer and a pointer to
 * data.
 */

#ifndef _CALLABLE_H_
#define _CALLABLE_H_

#include <stdint.h>

#include "reference.h"

typedef reference_t (*callable0_code_t)(void* closure_data);
typedef reference_t (*callable1_code_t)(void* closure_data, reference_t arg1);
typedef reference_t (*callable2_code_t)(void* closure_data, reference_t arg1, reference_t arg2);
typedef reference_t (*callable3_code_t)(void* closure_data, reference_t arg1, reference_t arg2, reference_t arg3);
typedef reference_t (*callable4_code_t)(void* closure_data, reference_t arg1, reference_t arg2, reference_t arg3, reference_t arg4);
typedef reference_t (*callable5_code_t)(void* closure_data, reference_t arg1, reference_t arg2, reference_t arg3, reference_t arg4, reference_t arg5);
typedef reference_t (*callable6_code_t)(void* closure_data, reference_t arg1, reference_t arg2, reference_t arg3, reference_t arg4, reference_t arg5, reference_t arg6);

typedef struct {
  callable0_code_t code;
  void* data;
} callable0_t;

typedef struct {
  callable1_code_t code;
  void* data;
} callable1_t;

typedef struct {
  callable2_code_t code;
  void* data;
} callable2_t;

typedef struct {
  callable3_code_t code;
  void* data;
} callable3_t;

typedef struct {
  callable4_code_t code;
  void* data;
} callable4_t;

typedef struct {
  callable5_code_t code;
  void* data;
} callable5_t;

typedef struct {
  callable6_code_t code;
  void* data;
} callable6_t;

#endif /* _CALLABLE_H_ */
