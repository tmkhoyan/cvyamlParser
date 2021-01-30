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
  * Revision : 		v2.1 (Jan 2021)
  *
  * Author: 			Tigran Mkhoyan
  * Email : 			t.mkhoyan@tudelft.nl
  */

#include "readcvYaml.h"
#include <type_traits>

  // globals
std::string defaultDelim;
std::vector < std::string > fnIgnoredKeys; //filenode keys 

/* ----------------------------------------------matlab data IO class -----------------------------------------------------*/

class MexFunction : public matlab::mex::Function {
	// Pointer to MATLAB engine to call fprintf
	std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
	// Factory to create MATLAB data arrays
	ArrayFactory factory;
	// Create an output stream
	std::ostringstream stream;

	int k = 0;

public:
	/* ---------------------------------------------- body  -----------------------------------------------------*/
	void operator()(ArgumentList outputs, ArgumentList inputs) {

		if (inputs.empty())
			erroOnMatlab("No input file provided. Usage: readcvYaml([input file],[unsorted || sorted ],[strip{strips delimiter _]");

		/* --------------- check if file exhistst ------------ */
		bool isSortUnique = false;
		defaultDelim = "_"; // must initialize
		fnIgnoredKeys.clear(); // may be needed as matlab hold the ref to data
		stream.clear();
		// SOUT.clear();
		// vector to push back the struct array later
		std::vector<matlab::data::StructArray> SOUT;
		cv::FileStorage fs;

		const CharArray flname = inputs[0];
		// std::string flnamestr = flname.toAscii();
		std::filesystem::path flpath = flname.toAscii();


		if (inputs.size() >= 2) {
			const CharArray opt = inputs[1];
			isSortUnique = (opt.toAscii() == "sorted");
		}

		if (inputs.size() >= 3) {
			const CharArray opt = inputs[2];
			defaultDelim = std::string(opt.toAscii());
			if (opt.toAscii() == "none") defaultDelim = "";
		}

		/* -------------------------------- check if file exhistst ------------------------------------------*/

		flpath = assertFilepath(flpath);

		fs.open(flpath.u8string(), cv::FileStorage::READ); // open file storage object and node
		cv::FileNode fnroot = fs.root(); // get the root key

		//containers for fn types
		std::vector < std::string > fnKeys = fnroot.keys(); //filenode keys 

		//data containers to hold unique keys
		std::map<std::string, unsigned int> keyCounterMap;  // this map is needed to increment keyBasenameMap
		std::map<std::string, unsigned int> keyBasenameMap; // this map id needed to get number of unique key Basenames

		//in matlab struct looks like
		//eg: out.basename(1).basename = [cv::Mat,data]
		//    out.basename(1).index    = stripped name

		/* ---------------------------------------------- run parser   -----------------------------------------------------*/
		//enter recursive function runParser. Everything happens here. It will recursively parse out all nested levels of nodes!
		if (!isSortUnique) {
			matlab::data::StructArray StructUnsorted = runParser(fnroot);
			SOUT.push_back(StructUnsorted);  //store to return to output later
		}
		else
		{
			// parser call recursively itself with function overloading
			matlab::data::StructArray StructSorted = runParser(fnroot, keyBasenameMap, keyCounterMap);
			SOUT.push_back(StructSorted);  //store to return to output later
		}

		// fill ignored keys if any
		if (!fnIgnoredKeys.empty()) {
			std::cout << "Ignored keys: " << std::endl;
			for (auto key : fnIgnoredKeys)
				std::cout << "\t" << key << std::endl;
		}
		/* ----------------------------------------------  output -----------------------------------------------------*/
		k = 0;
		for (const auto& S : SOUT)
			outputs[k++] = S;

	}
	/* ----------------------------------------------  helper functions  -----------------------------------------------------*/

