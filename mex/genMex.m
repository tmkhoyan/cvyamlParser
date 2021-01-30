% /* BSD 3-Clause License
%  *  
%  *  Copyright (c) 2019, tmkhoyan (Tigran Mkhoyan)
%  *  All rights reserved.
%  *  
%  *  Redistribution and use in source and binary forms, with or without
%  *  modification, are permitted provided that the following conditions are met:
%  *  
%  *  1. Redistributions of source code must retain the above copyright notice, this
%  *     list of conditions and the following disclaimer.
%  *  
%  *  2. Redistributions in binary form must reproduce the above copyright notice,
%  *     this list of conditions and the following disclaimer in the documentation
%  *     and/or other materials provided with the distribution.
%  *  
%  *  3. Neither the name of the copyright holder nor the names of its
%  *     contributors may be used to endorse or promote products derived from
%  *     this software without specific prior written permission.
%  *  
%  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
%  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
%  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
%  *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
%  *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
%  *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
%  *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
%  *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
%  *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
%  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%  */
% 
% /*
%  * Description: 	generate mex files from src
%  * 
%  *            										
%  * 					example usage: 	readcvYaml('test.yaml');
%  * Revision : 		v2.1 (Jan 2021)
%  * 
%  * Author: 			Tigran Mkhoyan
%  * Email : 			t.mkhoyan@tudelft.nl
%  */

if ismac || isunix
%V1 c++11 is fine
%mex -v CXXFLAGS="\$CXXFLAGS -std=c++11" COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" readcvYaml_v1.cpp -I/usr/local/include/opencv4 -I../V1 -I../ -L/usr/local/lib/ -lopencv_core 
%v2 needs c++17
    mex -v CXXFLAGS="\$CXXFLAGS -std=c++17" COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml.cpp -I/usr/local/include/opencv4 -I../ -L/usr/local/lib/ -lopencv_core

else 
    mex -v COMPFLAGS="$COMPFLAGS /std:c++17" COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" ../src/readcvYaml_win.cpp -I"F:\opt\OpenCV-4.5.1\opencv\build\include" -L"F:\opt\OpenCV-4.5.1\opencv\build\x64\vc15\lib" -lopencv_world451 -lopencv_world451d
end