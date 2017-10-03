/*Copyright 2017 Sushil Dubey, Shashi Dugad, TIFR

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// Pixel clusteriser algorithm
// Input: moduleStart[],moduleEnd[], 
// xx[], yy[] with applied ADC threshold
// Output: ClusterId[]


// System includes
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <iomanip>
#include <string>
#include <iostream>
#include <fstream>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/unique.h>
#include <thrust/execution_policy.h>
// CUDA runtime
#include <cuda.h>
#include <cuda_runtime.h>
#include "CudaError.h"
#include "EventInfoGPU.h"
#include "PixelClusterUtil.h"
#include "PixelClusterMem.h"
#include "PixelCluster.h"

using namespace std;
using namespace thrust;

/*
  The origin of (x,y) was shifted by (1,1) for clustering.
  Reshift the origin to (0,0)
  Input: x[], y[], size
  Output: x[]=x[]-1, y[]=y[]-1; 
*/
__global__ void shift_origin_kernel(uint wordCounter,uint *xx, uint *yy) {
  
  uint gIndex = threadIdx.x + blockIdx.x*blockDim.x;
  if(gIndex<wordCounter) {
    //since bad pixel has x=0,y=0
    if(xx[gIndex]>0) { // either both are 0 or none
      xx[gIndex] = xx[gIndex] -1;
      yy[gIndex] = yy[gIndex] -1;
    }
  }
}

/*
  This kernel sorts the xx[] and yy[] as per the sorted Index[]
  Input: Index[], xx[], yy[]
  Output: xx[], yy[]
*/
__global__ void copy_kernel(const uint *Index,const uint *xx, const uint *yy,
                            const uint *ADC, const uint size, uint *xx1,
                            uint *yy1, uint *ADC1) {
  uint tid = blockDim.x * blockIdx.x + threadIdx.x;
    if(tid < size) {
    xx1[tid] = xx[Index[tid]];
    yy1[tid] = yy[Index[tid]];
    ADC1[tid] = ADC[Index[tid]];
    }
}

