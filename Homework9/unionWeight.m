function X=unionWeight(B,adjacenc_matrix)
n=size(adjacenc_matrix,1);
judge=true(n,1);
judge(B(:))=false;
X=eye(n,n);
for i=1:n
    if judge(i)
        x=find(adjacenc_matrix(i,:));
        X(i,x(:))=1;
        k=sum(adjacenc_matrix(i,:));
        X(i,i)=-k;
    end
end
X=sparse(X);
end