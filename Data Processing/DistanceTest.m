clear
Color = {'r','g','b','k'};
Marker = {'o','s','d','^'};
figure(1)
hold on
for k = 1:4
    clearvars -except dir k aLat aLon Color Marker
    filename = sprintf('Drifter%d.csv',k);
    T = readtable(fullfile('PARENT DIRECTORY',filename),'Format','%f%f%{MM/dd/yyyy}D%{HH:mm:ss}D%u%f', 'Delimiter', ',','HeaderLines',0);
    aLat(1,k) = mean(T.latitude);
    aLon(1,k) = mean(T.longitude);
    [l,~] = size(T);
    n = 1;
    T.date.Format = 'MM/dd/yyyy HH:mm:ss';
    T.time.Format = 'MM/dd/yyyy HH:mm:ss';
    FullDatetime = T.date + timeofday(T.time);
    FullDatetime = datenum(FullDatetime);
    ElapsTime = FullDatetime - FullDatetime(1);
    
  while n <= l
    a(n,k) = (sind((aLat(1,k)-T{n,1})/2)).^2 + cosd(T{n,1})*cosd(aLat(1,k))*(sind((aLon(1,k)-T{n,2})/2)).^2;  %great circle dist
    d(n,k) = 2*6378.14*1000*atan2(sqrt(a(n,k)),sqrt(1-a(n,k)));     %distance in meters
    n = n+1;
  end
    
    
    subplot(4,1,k);                                            % distance vs elapsed time, broken into subplots w/ standardized x-axis
    scatter(ElapsTime,d(:,k),1,Color{k},Marker{k})
    tstart = datenum(0,0,0,0,0,0);
    tend = datenum(0,0,0,21,0,0);
    xlim([tstart tend]);
    ylim([0 40]);
    datetick('x','HH:MM','keeplimits')
    %legend(sprintf('Drifter %d',k))
    xlabel('Elapsed Time (hours)')

  
   %{
    k                                                          % summary stats
    medDist = median(d(:,k))
    avgDist = mean(d(:,k))
    minDist = min(d(:,k))
    maxDist = max(d(:,k))
    stdDist = std (d(:,k))
   %}
  
    %{
    subplot(5,1,k);
    scatter(T.sat,d(:,1),5,'k','o')
    xlim([0 12])
    %}
  
    %{
    subplot(5,1,k);
    histogram(d(:,k),'FaceColor','k')
    xlim([0 35])
    %}
    
    %scatter(T.longitude,T.latitude,20,Color{k},'o','filled')  % scatter plot of all points
    
    
    %{
    subplot(5,1,k);                                            % distance vs volts, broken into subplots w/ standardized x-axis
    scatter(T.volts,d(:,1),20,'k','o')
    xlim([3.4 4.2])
    set(gca,'xdir','reverse')
    %}
    
end
%scatter(aLon(1,:),aLat(1,:),80,'k','x')                        % average location of each drifter

%{
xlabel('Elapsed Time (hours)')
h1=subplot(4,1,1);
h2=subplot(4,1,4);
p1=get(h1,'position');
p2=get(h2,'position');
height=p1(2)+p1(4)-p2(2);
h3=axes('position',[p2(1) p2(2) p2(3) height],'visible','off');
h_label=ylabel('Distance Error (hours)','visible','on');
figure(1)
%}

%figure(6)
%histogram(d)                                                   % freq distribution of distance errors from mean
%figure(6)


h1=subplot(4,1,1);
h2=subplot(4,1,4);
p1=get(h1,'position');
p2=get(h2,'position');
height=p1(2)+p1(4)-p2(2);
h3=axes('position',[p2(1) p2(2) p2(3) height],'visible','off');
h_label=ylabel('Distance Error (m)','visible','on');
    