#include<atcoder/segtree>
using namespace atcoder;

/* S_type:セグ木で使う型 */
using S_type = ll;
/* op:セグ木に載せたい演算 */
S_type op(S_type a, S_type b) {return a + b;}
/* e:opを用いてもデータを変化させない値 */
S_type e(void) {return 0;}

/**
 * @brief Heavy-Light Decomposition (重軽分解)
 *
 * 頂点は build() 後に [0, n) へ並べ替えられる
 * 部分木と、パスを分解した各区間はいずれも半開区間 [l, r)
 */
class HLD {
 public:
	struct Segment {
		int l, r;			 // 常に l < r の半開区間
		bool reversed;	// パス上では r-1 -> l の順に読むなら true
	};

	explicit HLD(int n)
			: parent(n, -1), depth(n), subtree_size(n), heavy_child(n, -1),
				head(n), in(n), out(n), vertex_at(n), n_(n), graph_(n),
				seg_(n), reverse_seg_(n) {}

	void add_edge(int u, int v) {
		assert(0 <= u && u < n_ && 0 <= v && v < n_);
		graph_[u].push_back(v);
		graph_[v].push_back(u);
	}

	void build(int root = 0) {
		assert(0 <= root && root < n_);
		timer_ = 0;
		depth[root] = 0;
		dfs_size(root, -1);
		dfs_hld(root, root);
		seg_ = segtree<S_type, op, e>(n_);
		reverse_seg_ = segtree<S_type, op, e>(n_);
	}

	// values[v] を頂点 v の初期値として構築する。
	void build(const std::vector<S_type> &values, int root = 0) {
		assert(static_cast<int>(values.size()) == n_);
		build(root);
		std::vector<S_type> ordered(n_), reversed(n_);
		for (int v = 0; v < n_; ++v) ordered[in[v]] = values[v];
		for (int i = 0; i < n_; ++i) reversed[n_ - 1 - i] = ordered[i];
		seg_ = segtree<S_type, op, e>(ordered);
		reverse_seg_ = segtree<S_type, op, e>(reversed);
	}

	// 頂点 v の値を x に変更する。
	void set_vertex(int v, S_type x) {
		assert(0 <= v && v < n_);
		seg_.set(in[v], x);
		reverse_seg_.set(n_ - 1 - in[v], x);
	}

	S_type get_vertex(int v) const {
		assert(0 <= v && v < n_);
		return seg_.get(in[v]);
	}

	/**
	 * u -> v の順でパス上の値を op で畳み込む。
	 * edge=true の場合、辺の値は深い側の頂点に格納しておく。
	 * 計算量: O(log^2 N)
	 */
	S_type path_prod(int u, int v, bool edge = false) const {
		S_type answer = e();
		for (const auto &s : path(u, v, edge)) {
			S_type value = s.reversed
					? reverse_seg_.prod(n_ - s.r, n_ - s.l)
					: seg_.prod(s.l, s.r);
			answer = op(answer, value);
		}
		return answer;
	}

	// v の部分木を in-order 順に畳み込む。
	S_type subtree_prod(int v, bool edge = false) const {
		auto [l, r] = subtree(v, edge);
		return seg_.prod(l, r);
	}

	int lca(int u, int v) const {
		while (head[u] != head[v]) {
			if (depth[head[u]] > depth[head[v]])
				u = parent[head[u]];
			else
				v = parent[head[v]];
		}
		return depth[u] < depth[v] ? u : v;
	}

	int dist(int u, int v) const {
		int w = lca(u, v);
		return depth[u] + depth[v] - 2 * depth[w];
	}

	// v の k 個上の祖先。存在しなければ -1。
	int ancestor(int v, int k) const {
		if (k < 0 || depth[v] < k) return -1;
		while (k > depth[v] - depth[head[v]]) {
			k -= depth[v] - depth[head[v]] + 1;
			v = parent[head[v]];
		}
		return vertex_at[in[v] - k];
	}

	// u から v へ向かうパス上の k 番目 (0-indexed) の頂点。
	int kth_vertex(int u, int v, int k) const {
		int w = lca(u, v);
		int up = depth[u] - depth[w];
		int down = depth[v] - depth[w];
		if (k < 0 || k > up + down) return -1;
		return k <= up ? ancestor(u, k) : ancestor(v, up + down - k);
	}

	// v を根とする部分木に対応する区間。
	std::pair<int, int> subtree(int v, bool edge = false) const {
		return {in[v] + static_cast<int>(edge), out[v]};
	}

	/**
	 * u -> v の順にパスを区間へ分解する。
	 * edge=false: 頂点列、edge=true: 各辺を「深い側の頂点」に対応させた辺列。
	 * 非可換演算では reversed を見て、区間内を逆順に集約すること。
	 */
	std::vector<Segment> path(int u, int v, bool edge = false) const {
		std::vector<Segment> left, right;
		while (head[u] != head[v]) {
			if (depth[head[u]] > depth[head[v]]) {
				left.push_back({in[head[u]], in[u] + 1, true});
				u = parent[head[u]];
			} else {
				right.push_back({in[head[v]], in[v] + 1, false});
				v = parent[head[v]];
			}
		}
		if (depth[u] > depth[v]) {
			int l = in[v] + static_cast<int>(edge);
			if (l < in[u] + 1) left.push_back({l, in[u] + 1, true});
		} else {
			int l = in[u] + static_cast<int>(edge);
			if (l < in[v] + 1) right.push_back({l, in[v] + 1, false});
		}
		std::reverse(right.begin(), right.end());
		left.insert(left.end(), right.begin(), right.end());
		return left;
	}

	// 可換な区間演算用。f(l, r) を O(log N) 回呼ぶ。
	template <class F>
	void for_each_path(int u, int v, F f, bool edge = false) const {
		for (const auto &s : path(u, v, edge)) f(s.l, s.r);
	}

	int size() const { return n_; }

	std::vector<int> parent, depth, subtree_size, heavy_child;
	std::vector<int> head, in, out, vertex_at;

	private:
	int n_, timer_ = 0;
	std::vector<std::vector<int>> graph_;
	segtree<S_type, op, e> seg_, reverse_seg_;

	void dfs_size(int v, int p) {
		parent[v] = p;
		subtree_size[v] = 1;
		heavy_child[v] = -1;
		for (int to : graph_[v]) {
			if (to == p) continue;
			depth[to] = depth[v] + 1;
			dfs_size(to, v);
			subtree_size[v] += subtree_size[to];
			if (heavy_child[v] == -1 ||
					subtree_size[to] > subtree_size[heavy_child[v]])
				heavy_child[v] = to;
		}
	}

	void dfs_hld(int v, int h) {
		head[v] = h;
		in[v] = timer_;
		vertex_at[timer_++] = v;
		if (heavy_child[v] != -1) dfs_hld(heavy_child[v], h);
		for (int to : graph_[v]) {
			if (to == parent[v] || to == heavy_child[v]) continue;
			dfs_hld(to, to);
		}
		out[v] = timer_;
	}
};