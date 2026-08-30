clc
clear all

format long
clear f

Tn = 10;                       %期數 ie. 0,1,2,3
ni = 2;               
N = 0;                         %N0=0                
sigma = 0.1;                   %先假設每期sigma不變
delta = 1;
K = 0.05;
f0 = [0.05 ; 0.05; 0.05; 0.05; 0.05; 0.05; 0.05; 0.05; 0.05; 0.05; 0.05];    %f(i,Ti)  [f00 ; f01 ; f02....]
% f0(1:Tn+1,1) = 0.05;

iii = 1;


% clear f pathq q tempory tempory1 tempory2 tempory3 numerical_p1 numerical_p2 caplet black testsigma
    
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
  
%     tempory = f(:,:,k);
%     save( ['C:\headoil\f' num2str(k) '.txt'] , 'tempory' , '-ascii', '-double')  %存檔問題待解決
end

f

if(ni==1)
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



    %%數值P
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

    numerical_p1
    numerical_p2

    %Caplet

    for i=1:Tn
        tempory2 = 0;
        for j=1:i+1
            tempory2 = tempory2+max(f(j,i+1,i)-K,0)*pathq(j,i);  
        end
        caplet(i,1) = tempory2*numerical_p2(i+1,1);
    end

    caplet

    %black
    for i=1:Tn
        nd1 = normcdf(0.5*sigma*(i^0.5),0,1);
        tempory3 = 1;
        for j=1:i+1
            tempory3 = tempory3*(1+f0(j));
        end
        black(i,1) = 0.05*(2*nd1-1)/tempory3;  
    end

    black

% testsigma(1,1) = sigma;
% testsigma(1:Tn,2) = black-caplet;
% testsigma(1:Tn,3) = testsigma(1:Tn,2).^2;
% testsigma(1,4) = sum(testsigma(1:Tn,3));
% 
% testsigma;
% 
% check(iii,1) = sigma;
% check(iii,2) = testsigma(1,4);
% 
% iii = iii+1;
% end
% 
% check
end

if (ni~=1)
    N = 0;
    d = 2/(1+exp(2*sigma*((delta)/ni)^0.5));
    u = 2-d;                                     %因為假設關係  每期u.d為相同
    pathq(1:1+ni*Tn,Tn+1) = 0;
    pathq(1,1) = 1;
    
    for i=1:Tn
        N = N+ni;
        for j=1:(ni*(i-1)+1)
            sum1 = 0;
            sum2 = 0;
            x = log(f(j,i,Tn)/f(1,1,Tn));  %x(i-1,r)
            b = 1;
            
            if (i==1)
                for k=1:Tn
                    if (k~=Tn)
                        sum1 = sum1+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==Tn)
                        sum1 = sum1+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                e2 = log(1+sum1)-0.5*sigma*sigma;
                e1 = 0;  
                exi = e2-b*e1+b*x;
                
                q = (exi-(N-ni-j+1)*log(u)-(j-1)*log(d))/(ni*(log(u)-log(d)))-log(d)/(log(u)-log(d));
                
                for h=1:ni+1
                    pathq(h,2) = pathq(1,1)*nchoosek(ni,h-1)*q^(ni-h+1)*(1-q)^(h-1);
                end      
                
            elseif (i==2)
                for k=1:Tn
                    if (k~=Tn)
                        sum1 = sum1+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==Tn)
                        sum1 = sum1+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                e1 = log(1+sum1)-0.5*sigma*sigma;
                
                for k=i:Tn
                    if (k~=Tn)
                        sum2 = sum2+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==Tn)
                        sum2 = sum2+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                e2 = log((1+sum2)*f(j,i,Tn)/f(1,1,Tn))-0.5*sigma*sigma;
                
                exi = e2-b*e1+b*x;
                
                q = (exi-(N-ni-j+1)*log(u)-(j-1)*log(d))/(ni*(log(u)-log(d)))-log(d)/(log(u)-log(d));
                for h=1:ni+1
                    pathq(h+j-1,i+1) = pathq(h+j-1,i+1)+pathq(j,i)*nchoosek(ni,h-1)*q^(ni-h+1)*(1-q)^(h-1);
                end    
                    
            else
                for k=i:Tn
                    if (k~=Tn)
                        sum2 = sum2+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma;
                    elseif (k==Tn)
                        sum2 = sum2+(delta*f(j,i,k)/(1+delta*f(j,i,k)))*sigma*sigma;
                    end
                end
                e2 = log((1+sum2)*f(j,i,Tn)/f(1,1,Tn))-0.5*sigma*sigma;
                
                for k=(i-1):Tn
                    if (k~=Tn)
                        change = f(j,i-1,k);
                        if (change==0)
                            change = f(max(find(f(:,i-1,k)~=0)),i-1,k);
                        end
                        sum1 = sum1+(delta*change/(1+delta*change))*sigma;
                    elseif (k==Tn)
                        change = f(j,i-1,k);
                        if (change==0)
                            change = f(max(find(f(:,i-1,k)~=0)),i-1,k);
                        end
                        sum1 = sum1+(delta*change/(1+delta*change))*sigma*sigma;
                    end
                end
                change = f(j,i-1,Tn);
                if (change==0)
                    change = f(max(find(f(:,i-1,Tn)~=0)),i-1,Tn);
                end
                e1 = log((1+sum1)*change/f(1,1,Tn))-0.5*sigma*sigma;
                
                exi = e2-b*e1+b*x;
                
                q = (exi-(N-ni-j+1)*log(u)-(j-1)*log(d))/(ni*(log(u)-log(d)))-log(d)/(log(u)-log(d));
                for h=1:ni+1
                    pathq(h+j-1,i+1) = pathq(h+j-1,i+1)+pathq(j,i)*nchoosek(ni,h-1)*q^(ni-h+1)*(1-q)^(h-1);
                end
                
            end
            
            
            
            
        end
        
        
    end
    pathq    
    sum(pathq)
    
end
