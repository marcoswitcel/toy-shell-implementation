#ifndef SHELL_APPLICATION_H
#define SHELL_APPLICATION_H

#include <stdio.h>
#include <string.h>

typedef struct Shell_Application {
    FILE *stdin;
    FILE *stdout;
    FILE *stderr;
} Shell_Application;

static inline void write_string_to_stream(const char *cstring, FILE *stream)
{
    fwrite(cstring, 1, strlen(cstring), stream);
}

void shell_application_write(const Shell_Application *application, const char *cstring)
{
    write_string_to_stream(cstring, application->stdout);
}

void shell_application_write_error(const Shell_Application *application, const char *cstring)
{
    write_string_to_stream(cstring, application->stderr);
}

#endif // SHELL_APPLICATION_H
