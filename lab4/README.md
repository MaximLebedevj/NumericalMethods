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
$ g++ lab4.cpp -march=native -mtune=native -m64 -O3 -ffast-math -funroll-loops -o lab4 
$ ./lab4 'SIZE' 'ITER_TO_ASK' 
$ python plot.py
$ python show.py 'SIZE'.fig.pickle 
```
where 'ITER_TO_ASK' is an optional parameter with
which after each 'ITER_TO_ASK' iteration it will be asked for 1 second 
whether we want to interrupt the process.

### Continue mode
During long-term calculations, it is possible to interrupt the process
and continue from the last state using the parameter 'continue':
```sh
$ ./lab4 continue 'ITER_TO_ASK'
```
