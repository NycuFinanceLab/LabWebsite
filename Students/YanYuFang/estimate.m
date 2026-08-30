clear all; clc      % 此程式用來校正H-W的a和sigma  用戴慈學姐的方式
                    % 把戴慈學姊的C++程式寫成matlab
DATA = xlsread('data');
vol=DATA(:,1);
zero_rate=DATA(:,2);
forward_rate=DATA(:,3);

a_sigma(1:100,1:100)=zeros(100);       

strike=input(' cap strike rate : ');
B_cap(1:7,1)=zeros(7,1);
for i=1:39                                                           % use Black equation to price cap 
	reset_time=0.25*i;
	pay_time=0.25+0.25*i;
    B_caplet(i)=blacks(exp((-1)*zero_rate(i)*pay_time),forward_rate(i),strike,0.25,reset_time,vol(i)); 
      
	if i<4
		B_cap(1)=B_cap(1)+B_caplet(i);
    end
	if i<8
		B_cap(2)=B_cap(2)+B_caplet(i);
    end
    if i<12
		B_cap(3)=B_cap(3)+B_caplet(i);
    end
	if i<16
		B_cap(4)=B_cap(4)+B_caplet(i);
    end
	if i<20
		B_cap(5)=B_cap(5)+B_caplet(i);
    end
	if i<28
		B_cap(6)=B_cap(6)+B_caplet(i);
    end    
	if i<40
		B_cap(7)=B_cap(7)+B_caplet(i);
    end    
end
    

start_a=0.009;  end_a=0.909;  start_s=0.01;  end_s=0.5;  n=1;
k=1/(1+strike*0.25);

adjust=2;
while adjust==2                                                  

    for a=1:100
       A=(end_a-start_a)*a*0.01+start_a;

       for s=1:100
           S=(end_s-start_s)*s*0.01+start_s;
           H_cap(1:7,1)=zeros(7,1);
        
           for i=1:39                                                           % use HW equation to price cap 
	       reset_time=0.25*i;
	       pay_time=0.25+0.25*i;
           F=1.0/(1+forward_rate(i)*0.25);
           volp=(S^2/(2*A^3))*(1-exp(-2*A*reset_time))*(1-exp(-A*0.25))^2;   %sigma_p^2 
           d1=(log(F/k)+volp/2)/sqrt(volp);
		   d2=d1-sqrt(volp);
           H_caplet(i)=1/k*exp(-zero_rate(i)*reset_time)*(k*normcdf(-d2)-F*normcdf(-d1));
               
      
	       if i<4
		   H_cap(1)=H_cap(1)+H_caplet(i);
               end
	       if i<8
		   H_cap(2)=H_cap(2)+H_caplet(i);
               end
               if i<12
		   H_cap(3)=H_cap(3)+H_caplet(i);
               end
	       if i<16
	  	   H_cap(4)=H_cap(4)+H_caplet(i);
               end
	       if i<20
		   H_cap(5)=H_cap(5)+H_caplet(i);
               end
	       if i<28
		   H_cap(6)=H_cap(6)+H_caplet(i);
               end    
	       if i<40
		   H_cap(7)=H_cap(7)+H_caplet(i);
               end    
           end              

           for i=1:39
               a_sigma(a,s)=a_sigma(a,s)+(H_caplet(i)-B_caplet(i))*(H_caplet(i)-B_caplet(i));   %100*100
           end

       end
    end

    mini=1000;  index_a=0;  index_s=0;
    for a=1:100
        for s=1:100
	        if (a_sigma(a,s)<=mini )
	            mini=a_sigma(a,s);
	            index_a=(end_a-start_a)*a*0.01+start_a;
	            index_s=(end_s-start_s)*s*0.01+start_s;
            end
	    end				
    end			

    if ( (sqrt(mini) < 10^(-5)) || (n> 8) )
        adjust=3;
    end
 		                       
	period_a=(end_a-start_a);
	period_s=(end_s-start_s);
	end_a=index_a+period_a*0.01;
	start_a=index_a-period_a*0.01;
	end_s=index_s+period_s*0.01;
	start_s=index_s-period_s*0.01;				
    n=n+1;
    
end

index_a
index_s
mini=sqrt(mini)
