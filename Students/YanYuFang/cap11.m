%%%%%%%%%%%%%%%%%%%%%%%% 評價三個月Cap %%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;               
clc;            % a和sigma為由asigmatest跑出的值 平方和開根號誤差為0.00058374 
tic
DATA = xlsread('ZERO_RATE_3M');         
tenor=0.25;  b=7; n=1; K=0.0277;      % b為branch個數, n為期數, K為利率上限
a=0.011072;  sigma=0.0046369;
u=(b-1)/2;
h(1:n+1)=10*sigma*sqrt(tenor)/(b-1);  % 利率間隔高度
zero_rate=DATA(:,1);
alpha(1)=zero_rate(1); 
for i=1:n                             % 機率
      for ll=1:b                                                                                           
          p(i,b*(i-1)*u+ll)=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2))) ...
               -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2)));
      end
end
 
Q(1:n+1,1:b*n-(n-1))=zeros(1:n+1,1:b*n-(n-1));      % Q初始值0 ,Q:the value of paying $1 at node (i, j)
AA(1,1:n+1)=zeros(1,n+1);
Q(1,1)=1; 
    for j=1:b                                       % when i=2
        Q(2,j)=p(1,j)*exp(-alpha(1)*tenor);
        AA(2)=AA(2)+Q(2,j)*exp(-((b-1)/2-(j-1))*h(i)*tenor);
    end
    alpha(2)=(log(AA(2))+zero_rate(2)*2*tenor)/tenor;              

for i=2:n+1
    nn(i)=(abs(K-alpha(i))-mod(abs(K-alpha(i)),h(i)))/h(i)+1;     % 上高斯
    h(i)=(abs(K-alpha(i)))/nn(i);                          % 調整間隔高度
end

for i=1:n  
      for ll=1:b                                                                                           
          p(i,b*(i-1)*u+ll)=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*h(2)*(b-1)/2-0)/(sigma*tenor^(1/2))) ...
               -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*h(2)*(b-1)/2-0)/(sigma*tenor^(1/2)));
      end
end
 
Q(1:n+1,1:b*n-(n-1))=zeros(1:n+1,1:b*n-(n-1));                          
AA(1,1:n+1)=zeros(1,n+1);
Q(1,1)=1; 
    for j=1:b                                                          
        Q(2,j)=p(1,j)*exp(-alpha(1)*tenor);
        AA(2)=AA(2)+Q(2,j)*exp(-((b-1)/2-(j-1))*h(i)*tenor);
    end
    alpha(2)=(log(AA(2))+zero_rate(2)*2*tenor)/tenor;              


    r(1,1)=zero_rate(1);      
    for i=2:n+1                                             % 建造利率樹矩陣
        for j=1:b*(i-1)-(i-2)
		    r(i,j)=alpha(i)+( (b*(i-1)-(i-2)-1)/2-(j-1) )*h(i);
        end	
    end
    for j=1:b*n-(n-1)                       % value初始值 0 ,用來存放折現的值
        value(n+1,j)=max(r(n+1,j)-K,0);                 % cap最後一期payoff
        for i=1:n 
		    value(i,j)=0;                                            	
        end
    end                                  
    for i=2:n+1
        qq=1;
	    for j=1:(b-1)*((n+3-i)-2)+1                                          
            kk=j; mm=0;
			while(mm<b)
				value(n+2-i,j)=value(n+2-i,j)+exp(-r(n+2-i,j)*tenor)*p(n+2-i,qq)*(value(n+3-i,kk));
				mm=mm+1;  kk=kk+1; qq=qq+1;
            end
        end
    end

value(1,1)                       % 期初價格

toc