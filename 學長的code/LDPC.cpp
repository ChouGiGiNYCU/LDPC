#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string>
#include<vector>
#include <sstream>
#include <fstream>
#include"awgn.h"
#include"sgn_func.h"
#define iter 50 
//#define index_max 18
#define index_max 11
#define phi(x) log(1+2./(exp(x)-1))
using namespace std;
int main()
{
    int seed = 371;
    //--------------------------read file--------------------------
    int line = 0;
    int M,N;
    int m,n;
    ifstream file("H_10_5.txt");
    if(file.fail())
        cout << "Input file opening failed\n";
    else
    	cout << "Input file opening successful\n";
    string str;
    
    vector<vector<int> > B;
    while(getline(file,str))
    {
        stringstream ss(str);
        if(line>=4)
        {
            vector<int> row;
            int data;
            while(ss>>data)
            {
                row.push_back(data);
            }
            B.push_back(row);
        }
        else if(line==0)
        {
            ss>>N;
            ss>>M;
        }
        else if(line==1)
        {
            ss>>n;
            ss>>m;
        }
        line = line + 1;
    }
    file.close();
    double R = double(M)/N;
    int c[N] = {0};
    for(int i=0;i<N;i++)
        c[i] = 0;
    //-------------------matrix setting-----------------------
    // 就是把所有位置記下來而已不要花時間看 馬der，直接看SPA
    int **L_v2c_position,*L_v2c_position_row,**L_c2v_position,*L_c2v_position_row;
    double **L_v2c,*L_v2c_row,**L_c2v,*L_c2v_row;
    L_c2v_position = (int**)malloc(N*sizeof(int*));
    L_c2v_position_row = (int*)malloc(N*n*sizeof(int));
    L_c2v = (double**)malloc(N*sizeof(double*));
    L_c2v_row = (double*)malloc(N*n*sizeof(double));
    for(int i=0;i<N;i++,L_c2v_row+=n,L_c2v_position_row+=n)
    {
        L_c2v[i] = L_c2v_row;
        L_c2v_position[i] = L_c2v_position_row;
    }
    L_v2c_position = (int**)malloc(M*sizeof(int*));
    L_v2c_position_row = (int*)malloc(M*m*sizeof(int));
    L_v2c = (double**)malloc(M*sizeof(double*));
    L_v2c_row = (double*)malloc(M*m*sizeof(double));
    for(int i=0;i<M;i++,L_v2c_row+=m,L_v2c_position_row+=m)
    {
        L_v2c[i] = L_v2c_row;
        L_v2c_position[i] = L_v2c_position_row;
    }
    // 記錄所有的位置
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<n;j++)
            L_c2v_position[i][j] = B[i][j];
    }
    for(int i=0;i<M;i++)
    {
        for(int j=0;j<m;j++)
            L_v2c_position[i][j] = B[i+N][j];
    }
    //----------------------SPA---------------------------
    clock_t clk_start, clk_end;
	float clk_total;
    double rec[N];
    double guess[N];
    double frame_error[index_max+1] = {0};
    double bit_error[index_max+1][iter/5+2] = {0}; 		//[1,6,11,16,21,26,31,36,41,46,50]
    double frame_total[index_max+1] = {0};
//  double Eb_over_N0[index_max+1] = {0.,0.4,0.8,1.2,1.6,2.0,2.4};
	double Eb_over_N0[index_max+1] = {-2,-1.6,-1.2,-0.8,-0.4,0.0,0.4,0.8,1.2,1.6,2};
	// double Eb_over_N0[index_max+1] = {-2,-1.6,-1.2,-0.4,0.0,0.4,0.6,0.8,1.0,1.2,1.6};
