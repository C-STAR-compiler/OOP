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

#define TRY _ex_setup(); if (!setjmp(_ex_jump)) {
#define CATCH(EXCEPTION_TYPE) } else if (_exception && castable(TYPE(EXCEPTION_TYPE), gettype(_exception)) && (_ex_caught = 1)) {
#define END_TRY } tfree(_ex_teardown());

#define THROW(EXCEPTION) { _exception = (Exception*)EXCEPTION; _exception->filename = __FILE__; _exception->line = __LINE__; throw(_exception); }

#define CAST(TYPE, OBJECT) (TYPE)cast(TYPEOF(TYPE), OBJECT)

OBJECT (const char *message, ...) INHERIT (char*)
  const char *filename;
  int         line;
  long        code;
END_OBJECT("An unknown error occured!");

PUBLIC extern Exception *_exception;
PUBLIC extern Exception  _ex_plchold;
PUBLIC extern jmp_buf    _ex_jump;
PUBLIC extern int        _ex_caught;

PUBLIC void       _ex_setup();
PUBLIC Exception *_ex_teardown();

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