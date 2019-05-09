close all
[b,a] = butter(5,0.09);
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