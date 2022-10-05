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

int main() {
	xxx a = std::numeric_limits<xxx>::max() - 1;

	if(a == 0)
		cout<< "pou^et\n";

	xxx b = 3;

	cout << (int)a << ", " << (int)b << endl;

	xxx c = min(a, b);

	cout << (int)c << endl;

	xxx d = a < b ? a : b;
	
	cout << (int)d << endl;

	return 0;
}
