#ifndef TYPES_H
#define TYPES_H

// C
#include <stdlib.h>
#include <string.h>

// CUT
#include <osal.h>
#include <diagnostic.h>
#include <macro.h>

#include "oop.export.h"

#define TYPEOF(TYPE) IFNULL(findtype(#TYPE), (sizeof(TYPE) < sizeof(_typeof_natives) ? _typeof_natives + sizeof(TYPE) : NULL))
#define TYPE(T) &EXPAND2(_typeof_, T)

#define MAKETYPE(TYPE, CATEGORY) \
__attribute__((unused))\
static void *TYPE##_Construct(void *mem, TYPE val) {\
  memcpy(mem, &val, sizeof(TYPE));\
  return mem;\
}\
UNUSED_SECTION(reflection) \
STATIC_EXPORT const Type _typeof_##TYPE = { .name = #TYPE, .size = sizeof(TYPE), .category = CATEGORY};

#define MAKETYPE2(TYPE1, TYPE2, CATEGORY) \
typedef TYPE1 TYPE2 TYPE1##TYPE2; \
MAKETYPE(TYPE1##TYPE2, CATEGORY)

#define MAKETYPE3(TYPE1, TYPE2, TYPE3, CATEGORY) \
typedef TYPE1 TYPE2 TYPE3 TYPE1##TYPE2##TYPE3; \
MAKETYPE(TYPE1##TYPE2##TYPE3, CATEGORY)

// This emulates an enum with the long data type, (required to pad the struct)
typedef long Types;
#define TYPES_DEFAULT  0L
#define TYPES_POINTER  1L
#define TYPES_SIGNED   2L
#define TYPES_UNSIGNED 3L
#define TYPES_FLOAT    4L
#define TYPES_OBJECT   5L

typedef void *(*VirtualFunction)(void*, ...);
typedef void *(*ConstVirtualFunction)(const void*, ...);

typedef void *(*Constructor)(void*);
typedef void  (*Destructor)(void*);

typedef void *Pointer;

// This struct must be alignable on 16 both in x32 and x64 because 
// we depend on it being stacked properly for ve table iteration
typedef struct _virtual_entry {
  const char *method;
  const void *fptr;
} VirtualEntry;

// This struct must also be alignable on 16
typedef struct _type {
  const char          *name;
  const size_t         size;
  const char         **basename;
  const Constructor    construct;
  const Destructor     destruct;
  const VirtualEntry  *ve_start;
  const VirtualEntry  *ve_stop;
  const Types          category;
} Type;

DECLARE_SECTION(reflection, Type)

// Pointer
MAKETYPE(Pointer, TYPES_POINTER);

// Floats
MAKETYPE(float,      TYPES_FLOAT);
MAKETYPE(double,     TYPES_FLOAT);

#ifndef WIN
MAKETYPE(__float128, TYPES_FLOAT);
#endif

// Signed integers
MAKETYPE (char,       TYPES_SIGNED);
MAKETYPE (short,      TYPES_SIGNED);
MAKETYPE (int,        TYPES_SIGNED);
MAKETYPE (long,       TYPES_SIGNED);
MAKETYPE2(long, int,  TYPES_SIGNED);

#ifndef WIN
MAKETYPE2(long, long, TYPES_SIGNED);
#endif

// Unsigned integers
MAKETYPE2(unsigned, char,       TYPES_UNSIGNED);
MAKETYPE2(unsigned, short,      TYPES_UNSIGNED);
MAKETYPE2(unsigned, int,        TYPES_UNSIGNED);
MAKETYPE2(unsigned, long,       TYPES_UNSIGNED);
MAKETYPE3(unsigned, long, int,  TYPES_UNSIGNED);

#ifndef WIN
MAKETYPE3(unsigned, long, long, TYPES_UNSIGNED);
#endif

// Fallback types if normal ones are not found
UNUSED_SECTION(reflection) STATIC_EXPORT const Type _typeof_natives[17] = {
  { .name = "void",  .size = 0,  .category = TYPES_DEFAULT },
  { .name = "byte",  .size = 1,  .category = TYPES_DEFAULT },
  { .name = "word",  .size = 2,  .category = TYPES_DEFAULT },
  { .name = "?3",    .size = 3,  .category = TYPES_DEFAULT },
  { .name = "dword", .size = 4,  .category = TYPES_DEFAULT },
  { .name = "?5",    .size = 5,  .category = TYPES_DEFAULT },
  { .name = "?6",    .size = 6,  .category = TYPES_DEFAULT },
  { .name = "?7",    .size = 7,  .category = TYPES_DEFAULT },
  { .name = "quad",  .size = 8,  .category = TYPES_DEFAULT },
  { .name = "?8",    .size = 9,  .category = TYPES_DEFAULT },
  { .name = "?10",   .size = 10, .category = TYPES_DEFAULT },
  { .name = "?11",   .size = 11, .category = TYPES_DEFAULT },
  { .name = "?12",   .size = 12, .category = TYPES_DEFAULT },
  { .name = "?13",   .size = 13, .category = TYPES_DEFAULT },
  { .name = "?14",   .size = 14, .category = TYPES_DEFAULT },
  { .name = "?15",   .size = 15, .category = TYPES_DEFAULT },
  { .name = "octo",  .size = 16, .category = TYPES_DEFAULT },
};

OOP_EXPORT const Type *findtype(const char *typename);

// A cast to allow for arbitrary use of parameters
OOP_EXPORT VirtualFunction      virtual     (const Type *type, const char *name);
OOP_EXPORT ConstVirtualFunction constvirtual(const Type *type, const char *name);

// Typed malloc
OOP_EXPORT void       *talloc(const Type *type);

// Typed free
OOP_EXPORT void        tfree(void *object);

// Gets the type of an object
OOP_EXPORT const Type *gettype(const void *object);

// Get the base of the specified type
OOP_EXPORT const Type *getbase(const Type *type);

// Gets the typename of an object
OOP_EXPORT const char *typename(const void *object);

// True if the type represents an object
OOP_EXPORT int         isobject(const Type *type);

// Compares the two types
OOP_EXPORT int         sametype(const Type *a, const Type *b);

// Search the inheritance tree for the type
OOP_EXPORT int         castable(const Type *base, const Type *derived);

#endif