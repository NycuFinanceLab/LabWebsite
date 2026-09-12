#include <stdio.h>
#include <math.h>
#include<malloc.h>
#include<time.h>
#include <string.h>
int Smallest_Integer(double );
double rmax_rmin(double  ,double  ,double  ,double ,double ,double ,int  );
double Max(double , double );
double Mu_hat(int ,int  ,int,double ,double ,double );
void probability_(double,double P[4],double );
void tri_probability_(double ,double P[6],double,double ); 
double zerocouponbond(int ,double** ,int  ,int ,double ,double  );
double Vasicek(double , double , double , double , double );
double Credit_Spread(double ,double ,double ,double   );

//建立動態陣列副程式
//#define ARRAY_ROW	n+2  
//#define ARRAY_COL	n+1  
double **CreateArray(int row, int col)
{
	double ** arr = (double **) calloc(row, sizeof(double *));
	int i;

	for(i=0;i<row;i++)
		arr[i] = (double *) calloc(col, sizeof(double));

	return arr;
}
//釋放記憶體
void FreeArray(double **arr, int row)
{
	int i;

	for(i=0;i<row;i++)
		free(arr[i]);

	free(arr);
}

FILE *wf;
void main()

{
  int i,jA,jR,n,k;//  n:期數;
  int jumptime=0; //檢視期
  int    bin_node[4]={0};   //trinode,binode(in jumptime); trinode=binode+2；檢視期與到期日T差2dt
  double dR,dA;
  //公司，利率起始值。
  double r,S0=800,r0=0.05,T=0.5,a=0.1,b=0.1,sigmaA=0.3,sigmaR=0.035,tho=0.3,dt;//sigmaR-a*sigmaA>0
  double L=400,D=400,error=0.000001,w=0.5,ratio=0.1;   //barrier
  double x=0,ymax,y,Xmax,y0;
  double rmax,rmin;  
  
  wf=fopen("cs.txt","w");
  n=100;
  
  
  
  for(k=0;k<1;k++)
  {
	int  ARRAY_ROW=n+2;
	int  ARRAY_COL=n+1;
  double **lnVS= CreateArray(ARRAY_ROW, ARRAY_COL);
  double **B= CreateArray(ARRAY_ROW, ARRAY_COL);
  double P[6]={0.0};
  double jump[1]={0.0};      // jump size in jumptime;
  double mu_hat,ZCB;
  
 
  //  L=D*ratio;
  // rf1=fopen("bond.txt","w");
    dt=T/n; 
	dA=sigmaA*sqrt(dt);
    dR=sigmaR*sqrt(dt);
    //獨立two-dimensional Binomial v.s. BTT 起始值

	y0=r0/(sigmaR*sqrt(1-tho*tho));
		  
	rmax=rmax_rmin(sigmaA,sigmaR,tho,dt,a,b,1); //當切割長度很長時，決定的上下界使得機率恆正
	rmin=rmax_rmin(sigmaA,sigmaR,tho,dt,a,b,2);	
	bin_node[0]=1;
	mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r0,x);
//  printf("mu_hat:%lf\n",S0*exp(mu_hat));
	Xmax=mu_hat+2*dA+(n-1)*dA;
//    Xmax=dA*n;  //two-dimensional Binomial
//  printf("Xmax:%lf\n",S0*exp(Xmax));
	
	//算最後一期資產payoff
	for(jA=0;jA<n+2;jA++)    //jA:0~5(n+2)
	{
		  for(jR=0;jR<n+1;jR++)  //jR:0~4(n+1)
		  { 		 
			 x=Xmax-2*dA*jA;
			 
			 if(S0*exp(x)>=(L+error))
				lnVS[jA][jR]=D;
			 
			 else  
				lnVS[jA][jR]=(1-w)*S0*exp(x);//*(D/L);  //W%:bankruptcy cost;D/L:repayment ration  
		
			 //	lnVS[jA][jR]=Max(S0*exp(x)-L,0);  //equity
		//  	if(jR==0)
		//		printf("lnVS[%d][%d]:%lf\n",jA,jR,S0*exp(x));  
		/*	if(jR==0&&jA==n)
				printf("lnVSmin[%d][%d]:%lf\n",jA,jR,S0*exp(x));
		
		     */
		  }
		 
	      //   printf("%\n");
	}
	
	
	//Backward induction；碰觸利率上界前(2-dimensional Binomial Lattice)
	//Binomial discount

	for(i=n-1;i>=1;i--)
	{
	  //mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r,0); //第一期的mu_hat
		ymax=y0+sqrt(dt)*i;  //每一期y最大值   
		Xmax=mu_hat+2*dA+(i-1)*dA;
	//	Xmax=dA*i;// Two-dimensional Binomial Lattice 	
    //  printf("t:%d\n",i);


	for(jA=0;jA<i+2;jA++)
		{
		  for(jR=0;jR<i+1;jR++)
		  {
				y=ymax-jR*2*sqrt(dt);
				x=Xmax-jA*2*sigmaA*sqrt(dt);  	   
   			    r=sigmaR*(sqrt(1-tho*tho)*y+tho*x/sigmaA);			    
		
			 /*	if(i==2)
				{	
					if(jA==0&&jR==0)
					{
						//printf("hat_high[%d]=%lf\n",i+1,S0*exp(Xmax+3*dA));
						//printf("hat_low[%d]=%lf\n",i+1,S0*exp(Xmax-9*dA));
						
					}
				  printf("rmax[%d][%d]:%lf\n",jA,jR,r);
					
					//    if(jA==n&&jR==n-1)
				//		printf("rmin[%d][%d]:%lf\n",jA,jR,r);
				 	
				//	printf("mu_hat[%d]:%lf\t",jA,S0*exp(mu_hat));	//hat_low+dA<=mu_hat<=hat_high+dA
				//	printf("V[%d]:%lf\t",jA,S0*exp(x));	
					 
				}*/
				   //	if(i==1&&jR==0)
				  //  	printf("VS[%d][%d]:%lf\n",jA,jR,S0*exp(x));
				 
			
     		 //	r=r0;   // Merton's Model
				probability_(r,P,dt);  
				
			/*	if(i==0)
			 {
				
				 for(j=0;j<4;j++)   
				  printf("P[%d][%d]:%lf\n",jA,jR,P[j]);	 

			} 
			 */
				
			lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[jA][jR]+P[1]*lnVS[jA][jR+1]+P[2]*lnVS[jA+1][jR]+P[3]*lnVS[jA+1][jR+1]);
		
		//	if(i==1)
		//		printf("lnVS[%d][%d]:%lf\n",jA,jR,lnVS[jA][jR]);
			
			//	FPM :(Black&Cox)
							   			  
	       if((n-i)%2==0) //(連續檢視)到期日以及檢視期差2dt
			 {	
				if(S0*exp(x)<=(L+pow(0.1,8)))
					lnVS[jA][jR]=(1-w)*S0*exp(x);//*(D/L);  
				//對債權人保護 //discrete: Asset<=B+error //Debt=Asset*(1-B);B=0;無破產成本  
			 }	
		        	 
					 
			 
		  } 
		    //r:printf("\n");   
		   
		}
				
	
	//	printf("\n");
    } 
	
		 		//trinomial discount(at t=0)
  			
		if(i==0)
		{
			    r=r0;
				x=0;  //起始值出發
				mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r,x);				
				tri_probability_(r,P,mu_hat,dt);		//ok!  
			   
				/* for(j=0;j<6;j++)
				 {  
					 sum+=P[j];
					 printf("%lf\n",sum);
					 printf("%lf\n",P[j]); 
				 }  
				*/
				//  for(l=0;l<3;l++)
				 //         printf("P[%d]:%lf\n",l,P[0+2*l]+P[1+2*l]);
				
			  lnVS[0][0]=exp(-r*dt)*(P[0]*lnVS[0][0]+P[1]*lnVS[0][1]+P[2]*lnVS[1][0]+P[3]*lnVS[1][1]+P[4]*lnVS[2][0]+P[5]*lnVS[2][1]);
		                   	   
		}	
		 
              
