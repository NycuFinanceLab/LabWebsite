#include <stdio.h>
#include <math.h>
#include<malloc.h>
#include<time.h>
int Smallest_Integer(double );
double rmax_rmin(double  ,double  ,double  ,double ,double ,double ,int  );
double Max(double , double );
double Mu_hat(int ,int  ,int,double ,double ,double ,double,double);
void probability_(double,double P[4],double );
void tri_probability_(double ,double P[6],double,double ,double  ); 
double zerocouponbond(int ,double** ,int  ,int ,double ,double  );
double Vasicek(double , double , double , double , double );
double Credit_Spread(double ,double ,double ,double );

//建立動態陣列副程式
//#define ARRAY_ROW	n+4  
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

FILE *rf1;
void main()

{
  int i,jA,jR,n=4,k;//  n:期數;
  int jumptime=0,jumptime1=2; //檢視期
  int bin_node[4]={0},trinode;   //trinode,binode(in jumptime); trinode=binode+2；檢視期與到期日T差2dt
  int findnode=0,fn;
  double dR,dA;
 //公司，利率起始值。
  double r,S0=800,r0=0.05,T=1,a=0.1,b=0.1,sigmaA=0.3,sigmaR=0.035,tho=0.3,dt;//sigmaR-a*sigmaA>0
  double L1=400,L2=400,D=400,error=0.000001,ratio=0.1;   //ratio*barrier
  double X,x=0,ymax,y,Xmax,y0,ymin;
  double rmax,rmin,Rmax,Rmin,w=0.15,ZCB,EBIT=100,tax=0.5,C=0;  

  rf1=fopen("bond.txt","w");

  for(k=0;k<1;k++)  
{
  int ARRAY_ROW=5000;  
  int ARRAY_COL=n+1;
  double **lnVS= CreateArray(ARRAY_ROW, ARRAY_COL);
  double **B= CreateArray(ARRAY_ROW, ARRAY_COL);
  double P[6]={0.0};
  double jump[2]={0.0};      // jump size in jumptime;
  double mu,mu_hat[2]={0.0},muhat[2]={0.0},muhat_right;
  double binmax_noj,binmin_noj; 
  
  //調整違約門檻
  L1=D*ratio; 
  L2=D;   

  // rf1=fopen("bond.txt","w");
//  jump[1]=(EBIT-C)*tax;
	jump[1]=0;
	//	lnB=log((L-jump[1])/S0); //barrier
	dt=T/n; 
	dA=sigmaA*sqrt(dt);
    dR=sigmaR*sqrt(dt);
    //獨立two-dimensional Binomial v.s. BTT 起始值
	y0=r0/(sigmaR*sqrt(1-tho*tho));
		  
	rmax=rmax_rmin(sigmaA,sigmaR,tho,dt,a,b,1); //當切割長度很長時，決定的上下界使得機率恆正
	rmin=rmax_rmin(sigmaA,sigmaR,tho,dt,a,b,2);	
	
//***由起始mu_hat以及jump後的mu_hat開始建格子點	
	
	bin_node[0]=1;
	mu_hat[0]=Mu_hat(n,jumptime,bin_node[0],dt,r0,x,jump[0],L1); //ㄧ開始接的mu_hat;L1 barrier
	

	ymax=y0+sqrt(dt)*jumptime1;
	ymin=y0-sqrt(dt)*jumptime1;
	binmax_noj=mu_hat[0]+2*dA+(jumptime1-1)*dA;  //jump前bin最大值;Xmax
	binmin_noj=binmax_noj-(jumptime1+1)*2*dA;  //jump前bin最小值
	Rmax=sigmaR*(sqrt(1-tho*tho)*ymax+tho*binmax_noj/sigmaA);
	Rmin=sigmaR*(sqrt(1-tho*tho)*ymin+tho*binmin_noj/sigmaA);
	muhat[0]=Mu_hat(n,jumptime,bin_node[0],dt,Rmax,binmax_noj,jump[0],L2); //最高Mu_hat;L2 barrier 										
	muhat[1]=Mu_hat(n,jumptime,bin_node[0],dt,Rmin,binmin_noj,jump[0],L2); //最低Mu_hat
	
	trinode=3+(int)ceil((muhat[0]-muhat[1])/(2*dA)); //jump後節點數個數
    Xmax=muhat[0]+2*dA+(n-(jumptime1+1))*dA; 
//	printf("binmax:%lf\n",S0*exp(binmax_noj));
//	printf("binmax:%lf\n",S0*exp(binmin_noj));
//	printf("muhatmax:%lf\n",S0*exp(muhat[0]));
//	printf("muhatmin:%lf\n",S0*exp(muhat[1]));
//	printf("trinode:%d\n",trinode);
	//(mu_hat_hgih-mu_hat_low)/(2dA)

	//	Xmax=mu_hat+2*dA+(n-1)*dA; 		
		
	//	Xmax=dA*n;
	//  printf("mu_hat:%lf\n",S0*exp(mu_hat[0]));

	//  +MU_HAT	
	for(jA=0;jA<n-(jumptime1+1)+trinode+1;jA++)    //算最後一期資產payoff
	{
		  for(jR=0;jR<n+1;jR++)  //jR:0~4(n+1)
		  { 		 
			 x=Xmax-2*dA*jA;
			 
			 if(S0*exp(x)>=(L2+error))
				lnVS[jA][jR]=D;
			    	 
			 else  
				lnVS[jA][jR]=(1-w)*S0*exp(x);//*(D/L2);  
		
		//	 else if(<S0*exp(x)<(L2+error))
		//	 	 lnVS[jA][jR]=Max(S0*exp(x)-L,0);  //equity
		/*
		  if(jR==0)
			{	
				//printf("lnVS[%d][%d]:%lf\n",jA,jR,lnVS[jA][jR]);  
				 printf("lnVS[%d][%d]:%lf\n",jA,jR,S0*exp(x));						
			} */
		  }
	      //   printf("%\n");
	}
	
	


	 

  
  
  //最後變動Barrier至Debt面額的折現(T-1)~(T-2)
    
for(i=n-1;i>=n-2;i--)			 
{	
	ymax=y0+sqrt(dt)*i;
	ymin=y0-sqrt(dt)*i;
	binmax_noj=muhat[0]+2*dA+(i-(jumptime1+1))*dA;  //jump前bin最大值;Xmax
	Xmax=binmax_noj;	  
    	
  
		for(jA=0;jA<i-(jumptime1+1)+trinode+1;jA++)    //算最後一期資產payoff
		{
			for(jR=0;jR<n+1;jR++)  //jR:0~4(n+1)
			{ 		 
			   
			     x=Xmax-2*dA*jA;
				 y=ymax-jR*2*sqrt(dt);
				 X=Xmax-2*dA*jA;
				 r=sigmaR*(sqrt(1-tho*tho)*y+tho*X/sigmaA); 
			//   r=r0;
				 x=log((S0*exp(X)-jump[0])/S0);	//jump
				 		 
			   if(i==n-2)  
			   {
				   mu=x+(r-sigmaA*sigmaA*0.5)*dt; 
				 
					for(fn=0;fn<i-(jumptime1+1)+trinode+1;fn++)
					{
						mu_hat[1]=(binmax_noj+dA)-fn*2*dA;  //jump後的mu_hat
					
					
						if(fabs(mu_hat[1]-mu)<=dA)
						{
						findnode=fn;
						muhat_right=mu_hat[1]; //找符合的mu_hat
					 
						}		
					}						

				 tri_probability_(r,P,muhat_right,dt,x);		//ok!  
				 lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[findnode-1][jR]+P[1]*lnVS[findnode-1][jR+1]+P[2]*lnVS[findnode][jR]+P[3]*lnVS[findnode][jR+1]+P[4]*lnVS[findnode+1][jR]+P[5]*lnVS[findnode+1][jR+1]);

			   }
			   else //i==n-1  
			   {
				   probability_(r,P,dt);  
				   lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[jA][jR]+P[1]*lnVS[jA][jR+1]+P[2]*lnVS[jA+1][jR]+P[3]*lnVS[jA+1][jR+1]);
			   }
			
			   
			}
		}
	
}
		  	
    //Backward induction；碰觸利率上界前(2-dimensional Binomial Lattice)
	//Binomial discount
	//jump time後至到期日前

	for(i=n-3;i>=jumptime1+1;i--)
	{
	  
	//	mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r0,0); //第一期的mu_hat
		ymax=y0+sqrt(dt)*i;  //每一期y最大值   
		Xmax=muhat[0]+2*dA+(i-(jumptime1+1))*dA; //jump後bin最高點
	//	Xmax=dA*i;// Two-dimensional Binomial Lattice 	
    //  printf("t:%d\n",i);


	for(jA=0;jA<i-(jumptime1+1)+trinode;jA++)  //i-(jumptime1+1)
		{
		  for(jR=0;jR<i+1;jR++)
		  {
				y=ymax-jR*2*sqrt(dt);
				x=Xmax-jA*2*sigmaA*sqrt(dt);  	   
          	    r=sigmaR*(sqrt(1-tho*tho)*y+tho*x/sigmaA);			    		
			//ok!	if(jR==0)
			//		printf("VS[%d][%d]:%lf\n",jA,jR,S0*exp(x));
				
			//  r=r0;   // Merton's Model
				probability_(r,P,dt);  
				lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[jA][jR]+P[1]*lnVS[jA][jR+1]+P[2]*lnVS[jA+1][jR]+P[3]*lnVS[jA+1][jR+1]);
		  	 
	//ok!	   	if(jR==0)
	//			printf("lnVS[%d][%d]:%lf\n",jA,jR,lnVS[jA][jR]);  
			
			//	EDFPM :到期日前離散檢視
							   			  
			 if((n-i)%25==0) 
			 {	
				if(S0*exp(x)<=(L2+pow(0.1,8)))
					lnVS[jA][jR]=(1-w)*S0*exp(x);//*(D/L2);  //扣除破產成本實拿  
			 }		  
						
			 
		  } 
		    //r:printf("\n");   
			   
		}
			
		//	printf("\n");
    } 
	
	//Binomial discount
	//jump time
	
	Xmax=mu_hat[0]+2*dA+(jumptime1-1)*dA;
	ymax=y0+sqrt(dt)*jumptime1;

	for(jA=0;jA<jumptime1+2;jA++)    //jA:0~trinode-1
	{
		  for(jR=0;jR<jumptime1+1;jR++)  //jR:0~4(n+1)
		  { 		 
				 y=ymax-jR*2*sqrt(dt);
				 X=Xmax-2*dA*jA;
				 r=sigmaR*(sqrt(1-tho*tho)*y+tho*X/sigmaA); 
			//   r=r0;
				 x=log((S0*exp(X)-jump[1])/S0);	//jump
				 
				 mu=x+(r-sigmaA*sigmaA*0.5)*dt;
				 
				 for(fn=0;fn<trinode;fn++)
				 {
					mu_hat[1]=(muhat[0]+2*dA)-fn*2*dA;  //jump後的mu_hat
					
					
					if(fabs(mu_hat[1]-mu)<=dA)
					{
						findnode=fn;
						muhat_right=mu_hat[1]; //找符合的mu_hat
					 
					/*	if(jR==0)	
						{	
							printf("hat_right[%d]=%lf\n",jA,S0*exp(muhat_right));
							printf("X[%d]=%lf\n",jA,S0*exp(x));
						}*/
					}		
				 }
							
			//	 mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r,x,jump[1]);				
			
				 tri_probability_(r,P,muhat_right,dt,x);		//ok!  
			//	 X=x+dA;
	
			/*	 	if(jR==0&&jA==0)
				{
					for(j=0;j<6;j++)
					printf("p[%d]:%lf\n",j,P[j]);
				} */
					
			/*	 if(jR==0)
				{	
					if(jA==0)
					{
						printf("hat_high[%d]=%lf\n",i+1,S0*exp(Xmax+3*dA));
						printf("hat_low[%d]=%lf\n",i+1,S0*exp(Xmax-9*dA));
					}
				
					printf("mu_hat[%d]:%lf\n",jA,S0*exp(mu_hat));	//hat_low+dA<=mu_hat<=hat_high+dA		
				//	printf("mu[%d]:%lf\n",jA,mu);
				//	printf("dA:%lf\n",mu_hat-mu);
				//	printf("dA:%lf\n",dA);
		   	    //  printf("V[%d]:%lf\n\n",jA,S0*exp(X));	
				//	printf("r[%d][%d]:%lf\n",jA,jR,r); 
				 }  */
			 
	//		if(fabs(S0*exp(X)-mu_hat)<=error)
				lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[findnode-1][jR]+P[1]*lnVS[findnode-1][jR+1]+P[2]*lnVS[findnode][jR]+P[3]*lnVS[findnode][jR+1]+P[4]*lnVS[findnode+1][jR]+P[5]*lnVS[findnode+1][jR+1]);
	
	/*  jump time 可做檢視?		
			if(S0*exp(X)<=(L1+pow(0.1,8)))
				lnVS[jA][jR]=(1-w)*S0*exp(x)*(L2/L1);  //jump前違約 
			    
			if(S0*exp(x)<=(L2+pow(0.1,8)))             //jump後違約
					lnVS[jA][jR]=(1-w)*S0*exp(x);
	*/
			 //	if(jR==0)
			//	printf("lnVS[%d][%d]:%lf\n",jA,jR,lnVS[jA][jR]);  

	
	//		else  
	//        	lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[jA+1][jR]+P[1]*lnVS[jA+1][jR+1]+P[2]*lnVS[jA+2][jR]+P[3]*lnVS[jA+2][jR+1]+P[4]*lnVS[jA+3][jR]+P[5]*lnVS[jA+3][jR+1]);	 
		
	//		if(jR==0)
	//		printf("lnVS[%d][%d]:%lf\n",jA,jR,S0*exp(x));  
		  }
		 
	}
	
	//Binomial discount
	//t=1~jump 
		
	for(i=jumptime1-1;i>=1;i--)
	{
	   // mu_hat=Mu_hat(n,jumptime,bin_node[0],dt,r0,0); //第一期的mu_hat
		ymax=y0+sqrt(dt)*i;  //每一期y最大值   
		Xmax=mu_hat[0]+2*dA+(i-1)*dA;

	for(jA=0;jA<i+2;jA++)
		{
		  for(jR=0;jR<i+1;jR++)
		  {
				y=ymax-jR*2*sqrt(dt);
				x=Xmax-jA*2*sigmaA*sqrt(dt);  	   
			    r=sigmaR*(sqrt(1-tho*tho)*y+tho*x/sigmaA);			    		
			//ok!	if(jR==0)
			//		printf("VS[%d][%d]:%lf\n",jA,jR,S0*exp(x));
				
			 // r=r0;   // Merton's Model
				probability_(r,P,dt);  
				lnVS[jA][jR]=exp(-r*dt)*(P[0]*lnVS[jA][jR]+P[1]*lnVS[jA][jR+1]+P[2]*lnVS[jA+1][jR]+P[3]*lnVS[jA+1][jR+1]);
		 
				
				//離散檢視
				 if((n-i)%25==0) 
			 {	
				if(S0*exp(x)<=(L1+pow(0.1,8)))
					lnVS[jA][jR]=(1-w)*S0*exp(x);//*(D/L1);  //對債權人拿比例償還  
			 }		  

		  } 
		    //r:printf("\n");   
			   
		}
			
		//	printf("\n");
     } 
		
	
	//trinomial discount(at t=0)
		
				r=r0;
				x=0;  //起始值出發
				mu_hat[0]=Mu_hat(n,jumptime,bin_node[0],dt,r,x,jump[0],L1);				
				tri_probability_(r,P,mu_hat[0],dt,x);		//ok!  
			    //  for(j=0;j<6;j++)
				//      printf("%lf\n",P[j]);
				lnVS[0][0]=exp(-r*dt)*(P[0]*lnVS[0][0]+P[1]*lnVS[0][1]+P[2]*lnVS[1][0]+P[3]*lnVS[1][1]+P[4]*lnVS[2][0]+P[5]*lnVS[2][1]);
			   
		
