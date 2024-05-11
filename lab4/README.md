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
$ ./lab4 'SIZE'  
$ python plot.py
$ python show.py 'SIZE'.fig.pickle 
```

### Continue mode
During long-term calculations of large matrices, it is possible to interrupt the process
and continue from the last state using the parameter 'continue':
```sh
$ ./lab4 continue
```
