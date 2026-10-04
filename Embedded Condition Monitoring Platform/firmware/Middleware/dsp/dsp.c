#include "dsp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "filters.h"

/* Iterative radix-2 FFT; input is mean-removed and Hann-windowed in place. */
static void fft(float *re, float *im, size_t n) {
 size_t j=0; for(size_t i=1;i<n;i++){size_t bit=n>>1; for(;j&bit;bit>>=1)j^=bit; j^=bit; if(i<j){float t=re[i];re[i]=re[j];re[j]=t;t=im[i];im[i]=im[j];im[j]=t;}}
 for(size_t len=2;len<=n;len<<=1){float a=-2.0f*3.14159265358979323846f/(float)len;float wr0=cosf(a),wi0=sinf(a);for(size_t i=0;i<n;i+=len){float wr=1,wi=0;for(size_t k=0;k<len/2;k++){size_t u=i+k,v=u+len/2;float tr=wr*re[v]-wi*im[v],ti=wr*im[v]+wi*re[v];re[v]=re[u]-tr;im[v]=im[u]-ti;re[u]+=tr;im[u]+=ti;float nw=wr*wr0-wi*wi0;wi=wr*wi0+wi*wr0;wr=nw;}}}
}
void dsp_process(const float*x,size_t n,float fs,DspFeatures*o){dsp_process_configured(x,n,fs,1.0f,o);}
void dsp_process_configured(const float *x,size_t n,float fs,float hp,DspFeatures *o){memset(o,0,sizeof(*o));if(!x||!o||n<2||fs<=0)return;float *filtered=calloc(n,sizeof(float));if(!filtered)return;DcBlocker filter;dc_blocker_init(&filter,hp,fs);double mean=0;for(size_t i=0;i<n;i++){filtered[i]=dc_blocker_process(&filter,x[i]);mean+=filtered[i];}mean/=n;float lo=filtered[0],hi=filtered[0],peak=0;double s2=0,s4=0;for(size_t i=0;i<n;i++){double v=filtered[i]-(float)mean;if(filtered[i]<lo)lo=filtered[i];if(filtered[i]>hi)hi=filtered[i];s2+=v*v;s4+=v*v*v*v;if(fabs(v)>peak)peak=(float)fabs(v);}o->variance=(float)(s2/n);o->rms=sqrtf(o->variance);o->peak=peak;o->peak_to_peak=hi-lo;o->crest_factor=o->rms>0?peak/o->rms:0;o->kurtosis=o->variance>1e-20f?(float)((s4/n)/(o->variance*o->variance)):0;float *re=calloc(n,sizeof(float)),*im=calloc(n,sizeof(float));if(!re||!im){free(filtered);free(re);free(im);return;}for(size_t i=0;i<n;i++){float w=0.5f-0.5f*cosf(2*3.14159265f*i/(n-1));re[i]=(filtered[i]-(float)mean)*w;}fft(re,im,n);double energy=0,weighted=0,magnitude_sum=0;size_t best=1;float maxmag=0;for(size_t k=1;k<n/2;k++){float m=sqrtf(re[k]*re[k]+im[k]*im[k]);energy+=(double)m*m;weighted+=(double)m*k*fs/n;magnitude_sum+=m;if(m>maxmag){maxmag=m;best=k;}}o->spectral_energy=(float)energy;o->dominant_hz=(float)best*fs/n;o->centroid_hz=magnitude_sum>0?(float)(weighted/magnitude_sum):0;size_t fbin=(size_t)(o->dominant_hz*n/fs+0.5f);if(fbin<n/2){o->fundamental=maxmag;size_t h=2*fbin;if(h<n/2)o->second_harmonic=sqrtf(re[h]*re[h]+im[h]*im[h]);}for(size_t k=n/4;k<n/2;k++)o->high_band_energy+=re[k]*re[k]+im[k]*im[k];free(filtered);free(re);free(im);}
