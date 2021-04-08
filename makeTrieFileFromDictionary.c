#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *dictionary, *output_file;
    char *line = NULL;
    size_t line_length = 0;
    ssize_t read;
    size_t line_number = 0;

    dictionary = fopen("words.txt", "r");
    output_file = fopen("huge.trie", "w");
    
    while ((read = getline(&line, &line_length, dictionary)) != -1)
    {
        line_number++;
        line[read - 1] = '\0';
        fprintf(output_file, "\"%s\"\n{return %zu;\n}\n", line, line_number);
    }
    fclose(dictionary);
    fclose(output_file);
}
