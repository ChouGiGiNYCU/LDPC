
#ifndef AWGN
#define  AWGN
#include <stdio.h>
#include <stdlib.h>
#include  <math.h>


typedef unsigned long uint32;
#define MAX pow(2,32)-1
#define N_rand 624   // length of state vector
#define M_rand 397   // a period parameter
#define K_rand 0x9908B0DFU    // a magic constant
#define hiBit(u)  ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)  ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)  ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v


static uint32  state[N_rand+1];     // state vector + 1 extra to not violate ANSI C
static uint32  *Next;          // next random value is computed from here
static int LEFT = -1;      // can *next++ this many times before reloading

void seedMT(uint32 seed){
   
    register uint32 x = (seed | 1U) & 0xFFFFFFFFU, *s = state;
    register int    j;
    for(LEFT=0, *s++=x, j=N_rand; --j;
        *s++ = (x*=69069U) & 0xFFFFFFFFU);
 }


uint32 reloadMT(void){
    register uint32 *p0=state, *p2=state+2, *pM=state+M_rand, s0, s1;
    register int    j;
    if(LEFT < -1)
        seedMT(4357U);  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    LEFT=N_rand-1, Next=state+1;
    for(s0=state[0], s1=state[1], j=N_rand-M_rand+1; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K_rand : 0U);
    for(pM=state, j=M_rand; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K_rand : 0U);
    s1=state[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K_rand : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    return(s1 ^ (s1 >> 18));
 }

uint32 randomMT(void){
    uint32 y;
    if(--LEFT < 0)
        return(reloadMT());
    y  = *Next++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    return(y ^ (y >> 18));
 }

/***********************************************************************************************************/

#define A 16807.0
#define Mod 2147483647
static double r_seed=1.0;

 double rnd(){
	 r_seed=fmod(A*r_seed,Mod);
	 return (r_seed*4.656612875e-10);
 }

// N(0,1) Gaussian
double gasdev(){   
   static int iset=0;
   static double gset;
   double fac,rsq,v1,v2;
   if (iset == 0){
      do{
         //v1=2.0*rnd()-1,0;
         //v2=2.0*rnd()-1,0;
         // v1、v2 [-1~1]
		   v1=2.0 * (double)(randomMT())/(double)(MAX)-1.0;
         v2=2.0 * (double)(randomMT())/(double)(MAX)-1.0;
         rsq=v1*v1+v2*v2;
      }while (rsq >= 1.0 || rsq ==0.0);
      fac = sqrt(-2.0*log(rsq)/rsq);
      // printf("fac : %.2f \n",fac);
      gset=v1*fac;
      iset=1;
      return v2*fac;
   }
   else{
      iset=0;
      return gset;
   }
}

#endif 

