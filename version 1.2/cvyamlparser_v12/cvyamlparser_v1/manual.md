1: go to mex folder
	cd mex

2: invoke mex command with optimization flags:
 	-It's not necessary but goo to have, by default optimisation level2 is used. 
 	-Make sure to link with the open cv library and include paths:
		mex -v COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml.cpp -I [path_to_includes] -L [path_to_lib]

	-Example:
		mex -v COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml.cpp -I/usr/local/include/opencv4 -L/usr/local/lib/ -lopencv_core
	-If mex was successful a verbose message will be printed in the console.
	
    -For WIN:
        mex -v COMPFLAGS="$COMPFLAGS /std:c++11" COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" "../src/readcvYaml.cpp" -I"F:\opt\OpenCV-4.5.1\opencv\build\include" -L"F:\opt\OpenCV-4.5.1\opencv\build\x64\vc15\lib" -lopencv_world451 -lopencv_world451d
3: add mex path to matlab path variable:
	-you do this the easiest by navigating to mex folder and calling 
	-addpath(pwd); savepath;
	-From now the readcvYaml mex function should be accessible from any path in you matlab environment

4: Please refer to the licence file for information about code distribution, usage and copy rights. The code is provided under BSD 3-Clause License

-------------------------------------optional--------------------------------------

5: generate test data to test the function:
	go to src folder and compile genyamlData:
		cd src
		g++ -std=c++11 genyamlData.cpp -o genyamlData -I [path_to_cv_includes] [opencv_core_lib]

	-Example: g++ -std=c++11 genyamlData.cpp -o genyamlData -I /usr/local/include/opencv4 -lopencv_core
 	 
 	-Then run with: genyamlData [outout_path_of_yaml] 
 	-Example run:   genyamlData ../data/test_data.yaml

6: Run benchmark on you own pc:
	-In folder benchmark a simple script is provided to run readcvYaml on your own data
	-Simply choose the number of iterations with N parameter and run benchmarktest_cvYaml.m

