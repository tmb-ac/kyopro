#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

class dsu{
private:
    ll N;/*ノード数*/
    vector<ll>P;/*親*/
    vector<ll>S;/*連結成分のサイズ*/
    ll root(ll now){
        if(now!=P[now])P[now]=root(P[now]);
        return P[now];
    }
	vector<ll>L;
public:
    dsu(ll n){
        N=n;
        P=vector<ll>(N+1);
        for(ll i=0;i<=N;i++)P[i]=i;
        S=vector<ll>(N+1,1);
        L=vector<ll>(N+1);
        for(ll i=0;i<=N;i++)L[i]=i;
    }
    /*a,bを連結する*/
    void merge(ll a,ll b,bool is_a_leader=true){
        a=root(a);b=root(b);
        if(S[a]<=S[b]){
            P[b]=a;
            S[a]+=S[b];
        }else{
            P[a]=b;
            S[b]+=S[a];
        }
		if(is_a_leader)L[b]=L[a];
		else L[a]=L[b];
    }
    /*a,bの連結判定*/
    bool same(ll a,ll b){
        a=root(a);b=root(b);
        return a==b; 
    }
    /*aの連結成分のsizeを返す*/
    ll size(ll a){return S[root(a)];}
    /*aの連結成分の代表を返す*/
    ll leader(ll a){return L[root(a)];}
    /*全ての連結成分を列挙(1-indexed)*/
    vector<vector<ll>> groups(void){
        map<ll,vector<ll>>m;
        vector<vector<ll>>ret;
        for(ll i=1;i<=N;i++)m[root(i)].push_back(i);
        for(auto e:m)ret.push_back(e.second);
        return ret;
    }
};

int main(void){
    ll N,Q;
    ll T[200009];
    ll U[200009];
    ll V[200009];

    ll i,j,k;

    cin>>N>>Q;
    rep(i,Q)cin>>T[i]>>U[i]>>V[i];

    dsu d(N);
    rep(i,Q){
        if(T[i]==0)d.merge(U[i],V[i]);
        else cout<<d.same(U[i],V[i])<<endl;
    }

    return 0;
}