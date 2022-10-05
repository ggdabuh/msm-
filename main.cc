#include <vector>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <optional>
#include <iostream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <future>
#include <limits>

using namespace std;

using xxx = unsigned int32_t;
using Row = vector<xxx>;

ostream& operator<<(ostream& o, Row row);

vector<Row> init_rows(std::size_t const row_size, xxx const value_count) {
    vector<Row> res;
    Row gen(row_size, 0);
    res.push_back(gen);
    xxx i = 0;
    while (i < row_size) {
        auto & val = gen[i];
        val ++;
        if (val < value_count) {
            i = 0;
            res.push_back(gen);
        } else {
            i ++;
            val = 0;
        }
    }
    return res;
}

tuple<xxx,xxx> count_white_blacks(Row const& _lhs,Row const& _rhs,xxx value_count) {
    xxx b = 0;
    xxx w = 0;

    for(int num = 0; num < value_count; ++num) {
        xxx wl = 0;
        xxx wr = 0;
        for (int i = 0; i < _lhs.size(); ++i) {
            xxx const lhs = _lhs[i];
            xxx const rhs = _rhs[i];
            if (lhs == num) {
                if (rhs == lhs) {
                    b += 1;
                } else {
                    wl += 1;
                }
            } else if (rhs == num) {
                wr += 1;
            }
        }
        w += min(wl, wr);
    }
    return make_tuple(b, w);
}

void filter(vector<Row>& rows, Row const& crit, xxx w, xxx b, xxx value_count) {

    rows.erase(
        remove_if(rows.begin(), rows.end(),
            [&crit, b, w, value_count](auto const& r){
                auto const [b2, w2] = count_white_blacks(r, crit, value_count);
                return !(b == b2 && w == w2);
                }),
        rows.end());
}

xxx calc_min_eliminated(vector<Row> const& rows, Row const& row, xxx value_count) {

    xxx min_eliminated = std::numeric_limits<xxx>::max() - 1;
    for (int b = 0; b < value_count; ++b) {
        for (int w = 0; w < (value_count - b); ++w) {
            if (w == 1 && b == (row.size() - 1)) {
                continue;
            }
            xxx const matching = count_if(rows.begin(), rows.end(),
                [&row, b, w, value_count](auto const& r) {
                    auto const [b2, w2] = count_white_blacks(r, row, value_count);
                    return b == b2 && w == w2;
                });
            xxx score =  rows.size() - matching;
            
            min_eliminated = min(score, min_eliminated);
        }
    }
    return min_eliminated;
}

Row const& best(vector<Row> const& rows, xxx value_count) {
    tuple <xxx, Row const*> best {0, nullptr};

    for(auto const& row: rows) {
        auto const n = calc_min_eliminated(rows, row, value_count);
        if (get<0>(best) < n) {
            best = make_tuple(n, &row);
        }
    }
    if(!get<1>(best)) abort();
    return *get<1>(best);
}

Row const& best2(vector<Row> const& rows, xxx value_count) {
    tuple <xxx, Row const*> best {0, nullptr};

    vector<future<tuple <xxx, Row const*>>> f {rows.size()};
    transform(rows.begin(), rows.end(),
        f.begin(), [&rows, value_count](auto & row) {
            return async([&rows, &row, value_count]() {
                auto const n = calc_min_eliminated(rows, row, value_count);
                return make_tuple(n, &row);
            });
        });
    for(auto & fut: f) {
        auto [n, row] = fut.get();
        if (get<0>(best) < n) {
            best = make_tuple(n, row);
        }
    }
    if(!get<1>(best)) abort();
    return *get<1>(best);
}

ostream& operator<<(ostream& o, Row row) {
    o << "[ ";
    for(xxx val: row) {
        o << (int)val << ", ";
    }
    o << ']';
    return o;
}

int main() {
    

    cout << "row size:\n";
    int  row_size;
    cin >> row_size;
    if (!cin) {
        cout << "Please type a number!\n";
        abort();
    }


    cout << "values count:\n";
    int value_count;
    cin >> value_count;
    if (!cin) {
        cout << "Please type a number!\n";
        abort();
    }

    vector<Row> rows = init_rows(row_size, value_count);
    Row row = best2(rows, value_count);
    string line;
    for(;;) {
        cout << row << '\n';
        cin >> line;
        if (line.size() != 2)
            abort();
        xxx b = stoi(line.substr(0, 1).c_str());
        xxx w = stoi(line.substr(1, 2).c_str());

        filter(rows, row, w, b, value_count);
        if (rows.size() == 1) {
            row = rows[0];
            break;
        }
        auto now = chrono::steady_clock::now();
        row = best2(rows, value_count);
        auto end = chrono::steady_clock::now();
        chrono::duration<double, milli> diff = end - now;
        std::cout << "elapsed " << diff.count() << " ms\n";
    }
    cout << row << '\n';
}
