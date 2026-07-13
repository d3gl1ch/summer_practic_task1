/*
search_replace.c - реализация алгоритма поиска и замены байтовых последовательностей.
Использует блочное чтение для эффективной работы с файлами любого размера без загрузки всего файла в оперативную память.

Коликов Глеб Александрович
Группа МК-101
 */

// Подключаем наш заголовочный файл с объявлением функции
#include "search_replace.h"

// Стандартные библиотеки для работы с файлами, памятью и строками
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Внутренняя функция: наивный поиск подстроки (паттерна) в буфере.
// Возвращает индекс начала первого вхождения паттерна или -1, если он не найден.
static int find_pattern(const uint8_t *buf, size_t buf_len,
                        const uint8_t *pattern, size_t pattern_len,
                        size_t start_from) {
    // Базовые проверки: если паттерн пустой, или буфер меньше паттерна, или начало поиска за пределами буфера
    if (pattern_len == 0 || buf_len < pattern_len || start_from >= buf_len) {
        return -1;
    }

    // Ограничиваем поиск так, чтобы не выйти за границы буфера при проверке паттерна
    size_t limit = buf_len - pattern_len;
    
    // Проходим по буферу, начиная с заданной позиции start_from
    for (size_t i = start_from; i <= limit; i++) {
        size_t j;
        // Сравниваем каждый байт паттерна с соответствующим байтом в буфере
        for (j = 0; j < pattern_len; j++) {
            if (buf[i + j] != pattern[j]) {
                break; // При несовпадении прерываем внутренний цикл
            }
        }
        // Если цикл прошел до конца (j == pattern_len), значит все байты совпали
        if (j == pattern_len) {
            return i; // Возвращаем индекс начала совпадения
        }
    }
    
    // Если ничего не нашли, возвращаем -1
    return -1;
}

// Основная реализация функции поиска и замены
int search_and_replace(const char *input_path, const char *output_path,
                       const uint8_t *pattern, size_t pattern_len,
                       const uint8_t *replacement, size_t replacement_len,
                       size_t block_size) {
    // Открываем входной файл в бинарном режиме чтения ("rb")
    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        fprintf(stderr, "Ошибка: не удалось открыть входной файл '%s'.\n", input_path);
        return -1;
    }

    // Открываем выходной файл в бинарном режиме записи ("wb")
    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        fprintf(stderr, "Ошибка: не удалось открыть выходной файл '%s'.\n", output_path);
        fclose(in_file); // Обязательно закрываем уже открытый файл перед выходом (защита от утечек дескрипторов)
        return -1;
    }

    // Оптимизация: если паттерн пустой, нет смысла искать, просто копируем файл целиком блоками
    if (pattern_len == 0) {
        uint8_t *copy_buf = malloc(block_size);
        if (!copy_buf) {
            fclose(in_file);
            fclose(out_file);
            return -1;
        }
        size_t bytes_read;
        // Читаем и записываем, пока не достигнем конца файла (EOF)
        while ((bytes_read = fread(copy_buf, 1, block_size, in_file)) > 0) {
            fwrite(copy_buf, 1, bytes_read, out_file);
        }
        free(copy_buf);
        fclose(in_file);
        fclose(out_file);
        return 0;
    }

    // Выделяем буфер размером 2 * block_size.
    // Это ключевой момент: буфер в два раза больше блока, чтобы безопасно хранить 
    // "хвост" предыдущего чтения и проверять паттерны, пересекающие границу блоков.
    size_t buf_size = 2 * block_size;
    uint8_t *buffer = malloc(buf_size);
    if (!buffer) {
        fprintf(stderr, "Ошибка: не удалось выделить память для буфера.\n");
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    // read_offset: позиция в буфере, с которой начинается запись новых данных из файла (0 или block_size)
    size_t read_offset = 0;
    // search_start: позиция в буфере, с которой начинается поиск паттерна
    size_t search_start = 0;
    size_t bytes_read;

    // Основной цикл чтения файла блоками по block_size байт
    while ((bytes_read = fread(buffer + read_offset, 1, block_size, in_file)) > 0) {
        // current_block_end: реальный конец данных в буфере после текущего чтения
        size_t current_block_end = read_offset + bytes_read;

        // search_limit: граница, до которой безопасно искать паттерн. 
        // Мы не ищем ближе к концу буфера, чем (pattern_len - 1), чтобы не выйти за его пределы при сравнении.
        size_t search_limit = (current_block_end > pattern_len) ? current_block_end - pattern_len : 0;
        int pos;

        // Ищем все вхождения паттерна в текущем "безопасном" диапазоне буфера
        while ((pos = find_pattern(buffer, current_block_end, pattern, pattern_len, search_start)) != -1 && pos <= search_limit) {
            // Если до найденного паттерна есть данные, которые не нужно заменять, записываем их в выходной файл
            if (pos > search_start) {
                fwrite(buffer + search_start, 1, pos - search_start, out_file);
            }
            // Записываем байты замены вместо найденного паттерна
            fwrite(replacement, 1, replacement_len, out_file);
            // Сдвигаем указатель поиска сразу за замененный паттерн
            search_start = pos + pattern_len;
        }

        // Вычисляем "безопасный конец" (safe_end). 
        // Это часть буфера, которая гарантированно не содержит неполный паттерн на стыке со следующим блоком.
        size_t safe_end = (current_block_end > pattern_len - 1) ? current_block_end - (pattern_len - 1) : read_offset;

        // Записываем все безопасные данные, которые мы уже проверили и которые не являются частью паттерна
        if (safe_end > search_start) {
            fwrite(buffer + search_start, 1, safe_end - search_start, out_file);
            search_start = safe_end; // Обновляем позицию поиска
        }

        // Проверка на конец файла: если мы прочитали меньше байт, чем размер блока, значит это последний кусок файла
        if (bytes_read < block_size) {
            // Проверяем, не остался ли паттерн в самом конце файла (в "хвосте")
            if (current_block_end > search_start) {
                int tail_pos = find_pattern(buffer, current_block_end, pattern, pattern_len, search_start);
                if (tail_pos != -1) {
                    // Если нашли, записываем данные до него и саму замену
                    fwrite(buffer + search_start, 1, tail_pos - search_start, out_file);
                    fwrite(replacement, 1, replacement_len, out_file);
                    search_start = tail_pos + pattern_len;
                }
                // Записываем оставшийся хвост файла без изменений
                if (current_block_end > search_start) {
                    fwrite(buffer + search_start, 1, current_block_end - search_start, out_file);
                }
            }
            break; // Выходим из цикла, так как файл полностью обработан
        }

        // Подготовка к следующей итерации: 
        // Переключаем offset чтения между 0 и block_size, реализуя простую схему двойного буфера.
        // (Примечание: в данной реализации хвост буфера остается в своей половине, а fread пишет в другую половину,
        // что позволяет избежать дорогостоящей операции memmove для сдвига данных).
        read_offset = (read_offset == 0) ? block_size : 0;
        search_start = read_offset;
    }

    // Проверяем, не произошла ли аппаратная или системная ошибка при чтении файла
    if (ferror(in_file)) {
        fprintf(stderr, "Ошибка: произошла ошибка при чтении входного файла.\n");
        free(buffer);
        fclose(in_file);
        fclose(out_file);
        return -1;
    }

    // Очистка ресурсов: освобождаем динамическую память и закрываем файловые дескрипторы
    free(buffer);
    fclose(in_file);
    fclose(out_file);

    // Возвращаем 0, сигнализируя об успешном завершении операции
    return 0;
}