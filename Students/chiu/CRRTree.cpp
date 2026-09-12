// test.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <sys/timeb.h>

using namespace std;

double bsp( double s0, double K, double r, double T, double s );
double ncdf( double );

double ncdf( double x )
{
	double a1 = 0.319381530;
	double a2 = -0.356563782;
	double a3 = 1.781477937;
	double a4 = -1.821255978;
	double a5 = 1.330274429;
	double Pi = 3.141592653589793238462643383279502884197;

	double signx = (x>=0) ? 1 : -1;
	double p = 1/(1 + signx*0.2316419*x);

	return (0.5 + 0.5*signx - signx*(1/sqrt(2*Pi))*exp(-x*x/2)*p*(a1 + p*(a2 + p*(a3 + p*(a4 + a5*p)))));
}

double bsp( double s0, double K, double r, double T, double s )
{
	double d1 = (log(s0/K) + (r + s*s/2)*T)/(s*sqrt(T));
	double d2 = d1 - s*sqrt(T);
	return -s0*ncdf(-d1) + K*exp(-r*T)*ncdf(-d2);
}

int _tmain(int argc, _TCHAR* argv[])
{
	double s0 = 40;
	double K = 40;
	double r = 0.05;
	double T = 3;
	double s = 0.2;
	int n = 1000;
	double c[1001];
	double proxy[1001];
	double tmp[1001];
	
	struct timeb start, end;
	double times;
	double exercise, tmp1, tmp2;
		
	double dt = T/n;
	double u = exp(s*sqrt(dt));
	double d = 1/u;
	double p = (exp(r*dt)-d)/(u-d);
	double d_over_u = d/u;
	double discount_factor = exp(-r*dt);

		
	// n = 42000 答案是 3.48425, 再大的 n 程式就會出現例外
	// n = 1000 答案是 3.48373
	////////////////////////// 優化版 + 懶人 proxy //////////////////////////
	/*
	ftime(&start); // 計時開始


	tmp[0] = s0*pow(u, n-1);
	for(int i=1 ; i<=n-1 ; i++)
		tmp[i] = tmp[i-1] * d_over_u;
	for(int i=0 ; i<=n-1 ; i++){	
		c[i] = proxy[i] = bsp(s0*pow(u, n-1-i)*pow(d, i), K, r, dt, s);
		c[i] = (K-tmp[i] > c[i]) ? (K-tmp[i]) : c[i];
	}
	
	tmp1 = tmp2 = s0*pow(u, n-1);
	for(int i=n-2 ; i>=0 ; i--){
		tmp2 = tmp1 = tmp1/u;
		for(int j=0 ; j<=i+1 ; j++)
			c[j] = c[j] - proxy[j];
		for(int j=0 ; j<=i ; j++){
			tmp[j] = discount_factor * (p*c[j] + (1-p)*c[j+1]);
			
			proxy[j] = bsp(tmp2, K, r, (n-i)*dt, s);
			c[j] = tmp[j] + proxy[j];
			exercise = K - tmp2;
			c[j] = (exercise > c[j]) ? exercise : c[j];
			tmp2 = tmp2 * d_over_u;
		}
	}
	cout << c[0] << endl;	
	

	ftime(&end); // 計時結束
	if(start.millitm > end.millitm)
		times = (end.time-start.time-1)+(1000.0+end.millitm-start.millitm)/1000;
	else
		times = (end.time-start.time)+(double)(end.millitm-start.millitm)/1000;
	cout << times << endl;
	*/
	/*
	////////////////////////// 好讀版 + 懶人 proxy //////////////////////////
	ftime(&start); // 計時開始


	for(int i=0 ; i<=n-1 ; i++){	
		c[i] = proxy[i] = bsp(s0*pow(u, n-1-i)*pow(d, i), K, r, dt, s);
		c[i] = (K-s0*pow(u, n-1-i)*pow(d, i) > c[i]) ? 
			(K-s0*pow(u, n-1-i)*pow(d, i)) : c[i];
	}
	
	for(int i=n-2 ; i>=0 ; i--){
		for(int j=0 ; j<=i+1 ; j++)	// 小心！在減 proxy 的時候陣列長度還是 i+1 的
			c[j] = c[j] - proxy[j];
		for(int j=0 ; j<=i ; j++){
			tmp[j] = exp(-r*dt)*(p*c[j] + (1-p)*c[j+1]);
			
			tmp1 = s0*pow(u, i-j)*pow(d, j);
			proxy[j] = bsp(tmp1, K, r, (n-i)*dt, s);
			
			c[j] = tmp[j] + proxy[j];
			exercise = K - tmp1;
			c[j] = (exercise > c[j]) ? exercise : c[j];
		}
		
	}
	cout << c[0] << endl;
	

	ftime(&end); // 計時結束
	if(start.millitm > end.millitm)
		times = (end.time-start.time-1)+(1000.0+end.millitm-start.millitm)/1000;
	else
		times = (end.time-start.time)+(double)(end.millitm-start.millitm)/1000;
	cout << times << endl;
	
	*/
	////////////////////////// 優化版無 proxy //////////////////////////
	ftime(&start); // 計時開始

	tmp[0] = s0*pow(u, n);
	for(int i=1 ; i<=n ; i++)
		tmp[i] = tmp[i-1] * d_over_u;
	for(int i=0 ; i<=n ; i++)
		c[i] = (K-tmp[i] > 0) ? (K-tmp[i]) : 0;
	
	tmp1 = tmp2 = s0*pow(u, n);
	for(int i=n-1 ; i>=0 ; i--){
		tmp2 = tmp1 = tmp1/u;
		for(int j=0 ; j<=i ; j++){
			tmp[j] = discount_factor * (p*c[j] + (1-p)*c[j+1]);
			exercise = K - tmp2;
			c[j] = (exercise > tmp[j]) ? exercise : tmp[j];
			tmp2 = tmp2 * d_over_u;
		}
	}
	cout << c[0] << endl;	
	
	ftime(&end); // 計時結束
	if(start.millitm > end.millitm)
		times = (end.time-start.time-1)+(1000.0+end.millitm-start.millitm)/1000;
	else
		times = (end.time-start.time)+(double)(end.millitm-start.millitm)/1000;
	cout << times << endl;
	
	/*
	////////////////////////// 好讀版無 proxy //////////////////////////
	ftime(&start); // 計時開始

	for(int i=0 ; i<=n ; i++)		// 給最後一期的 c 值
		c[i] = (K-s0*pow(u, n-i)*pow(d, i) > 0) ? (K-s0*pow(u, n-i)*pow(d, i)) : 0;
	
	
	for(int i=n-1 ; i>=0 ; i--){	// backward induction
		for(int j=0 ; j<=i ; j++){
			tmp[j] = exp(-r*dt)*(p*c[j] + (1-p)*c[j+1]);
			exercise = K - s0*pow(u, i-j)*pow(d, j);
			c[j] = (exercise > tmp[j]) ? exercise : tmp[j];
		}
	}
	cout << c[0] << endl;
	
	ftime(&end); // 計時結束
	if(start.millitm > end.millitm)
		times = (end.time-start.time-1)+(1000.0+end.millitm-start.millitm)/1000;
	else
		times = (end.time-start.time)+(double)(end.millitm-start.millitm)/1000;
	cout << times << endl;
	*/

	system("PAUSE");
	return 0;
}

