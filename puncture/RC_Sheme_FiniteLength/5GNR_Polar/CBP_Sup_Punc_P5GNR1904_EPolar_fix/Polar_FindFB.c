#include<math.h>
#define Sqrt2 1.41421356237
#define SWAP(x,y) {double t; t = x; x = y; y = t;}
double B_temp[N]={0},B[N]={0};
int ReliabilityOrder[N]={0};

int partition(double number[], int flag[], int left, int right){
	int i, j;
	double s;
	s = number[right];
	i = left - 1;
	for(j = left; j < right; j++) {
		if(number[j] <= s){
			i++;
			SWAP(number[i], number[j]);
			SWAP(flag[i], flag[j]);
		}
	}
	SWAP(number[i+1], number[right]);
	SWAP(flag[i+1], flag[right]);
	return i+1;
}

void quicksort(double number[], int flag[], int left, int right) {
    int q;
	if(left < right) {
		q = partition(number, flag,left, right);
		quicksort(number,flag, left, q-1);
		quicksort(number,flag, q+1, right);
	}
}

double gassCDF(double value){
    return (0.5+0.5*(1-erfc(value /Sqrt2)));
}


void B_para_ini2(double snr, double R){
    double EdB=snr*R;
    double S=pow(10,EdB/10);
    double inival=exp(-S);
    int i=0, j=0;
    for(i=0;i<N;i++) B_temp[i]=inival;
    //for(i=0;i<N;i++) cout<<B_temp[i]<<endl;
    for(i=0;i<N;i++) ReliabilityOrder[i]=i;
}

void B_para(){
    int i=0, j=0;

    for(j=r-1;j>=0;j--){
        int p=pow(2,j), nextp=p*2;
        for(i=0;i<N;i++){

            if((i&p)==0){//CN
                double a=B_temp[i], b=B_temp[i+p];

                B[i]=a+b-a*b;
                    //cout<<j<<" "<<k<<" CN "<<LLR_L[j+1][k]<<" "<<LLR_L[j+1][k+p]<<" "<<(sgn(a))*(sgn(b))*(Min(fabs(a),fabs(b)))<<endl;
                }
            else{//VN
                 double a=B_temp[i], b=B_temp[i-p];
                 B[i]=a*b;
                // cout<<j<<" "<<k<<" CN "<<LLR_L[j+1][k]<<" "<<LLR_L[j+1][k+p]<<" "<<(Min(fabs(a),fabs(b)))<<endl;
                }
                //cout<<endl;
        }
        for(i=0;i<N;i++) B_temp[i]=B[i];
    }

    quicksort(B_temp, ReliabilityOrder, 0, N-1);

    for(i=0;i<N;i++){
   // cout<<i+1<<" "<<ReliabilityOrder[i]+1<<" "<<B[ReliabilityOrder[i]]<<endl;
    Frozen[i]=0;
    }

    for(i=N-1;i>=K;i--){
           // cout<<i<<" ";
        Frozen[ReliabilityOrder[i]]=1;
    }
//cout<<endl;
    int CNT=0;
    for(i=0;i<N;i++){
        if(Frozen[i]) CNT++;
    }
   // cout<<"CNT "<<CNT<<endl;
   // for(i=0;i<N;i++){
  //  cout<<i<<" "<<Frozen[i]<<endl;}

}
void Beta_expansion(double Beta)
{
    int i = 0, j = 0;
    // initalize
    for (i = 0; i < N; i++)
    {
        ReliabilityOrder[i] = i;
    }
    for (i = 0; i < N; i++)
    {
        int i_temp = i;
        B_temp[i] = 0;
        for (j = 0; j < r; j++)
        {
            B_temp[i] += ((i_temp % 2) * pow(Beta, j));
            i_temp >>= 1;
        }
        B[i] = B_temp[i];
        // cout << i + 1 << " " << B_temp[i] << endl;
    }
    quicksort(B_temp, ReliabilityOrder, 0, N - 1);
    for (i = 0; i < N; i++)
    {
        // cout << i + 1 << " " << ReliabilityOrder[i] + 1 << " " << B[ReliabilityOrder[i]] << endl;
        Frozen[i] = 0;
    }
    for (i = 0; i < K; i++)
    {
        // cout<<i<<" ";
        Frozen[ReliabilityOrder[i]] = 1;
    } // ³Ì¤jªº­È¥ýFrozen
    //  cout<<endl;
    int CNT = 0;
    for (i = 0; i < N; i++)
    {
        if (Frozen[i])
            CNT++;
    }
    // cout << "CNT " << CNT << endl;
    /*
    for (i = 0; i < N; i++)
    {
        cout << i << " " << Frozen[i] << endl;
    }
    */
}
