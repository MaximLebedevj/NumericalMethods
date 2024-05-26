#include <fcntl.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <matplot/matplot.h>
#include <set>

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


double u_next(unsigned long long size1, unsigned long long size2, double* u) {
    static int i = -1;
    static int j = 0;

    i++;
    
    if (i == size1 + 1) {
        j = (j + 1) % (size2 + 1);
        i = 0;
    }

    return u[j + i * (size2 + 1)];
}

double value;
unsigned long long N, M;
double* u;

int main(int argc, const char *argv[]) {
    int a, b;
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

    if (argc > 1 && !strcmp(argv[1], "continue")) {
        N = M = (unsigned long long)std::sqrt(file_size / 8) - 1;
    }

    printf("Allocating memory ...");
    u = static_cast<double*>(malloc(((N + 1) * (M + 1)) * sizeof(double)));
    size_t u_size = (N + 1) * (M + 1) * sizeof(double);
    if ((size_t)u_size / 1024) {
        if ((size_t)u_size / 1024 / 1024 / 1024) {
            printf(" -> %zu GB was alloacted\n", u_size / 1024 / 1024 / 1024);
        } else if (u_size / 1024 / 1024) {
            printf(" -> %zu MB was alloacted\n", u_size / 1024 / 1024);
        } else {
            printf(" -> %zu KB was alloacted\n", u_size / 1024);
        }
    } else {
        printf(" -> %zu Bytes was alloacted\n", u_size);
    }

    if (!rewrite) {
        int progress = 1;
        for (int i = 0; i < N + 1; ++i) {
            if (i == (int)(((N + 1) / 100)) * progress) {
                progress += 1;
                std::cout << "\rWriting ans.txt to RAM -> " << progress << "%"
                          << std::flush;
            }
            for (int j = 0; j < M + 1; ++j) {
                u[j + i * (M + 1)] = file_ans->dataPtr[j + i * (M + 1)];
            }
        }
    } else {
        int progress = 1;
        for (int i = 0; i < N + 1; ++i) {
            if (i == (int)(((N + 1) / 100)) * progress) {
                progress += 1;
                std::cout << "\rInitializing RAM with zeros -> " << progress << "%"
                          << std::flush;
            }
            for (int j = 0; j < M + 1; ++j) {
                u[j + i * (M + 1)] = 0.0;
            }
        }
    }
    printf("\n");

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

    if(rewrite) {
        // boundary condition u(x, 0)
        for (int i = 0; i < N + 1; ++i) {
            u[i * (N + 1)] = phi(xi[i], 0);
        }

        // boundary condition u(x, M)
        for (int i = 0; i < N + 1; ++i) {
            u[(M + 1) * (i + 1) - 1] = phi(xi[i], yi[M]);
        }

        // boundary condition u(0, y)
        for (int j = 0; j < M + 1; ++j) {
            u[j] = phi(0, yi[j]);
        }

        // boundary condition u(N, y)
        for (int j = 0; j < M + 1; ++j) {
            u[j + (M + 1) * N] = phi(xi[N], yi[j]);
        }
    }

    double max, tmp, diff;
    int progress = 1;
    int iter = 0;
    bool is_finished = false;
    while (true) {
        max = -1.0;
        progress = 1;
        iter++;
        for (int i = 1; i < N; ++i) {
            if (i == (int)(((N + 1) / 100)) * progress) {
                progress += 1;
                std::cout << "\rIteration " << iter << ": " << progress << "%"
                          << std::flush;
            }
            for (int j = 1; j < M; ++j) {
                tmp = u[j + i * (M + 1)];
                u[j + i * (M + 1)] =
                    0.25 * (u[(j - 1) + i * (M + 1)] +
                            u[(j + 1) + i * (M + 1)] +
                            u[j + (i + 1) * (M + 1)] +
                            u[j + (i - 1) * (M + 1)]);
                diff = fabs(u[j + i * (M + 1)] - tmp);
                max = diff > max ? diff : max;
            }
        }

        std::string ask_interrupt = "";
        if (argc > 2 && (iter % ask_iter) == 0) {

            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);

            timeval timeout;
            timeout.tv_sec = 1; // A second timeout
            timeout.tv_usec = 0;

            ask_interrupt = " -- > Press Enter to stop";
            int rc = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout);
            if (rc < 0) {
                perror("select");
            } else if (rc != 0) {
                write_to_file(u, file_ans, N, M, is_finished);
                return 0;
            }
        }

        std::cout << " -> max difference after " << iter
                  << " iterations: " << max << " " << ask_interrupt
                  << std::flush;

        if (max <= eps) {
	        is_finished = true;

            using namespace matplot;
            auto [X, Y] = meshgrid(linspace(0, a, N + 1), linspace(0, b, M + 1));
            auto Z = transform(X, Y, [](double x, double y) {
                value = u_next(N, M, u);
                return value;
            });
            surf(X, Y, Z);
            colorbar();

            show();

            /* save plot 
             * formats: svg, eps, gif, jpg, html, txt, tex
             */
            /*
            std::string figure_name = std::to_string(N + 1) + "figure.jpg";
            save(figure_name);
            */

            write_to_file(u, file_ans, N, M, is_finished);
            break;
        }
    }

    free(xi);
    free(yi);

    mmap_free(file_ans);

    return 0;
}
