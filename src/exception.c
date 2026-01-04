#include <exception.h>

Exception *_exception   = NULL;
Exception  _ex_plchold  = {};
jmp_buf    _ex_jump     = {};
int        _ex_caught   = 0;
int        _handler_set = 0;

void _ex_set_exception(int signal)
{
  if (!_exception)
  {
    switch (signal)
    {
    case SIGSEGV:
      _exception = (Exception*) NEW (SegmentationFaultException) ();
      break;
    case SIGFPE:
      _exception = (Exception*) NEW (ArithmeticException) ();
      break;
    }
  }
}

void _ex_default_handler(int signal)
{
  int code;

  _ex_set_exception(signal);

  code = _exception->code;

  if (_exception->filename)
  {
    fprintf(stderr, "%s(%d): ", _exception->filename, _exception->line);
  }

  fprintf(stderr, "%s\n", _exception->base);

  DELETE(_exception);

  exit(code);
}

void _ex_trycatch_handler(int signal)
{
  _ex_set_exception(signal);
  longjmp(_ex_jump, _exception->code);
}

void _ex_setup()
{
  if (!_handler_set)
  {
    signal(SIGUSR1, _ex_trycatch_handler);
    signal(SIGSEGV, _ex_trycatch_handler);
    signal(SIGFPE,  _ex_trycatch_handler);
  }

  _handler_set = 1;
}

void _ex_default()
{
  signal(SIGUSR1, _ex_default_handler);
  signal(SIGSEGV, _ex_default_handler);
  signal(SIGFPE,  _ex_default_handler);
}

Exception *_ex_teardown()
{
  Exception *to_be_freed = NULL;

  _ex_default();
  _handler_set = 0;

  if (_ex_caught) {
    const Type *type = gettype(_exception);
    
    if (type && type->destruct) {
      type->destruct(_exception);
    }

    to_be_freed = _exception;

    _exception = NULL;
    _ex_caught = 0;
  }

  if (_exception) {
    _ex_default_handler(SIGUSR1);
  }

  return to_be_freed;
}

void throw(Exception *exception)
{
  if (!_handler_set) {
    _ex_default();
  }

  _exception = exception;

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