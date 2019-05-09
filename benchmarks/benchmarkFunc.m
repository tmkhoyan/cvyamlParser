% /* BSD 3-Clause License
%  *  
%  *  Copyright (c) 2019, tmkhoyan
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
%  * Description: 	   helper functions for cvyamlParser to apply
%                      benchmarking
%  * Author: 			Tigran Mkhoyan
%  * Email : 			t.mkhoyan@tudelft.nl
%  */

function [TIME] = benchmarkFunc(f,N,varargin)

%setOptargs
%create functon handle
%f = @functname 

t_diff  = zeros(N,1);
for n = 1:N
tic
f(varargin{:}); %function handle
 t_diff(n) = toc;
 
end 

averagetime = mean(t_diff);
maxtime     = max(t_diff);
mintime     = min(t_diff);

averagetimeMsec = averagetime*1000;
TIME.averagetimeMsec = averagetimeMsec;
TIME.maxtimeMsec = maxtime*1000;
TIME.mintimeMsec = mintime*1000;
TIME.averagetime = averagetime;
TIME.maxtime = maxtime;
TIME.mintime = mintime;
TIME.history = t_diff;

fprintf('Average time in sec: ');
fprintf('%.8f \n', averagetime);
fprintf('Max time in sec: ');
fprintf('%.8f \n',TIME.maxtime);
fprintf('Min time in sec: ');
fprintf('%.8f \n', TIME.mintime);

%    % function setOptargs
%         numvarargs = length(varargin);
%         
%         % set defaults for optional inputs
%         if numvarargs > 2
%             error('functions:randRange:TooManyInputs', ...
%                 'requires at most 2 optional inputs');
%         end
%         
%         %defaul options
%         optargs = {1 '+'};
%         %overwrite default
%         [optargs{1:numvarargs}] = varargin{:};
%         [size, signval] = optargs{:};
%     end

end 

