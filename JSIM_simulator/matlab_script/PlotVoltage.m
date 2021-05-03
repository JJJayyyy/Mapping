clear all;
clc;
data = readJsim(['jsim_majority.raw']); %% specify the cell name%%
time_Jsim = sig(data, sprintf('time'));
j=0;
for k=1:size(data.names,1)
    if(strcmp(data.names(k,1:2),'V('))
        j=j+1;
        [C,matches] = strsplit(data.names(k,:),{',','('});
        V_points(j)=str2num(C{2});
    end
end
V_points=sort(V_points);
for k=1:length(V_points)
    out_Jsim(:,k) = sig(data, sprintf('V(%d,0)',V_points(k)));
end
% out_Jsim(:,k+1) = sig(data, 'RC1.I'); %% current
plot_size=size(out_Jsim,2);
%plot_size=size(out_Jsim,2)+1;
figure();
for i=1:plot_size
    subplot(plot_size,1,i);
    plot(time_Jsim,out_Jsim(:,i),'color',rand(1,3));
    set(gca,'xticklabel',{[]}); %% remove the x labels
    set(gca,'yticklabel',{[]}); %% remove the x labels
   ylabel(['V(',num2str(V_points(i)),')']);
end
% [pksin,locsin] = findpeaks(out_Jsim(:,1),time_Jsim);
% [pks,locs] = findpeaks(out_Jsim(:,i),time_Jsim,'MinPeakHeight',0.0001);
% delay = locs(1) - locsin(1)
% [pks,locs] = findpeaks(out_Jsim(:,i-5),time_Jsim,'MinPeakHeight',0.0001);
% delay = locs(1) - locsin(1)
%%% findpeaks(out_Jsim(:,i),time_Jsim,'MinPeakHeight',0.001) show in the
%%% figure when without [pks,locs]
%%% [pks,locs] =
%%% findpeaks(out_Jsim(:,i),time_Jsim,'MinPeakDistance',1e-10); another
%%% way by using minium distance!
