#include <math.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

struct FileMapping {
    int fd;
    size_t fsize;
    double* dataPtr;
};

double phi(double x, double y) {
    return -x + 4 * pow(y, 2); 
}

void fill_file(const char* fname, size_t size1, size_t size2) {
    FILE* file = fopen(fname, "wb");
    double write_val = 0.0;
    int progress = 1;
    for (int i = 0; i < size1; ++i) {
        if (i == (int)(((size1) / 100)) * progress) {
            progress += 1;
            std::cout << "\rInitializing ans.txt : " << progress << "%" << std::flush;
        }
        for (int j = 0; j < size2; ++j) {
            fprintf(file, "%lf", write_val);
        }
    }
    fclose(file);
}

int mmap_open_file_(const char* fname) {
    int fd = open(fname, O_RDWR, 0);
    if (fd < 0) {
        printf("fileMappingCreate - open failed\n");
    }
    return fd;
}

double* mmap_(int fd, size_t fsize) {
    double* dataPtr = (double*)mmap(nullptr, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dataPtr == MAP_FAILED) {
        printf("FileMappingCreate - mmap failed\n");
        close(fd);
    }
    return dataPtr;
}

FileMapping* mmap_malloc_(int fd, size_t fsize, double* dataPtr) {
    FileMapping* mapping = (FileMapping*)malloc(sizeof(FileMapping));
    if (mapping == nullptr) {
        printf("fileMappingCreate - malloc failed\n");
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
    struct stat st;
    if(fstat(fd, &st) < 0) {
        printf("fileMappingCreate - fstat failed\n");
        close(fd);
    }
    size_t fsize = (size_t)st.st_size;
    if ((int)(fsize / 1024)) {
        if ((int)(fsize / 1024 / 1024 / 1024)) {
            printf(" -> ans.txt size = %zu GB\n", fsize / 1024 / 1024 / 1024);
        } 
        else if ((int)(fsize / 1024 / 1024)) {
            printf(" -> ans.txt size = %zu MB\n", fsize / 1024 / 1024);
        }
        else {
            printf(" -> ans.txt size = %zu KB\n", fsize / 1024);
        }
    } 
    else {
        printf(" -> ans.txt size = %zu Bytes\n", fsize);
    }
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

int main(int argc, char* argv[]) {
    int a, b, M, N;
    a = b = 1;
    N = M = argc > 1 ? atoi(argv[1]) - 1 : 99;
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

    free(N_arr);
    free(M_arr);

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

    // mmap for u_curr
    const char* fname_u = "ans.txt";
    FileMapping* u = mmap_create(fname_u, N + 1, M + 1);

    // boundary condition u(x, 0)
    for (int i = 0; i < N + 1; ++i) {
        u->dataPtr[i * (N + 1)] = phi(xi[i], 0);
    }

    // boundary condition u(x, M)
    for (int i = 0; i < N + 1; ++i) {
        u->dataPtr[(M + 1) * (i + 1) - 1] = phi(xi[i], yi[M]);
    }

    // boundary condition u(0, y)
    for (int j = 0; j < M + 1; ++j) {
        u->dataPtr[j] = phi(0, yi[j]);
    }

    // boundary condition u(N, y)
    for (int j = 0; j < M + 1; ++j) {
        u->dataPtr[j + (M + 1) * N] = phi(xi[N], yi[j]);
    }

    double max, tmp, diff;
    int iter = 0;
    while (true) {
        max = -1.0;
        iter++;
        for (int i = 1; i < N; ++i) {
            for (int j = 1; j < M; ++j) {
                tmp = u->dataPtr[j + i * (M + 1)];
                u->dataPtr[j + i * (M + 1)] = 0.25 * (u->dataPtr[(j - 1) + i * (M + 1)] + u->dataPtr[(j + 1) + i * (M + 1)] + u->dataPtr[j + (i + 1) * (M + 1)] + u->dataPtr[j + (i - 1) * (M + 1)]);
                diff = fabs(u->dataPtr[j + i * (M + 1)] - tmp);
                max = diff > max ? diff : max;
            }
        }
        std::cout << "\rMax difference after " << iter << " iterations: " << max << std::flush;
        if (max <= eps) {
            printf("\nConvergence achieved after %d iterations.\n", iter);
            break;
        }
    }

    free(xi);
    free(yi);

    mmap_free(u);

    return 0;
}
