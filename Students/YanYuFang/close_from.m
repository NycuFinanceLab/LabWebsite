%%%%%%%%%%%%%%%%%%%%%%%%% 評價bond option的H-W close form %%%%%%%%%%%%%%%%%%%%%%%

clear all;               
clc; 
a=0.011072; sigma=0.0046369; U=3; T=2;
BU=0.9316715846 ; BT=0.9546899442 ; % BT:兩年期ZCB, BU:三年期ZCB
 
K=0.96;

v=sigma^2/(2*a^3)*((1-exp(-2*a*T)) +exp(-2*a*U)*(exp(2*a*T)-1)-2*exp(-a*U)* (exp(a*T)-exp(-a*T)) );
d1=(log(BU/(K*BT))+0.5*v)/sqrt(v);
d2=(log(BU/(K*BT))-0.5*v)/sqrt(v);
bond_call=BU*normcdf(d1)-K*BT*normcdf(d2)