#include <iostream>
#include <set>
using namespace std;

int main()
{
    ios_base::sync_with_stdio(false);
    int n;
    cin >> n;
    set<int> st;
    for(int i = 0; i < n; ++i) {
        int num;
        cin >> num;
        st.insert(num);
    }
    int m;
    cin >> m;
    for(int i = 0; i < m; ++i) {
        int num;
        cin >> num;
        if (st.count(num)) {
            cout << "Yes" << endl;
        }
        else {
            cout << "No" << endl;
        }
    }
    return 0;
}
