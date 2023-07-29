function X=floaterWeight(v,B,adjacenc_matrix)
n=size(adjacenc_matrix,1);
judge=true(n,1);
judge(B(:))=false;
X=eye(n);
for i=1:n
    if judge(i)
        k=sum(adjacenc_matrix(i,:));
        sigma=zeros(1,k);s=zeros(1,3);
        weight=zeros(k);
%         weight=zeros(1,k);
        x=find(adjacenc_matrix(i,:));
        for j=1:k-1
            sigma(j)=angle(v(i,:),v(x(j),:),v(x(j+1),:));
        end
        sigma(k)=angle(v(i,:),v(x(k),:),v(x(1),:));
        Sigma=sum(sigma);
        sigma=2*pi*sigma/Sigma;
%         weight(1)=(tan(sigma(k)/2)+tan(sigma(1)/2))/norm(v(x(1),:)-v(i,:));
%         for j=2:k
%             weight(j)=(tan(sigma(j-1)/2)+tan(sigma(j)/2))/norm(v(x(j),:)-v(i,:));
%         end
%         w=sum(weight);
%         labmda=weight/w;
        for j=1:k
            tempx=circshift(x,1-j);
            tempSigma=circshift(sigma,1-j);
            sumSigma=tempSigma(1);
            for l=2:k-1
                sumSigma=sumSigma+tempSigma(l);
                if sumSigma>pi break; end
            end
            s(1)=norm(v(i,:)-v(tempx(l),:))*norm(v(i,:)-v(tempx(l+1),:))*sin(tempSigma(l));
            s(2)=norm(v(i,:)-v(tempx(1),:))*norm(v(i,:)-v(tempx(l+1),:))*sin(2*pi-sumSigma);
            s(3)=norm(v(i,:)-v(tempx(l),:))*norm(v(i,:)-v(tempx(1),:))*sin(sumSigma-tempSigma(l));
            S=sum(s);
            s=s/S;
            m1=mod(l+j-1,k);if m1==0 m1=k;end
            m2=mod(l+j,k);if m2==0 m2=k;end
            weight(j,j)=s(1);weight(m1,j)=s(2);weight(m2,j)=s(3);
        end
        p=sum(weight,2)/k;
        X(i,x(:))=p(:);
%         X(i,x(:))=labmda(:);
        X(i,i)=-1;
    end
end
X=sparse(X);
end

function sigma=angle(p,p1,p2)
sigma = acos(dot(p1-p,p2-p)/(norm(p1-p)*norm(p2-p)));
end

% function S=area(p,p1,p2)
% S=norm(cross(p1-p,p2-p),2);
% end