clc
clear all
format long 

V = 800;
K = 400;
X = V/K;
n = 200;
T = 1;
r = 0.05;
sigma = 0.3;
eta = 0.035;
alpha = 0.05;
rho = 0.3;
beta = 0.1;
w = 1;

count = 1;

for w=0:0.01:1
    clear q
for i=1:n
    t = i*T/n;
    M = ((alpha-rho*sigma*eta)/beta-eta^2/beta^2-sigma^2/2)*t+(rho*sigma*eta/beta^2+eta^2/2/beta^3)*exp(-beta*T)*(exp(beta*t)-1)+(r/beta-alpha/beta^2+eta^2/beta^3)*(1-exp(-beta*t));
    S = (rho*sigma*eta/beta+eta^2/beta^2+sigma^2)*t-(rho*sigma*eta/beta^2+2*eta^2/beta^3)*(1-exp(-beta*t))+(eta^2/2/beta^3)*(1-exp(-2*beta*t));
    a = (-log(X)-M)/S^0.5;
    if(i==1)
        q(1,1) = normcdf(a);
    else
        sum1 = 0;
        for j=1:i-1
            t1 = j*T/n;
            M1 = ((alpha-rho*sigma*eta)/beta-eta^2/beta^2-sigma^2/2)*t1+(rho*sigma*eta/beta^2+eta^2/2/beta^3)*exp(-beta*T)*(exp(beta*t1)-1)+(r/beta-alpha/beta^2+eta^2/beta^3)*(1-exp(-beta*t1));
            S1 = (rho*sigma*eta/beta+eta^2/beta^2+sigma^2)*t1-(rho*sigma*eta/beta^2+2*eta^2/beta^3)*(1-exp(-beta*t1))+(eta^2/2/beta^3)*(1-exp(-2*beta*t1));
            b = (M1-M)/(S-S1)^0.5;
            sum1 = sum1+q(j,1)*normcdf(b);  
        end
        q(i,1) = normcdf(a)-sum1;
    end
end

sum(q);

D = 400*0.9514097;
p(count,1) = D-w*D*sum(q);
count = count+1;
end

p