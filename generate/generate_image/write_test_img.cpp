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
 * Description: 	Write image to yaml as multichannel test data for cvyamlParser
 * 					compile with make
 *            										
 * 					example usage: 	make && ./write_test_img [input] [output]
 * Revision : 		v2.1 (Jan 2021)
 * 
 * Author: 			Tigran Mkhoyan
 * Email : 			t.mkhoyan@tudelft.nl
 */

#include <iostream>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;



int main (int argc, char** argv){

	auto img_path  = (argc > 1)? std::string(argv[1]) : "img.tiff";
	auto file_out  = (argc > 2)? std::string(argv[2]) : "multi_channel.yml";

	//create writer
	FileStorage fs(file_out, FileStorage::WRITE);

	auto img = imread(img_path, IMREAD_COLOR);

	imshow("img read",img);

	fs << "img" << img;

	auto img_ = cv::Mat();

	fs["img"]>>img_; 

	imshow("img written",img);

	cv::waitKey(5000);

	return 0;

}
