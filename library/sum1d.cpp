#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

class sum1d{
private:
    ll x;
    vector<ll>sum;
public:
    sum1d(vector<ll>a,ll dim1){
        x=dim1;sum=a;
        ll i,j;
        rep(i,x)sum[i+1]+=sum[i];
    }
    ll get_sum(ll x_l,ll x_r){
        if(x_l<=0||x<x_r)return INT64_MAX;
        ll ret=0;
        ret+=sum[x_r  ];
        ret-=sum[x_l-1];
        return ret;
    }
};

int main(void){
    ll N,Q;
    ll query[200009][4];

    ll i,j,k;

    cin>>N;

    ll dim1=N;
    vector<ll>A(dim1+1,0);

    rep(i,N)cin>>A[i+1];
    cin>>Q;
    rep(i,Q)rep(j,2)cin>>query[i][j];

    sum1d sum(A,dim1);

    rep(i,Q){
        cout<<sum.get_sum(query[i][0],query[i][1])<<endl;
    }

    return 0;
}