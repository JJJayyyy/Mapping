%============================================================
% Developed by: Arash Fayyazi

%============================================================
function data= readJsim(filename)
text=fileread(filename);
toValues=strfind(text,'Values:');
toVariables=strfind(text,'Variables:');
hText=text(1:toVariables(2));
b=textscan(hText,'%s','Delimiter','\n');
headers=length(b{:,:}); %number of header lines
names=text(toVariables(2)+11:toValues);
names=(textscan(names,'%s %s %s'));  % read the names of variables
names= names{1,2};
clear text hText;
% now read the file
L=length(names)-1;
size(names);
fid=fopen(filename);
textscan(fid,'%s %s %s',L,'HeaderLines',headers); %scape the variable names
textscan(fid,'%s',1);
disp('Reading File (.raw file)')
i=1;
while true
    
    if(feof(fid))
        fclose(fid);
        break;
    end
    f=cell2mat(textscan(fid,'%f',length(names)+1,'TreatAsEmpty','failed'));
    if(length(f)==length(names)+1)
        vals2(:,i)=f;
    else
        break;
    end
    i=i+1;
end

data.names=char('index',names{:});
data.values=vals2;
fclose('all');

end