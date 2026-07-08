/*
search_replace.h - интерфейс модуля поиска и замены байтовых последовательностей в файле.

Коликов Глеб Александрович
Группа МК-101
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/*
@brief Читает входной файл блоками, ищет вхождения pattern и заменяет их на replacement. Результат записывает в output_path.
@param input_path Путь к входному файлу.
@param output_path Путь к выходному файлу.
@param pattern Искомая последовательность байт.
@param pattern_len Длина искомой последовательности.
@param replacement Заменяющая последовательность байт.
@param replacement_len Длина заменяющей последовательности.
@param block_size Размер блока чтения (N).
@return 0 при успехе, отрицательное число при ошибке.
 */

int search_and_replace(const char *input_path, const char *output_path,
                       const uint8_t *pattern, size_t pattern_len,
                       const uint8_t *replacement, size_t replacement_len,
                       size_t block_size);