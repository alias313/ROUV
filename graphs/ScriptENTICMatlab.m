clear all;
delete(instrfind({'Port'},{'COM9'}));
s=serialport('COM9', 9600); %Port initialization
configureTerminator(s,"CR/LF")
fopen(s);
fid=fopen('resultats_Final.txt','w');
t=datetime('now'); pause(2);
i=0;
flushinput(s);

figure('Name','Measured Data','NumberTitle','off');
subplot(2,2,1)
h=animatedline;
ax= gca;

ax.YGrid = 'on';
ax.XGrid = 'on';
ax.YLim = [-10 10];
xlabel('TIME [s]');
ylabel('DEPTH [m]');
title('\bf{REAL TIME DEPTH}');

subplot(2,2,2)
h2=animatedline;
ay= gca;

ay.YGrid = 'on';
ay.XGrid = 'on';
ay.YLim = [20, 500];
xlabel('TIME [s]');
ylabel('TEMPERATURE [ºC]');
title('\bf{REAL TIME TEMPERATURE}');

subplot(2,2,3)
h3=animatedline;
ax2= gca;

ax2.YGrid = 'on';
ax2.XGrid = 'on';
ax2.YLim = [100, 2000];
xlabel('TIME [s]');
ylabel('TDS [ppm]');
title('\bf{REAL TIME CONDUCTIVITY}');

subplot(2,2,4)
h4=animatedline;
ay2= gca;

ay2.YGrid = 'on';
ay2.XGrid = 'on';
ay2.YLim = [-500, 2000];
xlabel('TIME [s]');
ylabel('TURBIDITY [ntu]');
title('\bf{REAL TIME TURBIDITY}');

maxval=0;
minval=4;

variable=input('INGRESE EL VALOR DE LA VARIABLE: ', 's');

fwrite(s, variable);

% factor = (((5/1023)/163.46)/2)*1000;    

if(variable=='S')
    startTime = datetime('now');
end
j=0;
while(variable=='S')
    
    lectura_bits=str2double(split(readline(s)));
    temp = lectura_bits(1);
    depth = lectura_bits(2);
    tds = lectura_bits(3);
    ntu = lectura_bits(4);
    
    %SERVEIX PER VEURE QUIN VALOR ÉS EL MÀXIM I QUIN EL MÍNIM
    if(depth>maxval)
        maxval=depth;
    if(depth<minval)
        minval=depth;
    end
    end

    t=datetime('now') - startTime;
    datenum(t);
    depth;
    addpoints(h, datenum(t), depth);
    addpoints(h2, datenum(t), temp);
    addpoints(h3, datenum(t), tds);
    addpoints(h4, datenum(t), ntu);

    %Updated axes
    ax.XLim = datenum([t-seconds(10) t]); datetick('x','keeplimits');
    ay.XLim = datenum([t-seconds(10) t]); datetick('x','keeplimits');
    ax2.XLim = datenum([t-seconds(10) t]); datetick('x','keeplimits');
    ay2.XLim = datenum([t-seconds(10) t]); datetick('x','keeplimits');
    drawnow
    pause(0.01);
    i=i+1;
    j=j+1;
end
fprintf(fid,'\n MAXIMUM DEPTH: %f m', maxval);
fprintf(fid, '\n MINIMUM DEPTH: %f m', minval);
fprintf(s,'%s\n', 'e');
fclose(fid);

