%Benchmark test for readcvYaml mex function

%Author: Tigran Mkhoyan
%Delft University of Technology 2019

%% function handles
f      = @readcvYaml;
f_sh   = @readcvYaml;

N        = 1000; 
flname   = '../test_data1.yaml';

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
[~,OS] = system('uname');
if  strcmpi(strtrim(OS),'Darwin')
    basedir = 'osx';
elseif strcmpi(strtrim(OS),'Linux')
    basedir = 'linux';
end
    
save([basedir, '/benchmark.mat']);

fignames = {
    'time_comparison'
    'boxplot_comparison'
    };

FolderName = [basedir,'/figs/']; 
fignames = strcat(FolderName,fignames,'_N',num2str(N),'.pdf');
for n=1:numel(fignames)
saveas(hd(n), fignames{n}); 
end
