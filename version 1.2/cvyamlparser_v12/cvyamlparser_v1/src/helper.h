/* BSD 3-Clause License
 *  
 *  Copyright (c) 2019, tmkhoyan
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
 * Description: 	helper functions for cvyamlParser
 * Author: 			Tigran Mkhoyan
 * Email : 			t.mkhoyan@tudelft.nl
 */

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

	template <typename T>
		inline void printVector(const std::vector<T> v, std::string varname = ""){
			int k =0;
			std::cout << "vector: " << varname << std::endl;
			std::for_each(v.begin(),v.end(),
					[&k](const T & index){std::cout << k++ << ": " <<index << std::endl;});
		}

	template <typename T, typename S>
		inline void printMap(const std::map<T,S> m){
			int k =0;
			// for(auto p: m)
			// 	std::cout << k++ << ": " <<p.first << ", "<< p.second << std::endl;
			std::for_each(m.begin(),m.end(),
					[&k](std::pair<const T,S> index){std::cout << k++ <<": " <<index.first << ", \t " <<index.second << std::endl;});
		}
