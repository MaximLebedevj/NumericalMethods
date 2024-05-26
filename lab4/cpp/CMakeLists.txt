add_executable(lab4 lab4_ram_matplot++.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native -m64 -O3 -funroll-loops")
add_subdirectory(matplotplusplus)
target_link_libraries(lab4 PUBLIC matplot)
