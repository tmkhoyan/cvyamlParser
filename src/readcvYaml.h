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
 * Description: 	header file for cvYamlParser 
 * Revision : 		v2.1 (Jan 2021)
 * 
 * Author: 			Tigran Mkhoyan
 * Email : 			t.mkhoyan@tudelft.nl
 */

#include "mex.hpp"
#include "mexAdapter.hpp"
#include "opencv2/opencv.hpp"
#include <memory>
#include <filesystem>

// #include <variant>

using matlab::mex::ArgumentList;
using namespace matlab::data;


// 	/* ----------------------------------------------  serach and conversion helpers  -----------------------------------------------------*/
template <typename T>
inline std::map<T,unsigned int> findUniqueMap(const std::vector <T> &v){
	std::map<T, unsigned int> nameMap;
	std::for_each(std::begin(v), std::end(v),
			[&nameMap](const T& s) { ++nameMap[s];});
	return nameMap;
}
template <typename T>
void getBasenames(const std::vector <T> &names, std::vector <T> &basename, std::vector <T> &remainder){
	std::for_each(names.begin(),names.end(),
			[&basename,&remainder](const T& name){
			size_t last_char_pos = name.find_last_not_of("0123456789"); //lambda can have multiple lines!
			basename.push_back(name.substr(0,last_char_pos+1));
			remainder.push_back(name.substr(last_char_pos+1));});
	//or 
	// for(auto name : names){
	// 	size_t last_char_pos = name.find_last_not_of("0123456789");
	// 	basename.push_back(name.substr(0,last_char_pos+1));
	// 	remainder.push_back(name.substr(last_char_pos+1));
	// }
}

/* ----------------------------------------------  filenode  helpers  -----------------------------------------------------*/

std::vector <std::string> getNodeKeys(const cv::FileNode &fn){
	int k = 0;
	std::vector < std::string > fnKeys; //filenode keys 
	std::vector < std::string > fnArrayKeys; //filenode keys 
	std::vector < std::string > fnNoArrayKeys; //filenode keys 
	std::vector < int > fnIsmapIdx; //filenode keys 
	std::vector < int > fnIsNotmapIdx; //filenode keys 

	for(auto nodes: fn){
		fnKeys.push_back(nodes.name());
		if(nodes.isMap()){ // assume all maps are matrices 
			fnIsmapIdx.push_back(k);
			fnArrayKeys.push_back(nodes.name());

		} else{
			fnIsNotmapIdx.push_back(k);
			fnNoArrayKeys.push_back(nodes.name());
		}
		k++;
	}
	return fnKeys;
}
/* ----------------------------------------------  utility helpers  -----------------------------------------------------*/
void displayALLNodeAttributes(const cv::FileStorage &fs){

	cv::FileNode fn = fs.root(); // ge
	for(auto p : fn.keys())
		std::cout << "keys LL: " << p << "\n";

	for(auto nodes: fn){
		std::cout << "key: " << nodes.name() << ", type: " << nodes.type() << "\n"
			<< "nested keys:" << "\n";
		std::cout   << "\t" << "INT 		:" << ( nodes.type()==cv::FileNode::INT   ) << "\n"
			<< "\t" << "REAL 	:" << ( nodes.type()==cv::FileNode::REAL  ) << "\n"  
			<< "\t" << "float 	:" << ( nodes.type()==cv::FileNode::FLOAT ) << "\n" 
			<< "\t" << "STR 		:" << ( nodes.type()==cv::FileNode::STR   ) << "\n" 
			<< "\t" << "STRING 	:" << ( nodes.type()==cv::FileNode::STRING) << "\n"
			<< "\t" << "MAP 		:" << ( nodes.type()==cv::FileNode::MAP   ) << "\n"
			<< "\t" << "SEQ 		:" << ( nodes.type()==cv::FileNode::SEQ   ) << "\n";
		try{
			for(auto p : nodes.keys())
				std::cout << "\t" << "ikey: " << p << std::endl;
		} 
		catch (const std::exception& e){
			// cout << "\t" << e.what() << endl;
			std::cout << "\t" << "no inner nodes present" << std::endl;
		}

	}
}
/* ----------------------------------------------  testing  -----------------------------------------------------*/
// testimg
matlab::data::StructArray putStructArray() {
	using namespace matlab::engine;
	// Create MATLAB data array factory
	matlab::data::ArrayFactory factory;

	// Define 2-element struct array with two fields per struct
	auto structArray = factory.createStructArray({ 1, 2}, { "f1", "f2" });

	// Assign values to each field in first struct
	structArray[0]["f1"] = factory.createCharArray("First Data Set");
	structArray[0]["f2"] = factory.createArray<uint8_t>({ 1, 3 }, { 1, 2, 3 }); //1,3 array

	// Assign values to each field in second struct
	structArray[1]["f1"] = factory.createCharArray("Second Data Set");
	structArray[1]["f2"] = factory.createArray<double>({ 1, 5 }, { 4., 5., 6., 7., 8. });

	return structArray;
}
// define custom make unique if only c++11 compiler support need modifications in line 273
#if __cplusplus < 201402L
	template<typename T, typename... Args>
std::unique_ptr<T> mymake_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif