#define IKFAST_NAMESPACE ik_katana5d_trans
#define IKFAST_HEADER "plugindefs.h"

/// autogenerated analytical inverse kinematics code from ikfast program part of OpenRAVE
/// \author Rosen Diankov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// ikfast version 41 generated on 2011-04-24 18:32:44.299680
/// To compile with gcc:
///     gcc -lstdc++ ik.cpp
/// To compile without any main function as a shared object:
///     gcc -fPIC -lstdc++ -DIKFAST_NO_MAIN -shared -Wl,-soname,ik.so -o ik.so ik.cpp
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <complex>

#ifdef IKFAST_HEADER
#include IKFAST_HEADER
#endif

#ifndef IKFAST_ASSERT
#include <stdexcept>
#include <sstream>

#ifdef _MSC_VER
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

#define IKFAST_ASSERT(b) { if( !(b) ) { std::stringstream ss; ss << "ikfast exception: " << __FILE__ << ":" << __LINE__ << ": " <<__PRETTY_FUNCTION__ << ": Assertion '" << #b << "' failed"; throw std::runtime_error(ss.str()); } }

#endif

#if defined(_MSC_VER)
#define IKFAST_ALIGNED16(x) __declspec(align(16)) x
#else
#define IKFAST_ALIGNED16(x) x __attribute((aligned(16)))
#endif

#define IK2PI  ((IKReal)6.28318530717959)
#define IKPI  ((IKReal)3.14159265358979)
#define IKPI_2  ((IKReal)1.57079632679490)

#ifdef _MSC_VER
#ifndef isnan
#define isnan _isnan
#endif
#endif // _MSC_VER

// defined when creating a shared object/dll
#ifdef IKFAST_CLIBRARY
#ifdef _MSC_VER
#define IKFAST_API extern "C" __declspec(dllexport)
#else
#define IKFAST_API extern "C"
#endif
#else
#define IKFAST_API
#endif

// lapack routines
extern "C" {
  void dgetrf_ (const int* m, const int* n, double* a, const int* lda, int* ipiv, int* info);
  void zgetrf_ (const int* m, const int* n, std::complex<double>* a, const int* lda, int* ipiv, int* info);
  void dgetri_(const int* n, const double* a, const int* lda, int* ipiv, double* work, const int* lwork, int* info);
  void dgesv_ (const int* n, const int* nrhs, double* a, const int* lda, int* ipiv, double* b, const int* ldb, int* info);
  void dgetrs_(const char *trans, const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, int *info);
  void dgeev_(const char *jobvl, const char *jobvr, const int *n, double *a, const int *lda, double *wr, double *wi,double *vl, const int *ldvl, double *vr, const int *ldvr, double *work, const int *lwork, int *info);
}

using namespace std; // necessary to get std math routines

#ifdef IKFAST_NAMESPACE
namespace IKFAST_NAMESPACE {
#endif

#ifdef IKFAST_REAL
typedef IKFAST_REAL IKReal;
#else
typedef double IKReal;
#endif

class IKSolution
{
public:
    /// Gets a solution given its free parameters
    /// \param pfree The free parameters required, range is in [-pi,pi]
    void GetSolution(IKReal* psolution, const IKReal* pfree) const {
        for(std::size_t i = 0; i < basesol.size(); ++i) {
            if( basesol[i].freeind < 0 )
                psolution[i] = basesol[i].foffset;
            else {
                IKFAST_ASSERT(pfree != NULL);
                psolution[i] = pfree[basesol[i].freeind]*basesol[i].fmul + basesol[i].foffset;
                if( psolution[i] > IKPI ) {
                    psolution[i] -= IK2PI;
                }
                else if( psolution[i] < -IKPI ) {
                    psolution[i] += IK2PI;
                }
            }
        }
    }

    /// Gets the free parameters the solution requires to be set before a full solution can be returned
    /// \return vector of indices indicating the free parameters
    const std::vector<int>& GetFree() const { return vfree; }

    struct VARIABLE
    {
        VARIABLE() : freeind(-1), fmul(0), foffset(0) {}
        VARIABLE(int freeind, IKReal fmul, IKReal foffset) : freeind(freeind), fmul(fmul), foffset(foffset) {}
        int freeind;
        IKReal fmul, foffset; ///< joint value is fmul*sol[freeind]+foffset
    };