//	fprintf(wf,"n:%d\t",n);
//	fprintf(wf,"%.6lf\n",lnVS[0][0]);    
//	printf("T=%lf\n",T);
//	printf("mu_hat=%.7lf\n",S0*exp(mu_hat));
//  printf("p(0,T)=%.7lf\n",zerocouponbond(n,B,jumptime,bin_node[0],T,mu_hat));
//	printf("p(0,T)=%.7lf\n",Vasicek(r0,a,b,sigmaR,T));
//	printf("rmax=%lf\nrmin=%lf\n",rmax,rmin);
//	printf("n:%d\n",n);	
    ZCB=zerocouponbond(n,B,jumptime,bin_node[0],T,mu_hat);
//  ZCB=Vasicek(r0,a,b,sigmaR,T);
//	ZCB=exp(-r0*T);
//    printf("bond valude:%lf\n",lnVS[0][0]);
	printf("%lf\n",Credit_Spread(T,lnVS[0][0],D,ZCB));
    fprintf(wf,"%lf\n",Credit_Spread(T,lnVS[0][0],D,ZCB));
//	printf("\n");
//  n=n+100;
//    ratio=ratio+0.1;
	T=T+1;
	FreeArray(lnVS, ARRAY_ROW);
    FreeArray(B, ARRAY_ROW);


}

	fclose(wf);
}    
//計算(jA,jR)到下期的機率副程式2*2 ; //if  rmin<r<rmax  >>  0<pa,pr<1

