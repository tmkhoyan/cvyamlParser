/* BSD 3-Clause License
 *  
 *  Copyright (c) 2019, tmkhoyan (Tigran Mkhoyan)
 *  All rights reserved.
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Description: 	Helper function to generate yaml dataset in order to run bencmark with cvyamlParser
 * 					example compile: g++ -std=c++11 genyamlData.cpp -o genyamlData -I /usr/local/include/opencv4 -lopencv_core
 *					then run with: genyamlData [outout_path_of_yaml]
 *
 * Author: 			Tigran Mkhoyan
 * Email : 			t.mkhoyan@tudelft.nl
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace cv;

cv::FileStorage fs;
const std::string flname="test_data.yaml"; 

template <typename T>
inline void writeVecToFile(const std::vector<T> &v,const std::string &basename){
	int k = 0;
	std::for_each(v.begin(),v.end(),
		[&k,&basename](const T &mat){fs << (basename+std::to_string(k++)) << mat;});
}


int main(int argc, const char **argv){

std::string flname_out = argc>1? argv[1] : flname;

// open file storate writer
fs.open(flname_out, cv::FileStorage::WRITE);

std::vector<cv::Mat> matsA(5,cv::Mat(1000,3,CV_64FC1)); //double
std::vector<cv::Mat> matsB(5,cv::Mat(2000,3,CV_32FC1)); //double
std::vector<cv::Mat> matsC(5,cv::Mat(2000,3,CV_32S  )); //int
// std::vector<cv::Mat> matsD(5,cv::Mat(20,3,CV_16S  )); //short
// std::vector<cv::Mat> matsE(5,cv::Mat(20,3,CV_16U  )); //ushort
// std::vector<cv::Mat> matsF(5,cv::Mat(20,3,CV_8U  )); //uchar 
// std::vector<cv::Mat> matsG(5,cv::Mat(20,3,CV_8S  )); //char

// fill marices with uniformly distributed random double data
for(auto mat: matsA)
	randu(mat, Scalar(10.0), Scalar(50.0));

// fill marices with normally distributed random float data
for(auto mat: matsB)
	randn(mat, Scalar(10.0), Scalar(50.0));

// fill marices with normally distributed random float data
for(auto mat: matsC)
	randn(mat, Scalar(1), Scalar(5));

//genererate vector 
std::vector<double> vecA(matsA[0].rows*matsA[0].cols,0.0);
std::vector<float> 	vecB(matsB[0].rows*matsB[0].cols,0.0);
std::vector<int> 	vecC(matsC[0].rows*matsC[0].cols,0);
// std::vector<short> 	vecC(matsD[0].rows*matsD[0].cols,0);
// std::vector<char> 	vecC(matsF[0].rows*matsF[0].cols,0);

//generate single storage types
std::string myString 	= "Brown Vox";
int myInt				= 10;
float myFloat			= 10.2;
double myDouble			= 10.3;

std::vector<double> sig1(10000);
std::vector<double> sig2(10000);
std::vector<double> sig3(10000);

//unroll first matrix to vector
memcpy((unsigned char*)vecA.data(),(unsigned char*)matsA[0].data,sizeof(double)*matsA[0].rows*matsA[0].cols);
memcpy((unsigned char*)vecB.data(),(unsigned char*)matsB[0].data,sizeof(float )*matsB[0].rows*matsB[0].cols);
memcpy((unsigned char*)vecC.data(),(unsigned char*)matsC[0].data,matsC[0].elemSize()*matsC[0].rows*matsC[0].cols);

//write vector of matrices of data
writeVecToFile(matsA,"matA");
writeVecToFile(matsB,"matB");
writeVecToFile(matsC,"matC");



// noisy sine wave data
std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<double> udist1(0,.9);   std::random_device rand_dev1; std::mt19937 rand_engine1(rand_dev1()); 
std::uniform_real_distribution<double> udist2(0, 1.1); std::random_device rand_dev2; std::mt19937 rand_engine2(rand_dev2()); 
std::uniform_real_distribution<double> udist3(0, 1.7); std::random_device rand_dev3; std::mt19937 rand_engine3(rand_dev3()); 

double rads; int k = 0;

k = 0; for(auto & x : sig1){ rads = 8.0*M_PI*k++/sig1.size(); x = std::sin(rads+udist1(rand_engine1)); }
k = 0; for(auto & x : sig2){ rads = 4.0*M_PI*k++/sig1.size(); x = std::sin(rads+udist2(rand_engine2)); }
k = 0; for(auto & x : sig3){ rads = 3.0*M_PI*k++/sig1.size(); x = std::sin(rads+udist3(rand_engine3)); }


cout<< "storing data to: " << flname_out << endl;
//write vector of data
// fs << "vecA" 		<< vecA;
// fs << "vecB" 		<< vecB;
// fs << "vecC" 		<< vecC;

fs << "myString" 	<< myString;
fs << "myInt   "	<< myInt;
fs << "myFloat "	<< myFloat;
fs << "myDouble" 	<< myDouble;

fs << "sig1" 	<< cv::Mat(1,sig1.size(),CV_64FC1,sig1.data());
fs << "sig2" 	<< cv::Mat(1,sig2.size(),CV_64FC1,sig2.data());
fs << "sig3" 	<< cv::Mat(1,sig3.size(),CV_64FC1,sig3.data());



	return 0;
}