    std::vector<VARIABLE> basesol;       ///< solution and their offsets if joints are mimiced
    std::vector<int> vfree;
};

inline float IKabs(float f) { return fabsf(f); }
inline double IKabs(double f) { return fabs(f); }

inline float IKlog(float f) { return logf(f); }
inline double IKlog(double f) { return log(f); }

#ifndef IKFAST_SINCOS_THRESH
#define IKFAST_SINCOS_THRESH ((IKReal)0.000001)
#endif

inline float IKasin(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asinf(f);
}
inline double IKasin(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asin(f);
}

// return positive value in [0,y)
inline float IKfmod(float x, float y)
{
    while(x < 0) {
        x += y;
    }
    return fmodf(x,y);
}

// return positive value in [0,y)
inline float IKfmod(double x, double y)
{
    while(x < 0) {
        x += y;
    }
    return fmod(x,y);
}

inline float IKacos(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0;
return acosf(f);
}
inline double IKacos(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0;
return acos(f);
}
inline float IKsin(float f) { return sinf(f); }
inline double IKsin(double f) { return sin(f); }
inline float IKcos(float f) { return cosf(f); }
inline double IKcos(double f) { return cos(f); }
inline float IKtan(float f) { return tanf(f); }
inline double IKtan(double f) { return tan(f); }
inline float IKsqrt(float f) { if( f <= 0.0f ) return 0.0f; return sqrtf(f); }
inline double IKsqrt(double f) { if( f <= 0.0 ) return 0.0; return sqrt(f); }
inline float IKatan2(float fy, float fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2f(fy,fx);
}
inline double IKatan2(double fy, double fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2(fy,fx);
}

inline float IKsign(float f) {
    if( f > 0 ) {
        return 1.0f;
    }
    else if( f < 0 ) {
        return -1.0f;
    }
    return 0;
}

inline double IKsign(double f) {
    if( f > 0 ) {
        return 1.0;
    }
    else if( f < 0 ) {
        return -1.0;
    }
    return 0;
}

/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API void fk(const IKReal* j, IKReal* eetrans, IKReal* eerot) {
IKReal x0,x1,x2,x3,x4,x5,x6,x7;
x0=IKcos(j[0]);
x1=IKsin(j[1]);
x2=IKsin(j[2]);
x3=IKcos(j[1]);
x4=IKcos(j[2]);
x5=IKsin(j[3]);
x6=IKsin(j[0]);
x7=IKcos(j[3]);
eetrans[0]=((((0.190000000000000)*(x0)*(x1)))+(((x7)*(((((0.273000000000000)*(x0)*(x1)*(x4)))+(((0.273000000000000)*(x0)*(x2)*(x3)))))))+(((x5)*(((((0.273000000000000)*(x0)*(x1)*(x2)))+(((-0.273000000000000)*(x0)*(x3)*(x4)))))))+(((0.139000000000000)*(x0)*(x2)*(x3)))+(((0.139000000000000)*(x0)*(x1)*(x4))));
eetrans[1]=((((x5)*(((((-0.273000000000000)*(x1)*(x2)*(x6)))+(((0.273000000000000)*(x3)*(x4)*(x6)))))))+(((x7)*(((((-0.273000000000000)*(x1)*(x4)*(x6)))+(((-0.273000000000000)*(x2)*(x3)*(x6)))))))+(((-0.139000000000000)*(x2)*(x3)*(x6)))+(((-0.139000000000000)*(x1)*(x4)*(x6)))+(((-0.190000000000000)*(x1)*(x6))));
eetrans[2]=((0.201500000000000)+(((x7)*(((((-0.273000000000000)*(x1)*(x2)))+(((0.273000000000000)*(x3)*(x4)))))))+(((0.190000000000000)*(x3)))+(((x5)*(((((0.273000000000000)*(x2)*(x3)))+(((0.273000000000000)*(x1)*(x4)))))))+(((0.139000000000000)*(x3)*(x4)))+(((-0.139000000000000)*(x1)*(x2))));
}

IKFAST_API int getNumFreeParameters() { return 2; }
IKFAST_API int* getFreeParameters() { static int freeparams[] = {3, 4}; return freeparams; }
IKFAST_API int getNumJoints() { return 5; }

IKFAST_API int getIKRealSize() { return sizeof(IKReal); }

IKFAST_API int getIKType() { return 0x33000003; }

class IKSolver {
public:
IKReal j0,cj0,sj0,htj0,j1,cj1,sj1,htj1,j2,cj2,sj2,htj2,j3,cj3,sj3,htj3,j4,cj4,sj4,htj4,new_px,px,npx,new_py,py,npy,new_pz,pz,npz,pp;

bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
for(int dummyiter = 0; dummyiter < 1; ++dummyiter) {
vsolutions.resize(0); vsolutions.reserve(8);
j3=pfree[0]; cj3=cos(pfree[0]); sj3=sin(pfree[0]);
j4=pfree[1]; cj4=cos(pfree[1]); sj4=sin(pfree[1]);
px = eetrans[0]; py = eetrans[1]; pz = eetrans[2];

new_px=px;
new_py=((-1.00000000000000)*(py));
new_pz=((0.201500000000000)+(((-1.00000000000000)*(pz))));
px = new_px; py = new_py; pz = new_pz;
pp=(((px)*(px))+((py)*(py))+((pz)*(pz)));
{
IKReal j0array[2], cj0array[2], sj0array[2];
bool j0valid[2]={false};
IKReal x8=((-1.00000000000000)*(px));
IKReal x9=IKatan2(py, x8);
j0array[0]=((-1.00000000000000)*(x9));
sj0array[0]=IKsin(j0array[0]);
cj0array[0]=IKcos(j0array[0]);
j0array[1]=((3.14159265358979)+(((-1.00000000000000)*(x9))));
sj0array[1]=IKsin(j0array[1]);
cj0array[1]=IKcos(j0array[1]);
if( j0array[0] > IKPI )
{
    j0array[0]-=IK2PI;
}
else if( j0array[0] < -IKPI )
{    j0array[0]+=IK2PI;
}
j0valid[0] = true;
if( j0array[1] > IKPI )
{
    j0array[1]-=IK2PI;
}
else if( j0array[1] < -IKPI )
{    j0array[1]+=IK2PI;
}
j0valid[1] = true;
if( j0valid[0] && j0valid[1] && IKabs(cj0array[0]-cj0array[1]) < 0.0001 && IKabs(sj0array[0]-sj0array[1]) < 0.0001 )
{
    j0valid[1]=false;
}
for(int ij0 = 0; ij0 < 2; ++ij0)
{
if( !j0valid[ij0] )
{
    continue;
}
j0 = j0array[ij0]; cj0 = cj0array[ij0]; sj0 = sj0array[ij0];

{
IKReal dummyeval[1];
dummyeval[0]=((1.00000000000000)+(((3.85740903679934)*((cj3)*(cj3))))+(((3.85740903679934)*((sj3)*(sj3))))+(((3.92805755395683)*(cj3))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal dummyeval[1];
dummyeval[0]=((1.00000000000000)+(((3.85740903679934)*((cj3)*(cj3))))+(((3.85740903679934)*((sj3)*(sj3))))+(((3.92805755395683)*(cj3))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal dummyeval[1];
dummyeval[0]=(((((py)*(py))*((sj0)*(sj0))))+((pz)*(pz))+((((cj0)*(cj0))*((px)*(px))))+(((2.00000000000000)*(cj0)*(px)*(py)*(sj0))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j1array[2], cj1array[2], sj1array[2];
bool j1valid[2]={false};
IKReal x10=((0.380000000000000)*(cj0)*(px));
IKReal x11=((0.380000000000000)*(py)*(sj0));
IKReal x12=((x11)+(x10));
IKReal x13=(pz)*(pz);
IKReal x14=((0.144400000000000)*(x13));
IKReal x15=(x12)*(x12);
IKReal x16=((x15)+(x14));
if( (x16) < (IKReal)-0.00001 )
    continue;
IKReal x17=IKsqrt(x16);
IKReal x18=IKabs(x17);
IKReal x19=((IKabs(x18) != 0)?((IKReal)1/(x18)):(IKReal)1.0e30);
IKReal x20=((0.0758940000000000)*(cj3));
IKReal x21=((0.0577500000000000)+(x20));
IKReal x22=((x21)+(((-1.00000000000000)*(pp))));
IKReal x23=((x19)*(x22));
if( (x23) < -1-IKFAST_SINCOS_THRESH || (x23) > 1+IKFAST_SINCOS_THRESH )
    continue;
IKReal x24=IKasin(x23);
IKReal x25=((-0.380000000000000)*(pz));
IKReal x26=IKatan2(x25, x12);
j1array[0]=((((-1.00000000000000)*(x26)))+(((-1.00000000000000)*(x24))));
sj1array[0]=IKsin(j1array[0]);
cj1array[0]=IKcos(j1array[0]);
j1array[1]=((3.14159265358979)+(((-1.00000000000000)*(x26)))+(x24));
sj1array[1]=IKsin(j1array[1]);
cj1array[1]=IKcos(j1array[1]);
if( j1array[0] > IKPI )
{
    j1array[0]-=IK2PI;
}
else if( j1array[0] < -IKPI )
{    j1array[0]+=IK2PI;
}
j1valid[0] = true;
if( j1array[1] > IKPI )
{
    j1array[1]-=IK2PI;
}
else if( j1array[1] < -IKPI )
{    j1array[1]+=IK2PI;
}
j1valid[1] = true;
if( j1valid[0] && j1valid[1] && IKabs(cj1array[0]-cj1array[1]) < 0.0001 && IKabs(sj1array[0]-sj1array[1]) < 0.0001 )
{
    j1valid[1]=false;
}
for(int ij1 = 0; ij1 < 2; ++ij1)
{
if( !j1valid[ij1] )
{
    continue;
}
j1 = j1array[ij1]; cj1 = cj1array[ij1]; sj1 = sj1array[ij1];

{
IKReal dummyeval[1];
IKReal gconst0;
gconst0=IKsign(((19321.0000000000)+(((74529.0000000000)*((sj3)*(sj3))))+(((74529.0000000000)*((cj3)*(cj3))))+(((75894.0000000000)*(cj3)))));
dummyeval[0]=((1.00000000000000)+(((3.85740903679934)*((cj3)*(cj3))))+(((3.85740903679934)*((sj3)*(sj3))))+(((3.92805755395683)*(cj3))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal dummyeval[1];
dummyeval[0]=((1.00000000000000)+(((3.85740903679934)*((cj3)*(cj3))))+(((3.85740903679934)*((sj3)*(sj3))))+(((3.92805755395683)*(cj3))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j2array[1], cj2array[1], sj2array[1];
bool j2valid[1]={false};
IKReal x27=(sj3)*(sj3);
IKReal x28=((1416051.00000000)*(x27));
IKReal x29=(cj3)*(cj3);
IKReal x30=((1416051.00000000)*(x29));
IKReal x31=((1441986.00000000)*(cj3));
IKReal x32=((367099.000000000)+(x31)+(x30)+(x28));
IKReal x33=((IKabs(x32) != 0)?((IKReal)1/(x32)):(IKReal)1.0e30);
j2array[0]=IKatan2(((-1.00000000000000)*(x33)*(((((-2641000.00000000)*(cj0)*(cj1)*(px)))+(((-5187000.00000000)*(cj3)*(pz)*(sj1)))+(((1035953.10000000)*(cj3)*(sj3)))+(((1773817.50000000)*(sj3)))+(((-13650000.0000000)*(pp)*(sj3)))+(((-5187000.00000000)*(cj1)*(cj3)*(py)*(sj0)))+(((-2641000.00000000)*(cj1)*(py)*(sj0)))+(((-2641000.00000000)*(pz)*(sj1)))+(((-5187000.00000000)*(cj0)*(cj1)*(cj3)*(px)))))), ((-1.00000000000000)*(x33)*(((903152.500000000)+(((5187000.00000000)*(cj1)*(py)*(sj0)*(sj3)))+(((-6950000.00000000)*(pp)))+(((2301280.80000000)*(cj3)))+(((5187000.00000000)*(cj0)*(cj1)*(px)*(sj3)))+(((5187000.00000000)*(pz)*(sj1)*(sj3)))+(((-13650000.0000000)*(cj3)*(pp)))+(((1035953.10000000)*(x29)))))));
sj2array[0]=IKsin(j2array[0]);
cj2array[0]=IKcos(j2array[0]);
if( j2array[0] > IKPI )
{
    j2array[0]-=IK2PI;
}
else if( j2array[0] < -IKPI )
{    j2array[0]+=IK2PI;
}
j2valid[0] = true;
for(int ij2 = 0; ij2 < 1; ++ij2)
{
if( !j2valid[ij2] )
{
    continue;
}
j2 = j2array[ij2]; cj2 = cj2array[ij2]; sj2 = sj2array[ij2];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(5);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.vfree.resize(0);
}
}
}

}

}

} else
{
{
IKReal j2array[1], cj2array[1], sj2array[1];
bool j2valid[1]={false};
j2array[0]=IKatan2(((gconst0)*(((((273000.000000000)*(cj0)*(px)*(sj1)*(sj3)))+(((-51870.0000000000)*(sj3)))+(((-273000.000000000)*(cj1)*(pz)*(sj3)))+(((273000.000000000)*(py)*(sj0)*(sj1)*(sj3)))+(((273000.000000000)*(cj3)*(pz)*(sj1)))+(((139000.000000000)*(cj0)*(cj1)*(px)))+(((273000.000000000)*(cj0)*(cj1)*(cj3)*(px)))+(((139000.000000000)*(pz)*(sj1)))+(((273000.000000000)*(cj1)*(cj3)*(py)*(sj0)))+(((139000.000000000)*(cj1)*(py)*(sj0)))))), ((-1.00000000000000)*(gconst0)*(((26410.0000000000)+(((273000.000000000)*(cj0)*(cj1)*(px)*(sj3)))+(((-139000.000000000)*(cj0)*(px)*(sj1)))+(((273000.000000000)*(cj1)*(py)*(sj0)*(sj3)))+(((-273000.000000000)*(cj0)*(cj3)*(px)*(sj1)))+(((-273000.000000000)*(cj3)*(py)*(sj0)*(sj1)))+(((139000.000000000)*(cj1)*(pz)))+(((273000.000000000)*(cj1)*(cj3)*(pz)))+(((51870.0000000000)*(cj3)))+(((273000.000000000)*(pz)*(sj1)*(sj3)))+(((-139000.000000000)*(py)*(sj0)*(sj1)))))));
sj2array[0]=IKsin(j2array[0]);
cj2array[0]=IKcos(j2array[0]);
if( j2array[0] > IKPI )
{
    j2array[0]-=IK2PI;
}
else if( j2array[0] < -IKPI )
{    j2array[0]+=IK2PI;
}
j2valid[0] = true;
for(int ij2 = 0; ij2 < 1; ++ij2)
{
if( !j2valid[ij2] )
{
    continue;
}
j2 = j2array[ij2]; cj2 = cj2array[ij2]; sj2 = sj2array[ij2];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(5);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.vfree.resize(0);
}
}
}

}

}
}
}

}

}

} else
{
{
IKReal j2array[2], cj2array[2], sj2array[2];
bool j2valid[2]={false};
IKReal x34=((0.103740000000000)*(cj3));
IKReal x35=((0.0528200000000000)+(x34));
IKReal x36=(sj3)*(sj3);
IKReal x37=((0.0107619876000000)*(x36));
IKReal x38=(x35)*(x35);
IKReal x39=((x38)+(x37));
if( (x39) < (IKReal)-0.00001 )
    continue;
IKReal x40=IKsqrt(x39);
IKReal x41=IKabs(x40);
IKReal x42=((IKabs(x41) != 0)?((IKReal)1/(x41)):(IKReal)1.0e30);
IKReal x43=((0.0758940000000000)*(cj3));
IKReal x44=((0.129950000000000)+(x43));
IKReal x45=((x44)+(((-1.00000000000000)*(pp))));
IKReal x46=((x42)*(x45));
if( (x46) < -1-IKFAST_SINCOS_THRESH || (x46) > 1+IKFAST_SINCOS_THRESH )
    continue;
IKReal x47=IKasin(x46);
IKReal x48=((0.103740000000000)*(sj3));
IKReal x49=IKatan2(x35, x48);
j2array[0]=((((-1.00000000000000)*(x47)))+(((-1.00000000000000)*(x49))));
sj2array[0]=IKsin(j2array[0]);
cj2array[0]=IKcos(j2array[0]);
j2array[1]=((3.14159265358979)+(((-1.00000000000000)*(x49)))+(x47));
sj2array[1]=IKsin(j2array[1]);
cj2array[1]=IKcos(j2array[1]);
if( j2array[0] > IKPI )
{
    j2array[0]-=IK2PI;
}
else if( j2array[0] < -IKPI )
{    j2array[0]+=IK2PI;
}
j2valid[0] = true;
if( j2array[1] > IKPI )
{
    j2array[1]-=IK2PI;
}
else if( j2array[1] < -IKPI )
{    j2array[1]+=IK2PI;
}
j2valid[1] = true;
if( j2valid[0] && j2valid[1] && IKabs(cj2array[0]-cj2array[1]) < 0.0001 && IKabs(sj2array[0]-sj2array[1]) < 0.0001 )
{
    j2valid[1]=false;
}
for(int ij2 = 0; ij2 < 2; ++ij2)
{
if( !j2valid[ij2] )
{
    continue;
}
j2 = j2array[ij2]; cj2 = cj2array[ij2]; sj2 = sj2array[ij2];

{
IKReal dummyeval[1];
IKReal gconst1;
gconst1=IKsign(((((-19.0000000000000)*((cj0)*(cj0))*((px)*(px))))+(((-38.0000000000000)*(cj0)*(px)*(py)*(sj0)))+(((-19.0000000000000)*((py)*(py))*((sj0)*(sj0))))+(((-19.0000000000000)*((pz)*(pz))))));
dummyeval[0]=((((-1.00000000000000)*((cj0)*(cj0))*((px)*(px))))+(((-1.00000000000000)*((pz)*(pz))))+(((-2.00000000000000)*(cj0)*(px)*(py)*(sj0)))+(((-1.00000000000000)*((py)*(py))*((sj0)*(sj0)))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j1array[1], cj1array[1], sj1array[1];
bool j1valid[1]={false};
j1array[0]=IKatan2(((gconst1)*(((((2.88750000000000)*(cj0)*(px)))+(((5.18700000000000)*(cj2)*(pz)*(sj3)))+(((-50.0000000000000)*(pp)*(py)*(sj0)))+(((3.79470000000000)*(cj3)*(py)*(sj0)))+(((2.88750000000000)*(py)*(sj0)))+(((-5.18700000000000)*(cj3)*(pz)*(sj2)))+(((3.79470000000000)*(cj0)*(cj3)*(px)))+(((-50.0000000000000)*(cj0)*(pp)*(px)))+(((-2.64100000000000)*(pz)*(sj2)))))), ((gconst1)*(((((-2.64100000000000)*(py)*(sj0)*(sj2)))+(((-2.64100000000000)*(cj0)*(px)*(sj2)))+(((-5.18700000000000)*(cj0)*(cj3)*(px)*(sj2)))+(((-2.88750000000000)*(pz)))+(((-5.18700000000000)*(cj3)*(py)*(sj0)*(sj2)))+(((50.0000000000000)*(pp)*(pz)))+(((-3.79470000000000)*(cj3)*(pz)))+(((5.18700000000000)*(cj0)*(cj2)*(px)*(sj3)))+(((5.18700000000000)*(cj2)*(py)*(sj0)*(sj3)))))));
sj1array[0]=IKsin(j1array[0]);
cj1array[0]=IKcos(j1array[0]);
if( j1array[0] > IKPI )
{
    j1array[0]-=IK2PI;
}
else if( j1array[0] < -IKPI )
{    j1array[0]+=IK2PI;
}
j1valid[0] = true;
for(int ij1 = 0; ij1 < 1; ++ij1)
{
if( !j1valid[ij1] )
{
    continue;
}
j1 = j1array[ij1]; cj1 = cj1array[ij1]; sj1 = sj1array[ij1];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(5);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.vfree.resize(0);
}
}
}

}

}
}
}

}

}

} else
{
{
IKReal j2array[2], cj2array[2], sj2array[2];
bool j2valid[2]={false};
IKReal x50=((0.103740000000000)*(cj3));
IKReal x51=((0.0528200000000000)+(x50));
IKReal x52=(sj3)*(sj3);
IKReal x53=((0.0107619876000000)*(x52));
IKReal x54=(x51)*(x51);
IKReal x55=((x54)+(x53));
if( (x55) < (IKReal)-0.00001 )
    continue;
IKReal x56=IKsqrt(x55);
IKReal x57=IKabs(x56);
IKReal x58=((IKabs(x57) != 0)?((IKReal)1/(x57)):(IKReal)1.0e30);
IKReal x59=((0.0758940000000000)*(cj3));
IKReal x60=((0.129950000000000)+(x59));
IKReal x61=((x60)+(((-1.00000000000000)*(pp))));
IKReal x62=((x58)*(x61));
if( (x62) < -1-IKFAST_SINCOS_THRESH || (x62) > 1+IKFAST_SINCOS_THRESH )
    continue;
IKReal x63=IKasin(x62);
IKReal x64=((0.103740000000000)*(sj3));
IKReal x65=IKatan2(x51, x64);
j2array[0]=((((-1.00000000000000)*(x63)))+(((-1.00000000000000)*(x65))));
sj2array[0]=IKsin(j2array[0]);
cj2array[0]=IKcos(j2array[0]);
j2array[1]=((3.14159265358979)+(x63)+(((-1.00000000000000)*(x65))));
sj2array[1]=IKsin(j2array[1]);
cj2array[1]=IKcos(j2array[1]);
if( j2array[0] > IKPI )
{
    j2array[0]-=IK2PI;
}
else if( j2array[0] < -IKPI )
{    j2array[0]+=IK2PI;
}
j2valid[0] = true;
if( j2array[1] > IKPI )
{
    j2array[1]-=IK2PI;
}
else if( j2array[1] < -IKPI )
{    j2array[1]+=IK2PI;
}
j2valid[1] = true;
if( j2valid[0] && j2valid[1] && IKabs(cj2array[0]-cj2array[1]) < 0.0001 && IKabs(sj2array[0]-sj2array[1]) < 0.0001 )
{
    j2valid[1]=false;
}
for(int ij2 = 0; ij2 < 2; ++ij2)
{
if( !j2valid[ij2] )
{
    continue;
}
j2 = j2array[ij2]; cj2 = cj2array[ij2]; sj2 = sj2array[ij2];

{
IKReal dummyeval[1];
IKReal gconst1;
gconst1=IKsign(((((-19.0000000000000)*((cj0)*(cj0))*((px)*(px))))+(((-38.0000000000000)*(cj0)*(px)*(py)*(sj0)))+(((-19.0000000000000)*((py)*(py))*((sj0)*(sj0))))+(((-19.0000000000000)*((pz)*(pz))))));
dummyeval[0]=((((-1.00000000000000)*((cj0)*(cj0))*((px)*(px))))+(((-1.00000000000000)*((pz)*(pz))))+(((-2.00000000000000)*(cj0)*(px)*(py)*(sj0)))+(((-1.00000000000000)*((py)*(py))*((sj0)*(sj0)))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j1array[1], cj1array[1], sj1array[1];
bool j1valid[1]={false};
j1array[0]=IKatan2(((gconst1)*(((((2.88750000000000)*(cj0)*(px)))+(((5.18700000000000)*(cj2)*(pz)*(sj3)))+(((-50.0000000000000)*(pp)*(py)*(sj0)))+(((3.79470000000000)*(cj3)*(py)*(sj0)))+(((2.88750000000000)*(py)*(sj0)))+(((-5.18700000000000)*(cj3)*(pz)*(sj2)))+(((3.79470000000000)*(cj0)*(cj3)*(px)))+(((-50.0000000000000)*(cj0)*(pp)*(px)))+(((-2.64100000000000)*(pz)*(sj2)))))), ((gconst1)*(((((-2.64100000000000)*(py)*(sj0)*(sj2)))+(((-2.64100000000000)*(cj0)*(px)*(sj2)))+(((-5.18700000000000)*(cj0)*(cj3)*(px)*(sj2)))+(((-2.88750000000000)*(pz)))+(((-5.18700000000000)*(cj3)*(py)*(sj0)*(sj2)))+(((50.0000000000000)*(pp)*(pz)))+(((-3.79470000000000)*(cj3)*(pz)))+(((5.18700000000000)*(cj0)*(cj2)*(px)*(sj3)))+(((5.18700000000000)*(cj2)*(py)*(sj0)*(sj3)))))));
sj1array[0]=IKsin(j1array[0]);
cj1array[0]=IKcos(j1array[0]);
if( j1array[0] > IKPI )
{
    j1array[0]-=IK2PI;
}
else if( j1array[0] < -IKPI )
{    j1array[0]+=IK2PI;
}
j1valid[0] = true;
for(int ij1 = 0; ij1 < 1; ++ij1)
{
if( !j1valid[ij1] )
{
    continue;
}
j1 = j1array[ij1]; cj1 = cj1array[ij1]; sj1 = sj1array[ij1];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(5);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.vfree.resize(0);
}
}
}

}

}
}
}

}

}
}
}
}
return vsolutions.size()>0;
}
};


