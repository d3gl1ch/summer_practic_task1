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
    if (pattern_len == 0 || buf_len < pattern_len || start_from >= buf_len) {
        return -1;
    }

    size_t limit = buf_len - pattern_len;
    for (size_t i = start_from; i <= limit; i++) {
        size_t j;
        for (j = 0; j < pattern_len; j++) {
            if (buf[i + j] != pattern[j]) {
                break;
            }
        }
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

    // Если паттерн пустой, просто копируем файл без изменений
    if (pattern_len == 0) {
        uint8_t *copy_buf = malloc(block_size);
        if (!copy_buf) {
            fclose(in_file);
            fclose(out_file);
            return -1;
        }
        size_t bytes_read;
        while ((bytes_read = fread(copy_buf, 1, block_size, in_file)) > 0) {
            fwrite(copy_buf, 1, bytes_read, out_file);
        }
        free(copy_buf);
        fclose(in_file);
        fclose(out_file);
        return 0;
    }

    // Выделяем буфер размером 2*N для чтения без сдвига памяти
    size_t buf_size = 2 * block_size;
    uint8_t *buffer = malloc(buf_size);
    if (!buffer) {
        fprintf(stderr, "Ошибка: не удалось выделить память для буфера.\n");
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    size_t read_offset = 0;
    size_t search_start = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer + read_offset, 1, block_size, in_file)) > 0) {
        size_t current_block_end = read_offset + bytes_read;
        
        size_t search_limit = (current_block_end > pattern_len) ? current_block_end - pattern_len : 0;
        int pos;

        while ((pos = find_pattern(buffer, current_block_end, pattern, pattern_len, search_start)) != -1 && pos <= search_limit) {
            if (pos > search_start) {
                fwrite(buffer + search_start, 1, pos - search_start, out_file);
            }
            fwrite(replacement, 1, replacement_len, out_file);
            search_start = pos + pattern_len;
        }

        // Записываем только "безопасную" часть, оставляя хвост для проверки стыка
        size_t safe_end = (current_block_end > pattern_len - 1) ? current_block_end - (pattern_len - 1) : read_offset;
        
        if (safe_end > search_start) {
            fwrite(buffer + search_start, 1, safe_end - search_start, out_file);
            search_start = safe_end;
        }

        // Если файл закончился, дописываем оставшийся хвост
        if (bytes_read < block_size) {
            if (current_block_end > search_start) {
                int tail_pos = find_pattern(buffer, current_block_end, pattern, pattern_len, search_start);
                if (tail_pos != -1) {
                    fwrite(buffer + search_start, 1, tail_pos - search_start, out_file);
                    fwrite(replacement, 1, replacement_len, out_file);
                    search_start = tail_pos + pattern_len;
                }
                if (current_block_end > search_start) {
                    fwrite(buffer + search_start, 1, current_block_end - search_start, out_file);
                }
            }
            break;
        }

        read_offset = (read_offset == 0) ? block_size : 0;
        search_start = read_offset; 
    }

    if (ferror(in_file)) {
        fprintf(stderr, "Ошибка: произошла ошибка при чтении входного файла.\n");
        free(buffer);
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    free(buffer);
    fclose(in_file);
    fclose(out_file);

    return 0;
}