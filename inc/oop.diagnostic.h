#ifndef OOP_DIAGNOSTIC_H
#define OOP_DIAGNOSTIC_H

#include <diagnostic.h>
#include <types.h>
#include "oop.export.h"

#ifdef MEMORY_WATCH

#define talloc(TYPE) __talloc(TYPE, __FILE__, __LINE__)
#define tfree(PTR)   __tfree(PTR)

#endif

// Memory watch helpers
OOP_EXPORT void       *__talloc(const Type *type, const char *filename, int line);
OOP_EXPORT void        __tfree(void *object);

#endif
