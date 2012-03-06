#include <iostream>
#include <set>
using namespace std;

int main(int argc, char** argv)
{
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    cout << n << endl;
    set<int> nums;
    for(; nums.size() < n;) {
        nums.insert(rand() * rand() % (10 * n));
    }
    for(set<int>::const_iterator it = nums.begin(); it != nums.end(); ++it) {
        cout << *it << endl;
    }
    cout << m << endl;
    for(int i = 0; i < m; ++i) {
        cout << rand() * rand() % 100 << endl;
    }
    return 0;
}

