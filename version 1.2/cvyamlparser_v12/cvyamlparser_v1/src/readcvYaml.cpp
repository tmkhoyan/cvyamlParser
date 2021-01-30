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
  * Description: 	This code will run a simple benchmark test to parse cv. matrix from yaml file and output it to matlab
  * 					compile example mex readcvYaml.cpp -I/usr/local/include/opencv4
  *            											   -L/usr/local/lib/ -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_imgcodecs
  * 					example usage: 	readcvYaml('test.yaml');
  *
  * Author: 			Tigran Mkhoyan
  * Email : 			t.mkhoyan@tudelft.nl
  */

#include "mex.hpp"
#include "mexAdapter.hpp"
#include "opencv2/opencv.hpp"
#include "helper.h"

using matlab::mex::ArgumentList;
using namespace matlab::data;

/* ----------------------------------------------matlab data IO class -----------------------------------------------------*/

class MexFunction : public matlab::mex::Function {
	// Pointer to MATLAB engine to call fprintf
	std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
	// Factory to create MATLAB data arrays
	ArrayFactory factory;
	// Create an output stream
	std::ostringstream stream;
	cv::FileStorage fs;
	int k = 0;

public:

	/* ---------------------------------------------- body  -----------------------------------------------------*/
	void operator()(ArgumentList outputs, ArgumentList inputs) {

		if (inputs.empty())
			erroOnMatlab("No input file provided. Usage: readcvYaml([input file],[unsorted || sorted ]");

		const CharArray flname = inputs[0];
		bool SORT_UNIQUE = false;

		if (inputs.size() == 2) {
			const CharArray opt1 = inputs[1];
			SORT_UNIQUE = (opt1.toAscii() == "sorted");
		}

		fs.open(flname.toAscii(), cv::FileStorage::READ); // open file storage object and node
		cv::FileNode fn = fs.root(); // get the root key

		//containers for fn types
		std::vector < std::string > fnKeys = fn.keys(); //filenode keys 
		std::vector < std::string > fnIgnoredKeys; //filenode keys 

		// vector to push back the struct array later
		std::vector<matlab::data::StructArray> S_OUT;

		//data containers to hold unique keys
		std::map<std::string, unsigned int> keyCounterMap;  // this map is needed to increment keyBasenameMap
		std::map<std::string, unsigned int> keyBasenameMap; // this map id needed to get number of unique key Basenames
		std::map<std::string, matlab::data::StructArray> keyStructMap; // this map stores the unique structures with unique key labels
		std::vector<std::string> basename;
		std::vector<std::string> remainder;
		std::vector<std::string> uniquename;

		if (SORT_UNIQUE) {
			//get basenames and the remainder
			getBasenames(fn.keys(), basename, remainder);
			//get unique map of basenames
			keyBasenameMap = findUniqueMap(basename);  //map with unique keys ands occurences
			uniquename = pushMaptoVector(keyBasenameMap); // unique vector of keys since needed to construct the structure
			std::vector<matlab::data::StructArray> basenameStructs; //to hold the structures with unique keys

			//create structure to store the data with unique keys (fieldnames)
			for (auto&& indexmap : keyBasenameMap) {
				//make pair needs a string and struct pair. since probably no defaukt constructor is present for structure array we have tyo use insert. std::strings is needed since indexmap.first is const
				// keyStructMap.insert(std::make_pair<std::string, matlab::data::StructArray>(std::string(indexmap.first),factory.createStructArray({1,indexmap.second},{indexmap.first}))); //this will not include the index
				keyStructMap.insert(std::make_pair<std::string, matlab::data::StructArray>(std::string(indexmap.first), factory.createStructArray({ 1,indexmap.second }, { indexmap.first,"index" })));
			}
		}
		//in matlab struct looks like
		//eg: out.basename(1).basename = [cv::Mat]
		//    out.basename(1).index    = stripped name

		//fill structure array with data
		/* ----------------------------------------------  no sorting -----------------------------------------------------*/
		if (!SORT_UNIQUE) {
			//initiate the structure 
			matlab::data::StructArray S = factory.createStructArray({ 1,1 }, fnKeys);
			// S = factory.createStructArray({1,1},fn.keys());

			for (auto nodes : fn) {
				std::string key = nodes.name(); // when unique sorting this will contain stripped basenames
				switch (nodes.type()) {
				case cv::FileNode::INT: {
					int var;
					fs[key] >> var;
					// S[0][key] = var;
					S[0][key] = factory.createArray<int>({ 1,1 }, { var });
					break;
				}
				case cv::FileNode::FLOAT: { // or FileNode::REAL
					float var;
					fs[key] >> var;
					// S[0][key] = var;
					S[0][key] = factory.createArray<float>({ 1,1 }, { var });
					break;
				}
				case cv::FileNode::STRING: { // or FileNode::STRING
					std::string var;
					fs[key] >> var;
					S[0][key] = factory.createCharArray(var);
					break;
				}
										 //TODO: allow retun other types.  But not really interesting for matlab ?
				case cv::FileNode::SEQ: {
					// if map create matrix --> read from yaml --> transpose now needed (works only for 1 d vector!)
					std::vector<double> vec; fs[key] >> vec;
					S[0][key] = vec2TypedArray(vec); // for vector just pass vector, we know the type already
					break;
				}
				case cv::FileNode::MAP: {
					// if map create matrix --> read from yaml --> transpose to get correct ordering row major
					cv::Mat mat; fs[key] >> mat; mat = mat.t();
					// std::cout<< key << ": "<< mat.type() <<  mat.isContinuous()<< std::endl;
					switch (mat.type()) {
						// case CV_32S : case CV_8U : case CV_8S : case CV_16U : case CV_16S : {
					case CV_8U: case CV_8S: {
						int8_t vartype;
						S[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_16U: case CV_16S: {
						short vartype;
						S[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32S: {
						int vartype;
						S[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32F: {
						float vartype;
						S[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					default: { //CV_64F
						double vartype;
						S[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					}
					break;
				}
				case cv::FileNode::EMPTY: {
					std::cout << "Node: " << key << "is empty. Proceeding to next node." << std::endl;
					fnIgnoredKeys.push_back(key);
					break;
				}
										//FileNode::TYPE_MASK , FileNode::FLOW , FileNode::UNIFORM, FileNode::NAMED
				default: { //
						  //ignore store the index as ignored
					fnIgnoredKeys.push_back(key);
				}
				}

			}
			S_OUT.push_back(S); //store to return to output later
		} // if no sorting unique required
		/* ----------------------------------------------  unique sorting -----------------------------------------------------*/
		else {
			//sort and create ordered structure
			std::cout << "Sorting unique keys." << std::endl;
			//create structure containers
			// matlab::data::StructArray S = factory.createStructArray({1,1},uniquename); //output array
			matlab::data::StructArray S = factory.createStructArray({ 1,1 }, pushMaptoVector(keyBasenameMap)); //output array
			//temporary arrays

			k = 0;
			for (auto nodes : fn) {
				std::string key = nodes.name(); // this is the original key in yaml file 
				std::string baseKey = basename[k]; // when unique sorting this will contain stripped basenames
				std::string strippedKey = remainder[k]; // when unique sorting this will contain stripped basenames
				int indexCounter = ++keyCounterMap[baseKey] - 1; //index counter is not zetro indexed! it counts the number of uccurences so make zero indexed

				switch (nodes.type()) {
				case cv::FileNode::INT: {
					int var;
					fs[key] >> var;
					keyStructMap.at(baseKey)[indexCounter][baseKey] = factory.createArray<int>({ 1,1 }, { var });//contains these two elements 
					keyStructMap.at(baseKey)[indexCounter]["index"] = factory.createCharArray(strippedKey); // index is provided to keep track of original stripped name the numbering mey be non uniform

					// S[0][key] = var;
					// S[indexCounter][baseKey] = factory.createArray<int>({1,1},{var});
					break;
				}
				case cv::FileNode::FLOAT: { // or FileNode::REAL
					float var;
					fs[key] >> var;
					keyStructMap.at(baseKey)[indexCounter][baseKey] = factory.createArray<float>({ 1,1 }, { var });
					keyStructMap.at(baseKey)[indexCounter]["index"] = factory.createCharArray(strippedKey);

					// S[0][key] = var;
					// S[indexCounter][baseKey] = factory.createArray<float>({1,1},{var});

					break;
				}
				case cv::FileNode::STRING: { // or FileNode::STRING
					std::string var;
					fs[key] >> var;
					keyStructMap.at(baseKey)[indexCounter][baseKey] = factory.createCharArray(var);
					keyStructMap.at(baseKey)[indexCounter]["index"] = factory.createCharArray(strippedKey);

					// S[indexCounter][baseKey] = factory.createCharArray(var);
					break;
				}
				case cv::FileNode::SEQ: {
					// if map create matrix --> read from yaml --> transpose now needed (works only for 1 d vector!)
					std::vector<double> vec; fs[key] >> vec;
					keyStructMap.at(baseKey)[indexCounter][baseKey] = vec2TypedArray(vec); // for vector just pass vector, we know the type already
					keyStructMap.at(baseKey)[indexCounter]["index"] = factory.createCharArray(strippedKey);
					break;
				}
				case cv::FileNode::MAP: { //types // CV_8U, CV_8S, CV_16U, CV_16S, CV_32S, CV_32F or CV_64F
								// if map create matrix --> read from yaml --> transpose to get correct ordering row major
					cv::Mat mat; fs[key] >> mat; mat = mat.t();
					switch (mat.type()) {
					case CV_8U: case CV_8S: {
						int8_t vartype;
						keyStructMap.at(baseKey)[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_16U: case CV_16S: {
						short vartype;
						keyStructMap.at(baseKey)[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32S: {
						int vartype;
						keyStructMap.at(baseKey)[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32F: {
						float vartype;
						keyStructMap.at(baseKey)[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					default: { //CV_64F
						double vartype;
						keyStructMap.at(baseKey)[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					}

					keyStructMap.at(baseKey)[indexCounter]["index"] = factory.createCharArray(strippedKey);

					break;
				}
				case cv::FileNode::EMPTY: {
					std::cout << "Node: " << key << "is empty. Proceeding to next node." << std::endl;
					fnIgnoredKeys.push_back(key);
					break;
				}
										//FileNode::TYPE_MASK , FileNode::FLOW , FileNode::UNIFORM, FileNode::NAMED
				default: { //
						  //ignore store the index as ignored
					fnIgnoredKeys.push_back(key);
				}
				} // end case 
				k++;
			} // end for loop keys
			//now we need to fill the structure with unique multidim structures
			for (auto keystruct : keyStructMap)
				S[0][keystruct.first] = keystruct.second; //returns pair<unique name, struct> 
			S_OUT.push_back(S); //store to return to output later
		} //end else apply sorting

		std::cout << "Ignored keys: " << std::endl;
		for (auto key : fnIgnoredKeys)
			std::cout << "\t" << key << std::endl;

		/* ----------------------------------------------  output -----------------------------------------------------*/

		std::cout << S_OUT.size() << std::endl;
		k = 0;
		for (auto S : S_OUT)
			outputs[k++] = S;

	}

	/* ----------------------------------------------  helper  -----------------------------------------------------*/

	void displayOnMATLAB(std::ostringstream& stream) {
		// Pass stream content to MATLAB fprintf function
		matlabPtr->feval(u"fprintf", 0,
			std::vector<Array>({ factory.createScalar(stream.str()) }));
		// Clear stream buffer
		stream.str("");
	}
	void erroOnMatlab(std::string s) {
		matlabPtr->feval(u"error", 0,
			std::vector<matlab::data::Array>({ factory.createScalar(s) }));
	}
	//will create a sorted unique map of anything
	template <typename T>
	inline std::map<T, unsigned int> findUniqueMap(const std::vector <T>& v) {
		std::map<T, unsigned int> nameMap;
		std::for_each(std::begin(v), std::end(v),
			[&nameMap](const T& s) { ++nameMap[s]; });
		return nameMap;
	}
	template <typename T>
	void getBasenames(const std::vector <T>& names, std::vector <T>& basename, std::vector <T>& remainder) {
		std::for_each(names.begin(), names.end(),
			[&basename, &remainder](const T& name) {
				size_t last_char_pos = name.find_last_not_of("0123456789"); //lambda can have multiple lines!
				basename.push_back(name.substr(0, last_char_pos + 1));
				remainder.push_back(name.substr(last_char_pos + 1)); });
		//or 
		// for(auto name : names){
		// 	size_t last_char_pos = name.find_last_not_of("0123456789");
		// 	basename.push_back(name.substr(0,last_char_pos+1));
		// 	remainder.push_back(name.substr(last_char_pos+1));
		// }
	}
	//push map into vector
	template <typename T, typename S>
	inline 	std::vector<T> pushMaptoVector(const std::map<T, S> map_in) {
		std::vector <T> vec;
		std::for_each(map_in.begin(), map_in.end(),
			[&vec](std::pair<std::string, unsigned int> index) {vec.push_back(index.first); });
		return vec;
	}
	template <typename T>
	inline matlab::data::TypedArray<T> cvMat2TypedArray(T vartype, const cv::Mat& mat) { //vartuype is a hack to make template work
		matlab::data::TypedArray<T> tmpArray = factory.createArray<T>({ static_cast<unsigned long>(mat.cols),
				static_cast<unsigned long>(mat.rows) }); //temp array to store
		memcpy(&*tmpArray.begin(), (unsigned char*)mat.data, sizeof(T) * tmpArray.getNumberOfElements()); // unsigned char *pdata = (unsigned char*)tmp.data; //get pointer to cv mat
		return tmpArray;
	}
	template <typename T>
	inline matlab::data::TypedArray<T> vec2TypedArray(const std::vector<T>& vec) {
		matlab::data::TypedArray<T> tmpArray = factory.createArray<T>({ static_cast<unsigned long>(1),
				static_cast<unsigned long>(vec.size()) }); //temp array to store
		memcpy(&*tmpArray.begin(), (unsigned char*)vec.data(), sizeof(T) * tmpArray.getNumberOfElements()); // unsigned char *pdata = (unsigned char*)tmp.data; //get pointer to cv mat
		return tmpArray;
	}
};
