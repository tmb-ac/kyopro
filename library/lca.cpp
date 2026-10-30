#include<bits/stdc++.h>
using namespace std;
#define rep(i,N) for(i=0;i<N;i++)
#define ll long long

template <typename T = int>
struct Edge {
	int from, to;
	T cost;
	int idx;

	Edge() = default;

	Edge(int from, int to, T cost = 1, int idx = -1)
			: from(from), to(to), cost(cost), idx(idx) {}

	operator int() const { return to; }
};

template <typename T = int>
struct Graph {
	vector<vector<Edge<T> > > g;
	int es;

	Graph() = default;

	explicit Graph(int n) : g(n), es(0) {}

	size_t size() const { return g.size(); }

	void add_directed_edge(int from, int to, T cost = 1) {
		g[from].emplace_back(from, to, cost, es++);
	}

	void add_edge(int from, int to, T cost = 1) {
		g[from].emplace_back(from, to, cost, es);
		g[to].emplace_back(to, from, cost, es++);
	}

	void read(int M, int padding = -1, bool weighted = false,
						bool directed = false) {
		for (int i = 0; i < M; i++) {
			int a, b;
			cin >> a >> b;
			a += padding;
			b += padding;
			T c = T(1);
			if (weighted) cin >> c;
			if (directed)
				add_directed_edge(a, b, c);
			else
				add_edge(a, b, c);
		}
	}

	inline vector<Edge<T> > &operator[](const int &k) { return g[k]; }

	inline const vector<Edge<T> > &operator[](const int &k) const { return g[k]; }
};

template <typename T = int>
using Edges = vector<Edge<T> >;

/**
 * @brief Doubling-Lowest-Common-Ancestor(最小共通祖先)
 *
 */
template <typename T>
struct DoublingLowestCommonAncestor : Graph<T> {
 public:
	using Graph<T>::g;
	vector<int> dep;
	vector<T> sum;
	vector<vector<int> > table;
	const int LOG;

	explicit DoublingLowestCommonAncestor(int n)
			: Graph<T>(n), LOG(32 - __builtin_clz(g.size())) {}

	explicit DoublingLowestCommonAncestor(const Graph<T> &g)
			: LOG(32 - __builtin_clz(g.size())), Graph<T>(g) {}

	void build(int root = 0) {
		dep.assign(g.size(), 0);
		sum.assign(g.size(), 0);
		table.assign(LOG, vector<int>(g.size(), -1));
		dfs(root, -1, 0);
		for (int k = 0; k + 1 < LOG; k++) {
			for (int i = 0; i < (int)table[k].size(); i++) {
				if (table[k][i] == -1)
					table[k + 1][i] = -1;
				else
					table[k + 1][i] = table[k][table[k][i]];
			}
		}
	}

	int lca(int u, int v) {
		if (dep[u] > dep[v]) swap(u, v);
		v = climb(v, dep[v] - dep[u]);
		if (u == v) return u;
		for (int i = LOG - 1; i >= 0; i--) {
			if (table[i][u] != table[i][v]) {
				u = table[i][u];
				v = table[i][v];
			}
		}
		return table[0][u];
	}

	int climb(int u, int k) {
		if (dep[u] < k) return -1;
		for (int i = LOG - 1; i >= 0; i--) {
			if ((k >> i) & 1) u = table[i][u];
		}
		return u;
	}

	T dist(int u, int v) { return sum[u] + sum[v] - 2 * sum[lca(u, v)]; }

 private:
	void dfs(int idx, int par, int d) {
		table[0][idx] = par;
		dep[idx] = d;
		for (auto &to : g[idx]) {
			if (to != par) {
				sum[to] = sum[idx] + to.cost;
				dfs(to, idx, d + 1);
			}
		}
	}
};

// ABC045 - F
// https://atcoder.jp/contests/abc405/tasks/abc405_f

// Graphに親->子の有効辺を追加し，LCAを呼び出す．
// distによってダブリングLCAを計算できる．
int main(void){
	ll N,M;
	ll A[200009];
	ll B[200009];
	ll Q;
	ll C[200009];
	ll D[200009];

	ll i,j,k;

	cin>>N>>M;
	for(i=1;i<=M;i++)cin>>A[i]>>B[i];
	cin>>Q;
	for(i=1;i<=Q;i++)cin>>C[i]>>D[i];
	
	vector<pair<ll,ll>>P;
	for(i=1;i<=M;i++){
		P.push_back({A[i],i});
		P.push_back({B[i],i});
	}
	for(i=1;i<=Q;i++){
		P.push_back({C[i],i});
		P.push_back({D[i],i});
	}
	sort(P.begin(),P.end());
	
	Graph<ll> g(N+1);

	ll point[200009][2];
	ll cnt[200009]={0};
	stack<ll>s;
	s.push(0);
	for(auto p:P){
		if(p.first%2==0){
			if(s.top()==p.second)s.pop();
			else{
				g.add_directed_edge(s.top(),p.second);
				s.push(p.second);
			}
		}else{
			point[p.second][cnt[p.second]]=s.top();
			cnt[p.second]++;
		}
	}

	DoublingLowestCommonAncestor<ll>lca(g);
	lca.build();
	for(i=1;i<=Q;i++){
		cout<<lca.dist(point[i][0],point[i][1])<<endl;
	}

	return 0;
}