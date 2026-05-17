#ifndef EXCEPTION_H
#define EXCEPTION_H

// C
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

// CUT
#include <osal.h>
#include <diagnostic.h>
#include <oop.h>
#include <oop.diagnostic.h>

#include "oop.export.h"

#ifdef WIN
#define SIGUSR1 SIGABRT
#endif

#define PUBLIC OOP_EXPORT
#define TYPENAME  Exception

#define TRY {\
  struct _exception_context _ec; \
  _ex_setup(&_ec); \
  if (!setjmp(_ec.ex_jmp)) {
#define CATCH(EXCEPTION_TYPE) } else if (_ec.ex && castable(TYPE(EXCEPTION_TYPE), gettype(_ec.ex)) && (_ec.ex_caught = 1)) {
#define END_TRY } _ex_teardown(); };

#define THROW(EXCEPTION) { Exception *e = (Exception*)EXCEPTION; e->filename = __FILE__; e->line = __LINE__; throw(e); }

#define CAST(TYPE, OBJECT) (TYPE)cast(TYPEOF(TYPE), OBJECT)

OBJECT (const char *message, ...) INHERIT (char*)
  const char *filename;
  int         line;
  long        code;
END_OBJECT("An unknown error occured!");

struct _exception_context {
  Exception *ex;
  jmp_buf    ex_jmp;
  int        ex_caught;
};

PUBLIC void _ex_setup(struct _exception_context *context);
PUBLIC void _ex_teardown();

PUBLIC void throw(Exception *exception);

// Cast the object to the specified type
PUBLIC void *cast(const Type *type, void *object);

#undef TYPENAME
#define TYPENAME SegmentationFaultException
OBJECT () INHERIT (Exception)
END_OBJECT();
#undef TYPENAME

#define TYPENAME ArithmeticException
OBJECT () INHERIT (Exception)
END_OBJECT();
#undef TYPENAME

#define TYPENAME MemoryAllocationException
OBJECT () INHERIT (Exception)
END_OBJECT();
#undef TYPENAME

#undef PUBLIC

#endif