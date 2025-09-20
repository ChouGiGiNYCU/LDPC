// 5G LDPC Parity Matrix Generator
// I HSIANG LEE, TNT Lab
// 2018/08/15
#include<stdio.h>
#include<stdlib.h>


// Global variable
int B;
int matrixType; // There are 2 different LDPC matrix, 1&2.
int *setIndex; // Set index, iLS, in "setindex.txt".
int Kb;
int K; // Number of bits to encode.
int N; //Number of bits after encoding.
struct HBGNode{
    //int colAmounts;
    int *cols;
    int *Pij;
};// Each node represents a row, which contains columns and corresponding Pij.



int main(){
    // Some common variable
    int i,j,k;


    // Set input bits length, B
    printf("Please input the bits length:");
    scanf("%d",&B);
    printf("Please input the matrix type(1 or 2):");
    scanf("%d", &matrixType);
    // For the sake of convenience, we assume that B must less than maximum code block length.

    // Determine the Kb.
    if(matrixType == 1){
        Kb = 22;
    }
    else{
        if(B>640){
            Kb = 10;
        }
        else if(B>560){
            Kb = 9;
        }
        else if(B>192){
            Kb = 8;
        }
        else{
            Kb = 6;
        }
    }



    setIndex = (int*)malloc(sizeof(int)*51); // There are 51 elements in setindex.txt.
    // Reading the setindex.txt
    FILE *fptr;
    fptr = fopen("setindex.txt", "r");
    if(fptr == NULL){
        printf("Please check the file setindex.txt.\n");
        exit(1);
    }
    for(i=0;i<51;i++){
        fscanf(fptr,"%d", &setIndex[i]);
    }
    fclose(fptr);
    // Determine the Zc
    float KBratio;
    KBratio = B/(float)Kb;
    int Zc = 384;
    int iLS = 0;
    for(i=0;i<51;i++){
        if(setIndex[i]>=KBratio && setIndex[i]<Zc){
            Zc = setIndex[i];
            iLS = i;
        }
    }
    // Determine the set index, iLS
    if(iLS>=0 && iLS<=7){
        iLS = 0;
    }
    else if(iLS>=8 && iLS<=15){
        iLS = 1;
    }
    else if(iLS>=16 && iLS<=22){
        iLS = 2;
    }
    else if(iLS>=23 && iLS<=28){
        iLS = 3;
    }
    else if(iLS>=29 && iLS<=34){
        iLS = 4;
    }
    else if(iLS>=35 && iLS<=40){
        iLS = 5;
    }
    else if(iLS>=41 && iLS<=45){
        iLS = 6;
    }
    else{
        iLS = 7;
    }
    // Determine the number of bits before/after encoding.
    if(matrixType == 1){
        K = 22*Zc;
        N = 66*Zc;
    }
    else{
        K = 10*Zc;
        N = 50*Zc;
    }

    // Generate HBG
    struct HBGNode *HBG;
    int temp;
    int count=0;
    char ch;
    int *numOfColumns;
    int countCol = 0;
    int flag = 1;
    int rowDim;
    int colDim;
    int rowHBG;

    if(matrixType == 1){
        rowDim = Zc*46;
        colDim = Zc*68;
        rowHBG = 46;
        HBG = (struct HBGNode*)malloc(sizeof(struct HBGNode)*46);
        numOfColumns = (int*)malloc(sizeof(int)*46);
	for(i=0;i<rowHBG;i++)numOfColumns[i] = 0;
        // Calculate the column number of each row.
        fptr = fopen("graph1.txt","r");
        i = -1;
        while(!feof(fptr)){
            fscanf(fptr,"%d%c",&temp, &ch);
            count = count + 1;
            if(ch == '\n'){
                if(count == 9){
                    numOfColumns[i] += 1;
                }
                else{
                    if(i!=-1){
                        HBG[i].cols = (int*)malloc(sizeof(int)*numOfColumns[i]);
                        HBG[i].Pij = (int*)malloc(sizeof(int)*numOfColumns[i]);
                    }
                    i=i+1;
                    numOfColumns[i] += 1;
                }
                count = 0;
            }
        }
        numOfColumns[i] += 1;
        HBG[i].cols = (int*)malloc(sizeof(int)*numOfColumns[i]);
        HBG[i].Pij = (int*)malloc(sizeof(int)*numOfColumns[i]);
        fclose(fptr);

        fptr = fopen("graph1.txt","r");
        count = 0;
        i = 0;
        j = 0;

        while(!feof(fptr)){
            fscanf(fptr,"%d%c",&temp, &ch);
            count = count + 1;
            if(flag){
                if(count == 2){
                    HBG[i].cols[countCol] = temp;
                    //printf("%d\n", temp);
                }
                if(count == iLS+3){
                    HBG[i].Pij[countCol] = temp % Zc;
                }
            }
            else{
                if(count == 1){
                    HBG[i].cols[countCol] = temp;
                    //printf("%d\n", temp);
                }
                if(count == iLS+2){
                    HBG[i].Pij[countCol] = temp % Zc;
                }
            }

            if(ch == '\n'){
                count = 0;
                countCol += 1;
                flag = 0;
                if(countCol == numOfColumns[i]){
                    i = i+1;
                    countCol = 0;
                    flag = 1;
                }
            }
        }
        fclose(fptr);
    }
    else{
        rowDim = Zc*42;
        colDim = Zc*52;
        rowHBG = 42;

        HBG = (struct HBGNode*)malloc(sizeof(struct HBGNode)*42);
        numOfColumns = (int*)malloc(sizeof(int)*42);
	for(i=0;i<rowHBG;i++)numOfColumns[i] = 0;
        // Calculate the column number of each row.
        fptr = fopen("graph2.txt","r");
        i = -1;

        while(!feof(fptr)){
            fscanf(fptr,"%d%c",&temp, &ch);
            count = count + 1;
            if(ch == '\n'){
                if(count == 9){
                    numOfColumns[i] += 1;
                }
                else{
                    if(i!=-1){
                        HBG[i].cols = (int*)malloc(sizeof(int)*numOfColumns[i]);
                        HBG[i].Pij = (int*)malloc(sizeof(int)*numOfColumns[i]);
                    }
                    i=i+1;
                    numOfColumns[i] += 1;
                }
                count = 0;
            }
        }

        numOfColumns[i] += 1;
        HBG[i].cols = (int*)malloc(sizeof(int)*numOfColumns[i]);
        HBG[i].Pij = (int*)malloc(sizeof(int)*numOfColumns[i]);
        fclose(fptr);

        fptr = fopen("graph2.txt","r");
        count = 0;
        i = 0;
        j = 0;


        while(!feof(fptr)){

            fscanf(fptr,"%d%c",&temp, &ch);
            count = count + 1;

            if(flag){
                if(count == 2){
                    HBG[i].cols[countCol] = temp;
                    //printf("%d\n", temp);
                }
                if(count == iLS+3){
                    HBG[i].Pij[countCol] = temp % Zc;
                }
            }
            else{
                if(count == 1){
                    HBG[i].cols[countCol] =   temp;
                    //printf("%d\n", temp);
                }
                if(count == iLS+2){
                    HBG[i].Pij[countCol] = temp % Zc;
                }
            }

            if(ch == '\n'){
                count = 0;
                countCol += 1;
                flag = 0;
                if(countCol == numOfColumns[i]){
                    i = i+1;
                    countCol = 0;
                    flag = 1;
                }
            }
        }

        fclose(fptr);
    }

    // Generate H
    int **H;
    H = (int**)malloc(sizeof(int*)*rowDim);
    for(i=0;i<rowDim;i++){
        H[i] = (int*)malloc(sizeof(int)*colDim);
    }
    for(i=0;i<rowDim;i++){
        for(j=0;j<colDim;j++){
            H[i][j] = 0;

        }
    }

    for(i=0;i<rowHBG;i++){
        for(j=0;j<numOfColumns[i];j++){
            for(k=0;k<Zc;k++){
                H[i*Zc+k][(Zc*HBG[i].cols[j])+((k+HBG[i].Pij[j])%Zc)] = 1;
            }
        }
    }

    fptr = fopen("H.txt","w");
    fprintf(fptr, "%d ", colDim);
    fprintf(fptr, "%d\n", rowDim);
    int wr[rowDim];
    int wc[colDim];
    int maxWr=0;
    int maxWc=0;
    for(i=0;i<rowDim;i++)wr[i]=0;
    for(i=0;i<colDim;i++)wc[i]=0;

    for(i=0;i<rowDim;i++){
        for(j=0;j<colDim;j++){
            if(H[i][j] == 1){
                wr[i]+=1;
                wc[j]+=1;
            }
        }
        if(wr[i]>maxWr)maxWr=wr[i];// find the maximum term of wr
    }

    for(i=0;i<colDim;i++){
        if(wc[i]>maxWc)maxWc=wc[i];
    }
    fprintf(fptr, "%d ", maxWc);
    fprintf(fptr, "%d\n", maxWr);

    for(i=0;i<colDim;i++){
        fprintf(fptr,"%d ",wc[i]);
    }
    fprintf(fptr,"\n");

    for(i=0;i<rowDim;i++){
        fprintf(fptr,"%d ",wr[i]);
    }
    fprintf(fptr,"\n");

    for(j=0;j<colDim;j++){
        for(i=0;i<rowDim;i++){
            if(H[i][j] == 1){
                fprintf(fptr, "%d ", i+1);
            }
        }
        fprintf(fptr,"\n");
    }

    for(i=0;i<rowDim;i++){
        for(j=0;j<colDim;j++){
            if(H[i][j] == 1){
                fprintf(fptr, "%d ", j+1);
            }
        }
        fprintf(fptr,"\n");
    }



    fclose(fptr);
    printf("Zc: %d\n", Zc);
    printf("Kb: %d\n", Kb);
    printf("Row: %d\n", rowDim);
    printf("Col: %d\n", colDim);

    system("pause");

    return 0;



}


