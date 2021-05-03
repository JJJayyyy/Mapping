clear all;
clc;
data = readJsim('jsim.raw'); %% specify the cell name%%
time_Jsim = sig(data, sprintf('time'));
j=0;
for k=1:size(data.names,1)
    for m=1:(size(data.names,2)-1)
        if(strcmp(data.names(k,m:(m+1)),'.I'))
            j=j+1;
            I_points(j,:) = data.names(k,:); %#ok<SAGROW>
        end
    end
end

for k=1:size(I_points,1)
    A = I_points(k,:);
    A = A(~isspace(A));
    out_Jsim(:,k) = sig(data, A); %#ok<SAGROW>
end
% out_Jsim(:,k+1) = sig(data, 'RC1.I'); %% current
plot_size=size(out_Jsim,2);
%plot_size=size(out_Jsim,2)+1;
figure();
for i=1:plot_size
    subplot(plot_size,1,i);
    plot(time_Jsim,out_Jsim(:,i),'color',rand(1,3));
   ylabel(I_points(i,:));
end
[pksin,locsin] = findpeaks(out_Jsim(:,3),time_Jsim);
[pks,locs] = findpeaks(out_Jsim(:,i),time_Jsim,'MinPeakHeight',0.0001);
delay = locs(1) - locsin(7);
%[pks,locs] = findpeaks(out_Jsim(:,i-5),time_Jsim,'MinPeakHeight',0.0001);
%delay = locs(1) - locsin(1)