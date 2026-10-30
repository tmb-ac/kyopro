// 長方形に覆われた面積を求める

#include <bits/stdc++.h>
#include <atcoder/lazysegtree>
using namespace std;
using namespace atcoder;
#define rep(i, N) for (i = 0; i < N; i++)
#define ll long long

#define RECT_SIZE 200000ll

struct state { ll mini, cnt; };
struct rectangle { ll l, r, d, u; };

state op(state a, state b) {
    state ret = {min(a.mini, b.mini), 0};
    if (ret.mini == a.mini) ret.cnt += a.cnt;
    if (ret.mini == b.mini) ret.cnt += b.cnt;
    return ret;
}
state e() { return {0, 0}; }
state mpg(ll f, state x) {return {x.mini + f, x.cnt};}
ll cmp(ll f, ll g) {return f + g;}
ll id() { return 0; }

// 長方形に覆われた面積を求める
ll Area_of_Union_of_Rectangles(vector<rectangle> &R) {
    struct query { ll l, r, add; };
    vector<vector<query>> Q(RECT_SIZE + 9);
    for(auto r : R) {
        Q[r.d].push_back({r.l, r.r, 1});
        Q[r.u + 1].push_back({r.l, r.r, -1});
    }

    lazy_segtree<state, op, e, ll, mpg, cmp, id> lseg(RECT_SIZE + 9);
    for (ll i = 1; i <= RECT_SIZE; i++)
        lseg.set(i, {0, 1});

    ll ans = RECT_SIZE * RECT_SIZE;
    for(ll h = 1; h <= RECT_SIZE; h++) {
        for (auto q : Q[h]) {
            lseg.apply(q.l, q.r + 1, q.add);
        }
        state prod = lseg.all_prod();
        if (prod.mini == 0) {
            ans -= prod.cnt;
        }
    }
    return ans;
}

int main(void) {
    ll N;
    ll A[200009];

    ll i, j, k;

    cin >> N;
    for (i = 1; i <= N; i++) cin >> A[i];

    // A[i]ごとの位置をまとめる
    map<ll, deque<ll>> m;
    for (i = 1; i <= N; i++) {
        m[A[i]].push_back(i);
    }

    // 番兵を追加
    for (auto &x : m) {
        x.second.push_front(0);
        x.second.push_back(N + 1);
    }

    vector<rectangle> R;
    for (auto &x : m) {
        for (i = 1; i < x.second.size() - 1; i++) {
            ll l = x.second[i - 1] + 1;
            ll m = x.second[i];
            ll r = x.second[i + 1] - 1;

            R.push_back({l, m, m, r});
        }
    }

    ll ans = Area_of_Union_of_Rectangles(R);

    cout << ans << endl;

    return 0;
}