/*
  This kernel will check for the subdivision and forms the new cluster
  Input: Index[], xx[], yy[], number of pixels
  Output: gClusterId[]

*/
__global__ void sub_cluster_kernel(const uint *Index, const uint *xx, 
                                     const uint *yy,const uint gpCounter,
                                     uint64 *gClusterId) 
{
  uint tid     = threadIdx.x;
  uint blockid = blockIdx.x;
  uint start   = Index[blockid];
  uint end     = Index[blockid+1];
  __shared__ uint64 old_clusterId;
  __shared__ int nstripx,nstripy;

  // skip the empty clusters
  if(gClusterId[start] == 0) return;
  if(end==(start+1) ) return;

  
  // kernel to handle cluster with size <= 80
  // this is the maximum cluster size
  // found after analysing around 300 events
  if(end-start <= 80) {

    // assuming that cluster size is less than 80
    __shared__ uint xp[162],yp[417];
    __shared__ int tid0x,tid0y;
    __shared__ uint xmin, ymin, xmax, ymax;
    nstripx=0;  nstripy=0;
    tid0x=-1;   tid0y=-1;
    
    #pragma unroll    // tells the compiler to unroll the loop 
    for(uint i=0;i<6;i++) {
      uint gtid = 6*tid + i;  // gtid upto 416 from 80 threads.
      if(gtid<MAX_Y)
        yp[gtid] = 0;
    }
        
    // intialize xp[0] to xp[160] from 80 threads.
    xp[2*tid+ 0]  = 0;
    xp[2*tid + 1] = 0; 
    // find xmin, ymin and subtract from all the pixels
    if(tid==0) {
      xmin = xx[start];
      xmax = xx[start];
      for(int i=1;i<(end-start);i++) {  
        if(xmin > xx[start+i]) {
          xmin = xx[start+i];
        }
        if(xmax<xx[start+i]) {xmax=xx[start+i];}
      }
    }
    if(tid==1) {
      ymin = yy[start];
      for(int i=1;i<(end-start);i++) {  
        if(ymin >yy[start+i]) {
          ymin = yy[start+i];
        }
        if(ymax<yy[start+i]) {ymax=yy[start+i];}
      }
    }
    if(tid==2) {
      xp[160] =0;
      xp[161] =0;
      yp[416] =0;
      old_clusterId = gClusterId[start];
    }
    __syncthreads();  
  
    // to find the projection
    if((start+tid) < end)  {
      uint xc = xx[start + tid] - (xmin-1);
      uint yc = yy[start + tid] - (ymin-1);
      xp[xc] = xc;
      yp[yc] = yc;                                    
    }
    __syncthreads();
  
    if(tid && (nstripx==0 || nstripy==0)){
      if(xp[tid] && !xp[tid - 1]){
        nstripx=1; tid0x=tid;
      }
      if(yp[tid] && !yp[tid - 1]){
        nstripy=1; tid0y=tid;
      }  
    }  // end of if(tid)
    __syncthreads();

    if(tid != tid0x) {
      if(xp[tid] && !xp[tid - 1]){
        nstripx=2;
      }    
    }  // end of if(tid)

    if(tid != tid0y) {
      if(yp[tid] && !yp[tid - 1]) {
        nstripy=2;
      }
    }       
    __syncthreads();
  
    if(nstripx==2 || nstripy==2){ // form cluster only if it is divisible
      if(tid) {
        if(xp[tid] && !xp[tid - 1]) {
          uint i = tid;
          while(xp[i]) {
            xp[i] = tid;
            i++;
            if(i == 162) break;
          }
        }
        if(yp[tid] && !yp[tid - 1]) {
          uint i = tid;
          while(yp[i]) {
            yp[i] = tid;
            i++;
            if(i == 416) break;
          }
        }
      } //end of if(tid)

      // if the difference is grater that 80 then they are not 
      // covered by the thread (rarest case)
      if(xmax-xmin>=blockDim.x) { // if the difference is greater than blockDim.x
        uint ext_tid = blockDim.x + tid;
        if(xp[ext_tid] && !xp[ext_tid-1]) {
          uint i = ext_tid;
          while(xp[i]) {
            xp[i] = ext_tid;
            i++;
            if (i==162) break;
          }
        }
      }

      __syncthreads();

      //assign the cluster id to each pixel
      if((start + tid )< end) {
        uint px=0, py=0;
        uint64 new_clusterId=0;
        px = xp[xx[start + tid]-(xmin-1)] + xmin-1;            
        py = yp[yy[start + tid]-(ymin-1)] + ymin-1;         
        new_clusterId = (old_clusterId & (~uint64(0) << MODULE_shift)) | (py << YCOR_shift) | (px << XCOR_shift);
        if(old_clusterId!=new_clusterId && (px!=0 && py!=0))
          gClusterId[start+tid] = new_clusterId;  
      }
    }//end of if(nstripx==2 || nstripy==2) 
  } // end of if(end-start<=80)
  else if(blockid!=gridDim.x-1){  // sub-cluster kernel to handle cluster with size > 80
    
    __shared__ uint xp[162],yp[417];
    __shared__ uint cluster_size,itrn;
    nstripx=0;  nstripy=0;
    #pragma unroll    // tells the compiler to unroll the loop 
    for(uint i=0;i<6;i++) {
      uint gtid = 6*tid + i;  // gtid upto 416 from 80 threads.
      if(gtid<MAX_Y)
        yp[gtid] = 0;
    }
        
    // intialize yp[0] to yp[160] from 80 threads.
    xp[2*tid+ 0]  = 0;
    xp[2*tid + 1] = 0;  
   
    if(tid==0) {
      xp[160] = 0;
      cluster_size = end-start;
      itrn = cluster_size/blockDim.x + 1;
      old_clusterId = gClusterId[start];
    }
    __syncthreads();  

    //  to find the projection
    #pragma unroll 
    for(uint i=0;i<itrn;i++) {
      uint gtid  = 80*i + tid;
      if(gtid < cluster_size)  {
        uint xc = xx[start+gtid];
        uint yc = yy[start+gtid];
        xp[xc] = xc;
        yp[yc] = yc;                                                                            
      }
      __syncthreads();
    }

    #pragma unroll
    for(uint j =0;j<6; j++) {  // generates gtid form 0.....416 from 80 threads
      uint gtid = 6*tid + j;
      if(gtid && gtid<MAX_Y) {
        if(yp[gtid] && !yp[gtid - 1]) {
          uint i = gtid;
          uint old_nstripy = atomicAdd(&nstripy,1);  // to store the no of xstrip
          while(yp[i]) {
            yp[i] = gtid;
            i++;
            if(i >= MAX_Y) break;
          }
        }
        if(gtid < MAX_X) {
          if(xp[gtid] && !xp[gtid - 1]) {
            uint k = gtid;
            uint old_nstripx = atomicAdd(&nstripx,1);
            while(xp[k]) {
              xp[k] = gtid;
              k++;
              if(k >= MAX_X ) break;
            }
          }
        }//end of if(gtid<161)
      }
      __syncthreads();
    } // end of for loop  

    // assign the cluster id to each pixel only if it divisible 
    if(nstripx>1 || nstripy>1) {
      for(uint i=0;i<itrn;i++) {
        uint gtid = blockDim.x*i + tid;
        if(gtid < cluster_size)  {
          uint px=0, py=0;
          uint64 new_clusterId=0;
          px = xp[xx[start+gtid]];                    // find location of pixel on x strip
          py = yp[yy[start+gtid]];                    // find location of strip on y strip
          new_clusterId = (old_clusterId & (~uint64(0) << MODULE_shift)) | (py << YCOR_shift) | (px << XCOR_shift);
          if(old_clusterId!=new_clusterId && (px!=0 && py!=0))
            gClusterId[start+gtid] = new_clusterId;    
        }
      }
    }
  } // end of else

} // end of sub_cluster

