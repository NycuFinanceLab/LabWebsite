
// FFT
// SkyChen

#include <iostream>
#include <cstdlib>
#include <cmath>   //¥Îsin¡Bcos
#include <complex>    //¥Îcomplex

using namespace std;

#define PI 3.141592653589793238462643383279502884197
#define size 8

complex<double> * input = new complex<double> [size];   // Global


complex<double>* FFT(complex<double> * x , int N){  //Cooley¡VTukey FFT algorithm
    complex<double> * X = new complex<double> [size];   // local
    
    if(N == 1){
        X[0] = x[0];
    }else{
        complex<double> * odd_temp = new complex<double> [N / 2];
        complex<double> * even_temp = new complex<double> [N / 2];
        complex<double> * odd = new complex<double> [N / 2];
        complex<double> * even = new complex<double> [N / 2];
        complex<double> temp(0.0, 1.0);

        for(int i = 0; i < N / 2 ; i++) {
            even_temp[i] = x[2 * i];
            odd_temp[i] = x[2 * i + 1];
        }
		
		// deivde
        even = FFT( even_temp, N / 2 );
        odd = FFT( odd_temp, N / 2 );
        
        free(even_temp);
        free(odd_temp);
                
        for(int i= 0; i< N / 2; i++)   // calculate exponential
			odd[i] = exp(2 * PI * temp * ((double)i) / ((double)N)) * odd[i];
        
        
        //conquer
        for(int i= 0; i< N / 2; i++){
            X[i] = even[i] + odd[i];
            X[i + N/2] = even[i] - odd[i];
        }
        
        free(even);
        free(odd);
    }   // end of else
    
	//cout << "test " << endl;
    return X;
}


int main()
{
    cout << "Before FFT : " << endl;

    for (int i = 0; i < size; i++) {
        input[i].real(i + 1);
        cout << "i= "<< i << " : "<< input[i].real() << " + " << input[i].imag() << " i " << endl;
    }
    
    input = FFT( input , size );
    
    cout << "After FFT : " << endl;
    for (int i = 0; i < size; i++){
        cout << "i= "<< i << " : "<< input[i].real() << " + " << input[i].imag() << " i " << endl;
    }
    
    cout << endl << "Success!!!!" << endl;
    

	system("pause");
    return 0;
}