void probability_(double r,double P[4],double dt )
 {

  double a=0.1,b=0.1,sigmaA=0.3,sigmaR=0.035,tho=0.3;  
  double muA,muR; 
  double pr,pa;

			muA=r-sigmaA*sigmaA/2;
			muR=a*(b-r);
		
		  //利率與資產機率

		  pa=0.5+0.5*muA*sqrt(dt)/sigmaA;
		  pr=0.5+0.5*sqrt(dt/(1-tho*tho))*(-tho*muA/sigmaA+muR/sigmaR);
	  
		  P[0]=pa*pr;
		  P[1]=pa*(1-pr);
		  P[2]=(1-pa)*pr;
		  P[3]=(1-pa)*(1-pr);

}

//三元數機率(由mu_hat以及對應的muA計算)

void tri_probability_(double r,double P[6],double mu_hat,double dt)
{
  double a=0.1,b=0.1,sigmaA=0.3,sigmaR=0.035,tho=0.3;  
  double muA,muR; 
  double pr,pu,pm,pd; 
  double beta,alpha,gamma,Var;
  double delta,delta_u,delta_m,delta_d;
   
   muA=r-sigmaA*sigmaA/2;
   muR=a*(b-r);
   pr=0.5+0.5*sqrt(dt/(1-tho*tho))*(-tho*muA/sigmaA+muR/sigmaR);
   
   beta=mu_hat-muA*dt;  //    -dA<beta<dA
 //  beta=-sigmaA*sqrt(dt);;
   alpha=beta+2*sigmaA*sqrt(dt);
   gamma=beta-2*sigmaA*sqrt(dt);
   Var=sigmaA*sigmaA*dt;
   
   delta=(beta-alpha)*(gamma-alpha)*(gamma-beta);
   delta_u=(beta*gamma+Var)*(gamma-beta);
   delta_m=(alpha*gamma+Var)*(alpha-gamma);
   delta_d=(alpha*beta+Var)*(beta-alpha);
   
   pu=delta_u/delta;
   pm=delta_m/delta;
   pd=delta_d/delta;
 
   P[0]=pu*pr;
   P[1]=pu*(1-pr);	
   P[2]=pm*pr;
   P[3]=pm*(1-pr);
   P[4]=pd*pr;
   P[5]=pd*(1-pr);
}
//計算mu_hat副程式(到期日,jump發生時點,三元樹起始點個數)
double Mu_hat(int n,int jumptime ,int Binode,double dt,double r,double x)
{
  double S0=800,L=400,sigmaA=0.3,standB,mu_hat;  //standB:最靠近Barrier的點 
  double muA;		
  double R_mu_standB;  //mu到standB的距離
  double RmuB;  //R_mu_standB的整數倍2dA間隔
  double R,U,error=0.000001;
  int i;
	
	U=sigmaA*sqrt(dt);	

	
  for (i=0; i<Binode+1; i++)  //第 i個檢視期
	{
		muA=x+(r-0.5*sigmaA*sigmaA)*dt;  //每一期的mu
	
	
		if(n%2==0)
			standB=log(L/S0)+sigmaA*sqrt(dt);  //no jump barrier
		else
			standB=log(L/S0);
	  
		R_mu_standB=(muA-standB)/(2*U);
		
		RmuB=(int)floor(R_mu_standB);  // [3.2]>>3 補!
		
			R=fabs (R_mu_standB-RmuB);
		
			if (R*2*U > U+error)
				RmuB++;
			else
				RmuB=RmuB;
	}	
	
			mu_hat=standB+RmuB*2*U;  //k找mu_hat的個數

//	if(fabs(mu_hat-muA)<=U+error)
			return mu_hat;		
//	else	
//			return mu_hat+U;
}


