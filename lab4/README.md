## Run lab entirely written in Python
```sh
pipenv install
python3 lab4.py
```

## Run lab with C++ calculations and Python visualization
```sh
pipenv install
clang++ lab4.cpp -std=c++11 -O3 && ./a.out && python3 lab4_plot.py
```
