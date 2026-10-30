#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using i128 = __int128_t;
#define rep(i, N) for(ll i = 0; i < N; i++)

// [lo, hi] から一様ランダムに整数を返す。
template <class T>
T random_int(mt19937_64& rng, T lo, T hi) {
	return uniform_int_distribution<T>(lo, hi)(rng);
}

int main(int argc, char* argv[]) {
	// test.sh から seed が渡される。同じ seed を指定すれば再現できる。
	const uint64_t seed =
		argc >= 2 ? stoull(argv[1]) : random_device{}();
	mt19937_64 rng(seed);

	// 問題に合わせて、ここを入力形式どおりに書き換える。
	// 以下は「N」と「長さ N の整数列」を生成する例。
	int N=random_int(rng, 30, 30);
	int W=100;
	int C=10;
	cout<<N<<" "<<W<<" "<<C<<endl;
	rep(i,N){
		int w=random_int(rng,1,100);
		int v=random_int(rng,1,100);
		int c=random_int(rng,1,10);
		cout<<w<<" "<<v<<" "<<c<<endl;
	}
}
