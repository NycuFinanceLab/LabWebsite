clear all;   % 此程式用來校正H-W的a和sigma  用最小平方內建  結果比學姊的方式好
clc;         % 副程式 fun
tic
DATA = xlsread('data');
vol=DATA(:,1);
zero_rate=DATA(:,2);
forward_rate=DATA(:,3);
strike=0.0277;       

k=1/(1+strike*0.25);            

asigma0=[0.918,0.01];
asigma=lsqnonlin(@(asigma) fun(asigma,zero_rate,forward_rate,strike,k,vol),asigma0);
error=fun(asigma,zero_rate,forward_rate,strike,k,vol);
totalerror=norm(error);

toc