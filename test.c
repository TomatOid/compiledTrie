#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

int main(int argc, char **argv)
{
    char buf[50];
    for (;;)
    {
        int n = 0;
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, stdin);
        for (char *c = buf; *c; c++) { if (*c == '\n') *c = '\0'; }
        if ((n = generatedDictionaryCompare(buf)))
        {
            printf("That is the #%d most common word\n", n);
        }
        else
        {
            printf("That is not a common word\n");
        }
    }
}
