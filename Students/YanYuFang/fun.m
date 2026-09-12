function error=fun(asigma,zero_rate,forward_rate,strike,k,vol)       % 此程式為asigmatest的副程式  需要用到black副程式

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


       H_cap(1:7,1)=zeros(7,1);
        
       for i=1:39                                                           % use HW equation to price cap 
	       reset_time=0.25*i;
	       pay_time=0.25+0.25*i;
           F=1.0/(1+forward_rate(i)*0.25);
           volp=(asigma(2)^2/(2*asigma(1)^3))*(1-exp(-2*asigma(1)*reset_time))*(1-exp(-asigma(1)*0.25))^2;   %sigma_p^2 
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
      
error=H_caplet-B_caplet;      
       
    