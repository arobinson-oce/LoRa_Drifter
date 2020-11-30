function DrifterProcess(dir,i)
%DrifterProcess.m   Plot drifter tracks w/ speed-color or velocity vectors
%   Overlay data on outline of coast from GPSVisualizer.com
%   Ingest csv files from folder w/in GoogleDrive/.../Data Files
%   Load data into table, calculate velocity, vector components & plot
%   Drifter files should follow convention of DrifterX.csv, beginning at 1

figure(1)
hold on;
load ;          %add coast outlines to figure, repeat as needed for multiple files 
plot ;
fill ;

for k = 1:i
    filename = sprintf('Drifter%d.csv',k);
    T = readtable(fullfile('FILL WITH PARENT DIRECTORY',dir,filename),'Format','%f%f%{MM/dd/yyyy}D%{HH:mm:ss}D%u%f', 'Delimiter', ',','HeaderLines',0);
    n = 1;
    t =  ; %FILL WITH LOGGING INTERVAL CHOSEN
    [l,~] = size(T);
    a = zeros(l-1, 1);
    d = zeros(l-1, 1);
    v = zeros(l-1, 1);
    y = zeros(l-1, 1);
    x = zeros(l-1, 1);
    r = zeros(l-1, 1);
    dy = zeros(l-1, 1);
    dx = zeros(l-1, 1);
 while n < l
     %dt(n,1) = T{n+1,4} - T{n,4};
     a(n,1) = (sind((T{n+1,1}-T{n,1})/2)).^2 + cosd(T{n,1})*cosd(T{n+1,1})*(sind((T{n+1,2}-T{n,2})/2)).^2;  %great circle dist
     d(n,1) = 2*6378.14*1000*atan2(sqrt(a(n,1)),sqrt(1-a(n,1)));     %distance in meters
     v(n,1) = d(n,1)/t;
     y(n,1) = sind(T{n+1,2}-T{n,2})*cosd(T{n+1,1});
     x(n,1) = cosd(T{n,1})*sind(T{n+1,1}) - cosd(T{n+1,1})*sind(T{n,1})*cosd(T{n+1,2}-T{n,2});
     r(n,1) = atan2d(y(n,1),x(n,1));
     dy(n,1) = v(n,1)*sind(r(n,1));    %longitude diff vector comp in m/s
     dx(n,1) = v(n,1)*cosd(r(n,1));    %latitude diff vector comp in m/s
     n = n+1;
 end
    T(1,:) = [];
    set(gca, 'dataaspectratio',[1 1 1]);
    scatter(T{:,2},T{:,1},2,color,v,'filled')
    %quiver(T{:,2},T{:,1},dy,dx)       %plots velocity arrows (x,y,ycomp.vector,xcomp.vector)
    clearvars -except i k dir;
end


colorbar;
%caxis ([0 2]);
c = colorbar; 
c.Label.String = 'Speed (m/s)';           
c.Label.Rotation = -90;
c.Label.Position = [3 1.25 0];


xlabel('Longitude');
ylabel('Latitude');
figure(1)

%{
print(file,'-djpeg','-r300')
savefig(file)
%}

end

