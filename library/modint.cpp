#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

ll mod=998'244'353;//ll mod=1'000'000'007;
class mint{
private:
    ll v;
public:
    /*初期化*/
    mint(ll value=0){
        if(value<0){
            value*=-1;value%=mod;value*=-1;
            while(value<0)value+=mod;
        }
        v=value%mod;
    }
    /*演算子オーバーロード*/
    bool operator==(mint a){return v==a.val();}
    bool operator!=(mint a){return v!=a.val();}
    bool operator==(ll a){return v==a;}
    bool operator!=(ll a){return v!=a;}

    mint operator++(void){return v==mod-1 ? v=0 : v++;}
    mint operator--(void){return v==0 ? v=mod-1 : v--;}

    mint operator++(int){mint ret=*this; ++*this; return ret;}
    mint operator--(int){mint ret=*this; --*this; return ret;}

    mint operator+(mint a){return mod<=v+a.v ? v+a.v-mod : v+a.v;}
    mint operator-(mint a){return v-a.v<0 ? v-a.v+mod : v-a.v;}
    mint operator*(mint a){return (v*a.v)%mod;}
    mint operator/(mint a){
        for(int i=0;(1LL<<i)<(mod-2);i++){
            if(((mod-2)&(1LL<<i))!=0)*this=*this*a;
            a=a*a;
        }
        return v;
    }

    mint operator+(ll a){mint aa=a%mod; return *this+aa;}
    mint operator-(ll a){mint aa=a%mod; return *this-aa;}
    mint operator*(ll a){mint aa=a%mod; return *this*aa;}
    mint operator/(ll a){mint aa=a%mod; return *this/aa;}

    mint operator+=(mint a){*this=*this+a; return v;}
    mint operator-=(mint a){*this=*this-a; return v;}
    mint operator*=(mint a){*this=*this*a; return v;}
    mint operator/=(mint a){*this=*this/a; return v;}

    mint operator+=(ll a){*this=*this+a; return v;}
    mint operator-=(ll a){*this=*this-a; return v;}
    mint operator*=(ll a){*this=*this*a; return v;}
    mint operator/=(ll a){*this=*this/a; return v;}

    /*値を返す*/
    ll val(void){return v;}
    /*K乗の値を繰り返し二乗法で求める*/
    mint pow(ll K){
        mint a=*this;
        mint ret=1;
        for(int i=0;(1LL<<i)<=K;i++){
            if((K&(1LL<<i))!=0)ret=ret*a;
            a=a*a;
        }
        return ret;
    }
    /*逆元を求める*/
    mint inv(void){return this->pow(mod-2);}
};
mint operator+(ll a,mint b){mint aa=a;return aa+b;}
mint operator-(ll a,mint b){mint aa=a;return aa-b;}
mint operator*(ll a,mint b){mint aa=a;return aa*b;}
mint operator/(ll a,mint b){mint aa=a;return aa/b;}

int main(void){
    ll N;
    ll A[200009];

    ll i,j,k;

    return 0;
}