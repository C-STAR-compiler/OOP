#include <oop.diagnostic.h>

////////////////////////////////////////////////////////////////////////////////
void *__talloc(const Type *type, const char *filename, int line)
{
  const Type **mem = __malloc(sizeof(const Type*) + type->size, filename, line);
               
  if (mem) *mem++ = type;

  return mem;
}

////////////////////////////////////////////////////////////////////////////////
void __tfree(void *object)
{
  __free((const Type**)object - 1);
}
