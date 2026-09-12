clc
clear all

format long
clear f

Tn = 10;                       %期數 ie. 0,1,2,3
ni = 1;               
N = 0;                        %N0=0                
sigma = 0.3;                  %先假設每期sigma不變
delta = 1;
K = 0.05;
f0 = [0.05 ; 0.05 ; 0.05  ; 0.05  ; 0.05  ; 0.05  ; 0.05  ; 0.05  ; 0.05  ; 0.05  ; 0.05  ]   %f(i,Ti)  [f00 ; f01 ; f02....]

for k=Tn:-1:1
    N = 0;
    f(1,1,k) = f0(k+1);
    for i=1:k
        N = N+ni;
        d = 2/(1+exp(2*sigma*((delta)/ni)^0.5));
        u = 2-d;
        for j=1:N+1
            f(j,i+1,k) = f0(k+1)*u^(N-j+1)*d^(j-1);
        end  
    end
    f(:,:,k) = f(:,:,k)*100;
    tempory = f(:,:,k);
%     save( ['C:\headoil\f' num2str(k) '.txt'] , 'tempory' , '-ascii', '-double')  %存檔問題待解決
end

% f
f = f/100


for m=Tn:-1:1
    N = 0;
    for i=1:m
        N = N+ni;
        d = 2/(1+exp(2*sigma*((delta)/ni)^0.5));
        u = 2-d;
        
        for j=1:N
            sum = 0;
            if (i==1)
                for k=i:m
                    if (k~=m)
                        sum = sum+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==m)
                        sum = sum+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                exi = log(1+sum)-0.5*sigma*sigma;
                q(2*j-1,i,m) = (exi-(N-ni-j+1)*log(u)-(j-1)*log(d))/(ni*(log(u)-log(d)))-log(d)/(log(u)-log(d));
                q(2*j,i,m) = 1-q(2*j-1,i,m);    
            else
                for k=i:m
                    if (k~=m)
                        sum = sum+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==m)
                        sum = sum+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                exi = log((1+sum)*f(j,i,m)/f(1,1,m))-0.5*sigma*sigma;
                q(2*j-1,i,m) = (exi-(N-ni-j+1)*log(u)-(j-1)*log(d))/(ni*(log(u)-log(d)))-log(d)/(log(u)-log(d));
                q(2*j,i,m) = 1-q(2*j-1,i,m);   
            end
        end
    end
end

q
clear tempory sum

for k=1:Tn
    clear tempory2 
    if (k==1)
        pathq(1,1) = q(1,1,1);
        pathq(2,1) = q(2,1,1);
    else
        for i=1:k-1
            clear tempory1 
            tempory1(:,1) = q(:,k-i,k);
            tempory1(find(tempory1(:,1)==0),:) = [];
            
            for m=1:2*(k-i)
                tempory1(m,2) = mod(m,2);      
            end
            
            if (i==1)
                tempory2(:,1) = q(:,k,k);
                tempory2(find(tempory2(:,1)==0),:) = [];
                
                for m=1:2*k
                    tempory2(m,2) = mod(m,2);      
                end
                
            end   
            
            j = 1;              
            tempory = [];
            while (j<=(length(tempory1(:,1)))/2)
                clear tempory3 tempory4
                tempory3(:,1) = tempory1(2*j-1,1)*tempory2((2^i)*j-(2^i)+1:(2^i)*j-(2^i)+(2^i),1);
                tempory3(:,2) = tempory1(2*j-1,2)+tempory2((2^i)*j-(2^i)+1:(2^i)*j-(2^i)+(2^i),2);
                   
                tempory = [tempory ; tempory3];
                    
                tempory4(:,1) = tempory1(2*j,1)*tempory2((2^i)*j-(2^i)+1+(2^i):(2^i)*j-(2^i)+(2^i)+(2^i),1);
                tempory4(:,2) = tempory1(2*j,2)+tempory2((2^i)*j-(2^i)+1+(2^i):(2^i)*j-(2^i)+(2^i)+(2^i),2);
                   
                tempory = [tempory ; tempory4];
                   
                j = j+1;
            end
            tempory2 = tempory;     
                  
        end
        
        for l=k:-1:0            
            pathq(k-l+1,k) = sum(tempory2(find(tempory2(:,2)==l),1));
            
        end     
    end 
end



pathq

sum(pathq)


%%勿刪
% f1 = f;
% for i=1:Tn
%     f1(1:i+1,i+1,i) = 1./((1+f1(1:i+1,i+1,i)).^(delta));
% end
% 
% f1

%%數值Bond Price
for i=1:Tn+1
    if (i==1)
        numerical_p1(i,1) = 1/(1+f0(1));
        numerical_p2(i,1) = numerical_p1(i,1);
    else
        sump = 0;
        for j=1:i
            sump = sump+pathq(j,i-1)/(1+f(j,i,i-1));
        end
        numerical_p1(i,1) = sump;
        
        product_p = 1;
        for k=1:i
            product_p = product_p*numerical_p1(k,1);
        end
        numerical_p2(i,1) = product_p;
        
%         for k=1:i-1
%             numerical_p(i,1) = numerical_p(i,1)*numerical_p(k,1);
%         end
    end
end

numerical_p1%最後一期的債券價格P(Tn,Tn+1)
numerical_p2%零息債券價格P(0,i)~P(0,Tn)

%Caplet

for i=1:Tn
    tempory2 = 0;
    for j=1:i+1
        tempory2 = tempory2+max(f(j,i+1,i)-K,0)*pathq(j,i);  
    end
    caplet(i,1) = tempory2*numerical_p2(i+1,1);
end

caplet

%black model(需要改寫成公式的形式)
for i=1:Tn
    nd1 = normcdf(0.5*sigma*(i^0.5),0,1);
    tempory3 = 1;
    for j=1:i+1
        tempory3 = tempory3*(1+f0(j));
    end
    black(i,1) = 0.05*(2*nd1-1)/tempory3;  
end

black

%計算Black和Numerical Caplet的誤差
difference = caplet - black

RMSE = sqrt((sum(difference.^2)) / Tn)

