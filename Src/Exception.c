#include "Exception.h"
#include "CException.h"
#include <stdarg.h>
#include <malloc.h>
#include <stdio.h>

void throwError(int errorCode, char *format, ...)
{
  va_list valist;
  int neededSize;
  char *buffer;
  Exception *ex;
  
  va_start(valist, format);
  ex = (Exception *)malloc(sizeof(Exception));
  neededSize = vsnprintf(NULL, 0, format, valist) + 1;
  buffer = malloc(neededSize);
  vsnprintf(buffer, neededSize, format, valist);
  va_end(valist);
  
  ex->errorCode = errorCode;
  ex->errorMsg = buffer;
  Throw(ex);
}

void freeError(Exception *ex)
{
  if(ex)
    if(ex->errorMsg)
      free(ex->errorMsg);
    free(ex);  
}
