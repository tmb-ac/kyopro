#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

ll mod=998'244'353;
bool take_mod=1;

using type = ll;
#define v(type) vector<type>
#define vv(type) vector<v(type)>
/*N次正方行列の積 B=A×X^Kを求める*/
vv(type) matrix_power(vv(type) A,vv(type) X,ll N,ll K){
    ll i,j,k;
    vv(type)B_now=A;
    vv(type)B_next(N,v(type)(N));
    vv(type)X_now=X;
    vv(type)X_next(N,v(type)(N));
    for(;K!=0;K/=2){
        if(K%2){
            rep(i,N)rep(j,N){
                B_next[i][j]=0;
                rep(k,N){
                    B_next[i][j]+=B_now[i][k]*X_now[k][j];
                    if(take_mod)B_next[i][j]%=mod;
                }
            }
            swap(B_now,B_next);
        }
        rep(i,N)rep(j,N){
            X_next[i][j]=0;
            rep(k,N){
                X_next[i][j]+=X_now[i][k]*X_now[k][j];
                if(take_mod)X_next[i][j]%=mod;
            }
        }
        swap(X_now,X_next);
    }
    return B_now;
}


int main(void){
    ll N,K;

    ll i,j;

    cin>>N>>K;

    vv(type) A(N,v(type)(N,0));
    vv(type) E(N,v(type)(N,0));

    rep(i,N)rep(j,N)cin>>A[i][j];
    rep(i,N)E[i][i]=1;

    /*B=E×A^K  E,A:N×N正方行列*/
    vector<vector<type>>B=matrix_power(E,A,N,K);

    rep(i,N){
        rep(j,N)cout<<B[i][j]<<' ';
        cout<<endl;
    }

    return 0;
}