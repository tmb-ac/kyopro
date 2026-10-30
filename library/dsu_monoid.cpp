#include <bits/stdc++.h>
using namespace std;
#define rep(i, N) for (i = 0; i < N; i++)
#define ll long long

// モノイドを載せたDSU
// S: モノイドの型
// op(a, b): 二項演算
// e: 単位元
template <class S, S (*op)(S, S), S (*e)()> class dsu_monoid {
private:
	ll N;								// ノード数
	vector<ll> P;				// 親
	vector<ll> S_size;	 // 連結成分のサイズ
	vector<S> values;		// 各ノードのモノイド値
	vector<S> group_val; // 各連結成分のモノイド値（根のみ有効）

	ll root(ll now) {
		if (now != P[now])
			P[now] = root(P[now]);
		return P[now];
	}

public:
	// コンストラクタ: nノード、初期値は単位元
	dsu_monoid(ll n)
			: N(n), P(n + 1), S_size(n + 1, 1), values(n + 1, e()),
				group_val(n + 1, e()) {
		for (ll i = 0; i <= N; i++)
			P[i] = i;
	}

	// コンストラクタ: nノード、各ノードの初期値を指定
	dsu_monoid(ll n, const vector<S> &init_vals)
			: N(n), P(n + 1), S_size(n + 1, 1), values(n + 1), group_val(n + 1) {
		for (ll i = 0; i <= N; i++) {
			P[i] = i;
			if (i <= n && i < (ll)init_vals.size()) {
				values[i] = init_vals[i];
				group_val[i] = init_vals[i];
			} else {
				values[i] = e();
				group_val[i] = e();
			}
		}
	}

	// a, bを連結する（モノイド値もマージ）
	void merge(ll a, ll b) {
		ll ra = root(a);
		ll rb = root(b);
		if (ra == rb)
			return;

		// 小さい方を大きい方につける
		if (S_size[ra] > S_size[rb])
			swap(ra, rb);

		P[ra] = rb;
		S_size[rb] += S_size[ra];
		// モノイド値をマージ
		group_val[rb] = op(group_val[rb], group_val[ra]);
	}

	// a, bの連結判定
	bool same(ll a, ll b) { return root(a) == root(b); }

	// aの連結成分のsizeを返す
	ll size(ll a) { return S_size[root(a)]; }

	// aの連結成分のモノイド値を取得
	S get(ll a) { return group_val[root(a)]; }

	// ノードaのモノイド値を設定（連結成分の値も更新）
	void set(ll a, S x) {
		ll r = root(a);
		// 古い値を引いて新しい値を足す（モノイドに逆元がない場合は再計算が必要）
		// ここでは簡易的に、rootでない場合は再計算
		S old_val = values[a];
		values[a] = x;

		if (a == r) {
			// 根の場合は直接更新
			group_val[r] = op(op(group_val[r], op(e(), x)), e()); // これは不正確
			// より正確には全体を再計算する必要がある
			recalculate(r);
		} else {
			// 非根の場合は根を再計算
			recalculate(r);
		}
	}

	// 連結成分全体のモノイド値を再計算（set用の補助関数）
	void recalculate(ll r) {
		S val = e();
		for (ll i = 1; i <= N; i++) {
			if (root(i) == r) {
				val = op(val, values[i]);
			}
		}
		group_val[r] = val;
	}

	// 全ての連結成分を列挙(1-indexed)
	vector<vector<ll>> groups() {
		map<ll, vector<ll>> m;
		vector<vector<ll>> ret;
		for (ll i = 1; i <= N; i++)
			m[root(i)].push_back(i);
		for (auto elem : m)
			ret.push_back(elem.second);
		return ret;
	}
};

// 使用例: 整数の和
ll op_sum(ll a, ll b) { return a + b; }
ll e_sum() { return 0; }

// 使用例: 整数の最大値
ll op_max(ll a, ll b) { return max(a, b); }
ll e_max() { return LLONG_MIN; }

// 使用例: 整数の最小値
ll op_min(ll a, ll b) { return min(a, b); }
ll e_min() { return LLONG_MAX; }

int main() {
	// 使用例1: 整数の和を保持するDSU
	ll n = 5;
	vector<ll> vals = {0, 10, 20, 30, 40, 50}; // 0番目はダミー
	dsu_monoid<ll, op_sum, e_sum> d1(n, vals);

	cout << "初期状態:" << endl;
	for (ll i = 1; i <= n; i++) {
		cout << "Node " << i << " の連結成分の和: " << d1.get(i) << endl;
	}

	// 1と2をマージ
	d1.merge(1, 2);
	cout << "\n1と2をマージ後:" << endl;
	cout << "Node 1 の連結成分の和: " << d1.get(1) << endl; // 10 + 20 = 30
	cout << "Node 2 の連結成分の和: " << d1.get(2) << endl; // 30

	// 3と4をマージ
	d1.merge(3, 4);
	cout << "\n3と4をマージ後:" << endl;
	cout << "Node 3 の連結成分の和: " << d1.get(3) << endl; // 30 + 40 = 70

	// 1の連結成分と3の連結成分をマージ
	d1.merge(1, 3);
	cout << "\n1と3の連結成分をマージ後:" << endl;
	cout << "Node 1 の連結成分の和: " << d1.get(1)
			 << endl; // 10 + 20 + 30 + 40 = 100

	// 使用例2: 整数の最大値を保持するDSU
	dsu_monoid<ll, op_max, e_max> d2(n, vals);
	d2.merge(1, 2);
	d2.merge(3, 4);
	cout << "\n最大値DSU:" << endl;
	cout << "Node 1 の連結成分の最大値: " << d2.get(1)
			 << endl; // max(10, 20) = 20
	cout << "Node 3 の連結成分の最大値: " << d2.get(3)
			 << endl; // max(30, 40) = 40

	return 0;
}