// fills the Index[] with 0, 1,2,3.. upto the size of event
__global__ void createIndex_kernel(const uint wordCounter, uint *Index) {
  uint tid = blockDim.x * blockIdx.x + threadIdx.x;
    if(tid < wordCounter) {
      Index[tid] = tid;
    }
}
void createIndex(const uint wordCounter, uint *Index) {
  // to fill the index array Index[] 
  // which will be used in sort by key 
  int nthreads = 1024;
  int nblocks = wordCounter/nthreads +1;
  createIndex_kernel<<<nblocks, nthreads>>>(wordCounter, Index);
  cudaDeviceSynchronize();
  checkCUDAError("Error in createIndex_kernel");
}

/*
  This function sorts the cluster id, removes the duplicate
  and finds the sub-cluster within each cluster
  Input: d_Index[], d_gClusterId[], d_xx[], d_yy[], wordCounter, h_Index[]
  Output:d_xx1[], d_yy1[]
*/
void sub_cluster(uint *d_xx, uint *d_yy,const uint *d_ADC, uint *d_Index, uint64 *d_gClusterId, 
                  const uint wordCounter, uint64 *d_gClusterId1, uint *d_xx1, uint *d_yy1, uint *d_ADC1 ) 
{
  
  //cout<<"Inside sub_cluster function: wordCounter"<<endl;
  createIndex(wordCounter, d_Index);
  
  // get device_ptr needed for thrust operations
  thrust::device_ptr<uint> Index(d_Index); //Index is the index array
  thrust::device_ptr<uint64> gClusterId(d_gClusterId);

  // sort the cluster id by key
  thrust::sort_by_key(gClusterId , gClusterId + wordCounter, Index);
  
  cudaMemcpy(d_gClusterId1, d_gClusterId, wordCounter*sizeof(uint64),  cudaMemcpyDeviceToDevice );

  // launch kernel for sorting xx[] and yy[]
  uint N_threads = 1024;
  uint N_blocks  = wordCounter / N_threads +1;
  copy_kernel<<<N_blocks,N_threads>>>(d_Index, d_xx, d_yy,d_ADC, wordCounter,d_xx1,d_yy1,d_ADC1); 
  cudaDeviceSynchronize();
  checkCUDAError("Error in copy kernel");
  
  // removes the consecutive duplicate
  // new_end.first gives size of gClusterId with no ducplicate
  uint total_cluster=0;
  thrust::pair<thrust::device_ptr<uint64>, thrust::device_ptr<uint> > new_end;
  
  // Fill the index again which will be used for uniuqe 
  createIndex(wordCounter, d_Index);
  
  new_end = thrust::unique_by_key(gClusterId , gClusterId + wordCounter, Index );
  total_cluster = new_end.first - gClusterId;
  checkCUDAError(" Failed after unique operation");
 
  // launch the kernel for subdivision
  dim3 no_threads =  80; // maximum size of cluster found after analysis
  dim3 no_blocks  =  total_cluster;
  // cout<<"Total_clusters: "<<total_cluster<<endl;
  // Ignore first few cluster as they might contain 0s
  sub_cluster_kernel<<<no_blocks,no_threads>>>(d_Index, d_xx1, d_yy1,wordCounter, d_gClusterId1);
  cudaDeviceSynchronize();
  checkCUDAError(" Failed after sub-cluster-kernel");
} // End of sort_cluster

