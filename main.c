#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KeyAction
{
    char *key;
    char *action;
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage: %s infile outfile", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    int is_quoted = 0;
    int is_escaped = 0;
    size_t number_of_keys = 0;
    size_t buffer_size = 0;
    int brackets_depth = 0;
    int base_brackets_depth = 0;

    // figure out the file's size
    fseek(input_file, 0L, SEEK_END);
    size_t input_file_size = ftell(input_file) + 1;
    rewind(input_file);

    char *file_buffer = calloc(1, input_file_size);
    fread(file_buffer, 1, input_file_size, input_file);
    fclose(input_file);
    char *current_char = file_buffer;

    do
    {
        if (is_escaped)
        {
            buffer_size++;
            is_escaped = 0;
        }
        else
        {
            // if we are in curly braces
            if (brackets_depth > 0)
            {
                switch *current_char
                {
                    case '{':
                        brackets_depth++;
                        buffer_size++;
                        break;
                    case '}':
                        // say we have input ... { if (a) { doThing(); } } ... we don't want to count the two outer braces towards buffer_size
                        // this part deals with the last brace (when brackets_depth goes from one to zero)
                        if (brackets_depth--)
                        {
                            buffer_size++;
                        }
                        break;
                    default:
                        buffer_size++;
                }
            }
            else if (*current_char == '{')
            {
                brackets_depth++;
            }
            else if (*current_char == '\"')
            {
                if (!is_quoted) { number_of_keys++; }
                is_quoted = !is_quoted;
            }
            else if (is_quoted)
            {
                buffer_size++;
            }
        }
    } while (current_char++ != EOF);
    
    string_buffer
