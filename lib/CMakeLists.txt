# Файл сборки библиотеки (lib/CMakeLists.txt)
# Отвечает за компиляцию статической или динамической библиотеки с логикой замены.
#
# Коликов Глеб Александрович
# Группа МК-101

# Используем нашу кастомную функцию для сбора всех .c и .h файлов из папки src библиотеки.
# Результат сохраняется в переменную LIB_SOURCES.
get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SOURCES)

# Создаем библиотечный целевой объект с именем 'my_library' из найденных исходных файлов.
# По умолчанию CMake создаст статическую библиотеку (.a или .lib), если не указано иное.
add_library(my_library ${LIB_SOURCES})

# Указываем компилятору, где искать заголовочные файлы (.h) для этой библиотеки.
# Ключевое слово PUBLIC означает, что любой, кто подключит my_library через target_link_libraries, 
# автоматически получит этот путь в своих include-директориях (как будто подключил его сам).
target_include_directories(my_library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Условная компиляция для поддержки сборки в виде динамической библиотеки (Shared Library).
# Если пользователь передал -DBUILD_SHARED_LIBS=ON при конфигурации CMake:
if(BUILD_SHARED_LIBS)
    # Для Windows: эта команда заставляет компилятор экспортировать все символы (функции) из DLL,
    # чтобы они были видны приложению, которое будет использовать эту DLL (аналог __declspec(dllexport)).
    set_target_properties(my_library PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()