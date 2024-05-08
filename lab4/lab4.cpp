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
    M = N = 100;
    double eps = 1e-4;

    double hx = (double)a / N;
    double hy = (double)b / M; 

    const char* fname = "lab4_points.txt";

    int fd = open(fname, O_RDWR, 0);
    if (fd < 0) {
        std::cerr << "fileMappingCreate - open failed, fname = "
                  << fname << ", " << strerror(errno) << std::endl;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        std::cerr << "fileMappingCreate - fstat failed, fname = "
                  << fname << ", " << strerror(errno) << std::endl;
        close(fd);
    }
    size_t fsize = (N + 1) * (M + 1);

    double* dataPtr = (double*)mmap(nullptr, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dataPtr == MAP_FAILED) {
        std::cerr << "FileMappingCreate -nmap failed, fname = "
                  << fname << ", " << strerror(errno) << std::endl;
        close(fd);
    }

    FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));
    if (mapping == nullptr) {
        std::cerr << "fileMappingCreate - malloc failed, fname = "
                  << fname << std::endl;
        munmap(dataPtr, fsize);
        close(fd);
    }

    mapping->fd = fd;
    mapping->fsize = fsize;
    mapping->dataPtr = dataPtr;


    mapping->dataPtr[1] = 777.0;
    std::cout << mapping->dataPtr[1] << std::endl;


    std::cout << fsize << std::endl;

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
            u[i][j] = 0;
        }
    }

    // Граничное условие u(x, 0)
    for (int i = 0; i < N + 1; ++i) {
        u[i][0] = phi(xi[i], 0);
    }

    // Граничное условие u(x, M)
    for (int i = 0; i < N + 1; ++i) {
        u[i][M] = phi(xi[i], yi[M]);
    }

    // Граничное условие u(0, y)
    for (int j = 0; j < M + 1; ++j) {
        u[0][j] = phi(0, yi[j]);
    }

    // Граничное условие u(N, y)
    for (int j = 0; j < M + 1; ++j) {
        u[N][j] = phi(xi[N], yi[j]);
    }

    double **u0 = (double**)malloc((N + 1) * sizeof(double));
    for (int i = 0; i < M + 1; ++i) {
        u0[i] = (double*)malloc((M + 1) * sizeof(double));
    }

    double **u1 = (double**)malloc((N + 1) * sizeof(double));
    for (int i = 0; i < M + 1; ++i) {
        u1[i] = (double*)malloc((M + 1) * sizeof(double));
    }

    // Копируем u[][] в u0[][] и u1[][]
    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            u0[i][j] = u[i][j];
            u1[i][j] = u[i][j];
        }
    }

    double max;
    double temp;
    int iter = 0;
    while (true) {
        max = -1.0;
        if (iter % 100 == 0) {
            printf("%d\n", iter);
        }
        iter++;
        for (int i = 1; i < N; ++i) {
            for (int j = 1; j < M; ++j) {
                u1[i][j] = 1.0 / 4.0 * (u1[i - 1][j] + u0[i + 1][j] + u0[i][j + 1] + u1[i][j - 1]);
            }
        }
        for (int i = 0; i < N + 1; ++i) {
            for (int j = 0; j < M + 1; ++j) {
                temp = fabs(u1[i][j] - u0[i][j]);
                max = (temp > max) ? temp : max; 
            }
        }
        if (max <= eps) {
            break;
        }
        for (int i = 0; i < N + 1; ++i) {
            for (int j = 0; j < M + 1; ++j) {
                u0[i][j] = u1[i][j];
            }
        }
    }

    fwrite(u1, sizeof(double), (N + 1) * (M + 1), file);

    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < M + 1; ++j) {
            fprintf(file, "%lf ", u1[i][j]);
        }
        fprintf(file, "\n");
    }

    free(xi);
    free(yi);

    for (int i = 0; i < N + 1; ++i) {
        free(u[i]);
        free(u0[i]);
        free(u1[i]);
    }

    free(u);
    free(u0);
    free(u1);

    fclose(file);

    munmap(mapping->dataPtr, mapping->fsize);
    close(mapping->fd);
    free(mapping);

    return 0;
}