__device__ uint64 getClusterId(uint event, uint module, uint y, uint x) {
  uint64 clusterId = ((uint64)event << EVENT_shift) | (module << MODULE_shift) | (y << YCOR_shift) | (x << XCOR_shift);
  return clusterId;
}

/* 
  This is the main kernel for clustarisation
  Inputs:  xx[],yy[],module[], Imodule[]
  Outputs: gClusterId[]
*/
__global__ void cluster_kernel(uint *xx, uint *yy, const int *mIndexStart,
                               const int *mIndexEnd, uint64 *gClusterId) 
{
  __shared__ uint xp[MAX_X+1], yp[MAX_Y+1];   // Array to store x and y projection
  uint moduleId  = blockIdx.x;                // to get block id
  uint event = blockIdx.y;
  uint tid = threadIdx.x;                     // to get thread id
  int  moduleBegin, moduleEnd;
  
  moduleBegin = mIndexStart[event*NMODULE + moduleId];
  moduleEnd   = mIndexEnd[event*NMODULE+ moduleId];

  if(moduleBegin==-1 && moduleEnd==-1) {
    return;
  }
  if(moduleBegin==-1) {
    moduleBegin = moduleEnd;
  }
  if(moduleEnd==-1) {
    moduleEnd = moduleBegin;
  }
  __syncthreads();
  //module contains only one pixel
  if(moduleBegin==moduleEnd) {
    int px = xx[moduleBegin];
    int py = yy[moduleBegin];
    gClusterId[moduleBegin]= getClusterId(event, moduleId, py,px);
    return;
  }
  
  
  uint module_size = (moduleEnd - moduleBegin) + 1;

  yp[tid] = 0;                    // Initialize Y projection to false
  if(tid < MAX_X+1) {xp[tid] = 0;} // Initialize X projection to false
  
  __syncthreads();            // let all threads finish intialization
  // To get projection on x and y axis
  // For loop is used to deal with module with hits more than blockDim.x
  uint noItr = module_size/blockDim.x+1;
  for(uint i=0; i<noItr; i++) {
    uint idx = moduleBegin + tid + i*blockDim.x;
    if(idx <= moduleEnd) {
      uint tx = xx[idx];
      uint ty = yy[idx];
      xp[tx] = tx;
      yp[ty] = ty;
      gClusterId[idx] = 0;
    } // if(tid < module_size)
  } // End of for(uint i=0; i<noItr...
  __syncthreads();

  //  Store the unique strip# for all element in one strip.
  //  Divide the projections array into multiple strips.
  //  Distribute each strip to one thread.
  //  Each thread will process the strip independently
  //  and will store the start location of strip at all location of strip
  //  This procedure will be repeated for both x and y strip.
  if(tid) {
    if(yp[tid] && !yp[tid - 1]) {
      uint i = tid;
      while(yp[i]) {
        yp[i] = tid;
        i++;
        if(i > MAX_Y) break;
      }
    }

    if(tid < MAX_X) {
      if(xp[tid] && !xp[tid - 1]) {
        uint i = tid;
        while(xp[i]) {
          xp[i] = tid;
          i++;
          if(i > MAX_X) break;
        }
      }
    }//end of if(tid<MAX_X){
  } //end of if(tid)
 __syncthreads();
  //assign the cluster id to each pixel
  for(uint i=0; i<noItr; i++) {
    uint idx = moduleBegin + tid + i*blockDim.x;
    if(idx <= moduleEnd) {
      uint px = xp[xx[idx]];                    // find location of pixel on x strip
      uint py = yp[yy[idx]];                    // find location of strip on y strip
      if(px!=0 && py!=0)
        gClusterId[idx] = getClusterId(event, moduleId, py,px);     // gclusterId(idx) = f(event, module,px, py);
    } // if(idx <= moduleEnd)
  } // End of for(uint i=0; i<noItr...  

} //End of cluster_kernel

// assign the index of last cluster
__global__ void init_kernel(const uint wordCounter, const uint total_cluster, uint *Index) {
  Index[total_cluster] = wordCounter;
}

/************* origin of call to the kernel***************/

