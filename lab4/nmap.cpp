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

int main() {
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
    size_t fsize = (size_t)st.st_size;

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


    munmap(mapping->dataPtr, mapping->fsize);
    close(mapping->fd);
    free(mapping);

    return 0;
}
