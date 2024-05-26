#include <fcntl.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned long long file_size;

struct FileMapping {
    int fd;
    size_t fsize;
    double *dataPtr;
};

double phi(double x, double y) { return -x + 4 * pow(y, 2); }

void fill_file(const char *fname, size_t size1, size_t size2) {
    FILE *file = fopen(fname, "wb");
    double write_val = 0.0;
    int progress = 1;
    for (int i = 0; i < size1; ++i) {
        if (i == (int)(((size1) / 100)) * progress) {
            progress += 1;
            std::cout << "-> \rInitializing ans.txt : " << progress << "%"
                      << std::flush;
        }
        for (int j = 0; j < size2; ++j) {
            fprintf(file, "%lf", write_val);
        }
    }
    fclose(file);
}

int mmap_open_file_(const char *fname) {
    int fd = open(fname, O_RDWR, 0);
    if (fd < 0) {
        printf("fileMappingCreate - open failed\n");
    }
    return fd;
}

double *mmap_(int fd, size_t fsize) {
    double *dataPtr = (double *)mmap(nullptr, fsize, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, 0);
    if (dataPtr == MAP_FAILED) {
        printf("FileMappingCreate - mmap failed\n");
        close(fd);
    }
    return dataPtr;
}

FileMapping *mmap_malloc_(int fd, size_t fsize, double *dataPtr) {
    FileMapping *mapping = (FileMapping *)malloc(sizeof(FileMapping));
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

FileMapping *mmap_create(const char *fname, size_t size1, size_t size2,
                         bool rewrite = true) {
    if (rewrite) {
        fill_file(fname, size1, size2);
    }
    int fd = mmap_open_file_(fname);
    struct stat st;
    if (fstat(fd, &st) < 0) {
        printf("fileMappingCreate - fstat failed\n");
        close(fd);
    }
    size_t fsize = (size_t)st.st_size;
    file_size = fsize;
    if ((int)(fsize / 1024)) {
        if ((int)(fsize / 1024 / 1024 / 1024)) {
            printf(" -> ans.txt size = %zu GB\n", fsize / 1024 / 1024 / 1024);
        } else if ((int)(fsize / 1024 / 1024)) {
            printf(" -> ans.txt size = %zu MB\n", fsize / 1024 / 1024);
        } else {
            printf(" -> ans.txt size = %zu KB\n", fsize / 1024);
        }
    } else {
        printf(" -> ans.txt size = %zu Bytes\n", fsize);
    }
    double *dataPtr = mmap_(fd, fsize);
    FileMapping *mapping = mmap_malloc_(fd, fsize, dataPtr);

    mapping->fd = fd;
    mapping->fsize = fsize;
    mapping->dataPtr = dataPtr;

    return mapping;
}

void mmap_free(FileMapping *mapping) {
    munmap(mapping->dataPtr, mapping->fsize);
    close(mapping->fd);
    free(mapping);
}

void write_to_file(double *u, FileMapping* file, unsigned long long N, unsigned long long M, bool is_finished) {

    if (is_finished) {
        printf("\n\n");
    } else {
        printf("\n");
    }

    std::string finish_str = is_finished ? "Convergence achieved -> " : ""; 
    int progress = 1;
    for (int i = 0; i < N + 1; ++i) {
	if (i == (int)(((N + 1) / 100)) * progress) {
            progress += 1;
            std::cout << "\r" << finish_str << "Writing result to ans.txt : " << progress << "%"
                      << std::flush;
        }
        for (int j = 0; j < M + 1; ++j) {
            file->dataPtr[j + i * (M + 1)] = u[j + i * (M + 1)];
        }
    }

    if (is_finished) {
        printf("\n");
    } else {
        printf("\n\n");
    }
}

int main(int argc, const char *argv[]) {

    int a, b;
    unsigned long long N, M;
    a = b = 1;
    N = M = argc > 1 ? atoi(argv[1]) - 1 : 99;
    double eps = 1e-4;

    int ask_iter = 0;
    if (argc > 2) {
        ask_iter = atoi(argv[2]);
    }

    // mmap for u_curr
    printf("\nRAM mode\n\n");
    bool rewrite = argc > 1 && !strcmp(argv[1], "continue") ? false : true;
    if (!rewrite) {
        printf("Continue mode");
    }

    if (ask_iter) {
        printf(", ask_iter = %d\n", ask_iter);
    }

    const char *fname_u = "ans.txt";
    FileMapping *file_ans = mmap_create(fname_u, N + 1, M + 1, rewrite);

    // writing N, M to size.txt
    FILE *file = fopen("size.txt", "wb");

    unsigned long long *N_arr = static_cast<unsigned long long *>(malloc(sizeof(unsigned long long)));
    N_arr[0] = N + 1;
    unsigned long long *M_arr = static_cast<unsigned long long *>(malloc(sizeof(unsigned long long)));
    M_arr[0] = M + 1;

    fwrite(N_arr, sizeof(int), 1, file);
    fwrite(M_arr, sizeof(int), 1, file);

    fclose(file);

    free(N_arr);
    free(M_arr);

    double hx = (double)a / N;
    double hy = (double)b / M;

    // x steps
    double *xi = (double *)malloc((N + 1) * sizeof(double));
    int i;
    for (i = 0; i < N + 1; ++i) {
        xi[i] = hx * i;
    }

    // y steps
    double *yi = (double *)malloc((M + 1) * sizeof(double));
    for (i = 0; i < M + 1; ++i) {
        yi[i] = hy * i;
    }

    // writing xi, yi to steps.txt
    file = fopen("steps.txt", "wb");
    fwrite(xi, sizeof(double), N + 1, file);
    fwrite(yi, sizeof(double), M + 1, file);
    fclose(file);

    free(xi);
    free(yi);

    mmap_free(file_ans);

    return 0;
}
