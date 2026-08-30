function caplet= blacks(P,F,K,t,T,sigma)   %Black's評價caplet的公式
d1 = ( log(F/K)+(0.5*sigma^2)*T )/(sigma*sqrt(T));
d2 = d1-sigma*sqrt(T);
caplet= t*P*(F*normcdf(d1)-K*normcdf(d2));