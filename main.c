#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// format description: "key" { action }

typedef struct KeyAction
{
    char *key;
    char *action;
} KeyAction;

int compareKeyAction(const void *a, const void *b)
{
    return -strcmp(((KeyAction *)a)->key, ((KeyAction *)b)->key);
}

void addIndentation(FILE *out_file, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        fprintf(out_file, "\t");
    }
}

void fputs_indent(FILE *out_file, char *string, size_t n)
{
    do
    {
        addIndentation(out_file, n);
        do
        {
            fputc(*string, out_file);
        } while (*(string++) != '\n' && *string);
    } while (*string);
}

void recursivelyCompileTrie(FILE *out_file, KeyAction *array, size_t length, size_t word_offset)
{
    addIndentation(out_file, word_offset);
    fprintf(out_file, "switch (*c) {\n");
    for (size_t i = 0; i < length; )
    {
        size_t j;
        for (j = i; j < length && (array[i].key[word_offset] == array[j].key[word_offset]); j++);
        if (array[i].key[word_offset])
        {   
            addIndentation(out_file, word_offset);
            fprintf(out_file, "case \'%c\':\n", array[i].key[word_offset]);
            addIndentation(out_file, word_offset + 1);
            fprintf(out_file, "c++;\n");
            recursivelyCompileTrie(out_file, &array[i], j - i, word_offset + 1);
            i = j;
            addIndentation(out_file, word_offset);
            fprintf(out_file, "break;\n");
        }
        else
        {
            addIndentation(out_file, word_offset);
            fprintf(out_file, "case \'\\0\':\n");
            fputs_indent(out_file, array[i].action, word_offset + 1);
            addIndentation(out_file, word_offset + 1);
            fprintf(out_file, "break;\n");
            i++;
        }
    }
    fputs_indent(out_file, "default: \nreturn 0;\n}\n", word_offset);
    return;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage: %s infile outfile\n", argv[0]);
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
    file_buffer[input_file_size - 1] = EOF;
    do
    {
        // brackets have the highest precedence
        if (brackets_depth > 0)
        {
            switch (*current_char)
            {
                case '{':
                    brackets_depth++;
                    break;
                case '}':
                    brackets_depth--;
                    if (!brackets_depth)
                    {
                        *current_char = '\0';
                    }
                    break;
                default:
                    break;
            }
            buffer_size++;
        }
        else if (is_quoted)
        {
            if (is_escaped)
            {
                buffer_size++;
                is_escaped = 0;
            }
            else
            {
                switch (*current_char)
                {
                    case '\\':
                        is_escaped = 1;
                        break;
                    case '\"':
                        *current_char = '\0';
                        buffer_size++;
                        number_of_keys++;
                        is_quoted = 0;
                        break;
                    default:
                        buffer_size++;
                        break;
                }
            }
        }
        else
        {
            switch (*current_char)
            {
                case '{':
                    brackets_depth++;
                    break;
                case '\"':
                    is_quoted = 1;
                    break;
                default:
                    break;
            }
        }
    } while (*(current_char++) != EOF);
    printf("%d\n", number_of_keys);

    KeyAction *pairs = calloc(number_of_keys, sizeof(KeyAction));
    
    // rewind the current_char
    current_char = file_buffer;

    enum { TYPE_KEY, TYPE_ACTION };
    int last_type = TYPE_ACTION;
    size_t key_index = 0;

    do
    {
        switch (*current_char)
        {
            case '\"':
                current_char++;
                if (last_type == TYPE_KEY)
                {
                    printf("Two keys in a row %s\n", current_char);
                    exit(EXIT_FAILURE);
                }
                last_type = TYPE_KEY;
                pairs[key_index].key = current_char;
                current_char += strlen(current_char) + 1;
                break;
            case '{':
                current_char++;
                if (last_type == TYPE_ACTION)
                {
                    printf("An action must be preceded by a key %s\n", current_char);
                    exit(EXIT_FAILURE);
                }
                last_type = TYPE_ACTION;
                pairs[key_index].action = current_char;
                key_index++;
                current_char += strlen(current_char) + 1;
                break;
            default:
                current_char++;
                break;
        }
    } while (*current_char != EOF);
    
    qsort((void*)pairs, number_of_keys, sizeof(KeyAction), compareKeyAction);
    for (int i = 0; i < number_of_keys; i++)
    {
        printf("%s, %s\n", pairs[i].key, pairs[i].action);
    }
    fprintf(output_file, "int generated%sCompare(char* c) { \n", argv[1]);
    recursivelyCompileTrie(output_file, pairs, number_of_keys, 0);
    fprintf(output_file, "}\n");
}


