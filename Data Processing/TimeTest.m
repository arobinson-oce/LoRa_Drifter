function TimeTest(dir,i)
%Plots the voltage discharge profile for each unit
%Reports run time in hours
%Change legend, color & marker arrays to match number of units tested
figure(9)
hold on
Color = {'r','g','b','k','m'};
Marker = {'o','s','d','^','v'};
for k = 1:i
    clearvars -except Color Marker dir i k
    filename = sprintf('Drifter%d.csv',k);
    T = readtable(fullfile('/Volumes/GoogleDrive/My Drive/Robinson Summer 2019/Data Files',dir,filename),'Format','%f%f%{MM/dd/yyyy}D%{HH:mm:ss}D%u%f', 'Delimiter', ',','HeaderLines',0);
    T.date.Format = 'MM/dd/yyyy HH:mm:ss';
    T.time.Format = 'MM/dd/yyyy HH:mm:ss';
    FullDatetime = T.date + timeofday(T.time);
    [l,~] = size(FullDatetime);
    dt = FullDatetime(l)-FullDatetime(1);
    dt.Format = 'h';
    dt
    FullDatetime = datenum(FullDatetime);
    ElapsTime = FullDatetime - FullDatetime(1);
    scatter(ElapsTime,T.volts,20,Color{k},Marker{k})
    tstart = datenum(0,0,0,0,0,0);
    tend = datenum(0,0,0,21,0,0);
    xlim([tstart tend]);
    datetick('x','HH:MM','keeplimits')
    %scatter(FullDatetime,T.volts,20,Color{k},Marker{k})
    %datetick('x','mm/dd/yyyy HH:MM:SS','keepticks');
    xtickangle(45)
end
legend('Drifter 1','Drifter 2','Drifter 3','Drifter 4')
xlabel('Elapsed Time (hours)')
ylabel('Battery Voltage (V)')
hold off
figure(9)
end
