#include <exception.h>

#include <pthread.h>

pthread_mutex_t                      _lock         = PTHREAD_MUTEX_INITIALIZER;
__thread struct _exception_context **_context      = NULL;
__thread int                         _context_size = 0;
__thread int                         _context_cap  = 0;
__thread Exception                  *_exception    = NULL;
int                                  _handler_set  = 0;

struct _exception_context *_ex_context() {
  struct _exception_context *context = NULL;

  if (_context && _context_size > 0) {
    context = _context[_context_size - 1];
  }

  return context;
}

Exception *_ex_from_signal(int signal, Exception *actual)
{
  Exception *exception = actual;

  if (!exception)
  {
    switch (signal)
    {
    case SIGSEGV:
      exception = (Exception*) NEW (SegmentationFaultException) ();
      break;
    case SIGFPE:
      exception = (Exception*) NEW (ArithmeticException) ();
      break;
    }
  }

  return exception;
}

void _ex_handler(int signal)
{
  struct _exception_context *context = _ex_context();
  int code;

  if (context && !_exception) { // handled exception

    context->ex = _ex_from_signal(signal, context->ex);
    longjmp(context->ex_jmp, context->ex->code);

  } else { // default behaviour (crash)

    _exception = _ex_from_signal(signal, _exception);

    code = _exception->code;

    if (_exception->filename)
    {
      fprintf(stderr, "%s(%d): ", _exception->filename, _exception->line);
    }

    fprintf(stderr, "%s\n", _exception->base);

    DELETE(_exception);

    exit(code);
  }
}

void _ex_set_handler()
{
  pthread_mutex_lock(&_lock);

  if (!_handler_set)
  {
    signal(SIGUSR1, _ex_handler);
    signal(SIGSEGV, _ex_handler);
    signal(SIGFPE,  _ex_handler);
    
    _handler_set = 1;
  }

  pthread_mutex_unlock(&_lock);
}

void _ex_setup(struct _exception_context *context)
{
  _ex_set_handler();

  if (!_context) {
    _context     = malloc(4 * sizeof(struct _exception_context));
    _context_cap = 4;
  }

  if (_context_size == _context_cap) {
    _context_cap <<= 1;
    if (!(_context = realloc(_context, _context_cap))) {
      fprintf(stderr, "Could not allocate exception frame!");
      exit(-1);
    }
  }

  _context[_context_size++] = context;
}

void _ex_teardown()
{
  struct _exception_context *context = _ex_context();

  if (context->ex_caught) {
    DELETE (context->ex);
  } else {
    _exception = context->ex;
  }

  _context[--_context_size] = NULL;

  if (_exception) {
    _ex_handler(SIGUSR1);
  }

  if (_context_size == 0) {
    _context_cap = 0;
    free(_context);
  }
}

void throw(Exception *exception)
{
  struct _exception_context *context = _ex_context();

  _ex_set_handler();

  if (context) {
    context->ex = exception;
  } else {
    _exception = exception;
  }

  raise(SIGUSR1);
}

////////////////////////////////////////////////////////////////////////////////
void *cast(const Type *type, void *object)
{
  const Type *objType = gettype(object);

  if (!castable(type, objType)) {
    THROW (NEW (Exception) ("Cannot cast an object of type '%s' to '%s'", objType->name, type->name));
  }

  return object;
}

#define TYPENAME Exception

Exception *_(Construct)(const char *message, ...) {
  if (this) {
    char buffer[4096];

    va_list argptr;
    va_start(argptr, message);
    vsnprintf(buffer, sizeof(buffer), message, argptr);
    va_end(argptr);

    this->base = malloc(strlen(buffer) + 1);
    
    if (this->base) {
      strcpy(this->base, buffer);
    }
  }

  return this;
}

void _(Destruct)()
{ 
  if (this) {
    free(this->base);
    this->base = NULL;
  }
}

#undef TYPENAME
#define TYPENAME SegmentationFaultException

SegmentationFaultException *_(Construct)()
{
  return (SegmentationFaultException*)Exception_Construct(BASE(0), "Segmentation fault!");
}

void _(Destruct)()
{ 
  Exception_Destruct(BASE(0));
}

#undef TYPENAME
#define TYPENAME ArithmeticException

ArithmeticException *_(Construct)()
{
  return (ArithmeticException*)Exception_Construct(BASE(0), "Segmentation fault!");
}

void _(Destruct)()
{ 
  Exception_Destruct(BASE(0));
}

#undef TYPENAME
#define TYPENAME MemoryAllocationException

MemoryAllocationException *_(Construct)()
{
  return (MemoryAllocationException*)Exception_Construct(BASE(0), "Memory allocation failed!");
}

void _(Destruct)()
{ 
  Exception_Destruct(BASE(0));
}

#undef TYPENAME