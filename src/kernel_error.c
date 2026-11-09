#include <kernel_error.h>
#include <io.h>

enum kernel_error error_handler(enum kernel_error type)
{
    console_write("IKI-OS Encountered an error with code", 37);
    console_write(&type, sizeof(enum kernel_error));
    while (1);
    // return ERR_SYSTEM_FAILED;
}