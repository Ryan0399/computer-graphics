clear
clc

% 读取obj文件中的网格
filename='Balls.obj';
% varfacesizes=true;
 [v, f, ~, ~] = readObj(filename);
 % 寻找边界点并将其参数化到圆周上
 % f=cell2mat(f);
 [boundary_points, ~]=findBoundary(v,f);
 n=size(v,1);m=size(boundary_points,2);
 parameter=zeros(n,2);
for i=1:15
    parameter(boundary_points(i),:)=[(i-1)/15 0];
end
for i=16:30
    parameter(boundary_points(i),:)=[1 (i-16)/15];
end
for i=31:45
    parameter(boundary_points(i),:)=[1-(i-31)/15 1];
end
for i=46:60
    parameter(boundary_points(i),:)=[0 1-(i-46)/15];
end
% 构造所有点的邻接矩阵
adjacenc_matrix=zeros(n);
for i=1:length(f)
    adjacenc_matrix(f(i,1),f(i,2))=1;%adjacenc_matrix(f(i,2),f(i,1))=1;
    adjacenc_matrix(f(i,2),f(i,3))=1;%adjacenc_matrix(f(i,3),f(i,2))=1;
    adjacenc_matrix(f(i,3),f(i,1))=1;%adjacenc_matrix(f(i,1),f(i,3))=1;
end 
% 构造权重矩阵
% X=unionWeight(boundary_points,adjacenc_matrix); % 均匀权重
X=floaterWeight(v,boundary_points,adjacenc_matrix); % floater权重
parametric_points=X\parameter;
drawmesh(f,parametric_points,boundary_points);