	/* ---------------------------------------------- recursive worker function no sorting -----------------------------------------------------*/
	matlab::data::StructArray runParser(cv::FileNode fn) {
		// fn = fs.root();
		auto mapKeys = fn.keys();
		if (!defaultDelim.empty()) { // if dselimiter specified strip it. Note that nested maps kannot have numerical nested nodes (e.g. 1,2,3)
			for (auto& key : mapKeys) {
				std::stringstream ss(key.substr(key.find(defaultDelim) + 1)); //strip delimiters like '_'
				ss >> key;
			}
		}

		matlab::data::StructArray Sunsorted = factory.createStructArray({ (int)1,1 }, mapKeys);
		if (fn.empty()) { return Sunsorted; }

		int k = 0;
		for (cv::FileNodeIterator current = fn.begin(); current != fn.end(); ++current)
		{
			// fs[key ] = item
			cv::FileNode item = *current; //node element of the current iterator
			std::string key = mapKeys[k++];

			switch (item.type()) {
			case cv::FileNode::INT: {
				int var;
				item >> var; Sunsorted[0][key] = factory.createArray<int>({ 1,1 }, { var });;
				break;
			}
								  // NOTE FLOAT ARE COMVERTED TO MATLAB DOUBLE FOR CONCENIENCE
			case cv::FileNode::FLOAT: { // or FileNode::REAL 
				float var;
				item >> var; Sunsorted[0][key] = factory.createArray<double>({ 1,1 }, { (double)var });;
				break;
			}
			case cv::FileNode::STRING: { // or FileNode::STRING
				std::string var;
				item >> var; Sunsorted[0][key] = factory.createCharArray(var);
				break;
			}
			case cv::FileNode::SEQ: {
				// if map create matrix --> read from yaml --> transpose now needed (works only for 1 d vector!)
				std::vector<double> vec; item >> vec; Sunsorted[0][key] = vec2TypedArray(vec);
				break;
			}
								  // can be a mat or nested structure
			case cv::FileNode::MAP: {
				// // find out if its a opencv mat else a map structure

				if (findElement("dt", item.keys()) &&
					findElement("rows", item.keys()) &&
					findElement("cols", item.keys()) &&
					findElement("data", item.keys())) {
					// if map create matrix --> read from yaml --> transpose to get correct ordering row major
					cv::Mat mat; item >> mat; mat = mat.t();
					// std::cout<< key << ": "<< mat.type() <<  mat.isContinuous()<< std::endl;
					switch (CV_MAT_DEPTH(mat.type())) {
						// case CV_32S : case CV_8U : case CV_8S : case CV_16U : case CV_16S : {
					case CV_8U: case CV_8S: {
						// int8_t vartype;
						uint8_t vartype;
						Sunsorted[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_16U: case CV_16S: {
						short vartype;
						Sunsorted[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32S: {
						int vartype;
						Sunsorted[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32F: {
						float vartype;
						Sunsorted[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					default: { //CV_64F
						double vartype;
						Sunsorted[0][key] = cvMat2TypedArray(vartype, mat);
						break;
					}
					}
				}
				// IF not then extract nested levels. Now the function will call itself recursively!
				else {
					matlab::data::StructArray nestedStructMap = runParser(item);
					Sunsorted[0][key] = nestedStructMap;
				}
				// break case cv::FileNode::MAP 
				break;
			}
			case cv::FileNode::EMPTY: {
				std::cout << "Node: " << key << "is empty. Proceeding to next node." << std::endl;
				fnIgnoredKeys.push_back(key);
				break;
			}
									//FileNode::TYPE_MASK , FileNode::FLOW , FileNode::UNIFORM, FileNode::NAMED
			default: { //
				fnIgnoredKeys.push_back(key); //ignore store the index as ignored
			}
			}
		}
		return Sunsorted;
	}
	/* ---------------------------------------------- parser with unique sorting -----------------------------------------------------*/
	matlab::data::StructArray runParser(cv::FileNode fn, std::map<std::string, unsigned int> keyCounterMap, std::map<std::string, unsigned int> keyBasenameMap) {

		// initialize containers
		std::map<std::string, matlab::data::StructArray> keyStructMap; // this map stores the unique structures with unique key labels
		std::map<std::string, std::unique_ptr<matlab::data::StructArray>> keyStructMapPtr; // this map stores the unique structures with unique key labels

		std::vector<std::string> basename;
		std::vector<std::string> remainder;
		std::vector<std::string> uniquename;

		std::cout << "Sorting unique keys." << std::endl;
		//get basenames and the remainder
		getBasenames(fn.keys(), basename, remainder);
		//get unique map of basenames
		keyBasenameMap = findUniqueMap(basename);  //map with unique keys ands occurences
		uniquename = pushMaptoVector(keyBasenameMap); // unique vector of keys since needed to construct the structure
		/* ----------------------------------sorting engine   ------------------------- */
		matlab::data::StructArray Ssorted = factory.createStructArray({ 1,1 }, pushMaptoVector(keyBasenameMap)); //output array
		if (fn.empty()) { return Ssorted; }

		//create structure to store the data with unique keys (fieldnames). make pair needs a string and struct pair. since probably no default constructor is present for structure array we have tyo use insert. std::strings is needed since indexmap.first is const				
		for (auto&& indexmap : keyBasenameMap) { //first basename , second is size 
			// std::unique_ptr<matlab::data::StructArray> ptr =  std::make_unique<matlab::data::StructArray>(factory.createStructArray({1,indexmap.second},{indexmap.first,"index"}));
			// keyStructMapPtr[std::string(indexmap.first)] = std::move(ptr);
			// or
			keyStructMapPtr[std::string(indexmap.first)] = std::make_unique<matlab::data::StructArray>(factory.createStructArray({ 1,indexmap.second }, { indexmap.first,"index" }));
		}
		//create structure containers
		// Important step! Store data pointers to retrieved data in sorted map
		int k = 0;
		for (auto nodes : fn) {
			//	for (cv::FileNodeIterator current = fn.begin(); current != fn.end(); ++current)
			auto item = nodes;

			std::string key = nodes.name(); // this is the original key in yaml file 
			std::string baseKey = basename[k]; // when unique sorting this will contain stripped basenames
			std::string strippedKey = remainder[k]; // when unique sorting this will contain stripped basenames
			int indexCounter = ++keyCounterMap[baseKey] - 1; //index counter is not zetro indexed! it counts the number of uccurences so make zero indexed

			switch (item.type()) {
			case cv::FileNode::INT: {
				int var;
				item >> var;
				(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = factory.createArray<int>({ 1,1 }, { var }); // pointer to unsorted data
				(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1
				break;
			}
								  // NOTE FLOAT ARE COMVERTED TO MATLAB DOUBLE FOR CONCENIENCE
			case cv::FileNode::FLOAT: { // or FileNode::REAL 
				float var;
				item >> var;
				(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = factory.createArray<double>({ 1,1 }, { (double)var }); // pointer to unsorted data
				(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1
				break;
			}
			case cv::FileNode::STRING: { // or FileNode::STRING
				std::string var;
				item >> var;
				(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = factory.createCharArray(var); // pointer to unsorted data
				(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1
				break;
			}
			case cv::FileNode::SEQ: {
				// if map create matrix --> read from yaml --> transpose now needed (works only for 1 d vector!)
				std::vector<double> vec; item >> vec; //Sunsorted[0][key] = vec2TypedArray(vec);
				(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = vec2TypedArray(vec); // pointer to unsorted data
				(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1

				break;
			}
								  // can be a mat or nested structure
			case cv::FileNode::MAP: {
				// // find out if its a opencv mat else a map structure

				if (findElement("dt", item.keys()) &&
					findElement("rows", item.keys()) &&
					findElement("cols", item.keys()) &&
					findElement("data", item.keys())) {
					// if map create matrix --> read from yaml --> transpose to get correct ordering row major
					cv::Mat mat; item >> mat; mat = mat.t();
					// std::cout<< key << ": "<< mat.type() <<  mat.isContinuous()<< std::endl;
					switch (CV_MAT_DEPTH(mat.type())) {
						// case CV_32S : case CV_8U : case CV_8S : case CV_16U : case CV_16S : {
					case CV_8U: case CV_8S: {
						// int8_t vartype;
						uint8_t vartype;
						(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_16U: case CV_16S: {
						short vartype;
						(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32S: {
						int vartype;
						(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					case CV_32F: {
						float vartype;
						(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					default: { //CV_64F
						double vartype;
						(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = cvMat2TypedArray(vartype, mat);
						break;
					}
					}
					(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1
				}
				// IF not then extract nested levels. Now the function will call itself recursively!
				else {
					matlab::data::StructArray nestedStructMap = runParser(item);
					(*keyStructMapPtr[baseKey])[indexCounter][baseKey] = nestedStructMap;
					(*keyStructMapPtr[baseKey])[indexCounter]["index"] = factory.createCharArray(strippedKey); //sorted stripped index e.g. a_1
				}
				// break case cv::FileNode::MAP 
				break;
			}
			case cv::FileNode::EMPTY: {
				std::cout << "Node: " << key << "is empty. Proceeding to next node." << std::endl;
				fnIgnoredKeys.push_back(key);
				break;
			}
									//FileNode::TYPE_MASK , FileNode::FLOW , FileNode::UNIFORM, FileNode::NAMED
			default: { //
				fnIgnoredKeys.push_back(key); //ignore store the index as ignored
			}
			} // end case
			k++;
		} // end for loop keys

		// fill sorted struct array with the pointer map. The data behind pointer is now populated in this struct
		for (auto& keystructptr : keyStructMapPtr)
			Ssorted[0][keystructptr.first] = (*keystructptr.second);

		return Ssorted;
	}
	/* ----------------------------------data conversion helpers  ------------------------- */
	template <typename T, typename S>
	inline 	std::vector<T> pushMaptoVector(const std::map<T, S> map_in) {
		std::vector <T> vec;
		std::for_each(map_in.begin(), map_in.end(),
			[&vec](std::pair<std::string, unsigned int> index) {vec.push_back(index.first); });
		return vec;
	}
	template <typename T>
	inline matlab::data::TypedArray<T> cvMat2TypedArray_(T vartype, const cv::Mat& mat) { //vartuype is a hack to make template work
		matlab::data::TypedArray<T> tmpArray = factory.createArray<T>({ static_cast<unsigned long>(mat.cols),
				static_cast<unsigned long>(mat.rows) }); //temp array to store
		memcpy(&*tmpArray.begin(), (unsigned char*)mat.data, sizeof(T) * tmpArray.getNumberOfElements()); // unsigned char *pdata = (unsigned char*)tmp.data; //get pointer to cv mat
		return tmpArray;
	}
	template <typename T>
	inline matlab::data::TypedArray<T> cvMat2TypedArray(T vartype, const cv::Mat& mat) { //vartype is a hack to make template work
		matlab::data::TypedArray<T> tmpArray = factory.createArray<T>({ static_cast<unsigned long>(mat.cols),
				static_cast<unsigned long>(mat.rows),static_cast<unsigned long>(mat.channels()) }); //temp array to store

		// std::cout << "channels" << mat.channels() << std::endl;
		std::vector<cv::Mat> chan(mat.channels()); cv::split(mat, chan); 		// split channels
		int k = 0;
		for (auto ch : chan) { // ch are 1xN matrices (ch.rows*ch.cols)-->1xN
			memcpy(&*tmpArray.begin() + sizeof(T) * tmpArray.getNumberOfElements() / mat.channels() * k, (unsigned char*)ch.data,
				sizeof(T) * tmpArray.getNumberOfElements() / mat.channels());
			k++;
		}
		return tmpArray;
	}
	template <typename T>
	inline matlab::data::TypedArray<T> vec2TypedArray(const std::vector<T>& vec) {
		matlab::data::TypedArray<T> tmpArray = factory.createArray<T>({ static_cast<unsigned long>(1),
				static_cast<unsigned long>(vec.size()) }); //temp array to store
		memcpy(&*tmpArray.begin(), (unsigned char*)vec.data(), sizeof(T) * tmpArray.getNumberOfElements()); // unsigned char *pdata = (unsigned char*)tmp.data; //get pointer to cv mat
		return tmpArray;
	}
	inline bool findElement(std::string str, std::vector < std::string > fnKeys) {
		return (std::any_of(fnKeys.begin(), fnKeys.end(), [&str](const std::string& s) {return s == str; }));
	}
	/* -----------------------------------------utilities  ---------------------------------------------- */
	std::filesystem::path assertFilepath(std::filesystem::path flpath) {
		// test if path is present modify if needed otherwise return empty
		auto flpath_ = flpath;
		flpath_ = std::filesystem::exists(flpath_) ? flpath
			: std::filesystem::exists(flpath_.replace_extension("yaml")) ? flpath_.replace_extension("yaml") :
			std::filesystem::exists(flpath_.replace_extension("yml")) ? flpath_.replace_extension("yml") : "";
		// display error and exit

		if (flpath_.u8string().empty()) { erroOnMatlab(std::string("Input file [" + flpath.u8string() + "] does not exist")); return ""; }

		if (flpath_.compare(flpath)) {
			warnOnMatlab(std::string("Input file [" + flpath.u8string() + "] --> changed to [" + flpath_.u8string()) + "]");
		}

		return flpath_;
	}


	void displayOnMATLAB(std::ostringstream& stream) {
		matlabPtr->feval(u"fprintf", 0,
			std::vector<Array>({ factory.createScalar(stream.str()) }));
		stream.str("");
	}
	void erroOnMatlab(std::string s) {
		matlabPtr->feval(u"error", 0,
			std::vector<matlab::data::Array>({ factory.createScalar(s) }));
	}
	void warnOnMatlab(std::string s) {
		matlabPtr->feval(u"warning", 0,
			std::vector<matlab::data::Array>({ factory.createScalar(s) }));
	}
};
