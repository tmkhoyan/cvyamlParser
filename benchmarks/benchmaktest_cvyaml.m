%Benchmark test for readcvYaml mex function

%Author: Tigran Mkhoyan
%Delft University of Technology 2019

%% function handles
f      = @readcvYaml_struct_cases;
f_un   = @readcvYaml_struct_cases_unique;
f_unsh = @readcvYaml_struct_cases_unique_shorter;

N        = 10000; 
flname   = 'test2.yaml';
sort_opt = 'unsorted';

close all;
%% unsorted

% readcvYaml_struct_cases: no unique sorting
T = benchmarkFunc(f,N,flname);

% readcvYaml_struct_cases_unique: has unique sorting


T_un = benchmarkFunc(f_un,N,flname,sort_opt);

% readcvYaml_struct_cases_unique_short: sborter code base 
T_unsh = benchmarkFunc(f_unsh,N,flname,sort_opt);


%% sorted 
sort_opt = 'sorted';

% readcvYaml_struct_cases_unique: has unique sorting
T_un_sort = benchmarkFunc(f_un,N,flname,sort_opt);

% readcvYaml_struct_cases_unique_short: sborter code base 
T_unsh_sort = benchmarkFunc(f_unsh,N,flname,sort_opt);

%% results

data = struct2cell([T, T_un, T_unsh, T_un_sort, T_unsh_sort]);

data(end,:,:) = [];
data = reshape(data,6,5,1);

table1 = cell2table(data,'VariableNames',{'regular','unsorted','unsortedsh','sorted', 'sorted_sh'},...
'RowNames',...
{  'averagetimeMsec'...
       'maxtimeMsec'...
       'mintimeMsec'...
       'averagetime'...
           'maxtime'...
            'mintime'})
        
%% plotting

%time plot 
hd(1) = figure();
h1 = plot(rmoutliers([T.history, T_un.history, T_unsh.history])*1000);
xlabel('N iterations')
ylabel('time [ms]')
legend(h1,{'regular', 'unsorted', 'unsorted_sh'})
title(['comparison unsorted. N iterations=', num2str(N)]);

%box plot 
hd(2) = figure();
boxplot(rmoutliers([T.history, T_un.history, T_unsh.history])*1000,...
    'Labels', {'regular', 'unsorted', 'unsorted_sh'},...
    'whisker',10);
ylabel('time [ms]')
title(['comparison unsorted boxplot. N iterations=', num2str(N)]);

%time plot
hd(3) = figure(); 
h1 = plot(rmoutliers([T_un_sort.history, T_unsh_sort.history]*1000));
legend(h1,{'sorted', 'sorted_sh'})
xlabel('N iterations')
ylabel('time [ms]')
title(['comparison sorted. N iterations=', num2str(N)]);

%boxplot
hd(4)= figure(); 
boxplot(rmoutliers([T_un_sort.history, T_unsh_sort.history])*1000, ...
'Labels', {'sorted', 'sorted_sh'}, 'whisker',10);
% xlabel('N iterations')
ylabel('time [ms]')
title(['comparison sorted boxplot. N iterations=', num2str(N)]);

%time plot
hd(5) = figure(); 
h1 = plot(rmoutliers([T.history, T_un.history, T_unsh.history, T_un_sort.history, T_unsh_sort.history])*1000);
legend(h1,{'regular','unsorted','unsortedsh','sorted', 'sorted_sh'})
xlabel('N iterations')
ylabel('time [ms]')
title(['comparison all. N iterations=', num2str(N)]);


%boxplot
hd(6) = figure(); 
boxplot(rmoutliers([T.history, T_un.history, T_unsh.history, T_un_sort.history, T_unsh_sort.history])*1000, ...
'Labels',{'regular','unsorted','unsortedsh','sorted', 'sorted_sh'},...
 'whisker',10);
% xlabel('N iterations')
ylabel('time [ms]')
title(['comparison all boxplot. N iterations=', num2str(N)]);

save('benchmark.mat');

fignames = {
    'time'
    'boxplot'
    'time_sorted'
    'boxplot_sorted'
    'time_all'
    'boxplot_all'
    };

FolderName = './figs/'; 
fignames = strcat(FolderName,fignames,'_N',num2str(N),'.pdf');
for n=1:numel(fignames)
saveas(hd(n), fignames{n}); 
end