//	double Eb_over_N0[index_max+1] = {-2,-1.6,-1.2,-0.8,-0.4,0.0,0.2,0.4,0.6,0.7,0.8,0.9,1.0,1.2,1.4,1.6,1.8,2};
    double TH = 100;
    cout<<"\n******************SPA******************\n";
    cout<<"   Eb/N0       FER\t    BER"<<endl;
    FILE *result;
    result = fopen("SPA.csv","w");
    for(int index=0;index<index_max;index++)
    {
        clk_start = clock();
        srand(seed);
        double sigma = sqrt(1/(2*R*pow(10,Eb_over_N0[index]/10.)));
        bool error = 0;
        // error frame 要有一定的數量，total fram 也要跑多次一點
        while(frame_error[index]<TH && frame_total[index]<1000)
        {
            printf("%.lf%c",frame_error[index]/(TH/100),37);
            for(int i=0;i<N;i++)
                rec[i] = awgn(-2*double(c[i])+1,sigma); 
            //----------------initialization-----------------------
            int iteration = 1;
            int iter_idx = 0;
            bool wrong = 1;
            for(int i=0;i<M;i++)
            {
                for(int j=0;j<m;j++)
                {
                    if(L_v2c_position[i][j]!=0)
                        L_v2c[i][j] = 2*rec[L_v2c_position[i][j]-1]/pow(sigma,2); //LLR
                }
            }
            while(iteration<=iter && wrong==1)
            {
                //------------------CN update----------------------
                for(int i=0;i<N;i++)
                {
                    for(int j=0;j<n;j++)
                    {
                        double r = 0;
                        int sgn = 1;
                        double smallest = INT_MAX;
                        if(L_c2v_position[i][j]!=0)
                        {
                            for(int k=0;k<m;k++)
                            {
                                if(L_v2c_position[L_c2v_position[i][j]-1][k]!=0 && L_v2c_position[L_c2v_position[i][j]-1][k]-1!=i)
                                {
                                    if(L_v2c[L_c2v_position[i][j]-1][k]<0)
                                        sgn = -sgn;
                                    r = r + phi(fabs(L_v2c[L_c2v_position[i][j]-1][k]));
                                }
                            }
                            L_c2v[i][j] = sgn * phi(r);
                        }
                    }
                }
                //------------------VN update----------------------
                for(int i=0;i<M;i++)
                {
                    for(int j=0;j<m;j++)
                    {
                        if(L_v2c_position[i][j]!=0)
                        {
                            int pre_sign = sgn_func(L_v2c[i][j]);
                            L_v2c[i][j] = 2*rec[L_v2c_position[i][j]-1]/pow(sigma,2);
                            for(int k=0;k<n;k++)
                            {
                                if(L_c2v_position[L_v2c_position[i][j]-1][k]!=0 && L_c2v_position[L_v2c_position[i][j]-1][k]-1!=i)
                                    L_v2c[i][j] = L_v2c[i][j] + L_c2v[L_v2c_position[i][j]-1][k];
                            }
                        }
                    }
                }
                //------------------Decision-----------------------
                for(int i=0;i<N;i++)
                {
                    guess[i] = 2*rec[i]/pow(sigma,2);
                    for(int j=0;j<n;j++)
                    {
                        if(L_c2v_position[i][j]!=0)
                            guess[i] = guess[i] + L_c2v[i][j];
                    }
                    if(guess[i]>0)
                        guess[i] = 0;
                    else
                        guess[i] = 1;
                }
                //--------------------check------------------------
                int sum;
                for(int i=0;i<M;i++)
                {
                    sum = 0;
                    for(int j=0;j<m;j++)
                    {
                        if(L_v2c_position[i][j]!=0)
                            sum = sum + guess[L_v2c_position[i][j]-1];
                    }
                    if(sum%2!=0)
                    {
                        wrong = 1;
                        break;
                    }
                    wrong = 0;
                }
                //------------------Compare------------------------
        		error = 0;
                if(iteration%5==1 || iteration==50)
                {
		            for(int i=0;i<N;i++)
		            {
		                if(bool(c[i])^bool(guess[i])!=0)
		                {
		                    bit_error[index][iter_idx] = bit_error[index][iter_idx] + 1;
		                    error = 1;
		                }
		            }
		            	
		            iter_idx++;
				}
	           iteration = iteration + 1;
            }

            if(error)
            {
                frame_error[index] = frame_error[index] + 1;
                error = 0;
            }
            frame_total[index] = frame_total[index] + 1;
            printf("\b\b\b\b");
        }
        
        clk_end = clock();
        clk_total = (float)(clk_end - clk_start)/CLOCKS_PER_SEC;

        printf("  %.1f\t   %.8lf\t%.8lf\t| Time : %7.2f sec\tTotal frame : %6.0f\n",Eb_over_N0[index],frame_error[index]/frame_total[index]
		,bit_error[index][iter/5]/(N*frame_total[index]),clk_total, frame_total[index]);
		fprintf(result,"%g,%e,",Eb_over_N0[index],frame_error[index]/frame_total[index]);
		for(int i=0;i*5<=iter;i++)
			fprintf(result,"%e,",bit_error[index][i]/(N*frame_total[index]));
		fprintf(result,"\n");
    }
    fclose(result);