void PixelCluster_Wrapper(uint *d_xx, uint *d_yy, uint *d_ADC,const uint wordCounter,
                         const int *mIndexStart,const int *mIndexEnd) 
{
    checkCUDAError("Error in RawToDigi, didn't enter in Cluster");    
    cout<<"Clustering started on GPU!"<<endl;

    // cudaEventRecord(start);
    cudaMemset(d_gClusterId, 0, wordCounter*sizeof(uint64));
    checkCUDAError("Error in setting memory to 0");

    // launch clustering kernel
    int blockX  = MAX_MODULE_SIZE;   // # of blocks in x direction are modules
    int blockY  = NEVENT;            // # of blocks in Y direction are events 
    dim3 threadsize = NO_THREADS;    // # of threads
    dim3 gridsize(blockX, blockY);
    
    cluster_kernel <<< gridsize, threadsize >>>(d_xx, d_yy, mIndexStart, mIndexEnd, d_gClusterId);
    cudaDeviceSynchronize();
    checkCUDAError(" Failed after main kernel call");
    
    sub_cluster(d_xx, d_yy, d_ADC, Index, d_gClusterId, wordCounter, d_gClusterId1, d_xx1, d_yy1, d_ADC1);
    
    // FOR CPE formate the output of cluster before giving to CPE
    // sort the clusterIds and corrseponding attributes
    // to get the start and end index of cluster
    createIndex(wordCounter, Index);
    thrust::device_ptr<uint64> ClusterId_ptr(d_gClusterId1);
    thrust::device_ptr<uint> Index_ptr(Index);
    
    thrust::sort_by_key(ClusterId_ptr , ClusterId_ptr + wordCounter, Index_ptr);
    cudaMemcpy(d_gClusterId, d_gClusterId1, wordCounter * sizeof(uint64),  cudaMemcpyDeviceToDevice );
    // now sort the xx yy and ADC
    uint N_threads = 1024;
    uint N_blocks = wordCounter/N_threads +1;
    copy_kernel<<<N_blocks,N_threads>>>(Index, d_xx1, d_yy1,d_ADC1, wordCounter,d_xx,d_yy,d_ADC); 
    cudaDeviceSynchronize();
    checkCUDAError("Error in sorting ");

    uint total_cluster=0;
    thrust::pair<thrust::device_ptr<uint64>, thrust::device_ptr<uint> > new_end;
  
    // Fill the index again which will be used for uniuqe 
    createIndex(wordCounter, Index);
    new_end = thrust::unique_by_key(ClusterId_ptr , ClusterId_ptr + wordCounter, Index_ptr );
    total_cluster = new_end.first - ClusterId_ptr;
    checkCUDAError(" Failed at Clustering");
    
    // Call CPE function
    init_kernel<<<1,1>>>(wordCounter, total_cluster, Index);
    // Index[total_cluster] = wordCounter;
    // since origin is shifted by (1,1) move it back to (0,0) before giving it CPE
    shift_origin_kernel<<<N_blocks, N_threads>>>(wordCounter,d_xx,d_yy); 
    cudaDeviceSynchronize();

    // End of Clusterisation and starting CPE
    CPE_wrapper(total_cluster,d_gClusterId1, Index, d_xx, d_yy, d_ADC);
    
} //end of pixel clusterizer

void initDeviceMemCluster() {
    const int MAX_FED  = 150;   // not all are present typically 108
    const int MAX_WORD = 2000;  // don't know the exact max word, for PU70 max was 2900
    const int size = MAX_FED*MAX_WORD*NEVENT*sizeof(uint);
    cudaMalloc((void**)&Index , size*sizeof(uint));
    cudaMalloc((void**)&d_xx1, size*sizeof(uint));
    cudaMalloc((void**)&d_yy1, size*sizeof(uint));
    cudaMalloc((void**)&d_gClusterId, size*sizeof(uint64));
    cudaMalloc((void**)&d_gClusterId1, size*sizeof(uint64));
    cudaMalloc((void**)&d_ADC1,  size*sizeof(uint));
    checkCUDAError("Error in memory allocation for clustering");
}

void freeDeviceMemCluster() {
    cudaFree(Index);
    cudaFree(d_xx1);
    cudaFree(d_yy1);
    cudaFree(d_gClusterId);
    cudaFree(d_gClusterId1);
    cudaFree(d_ADC1);
}