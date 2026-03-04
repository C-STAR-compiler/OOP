#ifndef OOP_H
#define OOP_H

// C
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

// CUT
#include <diagnostic.h>
#include <variadic.h>
#include <types.h>
#include "macro.h"
#include "oop.diagnostic.h"

#ifdef WIN
#undef CONST
#undef DELETE
#endif

#define __VIRTUAL_TABLE_NAME   EXPAND3(virtual, _, TYPENAME)
#define __VIRTUAL_TABLE_START &EXPAND2(__start_virtual_, TYPENAME) + 1
#define __VIRTUAL_TABLE_STOP  &EXPAND2(__stop_virtual_, TYPENAME)

// The object declaration macro
#define OBJECT(...) \
typedef struct EXPAND2(_, TYPENAME) EXPAND(TYPENAME); \
PUBLIC EXPAND(TYPENAME) *_(Construct)(__VA_ARGS__); \
PUBLIC void _(Destruct)();

// Inherit from an object
#define INHERIT(BASE_TYPENAME) \
STATIC_EXPORT const char *EXPAND2(_baseof_, TYPENAME) = #BASE_TYPENAME; \
struct EXPAND2(_, TYPENAME) { \
BASE_TYPENAME base;

#define __ }
// For the linter only
#undef __

#define NOBASE \
STATIC_EXPORT const char *EXPAND2(_baseof_, TYPENAME) = NULL; \
struct EXPAND2(_, TYPENAME) {

// This is to standardize inheriting and non-inheriting objects' definitions
// (the brackets are hidden in the macros)
#define END_OBJECT(...) }; \
DECLARE_SECTION(__VIRTUAL_TABLE_NAME, VirtualEntry) \
static void *_(Default)() { \
  return EXPAND2(TYPENAME, _Construct)(this __VA_COMMA__(__VA_ARGS__)); \
}; \
UNUSED_SECTION(reflection) STATIC_EXPORT const Type EXPAND2(_typeof_, TYPENAME) = { \
  .name      = STRINGIZE(TYPENAME), \
  .size      = sizeof(EXPAND(TYPENAME)), \
  .basename  = &EXPAND2(_baseof_, TYPENAME), \
  .construct = (void*)EXPAND2(TYPENAME, _Default), \
  .destruct  = (void*)EXPAND2(TYPENAME, _Destruct), \
  .ve_start  = __VIRTUAL_TABLE_START, \
  .ve_stop   = __VIRTUAL_TABLE_STOP, \
  .category  = TYPES_OBJECT\
}

// Underscore to imply member method
#define _(METHOD_NAME) EXPAND3(TYPENAME, _, METHOD_NAME)(EXPAND(TYPENAME) *this __CONT__
#define __CONT__(...) __VA_COMMA__(__VA_ARGS__))

// Underscore to imply member method
#define CONST(METHOD_NAME) EXPAND3(TYPENAME, _, METHOD_NAME)(const EXPAND(TYPENAME) *this __CONST_CONT__
#define __CONST_CONT__(...) __VA_COMMA__(__VA_ARGS__))

// Replace underscore with static for static method
#define STATIC(FUNCTION_NAME) EXPAND3(TYPENAME, _, FUNCTION_NAME)

// Virtualization following this principle: https://stackoverflow.com/questions/3633896/append-items-to-an-array-with-a-macro-in-c

// This macro allows to add the current method in the virtual table of the current object
#define VIRTUAL(METHOD_NAME) ; USED_SECTION(__VIRTUAL_TABLE_NAME) STATIC_EXPORT const VirtualEntry EXPAND2(_ve_, __COUNTER__) = { \
.method = #METHOD_NAME, \
.fptr   = EXPAND3(TYPENAME, _, METHOD_NAME) \
}

// This macro allows to add any method to the virtual table of the current object
#define FOREIGN_VIRTUAL(METHOD_NAME, METHOD) USED_SECTION(__VIRTUAL_TABLE_NAME) STATIC_EXPORT const VirtualEntry EXPAND2(_fe_, __COUNTER__) = { \
.method = #METHOD_NAME, \
.fptr   = METHOD \
}

// Creates a new instance of the object
#define NEW(TYPENAME) TYPENAME ## _Construct((TYPENAME*)talloc(&EXPAND2(_typeof_, TYPENAME)) __NEW_CONT__
#define __NEW_CONT__(...) __VA_COMMA__(__VA_ARGS__))
 
// Call a virtual method on the object
#define CALL(OBJECT, METHOD_NAME) virtual(gettype(OBJECT), #METHOD_NAME)(OBJECT __CALL_CONT__
#define __CALL_CONT__(...) __VA_COMMA__(__VA_ARGS__))

#define DELETE(OBJECT) \
if (OBJECT) { \
  const Type *type = gettype(OBJECT); \
  if (type->destruct) type->destruct(OBJECT); \
  tfree(OBJECT); \
  OBJECT = NULL; \
}

#define BASE_0 &this->base
#define BASE_1 BASE_0.base
#define BASE_2 BASE_1.base
#define BASE_3 BASE_2.base
#define BASE_4 BASE_3.base
#define BASE_5 BASE_4.base
#define BASE_6 BASE_5.base
#define BASE_7 BASE_6.base
#define BASE_8 BASE_7.base
#define BASE_9 BASE_8.base
#define BASE(N) (EXPAND2(BASE_, N))

#endif