/*
    //---------------------MSA------------------------
    for(int i=0;i<7;i++)
    {
        frame_error[i] = 0;
        bit_error[i] = 0;
        frame_total[i] = 0;
    }
    cout<<"\n******************MSA******************\n";
    cout<<"   Eb/N0       FER\t    BER"<<endl;
    FILE *result_MS;
    result_MS = fopen("MSA.csv","w+");
    for(int index=0;index<index_max;index++)
    {
        srand(seed);
        double sigma = sqrt(1/(2*R*pow(10,Eb_over_N0[index]/10.)));
        bool error = 0;
        while(frame_error[index]<100)
        {
            printf("%.lf%c",frame_error[index],37);
            for(int i=0;i<N;i++)
                rec[i] = awgn(-2*double(c[i])+1,sigma);
            //----------------initialization-----------------------
            int iteration = iter;
            bool wrong = 1;
            for(int i=0;i<M;i++)
            {
                for(int j=0;j<m;j++)
                {
                    if(L_v2c_position[i][j]!=0)
                        L_v2c[i][j] = 2*rec[L_v2c_position[i][j]-1]/pow(sigma,2);
                }
            }
            while(iteration>0 && wrong==1)
            {
                //------------------CN update----------------------
                for(int i=0;i<N;i++)
                {
                    for(int j=0;j<n;j++)
                    {
                        double r = 0;
                        int sgn = 1;
                        double smallest = 1./0;
                        if(L_c2v_position[i][j]!=0)
                        {
                            for(int k=0;k<m;k++)
                            {
                                if(L_v2c_position[L_c2v_position[i][j]-1][k]!=0 && L_v2c_position[L_c2v_position[i][j]-1][k]-1!=i)
                                {
                                    if(L_v2c[L_c2v_position[i][j]-1][k]<0)
                                        sgn = -sgn;
                                    if(fabs(L_v2c[L_c2v_position[i][j]-1][k])<smallest)
                                        smallest = fabs(L_v2c[L_c2v_position[i][j]-1][k]);
                                }
                            }
                            L_c2v[i][j] = sgn * smallest;
                        }
                    }
                }
                //------------------VN update----------------------
                for(int i=0;i<M;i++)
                {
                    for(int j=0;j<m;j++)
                    {
                        if(L_v2c_position[i][j]!=0)
                        {
                            L_v2c[i][j] = 2*rec[L_v2c_position[i][j]-1]/pow(sigma,2);
                            for(int k=0;k<n;k++)
                            {
                                if(L_c2v_position[L_v2c_position[i][j]-1][k]!=0 && L_c2v_position[L_v2c_position[i][j]-1][k]-1!=i)
                                    L_v2c[i][j] = L_v2c[i][j] + L_c2v[L_v2c_position[i][j]-1][k];
                            }
                        }
                    }
                }
                //------------------Decision-----------------------
                for(int i=0;i<N;i++)
                {
                    guess[i] = 2*rec[i]/pow(sigma,2);
                    for(int j=0;j<n;j++)
                    {
                        if(L_c2v_position[i][j]!=0)
                            guess[i] = guess[i] + L_c2v[i][j];
                    }
                    if(guess[i]>0)
                        guess[i] = 0;
                    else
                        guess[i] = 1;
                }
                //--------------------check------------------------
                int sum;
                for(int i=0;i<M;i++)
                {
                    sum = 0;
                    for(int j=0;j<m;j++)
                    {
                        if(L_v2c_position[i][j]!=0)
                            sum = sum + guess[L_v2c_position[i][j]-1];
                    }
                    if(sum%2!=0)
                    {
                        wrong = 1;
                        break;
                    }
                    wrong = 0;
                }
                iteration = iteration - 1;
            }
            //------------------Compare------------------------
            for(int i=0;i<N;i++)
            {
                if(bool(c[i])^bool(guess[i])!=0)
                {
                    bit_error[index] = bit_error[index] + 1;
                    error = 1;
                }
            }
            if(error)
            {
                frame_error[index] = frame_error[index] + 1;
                error = 0;
            }
            frame_total[index] = frame_total[index] + 1;
            printf("\b\b\b");
        }
        printf("    %g\t   %.8lf\t%.8lf\n",Eb_over_N0[index],frame_error[index]/frame_total[index],bit_error[index]/(N*frame_total[index]));
        fprintf(result_MS,"%g,%e,%e\n",Eb_over_N0[index],frame_error[index]/frame_total[index],bit_error[index]/(N*frame_total[index]));
    }
    fclose(result_MS);
*/
    free(L_v2c_position[0]);free(L_v2c_position);
    free(L_c2v_position[0]);free(L_c2v_position);
    free(L_v2c[0]);free(L_v2c);
    free(L_c2v[0]);free(L_c2v);
}
