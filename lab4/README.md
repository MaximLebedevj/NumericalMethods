## Run lab entirely written in Python
```sh
$ pip install pipenv
$ pipenv install
$ python lab4.py
```

## Run lab with C++ calculations and Python visualization
```sh
$ pip install pipenv
$ pipenv install
$ g++ lab4_ram.cpp (lab4_mmap.cpp) -march=native -mtune=native -m64 -O3 -ffast-math -funroll-loops -o lab4 
$ ./lab4 'SIZE' 'ITER_TO_ASK' 
$ python plot.py
$ python show.py 'SIZE'.fig.pickle 
```
where 'ITER_TO_ASK' is an optional parameter with
which after each 'ITER_TO_ASK' iteration it will be asked for 1 second 
whether we want to interrupt the process.

### RAM and mmap mode
There are two modes: <br />
1). RAM mode - all calculations are performed in RAM -> the amount of RAM limits the maximum matrix size that can be calculated. <br />
2). mmap mode - all calcualtions are performed in storage devices (HDD, SSD) -> the maximum matrix size is limited by storage device 
capacity. <br /> ATTENTION: due to frequent overwriting of the ans.txt file, mmap mode can quickly reduce write cycle of storage device.

### Continue mode
During long-term calculations, it is possible to interrupt the process
and continue from the last state using the parameter 'continue':
```sh
$ ./lab4 continue 'ITER_TO_ASK'
```
