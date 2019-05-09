# cvyamlParser
A simple yaml parser for cv datatypes to Matlab using the new Matlab C++ Data API.
The mexfile readcvYaml can be used to transfer data efficiently from OpenCV to Matlab. 

## Installation 

### 1: go to mex folder

```bash
	cd mex/
```

### 2: invoke mex command with optimization flags:

It's not necessary but goo to have, by default optimisation level2 is used. 
 Make sure to link with the open cv library and include paths:

```bash
mex -v COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml.cpp -I [path_to_includes] -L [path_to_lib]
```
e.g.:

```bash
mex -v COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml.cpp -I/usr/local/include/opencv4 -L/usr/local/lib/ -lopencv_core
```
If mex was successful a verbose message will be printed in the console.
	
### 3: add mex path to matlab path variable:

You do this the easiest by navigating to mex folder and calling:

```bash 
addpath(pwd); savepath;
```
From now the readcvYaml mex function should be accessible from any path in you matlab environment

### 4: Licence 
Please refer to the licence file for information about code distribution, usage and copy rights. The code is provided under BSD 3-Clause License


## Usage


## Benchmarking
 A benchmark results are provided for linux and osx platforms in folders linux and osx. The benchmark test were perfomed on standard Dell Optiplex 7400 and 2,3 GHz Intel Core i5 16G macbook, respectively. 

 To run the benchmark follow the steps:

### 5: generate test data to test the function:

Go to src folder and compile genyamlData:

```bash
cd src
g++ -std=c++11 genyamlData.cpp -o genyamlData -I [path_to_cv_includes] [opencv_core_lib]
```
e.g.: 

```bash
g++ -std=c++11 genyamlData.cpp -o genyamlData -I /usr/local/include/opencv4 -lopencv_core
```
Then run with: 

```bash
genyamlData [outout_path_of_yaml] 
```
e.g.: 

```bash
genyamlData ../data/test_data.yaml
```

### 6: Run benchmark on you own pc:
In folder benchmark a simple script is provided to run readcvYaml on your own data.
Simply choose the number of iterations with N parameter and run benchmarktest_cvYaml.m



