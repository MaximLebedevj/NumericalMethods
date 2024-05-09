#include <cmath>
//#include <math.h>
//#include <iostream>
//#include <fstream>
//#include <time.h> 
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <iostream> 
#include <cstring>
#include <fstream>

struct FileMapping {
    int fd;
    size_t fsize;
    double* dataPtr;
};

double phi(double x, double y) {
    return -x + 4 * pow(y, 2); 
}

int main() {
    FILE *file = fopen("lab4_points.txt","wb");
    int a, b, M, N;
    a = b = 1;
    M = N = 10;
    double eps = 1e-4;

    double hx = (double)a / N;
    double hy = (double)b / M; 



    const char* fname_prev = "prev.txt";
    FILE* file_prev = fopen(fname_prev, "w");
    double out = 0.0;
    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            fprintf(file_prev, "%lf", out);
        }
    }
    fclose(file_prev);

    int fd_prev = open(fname_prev, O_RDWR, 0);
    if (fd_prev < 0) {
        std::cerr << "fileMappingCreate - open failed, fname = "
                  << fname_prev << ", " << strerror(errno) << std::endl;
    }

    size_t fsize_prev = (N + 1) * (M + 1);

    double* dataPtr_prev = (double*)mmap(nullptr, fsize_prev, PROT_READ | PROT_WRITE, MAP_SHARED, fd_prev, 0);
    if (dataPtr_prev == MAP_FAILED) {
        std::cerr << "FileMappingCreate -nmap failed, fname = "
                  << fname_prev << ", " << strerror(errno) << std::endl;
        close(fd_prev);
    }

    FileMapping* mapping_prev = (FileMapping*)malloc(sizeof(FileMapping));
    if (mapping_prev == nullptr) {
        std::cerr << "fileMappingCreate - malloc failed, fname = "
                  << fname_prev << std::endl;
        munmap(dataPtr_prev, fsize_prev);
        close(fd_prev);
    }

    mapping_prev->fd = fd_prev;
    mapping_prev->fsize = fsize_prev;
    mapping_prev->dataPtr = dataPtr_prev;


    const char* fname_curr = "curr.txt";
    FILE* file_curr = fopen(fname_curr, "w");
    out = 0.0;
    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            fprintf(file_curr, "%lf", out);
        }
    }
    fclose(file_curr);

    int fd_curr = open(fname_curr, O_RDWR, 0);
    if (fd_curr < 0) {
        std::cerr << "fileMappingCreate - open failed, fname = "
                  << fname_curr << ", " << strerror(errno) << std::endl;
    }

    size_t fsize_curr = (N + 1) * (M + 1);

    double* dataPtr_curr = (double*)mmap(nullptr, fsize_curr, PROT_READ | PROT_WRITE, MAP_SHARED, fd_curr, 0);
    if (dataPtr_curr == MAP_FAILED) {
        std::cerr << "FileMappingCreate -nmap failed, fname = "
                  << fname_curr << ", " << strerror(errno) << std::endl;
        close(fd_curr);
    }

    FileMapping* mapping_curr = (FileMapping*)malloc(sizeof(FileMapping));
    if (mapping_curr == nullptr) {
        std::cerr << "fileMappingCreate - malloc failed, fname = "
                  << fname_curr << std::endl;
        munmap(dataPtr_curr, fsize_curr);
        close(fd_curr);
    }

    mapping_curr->fd = fd_curr;
    mapping_curr->fsize = fsize_curr;
    mapping_curr->dataPtr = dataPtr_curr;


    //mapping->dataPtr[1] = 777.0;


    // code block #1
    double *xi = (double*)malloc((N + 1) * sizeof(double));
    int i;
    for (i = 0; i < N + 1; ++i) {
        xi[i] = hx * i;
    }
    fwrite(xi, sizeof(double), N + 1, file);
    fprintf(file, "\n"); 

    // code block #2
    double *yi = (double*)malloc((M + 1) * sizeof(double));
    for (i = 0; i < M + 1; ++i) {
        yi[i] = hy * i;
    }
    fwrite(yi, sizeof(double), M + 1, file);
    fprintf(file, "\n");

    // code block #3
    double **u = (double**)malloc((N + 1) * sizeof(double));
    for (int i = 0; i < M + 1; ++i) {
        u[i] = (double*)malloc((M + 1) * sizeof(double));
    }

    // Инициализируем u[][] нулями  
    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            mapping_prev->dataPtr[j + i * (M + 1)] = 0.0;
        }
    }

    // Граничное условие u(x, 0)
    for (int i = 0; i < N + 1; ++i) {
        mapping_prev->dataPtr[i * (N + 1)] = phi(xi[i], 0);
    }

    // Граничное условие u(x, M)
    for (int i = 0; i < N + 1; ++i) {
        mapping_prev->dataPtr[(M + 1) * (i + 1) - 1] = phi(xi[i], yi[M]);
    }

    // Граничное условие u(0, y)
    for (int j = 0; j < M + 1; ++j) {
        mapping_prev->dataPtr[j] = phi(0, yi[j]);
    }

    // Граничное условие u(N, y)
    for (int j = 0; j < M + 1; ++j) {
        mapping_prev->dataPtr[j + (M + 1) * N] = phi(xi[N], yi[j]);
    }

    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            mapping_curr->dataPtr[j + i * (M + 1)] = mapping_prev->dataPtr[j + i * (M + 1)];
        }
    }

    double max;
    double temp;
    int iter = 0;
    while (true) {
        max = -1.0;
        if (iter % 100 == 0) {
            //printf("%d\n", iter);
        }
        iter++;
        for (int i = 1; i < N; ++i) {
            for (int j = 1; j < M; ++j) {
                mapping_curr->dataPtr[j + i * (M + 1)] = 1.0 / 4.0 * (mapping_curr->dataPtr[(j - 1) + i * (M + 1)] + mapping_prev->dataPtr[(j + 1) + i * (M + 1)] + mapping_prev->dataPtr[j + (i + 1) * (M + 1)] + mapping_curr->dataPtr[j + (i - 1) * (M + 1)]);
            }
        }
        for (int i = 0; i < N + 1; ++i) {
            for (int j = 0; j < M + 1; ++j) {
                temp = fabs(mapping_curr->dataPtr[j + i * (M + 1)] - mapping_prev->dataPtr[j + i * (M + 1)]);
                max = (temp > max) ? temp : max; 
            }
        }
        if (max <= eps) {
            break;
        }
        for (int i = 0; i < N + 1; ++i) {
            for (int j = 0; j < M + 1; ++j) {
                mapping_prev->dataPtr[j + i * (M + 1)] = mapping_curr->dataPtr[j + i * (M + 1)];
            }
        }
    }

    std::cout << std::endl;
    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            std::cout << mapping_curr->dataPtr[j + i * (M + 1)] << " ";
        }
        std::cout << std::endl;
    }






    free(xi);
    free(yi);


    fclose(file);

    munmap(mapping_prev->dataPtr, mapping_prev->fsize);
    close(mapping_prev->fd);
    free(mapping_prev);

    return 0;
}
