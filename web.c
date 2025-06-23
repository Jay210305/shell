#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_web(const char *url)
{
    if (url == NULL)
    {
        fprintf(stderr, "Uso: openweb <URL>\n");
        return;
    }

    char command[512];
    snprintf(command, sizeof(command), "xdg-open \"%s\" > /dev/null 2>&1 &", url);
    system(command);
}
