% Copyright [yyyy] [name of copyright owner]
% 
% Licensed under the Apache License, Version 2.0 (the "License");
% you may not use this file except in compliance with the License.
% You may obtain a copy of the License at
% 
%     http://www.apache.org/licenses/LICENSE-2.0
% 
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
% See the License for the specific language governing permissions and
% limitations under the License.

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

