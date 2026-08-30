function v=HWBondVol(a,sigma,T,U)
v=sigma^2/(2*a^3)*((1-exp(-2*a*T)) +exp(-2*a*U)*(exp(2*a*T)-1)+ 2*exp(-a*U)* (exp(a*T)-exp(-a*T)) );