/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
IKSolver solver;
return solver.ik(eetrans,eerot,pfree,vsolutions);
}

IKFAST_API const char* getKinematicsHash() { return "afe50514bf09aff5f2a84beb078bafbd"; }

#ifdef IKFAST_NAMESPACE
} // end namespace
#endif

#ifndef IKFAST_NO_MAIN
#include <stdio.h>
#include <stdlib.h>
#ifdef IKFAST_NAMESPACE
using namespace IKFAST_NAMESPACE;
#endif
int main(int argc, char** argv)
{
    if( argc != 12+getNumFreeParameters()+1 ) {
        printf("\nUsage: ./ik r00 r01 r02 t0 r10 r11 r12 t1 r20 r21 r22 t2 free0 ...\n\n"
               "Returns the ik solutions given the transformation of the end effector specified by\n"
               "a 3x3 rotation R (rXX), and a 3x1 translation (tX).\n"
               "There are %d free parameters that have to be specified.\n\n",getNumFreeParameters());
        return 1;
    }

    std::vector<IKSolution> vsolutions;
    std::vector<IKReal> vfree(getNumFreeParameters());
    IKReal eerot[9],eetrans[3];
    eerot[0] = atof(argv[1]); eerot[1] = atof(argv[2]); eerot[2] = atof(argv[3]); eetrans[0] = atof(argv[4]);
    eerot[3] = atof(argv[5]); eerot[4] = atof(argv[6]); eerot[5] = atof(argv[7]); eetrans[1] = atof(argv[8]);
    eerot[6] = atof(argv[9]); eerot[7] = atof(argv[10]); eerot[8] = atof(argv[11]); eetrans[2] = atof(argv[12]);
    for(std::size_t i = 0; i < vfree.size(); ++i)
        vfree[i] = atof(argv[13+i]);
    bool bSuccess = ik(eetrans, eerot, vfree.size() > 0 ? &vfree[0] : NULL, vsolutions);

    if( !bSuccess ) {
        fprintf(stderr,"Failed to get ik solution\n");
        return -1;
    }

    printf("Found %d ik solutions:\n", (int)vsolutions.size());
    std::vector<IKReal> sol(getNumJoints());
    for(std::size_t i = 0; i < vsolutions.size(); ++i) {
        printf("sol%d (free=%d): ", (int)i, (int)vsolutions[i].GetFree().size());
        std::vector<IKReal> vsolfree(vsolutions[i].GetFree().size());
        vsolutions[i].GetSolution(&sol[0],vsolfree.size()>0?&vsolfree[0]:NULL);
        for( std::size_t j = 0; j < sol.size(); ++j)
            printf("%.15f, ", sol[j]);
        printf("\n");
    }
    return 0;
}

#endif

#include "ikbase.h"
namespace IKFAST_NAMESPACE {
#ifdef RAVE_REGISTER_BOOST
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(IKSolution)
#endif
IkSolverBasePtr CreateIkSolver(EnvironmentBasePtr penv, const std::vector<dReal>& vfreeinc) {
    std::vector<int> vfree(getNumFreeParameters());
    for(size_t i = 0; i < vfree.size(); ++i) {
        vfree[i] = getFreeParameters()[i];
    }
    return IkSolverBasePtr(new IkFastSolver<IKReal,IKSolution>(ik,vfree,vfreeinc,getNumJoints(),(IkParameterization::Type)getIKType(), boost::shared_ptr<void>(), getKinematicsHash(), penv));
}
} // end namespace
