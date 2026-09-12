%%%%%%%%%%%%%%%%%%%%% 評價 bond option %%%%%%%%%%%%%%%%%%%%%%%

clear all;           
clc;            % a和sigma為由asigmatest跑出的值 平方和開根號誤差為0.00058374 
tic
DATA = xlsread('ZERO_RATE_3M');
 tenor=0.25;  b=29; n=12; K=0.96;   option_date=9;             
a=0.011072;    sigma=0.0046369; 
c=0;%.02*1*tenor;                             %%%%%% coupon
u=(b-1)/2;
h(1:n+1)=10*sigma*sqrt(tenor)/(b-1);
zero_rate=DATA(:,1);
alpha(1)=zero_rate(1); 
for i=1:n 
      p(i,b*(i-1)*u+1)=normcdf(((10/(b*2)-(10/b)*((b-1)/2))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2))) ;
      p(i,b*(i-1)*u+b)=1-normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*((b-1)-1))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2)));     
      for ll=2:b-1                                                                                           
          p(i,b*(i-1)*u+ll)=normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*(ll-1))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2))) ...
               -normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*((ll-1)-1))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2)));
      end
  end
  for i=2:n
      jj=(b*(i-1)-(i-2)-1)/2;
      for j=1:(b*(i-1)-(i-2)-1)/2      
          p(i,1+b*(j-1))=normcdf(((10/(b*2)-(10/b)*((b-mod(b,2))/2)+(10/b)*(1-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2))) ;
          p(i,b+b*(j-1))=1-normcdf(((10/(b*2)-(10/b)*((b-mod(b,2))/2)+(10/b)*((b-1)-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2)));     
          for ll=2:b-1
              p(i,ll+b*(j-1))=normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*(ll-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2))) ...
              -normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*((ll-1)-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2)));
          end
          jj=jj-1;
      end
      mmm=0;
      for j=b*(i-1)*u+b+1:b*(b*(i-1)-(i-2))
          p(i,j)=p(i,b*(i-1)*u+mmm);
          mmm=mmm-1;
      end
  end
Q(1:n+1,1:b*n-(n-1))=zeros(1:n+1,1:b*n-(n-1));              % Q初始值 0 ,Q:the value of paying $1 at node (i, j)
AA(1,1:n+1)=zeros(1,n+1);
Q(1,1)=1; 
    for j=1:b                                               % when i=2
        Q(2,j)=p(1,j)*exp(-alpha(1)*tenor);
        AA(2)=AA(2)+Q(2,j)*exp(-((b-1)/2-(j-1))*h(i)*tenor);
    end
    alpha(2)=(log(AA(2))+zero_rate(2)*2*tenor)/tenor;              

for i=3:n+1
    for j=1:(b*(i-1)-(i-2)+1)/2
        x=j;
        if (j>b)
            x=b+(j-b)*b; 
        end
        for k=1:min(j,b)
            if (j>b)
                k=k+(j-b);
            end
            Q(i,j)=Q(i,j)+Q(i-1,k)*p(i-1,x)*exp(-(alpha(i-1)+((b*(i-2)-(i-3)-1)/2-(k-1))*h(i))*tenor);
            x=x+(b-1);
        end
    end
    j=b*(i-1)-(i-2);
    z=1;
    for jj=(b*(i-1)-(i-2)+1)/2+1:b*(i-1)-(i-2) 
        x=b*(b*(i-2)-(i-3));
        if (z<=b)
            x=x-(z-1);
        end
        if (z>b)
            x=b*(b*(i-2)-(i-3))-(b-1)-(z-b)*b; 
        end
        for k=1:min(z,b)
            if (z>b)
                k=k+(z-b);
            end
            Q(i,j)=Q(i,j)+Q(i-1,b*(i-2)-(i-3)-(k-1))*p(i-1,x)*exp(-(alpha(i-1)+((b*(i-2)-(i-3)-1)/2-(b*(i-2)-(i-3)-(k-1)-1))*h(i))*tenor);
            x=x-(b-1);
        end
        j=j-1; z=z+1;
    end
    for j=1:b*(i-1)-(i-2)
        AA(i)=AA(i)+Q(i,j)*exp(-(((b*(i-1)-(i-2)-1)/2-(j-1))*h(i))*tenor);
    end
    alpha(i)=(log(AA(i))+zero_rate(i)*i*tenor)/tenor;