//	fprintf(rf1,"n:%d\t",n);
	ZCB=zerocouponbond(n,B,jumptime,bin_node[0],T,mu_hat[0]);
//	fprintf(rf1,"%.6lf\n",Credit_Spread(T,lnVS[0][0],D,ZCB));    
	fprintf(rf1,"%.6lf\n",lnVS[0][0]);
//	printf("n=%d\n",n);
//	printf("mu_hat=%.7lf\n",S0*exp(mu_hat));	
//  printf("p(0,T)=%.7lf\n",zerocouponbond(n,B,jumptime,bin_node[0],T,mu_hat[0]));
//	printf("p(0,T)=%.7lf\n",Vasicek(r0,a,b,sigmaR,T));
//	printf("rmax=%lf\nrmin=%lf\n",rmax,rmin);
//	printf("credit spread:%lf\n",Credit_Spread(T,lnVS[0][0],D,ZCB));
	printf("bond value:%lf\n",lnVS[0][0]);
//	printf("\n");
//  T=T+0.5;
//  n=n+50;
	ratio=ratio+0.1;
	FreeArray(lnVS, ARRAY_ROW);
    FreeArray(B, ARRAY_ROW);



	
}
    fclose(rf1);
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

void tri_probability_(double r,double P[6],double mu_hat,double dt,double x)
{
  double a=0.1,b=0.1,sigmaA=0.3,sigmaR=0.035,tho=0.3;  
  double mu,muA,muR; 
  double pr,pu,pm,pd; 
  double beta,alpha,gamma,Var;
  double delta,delta_u,delta_m,delta_d;
  
  
   mu=x/dt+(r-sigmaA*sigmaA/2);
   muA=(r-sigmaA*sigmaA/2);
   muR=a*(b-r);
   pr=0.5+0.5*sqrt(dt/(1-tho*tho))*(-tho*muA/sigmaA+muR/sigmaR);
      
   beta=mu_hat-mu*dt;  //    -dA<beta<dA
 
   
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
//計算mu_hat副程式(到期日,jump發生時點,三元樹起始點個數):計算三元樹點個數
double Mu_hat(int n,int jumptime ,int Binode,double dt,double r,double x,double jumpsize,double L)
{
  double S0=800,sigmaA=0.3,standB,mu_hat;//,mu_hat[1]={0.0}; //mu_hat[0]、mu_hat[1]最高及最低mu_hat: 
  //standB:最靠近Barrier的點 
  double muA;		
  double R_mu_standB;  //mu到standB的距離
  double RmuB;  //R_mu_standB的整數倍2dA間隔
  double R,U,error=0.000001,lnB;  //xlow:binomial最低點
  int i;
	
	U=sigmaA*sqrt(dt);	
    lnB=log((L-jumpsize)/S0); //barrier
	
	
		for (i=0; i<Binode+1; i++)  //1個檢視期
		{
		muA=x+(r-0.5*sigmaA*sigmaA)*dt;  //每一期的mu; x:binomial最高點
	
	
		if(n%2==0)
			standB=lnB+sigmaA*sqrt(dt);  //no jump barrier
		else
			standB=lnB;
	  
		R_mu_standB=(muA-standB)/(2*U);
		
		RmuB=(int)floor(R_mu_standB);  // [3.2]>>3 補!
		
			R=fabs (R_mu_standB-RmuB);
		
			if (R*2*U > U+error)
				RmuB++;
			else
				RmuB=RmuB;
		}	
	
			mu_hat=standB+RmuB*2*U;  //k找mu_hat的個數
		
		
			//	x=x-2*U*(t+1);//binode-1  ;binode:第t期bin個數
    
	
			return mu_hat; 
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

//p(0,T)到期日T零息債券的價值;發生jump算的ZCB與no jump的算不ㄧ致?
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
	 //   Xmax=dA*i;// Two-dimensional Binomial Lattice  paper test
       
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
				mu_hat=Mu_hat(n,jumptime,Binode,dt,r,X,0,L);				
		   //	probability_(r,P,dt);
				tri_probability_(r,P,mu_hat,dt,X);		//ok!  
			 //     for(j=0;j<6;j++)
			//	printf("%lf\n",P[j]);
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
	r0=0.1;
	a=0.9;
	b=0.1;
	sigmaR=0.1;*/

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