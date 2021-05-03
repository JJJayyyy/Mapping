siFileName = 'jtlExample'; % netlist file name
datFileName = 'OUT'; % the output file name of simulation
% Output file is named in the netlist on the line: .file OUT
% The name and file type can be adjusted easily. Eg: .file OUTPUT.txt
jsimType = 'jsim_n'; % define the exe file, you need to put exe file into the same directory
system([jsimType ' ' siFileName]); % simulate and get the data, simulation will create OUT file
data = dlmread(datFileName); % read the data from file
figure; % create the figure window
hold on;
plot(data(:,1),data(:,2),'b'); % draw the data
plot(data(:,1),data(:,3),'r'); % draw the data