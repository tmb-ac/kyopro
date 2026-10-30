namespace SegmentTreeBeats {
using size_type = uint32_t;

struct Ignore {};

template <typename Node> struct Tree {
    using node = Node;

    struct DefaultGetter {
        using value_type = decltype(declval<node>().get());
        const value_type &operator()(node *x) const { return x->get(); }
    };

    mutable vector<node> m_sub;
    size_type m_capacity, m_depth, m_size;

    template <typename Modify>
    static void _apply(node *sub, size_type i, Modify &&modify, size_type len) {
        if (!node::map(modify, sub + i, len)) {
            _pushdown(sub, i, len);
            _apply(sub, i * 2, modify, len >> 1);
            _apply(sub, i * 2 + 1, modify, len >> 1);
            _pushup(sub, i);
        }
    }

    static void _pushdown(node *sub, size_type i, size_type len) {
        sub[i].pushdown(sub + i * 2, sub + (i * 2 + 1), len);
    }

    static void _pushup(node *sub, size_type i) {
        sub[i].pushup(sub + (i * 2), sub + (i * 2 + 1));
    }

    static void _fetch(node *sub, size_type l, size_type r, size_type len) {
        if (l == 1) return;
        _fetch(sub, l >> 1, r >> 1, len << 1);
        for (size_type i = l >> 1; i <= r >> 1; i++)
            _pushdown(sub, i, len);
    }

    Tree() = default;

    template <typename InitMapping = Ignore>
    Tree(size_type length, InitMapping mapping = InitMapping()) {
        resize(length, mapping);
    }

    template <typename Iterator> Tree(Iterator first, Iterator last) {
        reset(first, last);
    }

    template <typename InitMapping>
    void resize(size_type length, InitMapping mapping = InitMapping()) {
        if (!(m_size = length)) return;
        m_depth = bit_width(m_size - 1);
        m_capacity = 1 << m_depth;
        m_sub.resize(m_capacity * 2);
        node *sub = m_sub.data();
        if constexpr (!is_same<InitMapping, Ignore>::value) {
            for (size_type i = 0; i < m_size; i++)
                sub[m_capacity + i].set(mapping(i));
            for (size_type len = m_capacity / 2, cnt = (m_size + 1) / 2; len; len >>= 1, cnt = (cnt + 1) / 2)
                for (size_type i = len; i != len + cnt; i++)
                    _pushup(sub, i);
        }
    }

    template <typename Iterator> void reset(Iterator first, Iterator last) {
        resize(last - first, [&](size_type i) { return *(first + i); });
    }

    template <typename Modify> void modify(size_type i, Modify &&modify) {
        i += m_capacity;
        node *sub = m_sub.data();
        for (size_type d = m_depth, len = m_capacity; d; d--, len >>= 1)
            _pushdown(sub, i >> d, len);
        modify(sub + i);
        while (i >>= 1)
            _pushup(sub, i);
    }

    template <typename Modify> void add(size_type i, Modify &&modify) {
        i += m_capacity;
        node *sub = m_sub.data();
        for (size_type d = m_depth, len = m_capacity; d; d--, len >>= 1)
            _pushdown(sub, i >> d, len);
        _apply(sub, i, modify, 1);
        while (i >>= 1)
            _pushup(sub, i);
    }

    template <typename Modify>
    void add(size_type left, size_type right, Modify &&modify) {
        if (left == right) return add(left, modify);
        left += m_capacity, right += m_capacity;
        node *sub = m_sub.data();
        size_type j = bit_width(left ^ right) - 1, len = m_capacity;
        for (size_type d = m_depth; d > j; d--, len >>= 1)
            _pushdown(sub, left >> d, len);
        for (size_type d = j; d; d--, len >>= 1)
            _pushdown(sub, left >> d, len), _pushdown(sub, right >> d, len);
        _apply(sub, left, modify, 1), _apply(sub, right, modify, 1), len = 1;
        while (left >> 1 < right >> 1) {
            if (!(left & 1)) _apply(sub, left + 1, modify, len);
            _pushup(sub, left >>= 1);
            if (right & 1) _apply(sub, right - 1, modify, len);
            _pushup(sub, right >>= 1);
            len <<= 1;
        }
        while (left >>= 1)
            _pushup(sub, left);
    }

    template <typename Getter = DefaultGetter>
    typename Getter::value_type query(size_type i) const {
        i += m_capacity;
        node *sub = m_sub.data();
        for (size_type d = m_depth, len = m_capacity; d; d--, len >>= 1)
            _pushdown(sub, i >> d, len);
        return Getter()(sub + i);
    }

    template <typename Getter>
    typename Getter::value_type query(size_type left, size_type right) const {
        if (left == right) return query<Getter>(left);
        left += m_capacity, right += m_capacity;
        node *sub = m_sub.data();
        size_type j = bit_width(left ^ right) - 1, len = m_capacity;
        for (size_type d = m_depth; d > j; d--, len >>= 1)
            _pushdown(sub, left >> d, len);
        for (size_type d = j; d; d--, len >>= 1)
            _pushdown(sub, left >> d, len), _pushdown(sub, right >> d, len);
        typename Getter::value_type resl = Getter()(sub + left),
                                                                resr = Getter()(sub + right);
        for (; left >> 1 != right >> 1; left >>= 1, right >>= 1) {
            if (!(left & 1)) resl = Getter()(resl, Getter()(sub + (left ^ 1)));
            if (right & 1) resr = Getter()(Getter()(sub + (right ^ 1)), resr);
        }
        return Getter()(resl, resr);
    }

    template <typename Callback> void do_for_each(Callback &&call) {
        node *sub = m_sub.data();
        _fetch(sub, m_capacity, m_capacity + m_size - 1, 2);
        for (size_type i = m_capacity, j = 0; j != m_size; i++, j++)
            call(sub + i);
    }
};

// ChminChmaxAddNode implementation (simplified for ChminChmaxAddTree only)
template <typename ValueType, typename CountType, typename SumType,
                    ValueType Min = numeric_limits<ValueType>::min() / 2,
                    ValueType Max = numeric_limits<ValueType>::max() / 2>
struct ChminChmaxNode {
    SumType m_sum;
    CountType m_max_cnt, m_min_cnt;
    ValueType m_max1, m_max2, m_min1, m_min2, m_inc;
    static constexpr ValueType min = Min, max = Max;
    using node_type = ChminChmaxNode<ValueType, CountType, SumType, Min, Max>;

    struct Chmin {ValueType m_chmin_by;};
    struct Chmax {ValueType m_chmax_by;};
    struct Assign {ValueType m_val;};
    struct Add {ValueType m_add_by;};

    struct MinGetter {
        using value_type = ValueType;
        value_type operator()(node_type *x) const { return x->m_min1; }
        value_type operator()(value_type x, value_type y) const {
            return min(x, y);
        }
    };

    struct MaxGetter {
        using value_type = ValueType;
        value_type operator()(node_type *x) const { return x->m_max1; }
        value_type operator()(value_type x, value_type y) const {
            return max(x, y);
        }
    };

    struct SumGetter {
        using value_type = SumType;
        value_type operator()(node_type *x) const { return x->m_sum; }
        value_type operator()(value_type x, value_type y) const { return x + y; }
    };

    static bool map(const Chmin &chmin, node_type *x, CountType len) {
        if (x->m_max1 <= chmin.m_chmin_by) return true;
        if (x->m_max2 < chmin.m_chmin_by) return x->chmin_by(chmin.m_chmin_by), true;
        return false;
    }

    static bool map(const Chmax &chmax, node_type *x, CountType len) {
        if (x->m_min1 >= chmax.m_chmax_by) return true;
        if (x->m_min2 > chmax.m_chmax_by) return x->chmax_by(chmax.m_chmax_by), true;
        return false;
    }

    static bool map(const Assign &assign, node_type *x, CountType len) {
        return x->m_min1 == x->m_max1 ? x->add_by(assign.m_val - x->m_max1, len), true : false;
    }

    static bool map(const Add &inc, node_type *x, CountType len) {
        return x->add_by(inc.m_add_by, len), true;
    }

    void set(ValueType val) {
        m_sum = val;
        m_max1 = val, m_max2 = min, m_max_cnt = 1;
        m_min1 = val, m_min2 = max, m_min_cnt = 1;
    }

    const ValueType &get() const { return m_max1; }

    void add_by(ValueType inc, CountType len) {
        m_sum += SumType(inc) * len;
        m_max1 += inc;
        if (m_max2 != min) m_max2 += inc;
        m_min1 += inc;
        if (m_min2 != max) m_min2 += inc;
        m_inc += inc;
    }

    void chmin_by(ValueType val) {
        m_sum += SumType(val - m_max1) * m_max_cnt;
        if (m_min1 == m_max1) m_min1 = val;
        if (m_min2 == m_max1) m_min2 = val;
        m_max1 = val;
    }

    void chmax_by(ValueType val) {
        m_sum += SumType(val - m_min1) * m_min_cnt;
        if (m_max1 == m_min1) m_max1 = val;
        if (m_max2 == m_min1) m_max2 = val;
        m_min1 = val;
    }

    void pushup(node_type *lchild, node_type *rchild) {
        m_sum = lchild->m_sum + rchild->m_sum;
        if (lchild->m_max1 == rchild->m_max1) {
            m_max1 = lchild->m_max1;
            m_max2 = max(lchild->m_max2, rchild->m_max2);
            m_max_cnt = lchild->m_max_cnt + rchild->m_max_cnt;
        } else if (lchild->m_max1 > rchild->m_max1) {
            m_max1 = lchild->m_max1;
            m_max_cnt = lchild->m_max_cnt;
            m_max2 = max(lchild->m_max2, rchild->m_max1);
        } else {
            m_max1 = rchild->m_max1;
            m_max_cnt = rchild->m_max_cnt;
            m_max2 = max(lchild->m_max1, rchild->m_max2);
        }
        if (lchild->m_min1 == rchild->m_min1) {
            m_min1 = lchild->m_min1;
            m_min2 = min(lchild->m_min2, rchild->m_min2);
            m_min_cnt = lchild->m_min_cnt + rchild->m_min_cnt;
        } else if (lchild->m_min1 < rchild->m_min1) {
            m_min1 = lchild->m_min1;
            m_min_cnt = lchild->m_min_cnt;
            m_min2 = min(lchild->m_min2, rchild->m_min1);
        } else {
            m_min1 = rchild->m_min1;
            m_min_cnt = rchild->m_min_cnt;
            m_min2 = min(lchild->m_min1, rchild->m_min2);
        }
    }

    void pushdown(node_type *lchild, node_type *rchild, CountType len) {
        if (m_inc) {
            lchild->add_by(m_inc, len >> 1);
            rchild->add_by(m_inc, len >> 1);
            m_inc = 0;
        }
        if (m_max1 < lchild->m_max1) lchild->chmin_by(m_max1);
        if (m_min1 > lchild->m_min1) lchild->chmax_by(m_min1);
        if (m_max1 < rchild->m_max1) rchild->chmin_by(m_max1);
        if (m_min1 > rchild->m_min1) rchild->chmax_by(m_min1);
    }
};

// Type alias for ChminChmaxAddTree only
template <typename ValueType, typename CountType = int32_t, typename SumType = int64_t>
using ChminChmaxAddTree = Tree<ChminChmaxNode<ValueType, CountType, SumType>>;
}

