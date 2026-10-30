#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using i128 = __int128_t;
#define rep(i, N) for(ll i = 0; i < N; i++)

int main(void) {
	ll N,W,C;
	ll w[109];
	ll v[109];
	ll c[109];

	cin>>N>>W>>C;
	rep(i,N)cin>>w[i]>>v[i]>>c[i];

	ll dp[59][10009]={0};
	rep(i,59)rep(j,10009)dp[i][j]=INT64_MIN;
	dp[0][0]=0;
	for(ll col=1;col<=50;col++){
		ll sdp[10009];
		rep(i,10009)sdp[i]=INT64_MIN;
		sdp[0]=0;
		rep(i,N){
			if(c[i]!=col)continue;
			for(ll j=W;0<=j;j--){
				if(W<j+w[i])continue;
				sdp[j+w[i]]=max(sdp[j+w[i]],sdp[j]+v[i]);
			}
		}

		for(ll i=min(col,C)-1;0<=i;i--){
			for(ll j=W;0<=j;j--){
				rep(k,W+1){
					if(sdp[k]<0)continue;
					if(W<j+k)break;
					dp[i+1][j+k]=max(dp[i+1][j+k],dp[i][j]+sdp[k]);
				}
			}
		}
	}

	ll ans=0;
	rep(i,59)rep(j,10009)ans=max(ans,dp[i][j]);
	cout<<ans<<endl;

	return 0;
}