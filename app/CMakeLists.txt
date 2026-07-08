# CMakeLists.txt - настройки сборки основной программы.
#
# Коликов Глеб Александрович
# Группа МК-101

# Собираем все .c и .h файлы из папки src
get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" APP_SOURCES)

# Создаем исполняемый файл из этих файлов
add_executable(my_program ${APP_SOURCES})

# Подключаем нашу библиотеку к программе
target_link_libraries(my_program PRIVATE my_library)