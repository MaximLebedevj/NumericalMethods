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

void fill_file(const char* fname, size_t size1, size_t size2) {
    FILE* file = fopen(fname, "w");
    double write_val = 0.0;
    for (int i = 0; i < size1; ++i) {
        for (int j = 0; j < size2; ++j) {
            fprintf(file, "%lf", write_val);
        }
    }
    fclose(file);
}

int mmap_open_file_(const char* fname) {
    int fd = open(fname, O_RDWR, 0);
    if (fd < 0) {
        std::cerr << "fileMappingCreate - open failed, fname = "
                  << fname << ", " << strerror(errno) << "\n";
    }
    return fd;
}

double* mmap_(int fd, size_t fsize) {
    double* dataPtr = (double*)mmap(nullptr, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dataPtr == MAP_FAILED) {
        std::cerr << "FileMappingCreate - mmap failed, fname = "
        << fd << ", " << strerror(errno) << std::endl;
        close(fd);
    }
    return dataPtr;
}

FileMapping* mmap_malloc_(int fd, size_t fsize, double* dataPtr) {
    FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));
    if (mapping == nullptr) {
        std::cerr << "fileMappingCreate - malloc failed, fname = "
        << fd << std::endl;
        munmap(dataPtr, fsize);
        close(fd);
    }

    mapping->fd = fd;
    mapping->fsize = fsize;
    mapping->dataPtr = dataPtr;

    return mapping;
}

FileMapping* mmap_create(const char* fname, size_t size1, size_t size2) {
    fill_file(fname, size1, size2);
    int fd = mmap_open_file_(fname);
    size_t fsize = size1 * size2;
    double* dataPtr = mmap_(fd, fsize);
    FileMapping* mapping = mmap_malloc_(fd, fsize, dataPtr);

    mapping->fd = fd;
    mapping->fsize = fsize;
    mapping->dataPtr = dataPtr;

    return mapping;
}

void mmap_free(FileMapping* mapping) {
    munmap(mapping->dataPtr, mapping->fsize);
    close(mapping->fd);
    free(mapping);
}

int main() {
    int a, b, M, N;
    a = b = 1;
    N = M = 10;
    double eps = 1e-4;

    // writing N, M to size.txt
    FILE *file = fopen("size.txt","wb");
    int* N_arr = (int*)malloc(sizeof(int));
    N_arr[0] = N + 1;
    int* M_arr = (int*)malloc(sizeof(int));
    M_arr[0] = M + 1;
    fwrite(N_arr, sizeof(int), 1, file);
    fwrite(M_arr, sizeof(int), 1, file);
    fclose(file);

    double hx = (double)a / N;
    double hy = (double)b / M; 

    // x steps
    double *xi = (double*)malloc((N + 1) * sizeof(double));
    int i;
    for (i = 0; i < N + 1; ++i) {
        xi[i] = hx * i;
    }

    // y steps
    double *yi = (double*)malloc((M + 1) * sizeof(double));
    for (i = 0; i < M + 1; ++i) {
        yi[i] = hy * i;
    }

    // writing xi, yi to steps.txt
    file = fopen("steps.txt","wb");
    fwrite(xi, sizeof(double), N + 1, file);
    fwrite(yi, sizeof(double), M + 1, file);
    fclose(file);

    // mmap for u_prev
    const char* fname_prev = "prev.txt";
    FileMapping* mapping_prev = mmap_create(fname_prev, N + 1, M + 1);

    // mmap for u_curr
    const char* fname_curr = "curr.txt";
    FileMapping* mapping_curr = mmap_create(fname_curr, N + 1, M + 1);

    // Инициализируем u_prev нулями
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

    mmap_free(mapping_prev);
    mmap_free(mapping_curr);

    return 0;
}