using namespace SegmentTreeBeats;
using Tree = ChminChmaxAddTree<ll>;
using node = Tree::node;

    Tree t(A.begin(), A.end());

    // [left, right]に区間演算
    t.add(left, right, node::Add{2});
    t.add(left, right, node::Chmin{7});
    t.add(left, right, node::Chmax{6});

    auto sum = t.query<node::SumGetter>(0, n - 1);
    auto mn = t.query<node::MinGetter>(0, n - 1);
    auto mx = t.query<node::MaxGetter>(0, n - 1);

    for (int i = 0; i < n; i++) cout << ' ' << t.query(i);
    cout << endl;

int main() {
    ll n, q;
    cin >> n;
    SegmentTreeBeats::ChminChmaxAddTree<int64_t> S(n, [](auto...) {
        ll x;
        cin >> x;
        return x;
    });
    cin >> q;
    using Node = decltype(S)::node;
    while (q--) {
        auto sum = S.query<Node::SumGetter>(0, n - 1);
        uint32_t l, r;
        int64_t b;
        cin >> l >> r >> b;
        S.add(--l, --r, Node::Add{-b});
        S.add(l, r, Node::Chmax{0});
        cout << sum - S.query<Node::SumGetter>(0, n - 1) << endl;
    }
    return 0;
}
