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
%  * Description: 	   Benchmark masterfile to run benchmarks for
%  cvyamlParser
%                      benchmarking
%  * Author: 			Tigran Mkhoyan
%  * Email : 			t.mkhoyan@tudelft.nl
%  */


%% New features: 
%   - nested map, 
%   - vectors , mixed datatypes 
%   - and multidim/multichannel matrices 
%   - improved sorting, numbering

% usage: readcvYaml([file],[opt1],[opt2])
%   - opt1 = "sorted" or leave out, opt2 = delimiter node name stripping e.g. "_" 
%   - provide path, parser will automatically fix extensions yml/yaml


%% parse test data. Unsorted
dataA = readcvYaml('data/test_data.yaml')

% =
%   struct with fields:
% 
%           structA1: [1×1 struct]
%           structA2: [1×1 struct]
%            structB: [1×1 struct]
%     structnumbered: [1×1 struct]
%              matA0: [6×12 double]
%              matA1: [6×12 single]
%              matA2: [6×12 single]
%              matA5: [6×12 single]
%              matB1: [5×12 single]
%               vec1: [1.2000 1.3000 1.4000]
%               vec2: [1.1000 1.3000 1.1000]
%              myint: 100
%            myfloat: 100.5000
%           mystring: 'Brown Vox'

% nested structure with mixed data types
dataA.structA1

%   struct with fields:
% 
%     a: 1
%     b: 'string1'
%     c: 10.2000
%     d: [1 2 3 4 5]

%% sorted
dataB  = readcvYaml('data/test_data.yaml','sorted')
% = 
%   struct with fields:
% 
%               matA: [1×4 struct]
%               matB: [1×1 struct]
%            myfloat: [1×1 struct]
%              myint: [1×1 struct]
%           mystring: [1×1 struct]
%            structA: [1×2 struct]
%            structB: [1×1 struct]
%     structnumbered: [1×1 struct]
%                vec: [1×2 struct]

% data is folded into a structure based on unique basenames e.g. matA:
% to access the 4th structure of matA use:
dataB.matA(4).matA

%   6×12 single matrix
% 
%     50   960   168     0   960   168     0   960   166     0   962   162
%      0   966   156     0   968   154     0   964   160     0   960   168
%    ...

% index is trored in .index which corresponds to stripped number after basename: 
dataB.matA(4).index


%% numbered structs pass delimiter option
% normally not allowed to be contructed directly via matlab but possible
% via C++ API. delimiter "_" stripts numbers from node names
dataC  = readcvYaml('data/test_data.yaml','','_')

dataC.structnumbered
% =   struct with fields:
% 
%     0: 'string'
%     1: 1
%     2: 3.3000
%     3: 4
%     4: 5
%     5: [1 2 3]

%% Multichannel image or multidimentinal matrices
% write image directly to yaml file. See sourcefiles generate.

img_data = readcvYaml('data/multi_channel.yaml','_')

%note opencv uses BGR covert to RGB
img_RGB = img_data.img(:,:,[3 2 1]);

figure();
% show image, 
imshow(img_RGB)
