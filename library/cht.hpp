/**
 * 直線 ax+b を管理，x における最小値または最大値を求める 
 * 最小化: CHT<true>，最大化: CHT<false>
 *
 * add の傾きは単調増加 or 単調減少
 * query は任意の順番で呼べる 
 * query_monotone_inc / query_monotone_dec は，x が単調増加 / 単調減少
 * に限り呼べる（不要になった直線を削除するため） 
 *
 * 係数，x，返り値は int64_t の範囲内であることを仮定
 */
template<bool Minimize = true>
struct CHT {
	using i64 = int64_t;
	using i128 = __int128_t;

	struct Line {
		i64 slope;
		i64 intercept;

		i128 eval(i64 x) const {
			return static_cast<i128>(slope) * x + intercept;
		}
	};

	deque<Line> lines;

	CHT() = default;

	bool empty() const {
		return lines.empty();
	}

	void clear() {
		lines.clear();
	}

	// a, b, c の傾きが単調なとき，b が下側凸包に不要か判定する 
	static bool unnecessary(const Line& a, const Line& b, const Line& c) {
		return (static_cast<i128>(b.intercept) - a.intercept)
				* (static_cast<i128>(b.slope) - c.slope)
			>= (static_cast<i128>(c.intercept) - b.intercept)
				* (static_cast<i128>(a.slope) - b.slope);
	}

	/* 直線 ax+b を追加 */
	void add(i64 a, i64 b) {
		if constexpr(!Minimize) {
			a = -a;
			b = -b;
		}

		Line line{a, b};
		if(empty()) {
			lines.emplace_front(line);
			return;
		}

		if(lines.front().slope <= a) {
			if(lines.front().slope == a) {
				if(lines.front().intercept <= b) return;
				lines.pop_front();
			}
			while(lines.size() >= 2 && unnecessary(line, lines[0], lines[1])) {
				lines.pop_front();
			}
			lines.emplace_front(line);
		} else {
			assert(a <= lines.back().slope);
			if(lines.back().slope == a) {
				if(lines.back().intercept <= b) return;
				lines.pop_back();
			}
			while(lines.size() >= 2 && unnecessary(lines[lines.size() - 2], lines.back(), line)) {
				lines.pop_back();
			}
			lines.emplace_back(line);
		}
	}

	/* x における最小値・最大値を求める */
	i64 query(i64 x) const {
		assert(!empty());
		size_t left = 0;
		size_t right = lines.size() - 1;
		while(left < right) {
			const size_t mid = (left + right) / 2;
			if(lines[mid].eval(x) >= lines[mid + 1].eval(x)) left = mid + 1;
			else right = mid;
		}
		return restore_sign(lines[left].eval(x));
	}

	i64 query_monotone_inc(i64 x) {
		assert(!empty());
		while(lines.size() >= 2 && lines[0].eval(x) >= lines[1].eval(x)) {
			lines.pop_front();
		}
		return restore_sign(lines.front().eval(x));
	}

	i64 query_monotone_dec(i64 x) {
		assert(!empty());
		while(lines.size() >= 2 && lines.back().eval(x) >= lines[lines.size() - 2].eval(x)) {
			lines.pop_back();
		}
		return restore_sign(lines.back().eval(x));
	}

private:
	static i64 restore_sign(i128 value) {
		if constexpr(!Minimize) value = -value;
		return static_cast<i64>(value);
	}
};

	CHT<true> cht; // true: min, false: max
	cht.add(a, b);
	cht.query(x);