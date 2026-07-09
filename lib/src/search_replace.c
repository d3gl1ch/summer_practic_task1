/*
search_replace.c - реализация поиска и замены байтовых последовательностей в файле.

Коликов Глеб Александрович
Группа МК-101
 */

#include "search_replace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Наивный поиск подстроки в буфере. Возвращает индекс начала первого вхождения или -1, если не найдено.
static int find_pattern(const uint8_t *buf, size_t buf_len,
                        const uint8_t *pattern, size_t pattern_len,
                        size_t start_from) {
    // Если паттерн длиннее буфера или мы уже вышли за пределы, искать нечего
    if (pattern_len == 0 || buf_len < pattern_len || start_from >= buf_len) {
        return -1;
    }

    // Внешний цикл идет по возможным позициям начала паттерна в буфере
    for (size_t i = start_from; i <= buf_len - pattern_len; i++) {
        size_t j;
        // Внутренний цикл сравнивает байты паттерна с байтами буфера
        for (j = 0; j < pattern_len; j++) {
            if (buf[i + j] != pattern[j]) {
                break; // Если байты не совпали, прерываем внутренний цикл
            }
        }
        // Если внутренний цикл дошел до конца, значит все байты совпали
        if (j == pattern_len) {
            return i;
        }
    }
    return -1;
}

int search_and_replace(const char *input_path, const char *output_path,
                       const uint8_t *pattern, size_t pattern_len,
                       const uint8_t *replacement, size_t replacement_len,
                       size_t block_size) {
    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        fprintf(stderr, "Ошибка: не удалось открыть входной файл '%s'.\n", input_path);
        return -1;
    }

    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        fprintf(stderr, "Ошибка: не удалось открыть выходной файл '%s'.\n", output_path);
        fclose(in_file);
        return -1;
    }

    uint8_t *buffer = malloc(block_size);
    if (!buffer) {
        fprintf(stderr, "Ошибка: не удалось выделить память для буфера.\n");
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, block_size, in_file)) > 0) {
        // Пока просто ищем паттерн в только что прочитанном блоке
        int pos = find_pattern(buffer, bytes_read, pattern, pattern_len, 0);
        
        if (pos != -1) {
            printf("Найдено вхождение по индексу %d\n", pos);
        }

        size_t bytes_written = fwrite(buffer, 1, bytes_read, out_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Ошибка: не удалось записать блок в выходной файл.\n");
            free(buffer);
            fclose(in_file);
            fclose(out_file);
            return -1;
        }
    }

    if (ferror(in_file)) {
        fprintf(stderr, "Ошибка: произошла ошибка при чтении входного файла.\n");
        free(buffer);
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    (void)replacement;
    (void)replacement_len;

    free(buffer);
    fclose(in_file);
    fclose(out_file);

    return 0;
}