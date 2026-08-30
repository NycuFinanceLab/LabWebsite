%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 評價三年Cap %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;               
clc;            % a和sigma為由asigmatest跑出的值 平方和開根號誤差為0.00058374 
tic
DATA = xlsread('ZERO_RATE_3M');
tenor=0.25;  b=7; n=12; K=0.0277;
a=0.011072;sigma=0.0046369;
u=(b-1)/2;
h(1:n+1)=10*sigma*sqrt(tenor)/(b-1);
zero_rate=DATA(:,1);
alpha(1)=zero_rate(1); 
for i=1:n 
      for ll=1:b                                                                                         
          p(i,b*(i-1)*u+ll)=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2))) ...
               -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2)));
      end
  end
  for i=2:n
      jj=(b*(i-1)-(i-2)-1)/2;
      for j=1:(b*(i-1)-(i-2)-1)/2      
          for ll=1:b
              p(i,ll+b*(j-1))=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*5*sigma*sqrt(tenor)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2))) ...
              -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*5*sigma*sqrt(tenor)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2)));
          end
          jj=jj-1;
      end
      mmm=0;
      for j=b*(i-1)*u+b+1:b*(b*(i-1)-(i-2))
          p(i,j)=p(i,b*(i-1)*u+mmm);
          mmm=mmm-1;
      end
  end
Q(1:n+1,1:b*n-(n-1))=zeros(1:n+1,1:b*n-(n-1));                            % Q初始值 0 ,Q:the value of paying $1 at node (i, j)
AA(1,1:n+1)=zeros(1,n+1);
Q(1,1)=1; 
    for j=1:b                                                             % when i=2
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

for i=2:n+1
    nn(i)=(abs(K-alpha(i))-mod(abs(K-alpha(i)),h(i)))/h(i)+1;                     % 上高斯
    h(i)=(abs(K-alpha(i)))/nn(i);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for i=1:n 
%       p(i,b*(i-1)*u+1)=normcdf(((10/(b*2)-(10/b)*((b-1)/2))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2))) ;
%       p(i,b*(i-1)*u+b)=1-normcdf(((10/(b*2)-(10/b)*((b-1)/2)+(10/b)*((b-1)-1))*sigma*tenor^(1/2)-0)/(sigma*tenor^(1/2)));     
      for ll=1:b                                                                                         
          p(i,b*(i-1)*u+ll)=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2))) ...
               -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*5*sigma*sqrt(tenor)-0)/(sigma*tenor^(1/2)));
      end
  end
  for i=2:n
      jj=(b*(i-1)-(i-2)-1)/2;
      for j=1:(b*(i-1)-(i-2)-1)/2      
%           p(i,1+b*(j-1))=normcdf(((10/(b*2)-(10/b)*((b-mod(b,2))/2)+(10/b)*(1-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2))) ;
%           p(i,b+b*(j-1))=1-normcdf(((10/(b*2)-(10/b)*((b-mod(b,2))/2)+(10/b)*((b-1)-1))*sigma*tenor^(1/2)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2)));     
          for ll=1:b
              p(i,ll+b*(j-1))=normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*(ll-1))*5*sigma*sqrt(tenor)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2))) ...
              -normcdf(((1/b-(2/b)*((b-1)/2)+(2/b)*((ll-1)-1))*5*sigma*sqrt(tenor)-(a*jj*h(i)*tenor))/(sigma*tenor^(1/2)));
          end
          jj=jj-1;
      end
      mmm=0;
      for j=b*(i-1)*u+b+1:b*(b*(i-1)-(i-2))
          p(i,j)=p(i,b*(i-1)*u+mmm);
          mmm=mmm-1;
      end
  end
Q(1:n+1,1:b*n-(n-1))=zeros(1:n+1,1:b*n-(n-1));                            % Q初始值 0 ,Q:the value of paying $1 at node (i, j)
AA(1,1:n+1)=zeros(1,n+1);
Q(1,1)=1; 
    for j=1:b                                                             % when i=2
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    r(1,1)=zero_rate(1);      
    for i=2:n+1                                                                  
        for j=1:b*(i-1)-(i-2)
		    r(i,j)=alpha(i)+( (b*(i-1)-(i-2)-1)/2-(j-1) )*h(i);
        end	
    end
    for j=1:b*n-(n-1)                                        
        value(n+1,j)=0;                                              
        for i=1:n 
		    value(i,j)=0;                                            	
        end
    end                                  
    for i=2:n+1
        qq=1;
	    for j=1:(b-1)*((n+3-i)-2)+1                                          
            kk=j; mm=0;
			while(mm<b)
				value(n+2-i,j)=value(n+2-i,j)+exp(-r(n+2-i,j)*tenor)*p(n+2-i,qq)*(value(n+3-i,kk)+max(r(n+3-i,kk)-K,0));
				mm=mm+1;  kk=kk+1; qq=qq+1;
            end
        end
    end

value(1,1)

toc