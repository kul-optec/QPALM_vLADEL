#include "qpalm.h"
#include "qpalm_qps.h"
#include "qps_conversion.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_LEN 9 

void c_strcpy_limit(char dest[], const char source[], size_t len) {
    size_t i;
    for(i = 0; i < len && (dest[i] = source[i]) != '\0'; i++);
    dest[len-1] = '\0';
}

void remove_spaces (char* restrict str_trimmed, const char* restrict str_untrimmed)
{
  while (*str_untrimmed != '\0')
  {
    if(!isspace(*str_untrimmed))
    {
      *str_trimmed = *str_untrimmed;
      str_trimmed++;
    }
    str_untrimmed++;
  }
  *str_trimmed = '\0';
}
void add_spaces(char *line, size_t nb) {
  size_t k;
  for (k = 0; k < nb; k++)
    strcat(line, " ");
}

char* convert_qps_to_new_format(const char* filename) {
    FILE* fp_in, *fp_out;
    char* new_filename = qpalm_malloc(strlen(filename)+5+1);
    new_filename[0] = '\0';
    strcat(new_filename, filename);
    new_filename[strlen(filename)-4] = '\0';
    strcat(new_filename, "_copy");
    strcat(new_filename, ".qps");

    fp_in = fopen(filename, "r");
    if(fp_in == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }
    fp_out = fopen(new_filename, "w");
    if(fp_out == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }

    char line[100], command[20], buf1[9], buf1_copy[9], buf2[9], buf2_copy[9], buf3[9], buf3_copy[9], temp1[BUFFER_LEN+1], temp2[BUFFER_LEN+1];
    size_t len;

    fgets(line, 100, fp_in);
    fputs(line, fp_out);
    char next_char;
    next_char = (char)fgetc(fp_in);

    while (get_next_command(command, next_char, fp_in)) {
      fputs(command, fp_out);
      fputs("\n", fp_out);
      next_char = (char)fgetc(fp_in);
      if (!strcmp(command, "ROWS")) {
        while (next_char == ' ') {
            fgets(line, 100, fp_in);
            c_strcpy_limit(buf1, &line[3], BUFFER_LEN);
            remove_spaces(buf1_copy, buf1);
            line[3] = '\0';
            strcat(line, buf1_copy);
            fputs(" ", fp_out); /*compensate for next_char*/
            fputs(line, fp_out);
            fputs("\n", fp_out);
            next_char = (char)fgetc(fp_in);
        }
      } else if (!strcmp(command, "COLUMNS") || !strcmp(command, "RHS") || !strcmp(command, "RANGES")) {
          while (next_char == ' ') {
            fgets(line, 100, fp_in);
            len = strlen(line);
            c_strcpy_limit(buf1, &line[3], BUFFER_LEN);
            remove_spaces(buf1_copy, buf1);
            c_strcpy_limit(buf2, &line[13], BUFFER_LEN);
            remove_spaces(buf2_copy, buf2);
            c_strcpy_limit(temp1, &line[27], BUFFER_LEN+1);
            if (len > 40) {
              c_strcpy_limit(buf3, &line[38], BUFFER_LEN);
              remove_spaces(buf3_copy, buf3);
              c_strcpy_limit(temp2, &line[51], BUFFER_LEN+1);
            }

            line[3] = '\0';
            strcat(line, buf1_copy);
            add_spaces(line, BUFFER_LEN - strlen(buf1_copy) + 1);
            strcat(line, buf2_copy);
            add_spaces(line, 22 - strlen(temp1) - strlen(buf2_copy));
            strcat(line, temp1);
            if (len > 40) {
              add_spaces(line, 3);
              strcat(line, buf3_copy);
              add_spaces(line, 22 - strlen(temp2) - strlen(buf3_copy));
              strcat(line, temp2);
            }
            fputs(" ", fp_out); /*compensate for next_char*/
            fputs(line, fp_out);
            fputs("\n", fp_out);
            next_char = (char)fgetc(fp_in);
        }
      } else if (!strcmp(command, "BOUNDS") || !strcmp(command, "QUADOBJ")) {
        while (next_char == ' ') {
            fgets(line, 100, fp_in);
            len = strlen(line);
            c_strcpy_limit(buf1, &line[3], BUFFER_LEN);
            remove_spaces(buf1_copy, buf1);
            c_strcpy_limit(buf2, &line[13], BUFFER_LEN);
            remove_spaces(buf2_copy, buf2);
            c_strcpy_limit(temp1, &line[27], BUFFER_LEN+1);

            line[3] = '\0';
            strcat(line, buf1_copy);
            add_spaces(line, 10 - strlen(buf1_copy));
            strcat(line, buf2_copy);
            add_spaces(line, 22 - strlen(buf2_copy) - strlen(temp1));
            // strcat(line, "    ");
            strcat(line, temp1);
            fputs(" ", fp_out); /*compensate for next_char*/
            fputs(line, fp_out);
            fputs("\n", fp_out);
            next_char = (char)fgetc(fp_in);
        }
      } else {
        printf("Unrecognized command: %s\n", command); 
        break;
      }
    }
    fputs("ENDATA\n", fp_out);

    fclose(fp_in);
    fclose(fp_out);
    return new_filename;

}
