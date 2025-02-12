#include "error.h"

char last_error[LAST_ERROR_SZ] = "";

const char* ps_last_error()
{
    return last_error;
}