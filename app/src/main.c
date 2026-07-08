/*
main.c - главный модуль программы. Парсинг аргументов командной строки.

Коликов Глеб Александрович
Группа МК-101
 */

#include "search_replace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 4096
#endif

// Преобразует один hex-символ в число. Возвращает -1 при ошибке.
static int hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Преобразует hex-строку в массив байт. Возвращает NULL при ошибке.
static uint8_t *hex_to_bytes(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return NULL;

    *out_len = len / 2;
    uint8_t *bytes = malloc(*out_len);
    if (!bytes) return NULL;

    for (size_t i = 0; i < *out_len; i++) {
        int high = hex_char_to_val(hex[2 * i]);
        int low = hex_char_to_val(hex[2 * i + 1]);
        if (high == -1 || low == -1) {
            free(bytes);
            return NULL;
        }
        bytes[i] = high * 16 + low;
    }
    return bytes;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Использование: %s <входной_файл> <выходной_файл> <pattern_hex> <replacement_hex>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    size_t pattern_len = 0;
    uint8_t *pattern = hex_to_bytes(argv[3], &pattern_len);
    if (!pattern) {
        fprintf(stderr, "Ошибка: некорректная hex-строка для патерна.\n");
        return 1;
    }

    size_t replacement_len = 0;
    uint8_t *replacement = hex_to_bytes(argv[4], &replacement_len);
    if (!replacement) {
        fprintf(stderr, "Ошибка: некорректная hex-строка для замены.\n");
        free(pattern);
        return 1;
    }

    printf("Запуск замены. Размер блока N = %d\n", BLOCK_SIZE);

    int result = search_and_replace(input_path, output_path,
                                    pattern, pattern_len,
                                    replacement, replacement_len,
                                    BLOCK_SIZE);

    free(pattern);
    free(replacement);

    if (result != 0) {
        fprintf(stderr, "Ошибка при обработке файла.\n");
        return 1;
    }

    printf("Успешно завершено.\n");
    return 0;
}