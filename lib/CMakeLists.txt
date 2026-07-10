# CMakeLists.txt - настройки сборки библиотеки.
#
# Коликов Глеб Александрович
# Группа МК-101

# Собираем все .c и .h файлы из папки src
get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SOURCES)

# Создаем библиотеку из этих файлов
add_library(my_library ${LIB_SOURCES})

# Указываем, где лежат заголовочные файлы (.h), чтобы main.c мог их найти
target_include_directories(my_library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Поддержка сборки как статической, так и динамической библиотеки
if(BUILD_SHARED_LIBS)
    set_target_properties(my_library PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()