end

    r(1,1)=zero_rate(1);      
    for i=2:n+1                                             % 建造利率樹矩陣
        for j=1:b*(i-1)-(i-2)
		    r(i,j)=alpha(i)+( (b*(i-1)-(i-2)-1)/2-(j-1) )*h(i);
        end	
    end
    for j=1:b*n-(n-1)                                       % value初始值 0 ,用來存放折現的值
        value(n+1,j)=1+c;                                   % bond option 最後一期payoff
        value(n,j)=(1+c)*exp(-r(n,j)*tenor);                % coupon           
        for i=1:n-1  
		    value(i,j)=0;                                            	
        end
    end                                  
    for i=3:n+1
        qq=1;
	    for j=1:(b-1)*((n+3-i)-2)+1                                                      
            kk=j; mm=0;
			while(mm<b)
				value(n+2-i,j)=value(n+2-i,j)+exp(-r(n+2-i,j)*tenor)*p(n+2-i,qq)*(value(n+3-i,kk)+c);  % coupon
				mm=mm+1;  kk=kk+1; qq=qq+1;
            end
        end
    end
%%%%%%%%%%%%%%%%%%%%%%%% 給定K可得知RK %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
    for j=1:120
        if (K>=value(9,j) && K<value(9,j+1)) 
            RK=r(9,j);
            A=value(9,j-1);
            B=value(9,j);
            C=value(9,j+1);
            D=value(9,j+2);
  %          G=value(9,j-2);
            v=j;
        end
    end

E=0;
while( abs(K-B)>0.00001 && abs(K-C) >0.00001 && abs(K-A) >0.00001 && abs(K-D) >0.00001)% && abs(K-G) >0.00001
    RK=RK-E*0.0000000001
    nn(9)=(abs(RK-alpha(9))-mod(abs(RK-alpha(9)),h(9)))/h(9)+1;              % 上高斯
    h(9)=(abs(RK-alpha(9)))/nn(9);                                       % 調一期
                                                    
    for j=1:b*(9-1)-(9-2)                                       % i=9
	    r(9,j)=alpha(9)+( (b*(9-1)-(9-2)-1)/2-(j-1) )*h(9);
    end	
     for j=1:b*n-(n-1)                                       % value初始值 0 ,用來存放折現的值
        value(n+1,j)=1+c;                                   % bond option 最後一期payoff
        value(n,j)=(1+c)*exp(-r(n,j)*tenor);                % coupon           
        for i=1:n-1  
		    value(i,j)=0;                                            	
        end
    end                                  
    for i=3:n+1
        qq=1;
	    for j=1:(b-1)*((n+3-i)-2)+1                                                      
            kk=j; mm=0;
			while(mm<b)
				value(n+2-i,j)=value(n+2-i,j)+exp(-r(n+2-i,j)*tenor)*p(n+2-i,qq)*(value(n+3-i,kk)+c);  % coupon
				mm=mm+1;  kk=kk+1; qq=qq+1;
            end
        end
    end
    A=value(9,v-1);
    B=value(9,v);
    C=value(9,v+1);
    D=value(9,v+2);
%    G=value(9,v-2);
E=E+1;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    for j=1:b*(option_date-1)-(option_date-2)
        payoff(option_date,j)=value(option_date,j);
        if (payoff(option_date,j)-K>0)
            payoff(option_date,j)=payoff(option_date,j)-K;    %value(option_date,same)    
        else
            payoff(option_date,j)=0;
        end
    end
    
    for i=6:n+1                               %%%%%%%%%%%% 4+2=6
        qq=1;
	    for j=1:(b-1)*((n+3-i)-2)+1                                                              
            kk=j; mm=0;
			while(mm<b)
				payoff(n+2-i,j)=payoff(n+2-i,j)+exp(-r(n+2-i,j)*tenor)*p(n+2-i,qq)*payoff(n+3-i,kk);
				mm=mm+1;  kk=kk+1; qq=qq+1;
            end
        end
    end
payoff(1,1)

toc