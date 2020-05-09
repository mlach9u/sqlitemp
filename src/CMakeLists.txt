cmake_minimum_required (VERSION 3.8)

project(sqlitempTest)

find_package(Threads)

add_executable (sqlitempTest
    "sqlitempTest.cpp"
    "lib/sqlite3.c"
)

target_link_libraries (sqlitempTest
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)
