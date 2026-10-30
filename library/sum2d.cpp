#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

class sum2d{
private:
    ll x,y;
    vector<vector<ll>>sum;
public:
    sum2d(vector<vector<ll>>a,ll dim1,ll dim2){
        x=dim1;y=dim2;sum=a;
        ll i,j;
        rep(i,x)rep(j,y+1)sum[i+1][j]+=sum[i][j];
        rep(i,x+1)rep(j,y)sum[i][j+1]+=sum[i][j];
    }
    ll get_sum(ll x_l,ll x_r,ll y_l,ll y_r){
        if(x_l<=0||x<x_r)return INT64_MAX;
        if(y_l<=0||y<y_r)return INT64_MAX;
        ll ret=0;
        ret+=sum[x_r  ][y_r  ];
        ret-=sum[x_l-1][y_r  ];
        ret-=sum[x_r  ][y_l-1];
        ret-=sum[x_l-1][y_l-1];
        return ret;
    }
};

int main(void){
    ll N,Q;
    ll query[200009][4];

    ll i,j,k;

    cin>>N;

    ll dim1=N,dim2=N;
    vector<vector<ll>>A(dim1+1,vector<ll>(dim2+1,0));

    rep(i,N)rep(j,N)cin>>A[i+1][j+1];
    cin>>Q;
    rep(i,Q)rep(j,4)cin>>query[i][j];

    sum2d sum(A,dim1,dim2);

    rep(i,Q){
        cout<<sum.get_sum(query[i][0],query[i][1],query[i][2],query[i][3])<<endl;
    }

    return 0;
}