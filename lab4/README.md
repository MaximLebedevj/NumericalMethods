## Run lab entirely written in Python
```sh
$ pip install pipenv
$ pipenv install
$ python3 lab4.py
```

## Run lab with C++ calculations and Python visualization
```sh
$ pip install pipenv
$ pipenv install
$ g++ lab4.cpp -march=native -mtune=native -m64 -O3 -ffast-math -funroll-loops -o lab4 
$ ./lab4 'SIZE' && python plot.py
```
