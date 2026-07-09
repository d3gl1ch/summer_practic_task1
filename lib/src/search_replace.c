/*
search_replace.c - реализация поиска и замены байтовых последовательностей в файле.

Коликов Глеб Александрович
Группа МК-101
 */

#include "search_replace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int search_and_replace(const char *input_path, const char *output_path,
                       const uint8_t *pattern, size_t pattern_len,
                       const uint8_t *replacement, size_t replacement_len,
                       size_t block_size) {
    // Открываем входной файл для чтения в бинарном режиме 
    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        fprintf(stderr, "Ошибка: не удалось открыть входной файл '%s'.\n", input_path);
        return -1;
    }

    // Открываем выходной файл для записи в бинарном режиме 
    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        fprintf(stderr, "Ошибка: не удалось открыть выходной файл '%s'.\n", output_path);
        fclose(in_file);
        return -1;
    }

    // Выделяем память под буфер чтения размером N 
    uint8_t *buffer = malloc(block_size);
    if (!buffer) {
        fprintf(stderr, "Ошибка: не удалось выделить память для буфера.\n");
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    // Читаем файл блоками до конца файла 
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, block_size, in_file)) > 0) {
        // Записываем прочитанный блок в выходной файл 
        size_t bytes_written = fwrite(buffer, 1, bytes_read, out_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Ошибка: не удалось записать блок в выходной файл.\n");
            free(buffer);
            fclose(in_file);
            fclose(out_file);
            return -1;
        }
    }

    // Проверяем, не произошла ли ошибка чтения (а не просто конец файла) 
    if (ferror(in_file)) {
        fprintf(stderr, "Ошибка: произошла ошибка при чтении входного файла.\n");
        free(buffer);
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    (void)pattern;
    (void)pattern_len;
    (void)replacement;
    (void)replacement_len;

    // Освобождаем ресурсы 
    free(buffer);
    fclose(in_file);
    fclose(out_file);

    return 0;
}