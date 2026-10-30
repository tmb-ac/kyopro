#pragma once

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

// デフォルト構築・加算・乗算が可能な任意の係数型で動作する，
// O(|a||b|) の愚直な畳み込み
struct naive_convolution {
    template <class T>
    std::vector<T> operator()(const std::vector<T>& a,
                              const std::vector<T>& b) const {
        if (a.empty() || b.empty()) return {};
        std::vector<T> c(a.size() + b.size() - 1);
        for (std::size_t i = 0; i < a.size(); ++i) {
            for (std::size_t j = 0; j < b.size(); ++j) {
                c[i + j] += a[i] * b[j];
            }
        }
        return c;
    }
};

/**
 * P(x) / Q(x) の x^n の係数を返す
 * T に必要な演算：
 * T{}，単項マイナス，乗算，除算，等値比較
 * convolve(a, b) の要件：
 * a(x)b(x) の係数列を返す
 *
 * 計算量は O(M(d) log n)
 * d := deg(Q)
 * M(d) := 次数 d の多項式同士の畳み込みにかかる計算量
 */
template <class T, class Convolution>
T bostan_mori(std::vector<T> p, std::vector<T> q,
              unsigned long long n, Convolution convolve) {
    assert(!q.empty() && q[0] != T{});

    while (!p.empty() && p.back() == T{}) p.pop_back();
    assert(p.size() < q.size());

    const std::size_t d = q.size();
    p.resize(d - 1);

    while (n != 0) {
        std::vector<T> q_neg = q;
        for (std::size_t i = 1; i < q_neg.size(); i += 2) {
            q_neg[i] = -q_neg[i];
        }

        auto pq = convolve(p, q_neg);
        auto qq = convolve(q, q_neg);

        std::vector<T> next_p(d - 1);
        std::vector<T> next_q(d);
        const std::size_t parity = static_cast<std::size_t>(n & 1ULL);
        for (std::size_t i = 0; i < next_p.size(); ++i) {
            if (2 * i + parity < pq.size()) next_p[i] = pq[2 * i + parity];
        }
        for (std::size_t i = 0; i < next_q.size(); ++i) {
            if (2 * i < qq.size()) next_q[i] = qq[2 * i];
        }

        p = std::move(next_p);
        q = std::move(next_q);
        n >>= 1;
    }

    return p.empty() ? T{} : p[0] / q[0];
}

template <class T>
T bostan_mori(std::vector<T> p, std::vector<T> q,
              unsigned long long n) {
    return bostan_mori(std::move(p), std::move(q), n,
                       naive_convolution{});
}