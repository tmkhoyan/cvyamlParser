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

%% function handles (ensure to copy mexfiles here)
f      = @readcvYaml;
f_sh   = @readcvYaml;

N        = 1000;
flname   = '../data/rand_test_data.yaml';

close all;
%% unsorted

% readcvYaml_struct_cases: no unique sorting
T = benchmarkFunc(f,N,flname);

%% sorted
sort_opt = 'sorted';

% readcvYaml_struct_cases_unique: has unique sorting
T_s = benchmarkFunc(f_sh,N,flname,sort_opt);

%% results

data = struct2cell([T, T_s]);

data(end,:,:) = [];
data = reshape(data,6,2,1);

table1 = cell2table(data,'VariableNames',{'regular','sorted'},...
    'RowNames',...
    {  'averagetimeMsec'...
    'maxtimeMsec'...
    'mintimeMsec'...
    'averagetime'...
    'maxtime'...
    'mintime'});


%% plotting

%time plot
hd(1) = figure();
% filter time data
%[b,a] = butter(8,0.1);
[b,a] = butter(10,0.05);
if numel(T.history)>24
    Tdata_u = rmoutliers([T.history, T_s.history])*1000;
    Tdata   = filtfilt(b,a,Tdata_u);
    plot(Tdata_u,':','LineWidth',1); hold on;
    h1 = plot(Tdata,'LineWidth',2);
else
    Tdata = rmoutliers([T.history, T_s.history])*1000;
    h1 = plot(Tdata);
end
xlabel('iteration')
ylabel('time [ms]')
legend(h1,{'regular', 'sorted'})
title(['comparison unsorted vs sorted time. N iterations=', num2str(N)]);

%box plot
hd(2) = figure();
boxplot(rmoutliers([T.history, T_s.history])*1000,...
    'Labels', {'regular', 'sorted'},...
    'whisker',10);
ylabel('time [ms]')
title(['comparison unsorted vs sorted boxplot. N iterations=', num2str(N)]);

%% save output
%determine system
if ispc
    basedir = 'win';
else
    [~,OS] = system('uname');
    if  strcmpi(strtrim(OS),'Darwin')
        basedir = 'osx';
    elseif strcmpi(strtrim(OS),'Linux')
        basedir = 'linux';
    end
    
end

mkdir(basedir);

save([basedir, '/benchmark.mat']);

fignames = {
    'time_comparison'
    'boxplot_comparison'
    };

FolderName = [basedir,'/figs/'];
fignames = strcat(FolderName,fignames,'_N',num2str(N),'.pdf');

mkdir(FolderName);

for n=1:numel(fignames)
    saveas(hd(n), fignames{n});
end
