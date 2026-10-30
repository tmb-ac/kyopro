#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

class sum3d{
private:
    ll x,y,z;
    vector<vector<vector<ll>>>sum;
public:
    sum3d(vector<vector<vector<ll>>>a,ll dim1,ll dim2,ll dim3){
        x=dim1;y=dim2;z=dim3;sum=a;
        ll i,j,k;
        rep(i,x)rep(j,y+1)rep(k,z+1)sum[i+1][j][k]+=sum[i][j][k];
        rep(i,x+1)rep(j,y)rep(k,z+1)sum[i][j+1][k]+=sum[i][j][k];
        rep(i,x+1)rep(j,y+1)rep(k,z)sum[i][j][k+1]+=sum[i][j][k];
    }
    ll get_sum(ll xl,ll xr,ll yl,ll yr,ll zl,ll zr){
        if(xl<=0||x<xr)return INT64_MAX;
        if(yl<=0||y<yr)return INT64_MAX;
        if(zl<=0||z<zr)return INT64_MAX;
        ll ret=0;
        ret+=sum[xr  ][yr  ][zr  ];
        ret-=sum[xl-1][yr  ][zr  ];
        ret-=sum[xr  ][yl-1][zr  ];
        ret-=sum[xr  ][yr  ][zl-1];
        ret+=sum[xl-1][yl-1][zr  ];
        ret+=sum[xr  ][yl-1][zl-1];
        ret+=sum[xl-1][yr  ][zl-1];
        ret-=sum[xl-1][yl-1][zl-1];
        return ret;
    }
};

int main(void){
    ll N,Q;
    ll query[200009][6];

    ll i,j,k;

    cin>>N;

    ll dim1=N,dim2=N,dim3=N;
    vector<vector<vector<ll>>>A(dim1+1,vector<vector<ll>>(dim2+1,vector<ll>(dim3+1,0)));

    rep(i,N)rep(j,N)rep(k,N)cin>>A[i+1][j+1][k+1];
    cin>>Q;
    rep(i,Q)rep(j,6)cin>>query[i][j];

    sum3d sum(A,dim1,dim2,dim3);

    rep(i,Q){
        cout<<sum.get_sum(query[i][0],query[i][1],query[i][2],query[i][3],query[i][4],query[i][5])<<endl;
    }

    return 0;
}