int Smallest_Integer(double value)
{
   int temp;
   value=fabs(value);
   temp=(int)value;
   if(fabs((double)temp-value)<0.000001)
	   return temp;
    return temp+1;
}

 
 //決定r上下界的副程式
 
 double rmax_rmin(double sigmaA ,double sigmaR ,double tho ,double dt,double a,double b,int C  )
 {
 double l,c,d;
 double rmax,rmin,lr1,lr2,ur1,ur2; 
	
    l=sigmaA*sigmaR*sqrt((1-tho*tho)/dt); 
	c=0.5*(sigmaA*sigmaA*sigmaR*tho);
	d=sigmaA*a*b;

	//計算r的上界
   if(C==1)
   {
	   
	ur1=    (-l-c-d)/(-tho*sigmaR-a*sigmaA);
	ur2=	sigmaA/sqrt(dt)+0.5*sigmaA*sigmaA;
	
    rmax=fabs(Max(-ur1,-ur2));//取min rmax為上界
	
    return rmax; 

   }
   //計算r的下界
   else if(C==2)
   {
	
    lr1=  (l+c+d)/(-tho*sigmaR-a*sigmaA);
	lr2=  -sigmaA/sqrt(dt)+0.5*sigmaA*sigmaA;
	
	rmin=Max(lr1,lr2); //取max rmin為下界
		
	return rmin;
   
   }
	  
   else 
     return 0;
 }

//p(0,T)到期日T零息債券的價值
double zerocouponbond(int n,double**p ,int jumptime ,int Binode,double T,double mu_hat  )
{
	int jA,jR,i;
	double ymax,y,r,dA,y0,r0=0.05,X,Xmax;	
	double S0=800,L=400,sigmaA=0.3,sigmaR=0.035,dt,tho=0.3;
	double P[6]={0.0};

	
	dt=T/n;
	dA=sigmaA*sqrt(dt);
	y0=r0/(sigmaR*sqrt(1-tho*tho));
	//到期日payoff=1;
	for(jA=0;jA<n+2;jA++)    //jA:0~5(n+2)
	{
		  for(jR=0;jR<n+1;jR++)  //jR:0~4(n+1)			  
			 p[jA][jR]=1.0;  		           	
	
	}
	
	for(i=n-1;i>=1;i--)
	{
	//  Xmax=dA*i;// Two-dimensional Binomial Lattice  paper test
       
		Xmax=mu_hat+2*dA+(i-1)*dA;

		ymax=y0+sqrt(dt)*i;  //每一期y最大值   
		
	         /* if(n%2==0) 
		 			xmax=(log(L/S0)+2*dA*3+(i-2)*dA); 
			  else
					xmax=(log(L/S0)+2*dA*2+(i-1)*dA);
		      */
		for(jA=0;jA<i+2;jA++)
		{
		  for(jR=0;jR<i+1;jR++)
		  {	  			  
					y=ymax-jR*2*sqrt(dt);
			//		x=xmax-jA*2*sigmaA*sqrt(dt); //error!  ?
				    X=Xmax-jA*2*sigmaA*sqrt(dt);
					r=sigmaR*(sqrt(1-tho*tho)*y+tho*X/sigmaA);

			 // if(i==jumptime)
			//	r=0.05;
			  //discount ok!  
			  probability_(r,P,dt);
			  p[jA][jR]=exp(-r*dt)*(P[0]*p[jA][jR]+P[1]*p[jA][jR+1]+P[2]*p[jA+1][jR]+P[3]*p[jA+1][jR+1]);          			

		  }
	
		    //   printf("%\n"); 
		}
  
    } 
	        
				r=r0;
				X=0;
				mu_hat=Mu_hat(n,jumptime,Binode,dt,r,X);				
		   //	probability_(r,P,dt);
				tri_probability_(r,P,mu_hat,dt);		//ok!  
			 //     for(l=0;l<6;l++)
			//	    printf("%lf\n",P[l]);
				p[0][0]=exp(-r*dt)*(P[0]*p[0][0]+P[1]*p[0][1]+P[2]*p[1][0]+P[3]*p[1][1]+P[4]*p[2][0]+P[5]*p[2][1]);
			
				
	
	return  p[0][0];

}
 
 double Max(double a, double b)
{
  if(a>b) return a;
  else return b;
}

 //Closed-form Vasicek for zero-coupon bond; It's ok for CIR.

 double Vasicek(double r0, double a, double b, double sigmaR, double T)
 {
	double tau,t=0;
	double A,B,P;
 /*test
	r0=0.05;
	a=0.1;
	b=0.05;
	sigmaR=0.035;*/

	tau=T-t;
	B=(1-exp(-a*tau))/a;
	A=exp((B-tau)*(a*a*b-0.5*sigmaR*sigmaR)/(a*a)-sigmaR*sigmaR*B*B/(4*a));
	P=A*exp(-B*r0);

	return P;
 
 
 }

double Credit_Spread(double T,double bondvalue,double D,double ZCB  )
{
  double CS; 

	CS=-log((bondvalue)/(D*ZCB))/T;
    
	return CS;
}