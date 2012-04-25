#include <algorithm>
#include <set>
#include <queue>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#define REQUIRE(cond, message) \
    do { \
        if (!(cond)) { \
            std::cerr << message << std::endl; \
        } \
    } while (false)

std::vector<size_t> zFunction(const std::string& word)
{
    size_t length = word.length();
    std::vector<size_t> result(length);
    for (size_t i = 1, l = 0, r = 0; i < length; ++i) {
        if (i <= r)
            result[i] = std::min(r - i + 1, result[i - l]);
        while (i + result[i] < length &&
               word[result[i]] == word[i + result[i]]) {
            ++result[i];
        }
        if (i + result[i]-1 > r) {
            l = i;
            r = i + result[i] - 1;
        }
    }
    return result;
}

void solve()
{
    std::string word;
    std::cin >> word;
    word += word;
    std::vector<size_t> zf = zFunction(word);
    size_t answer = 1;
    for (size_t i = 1; i < word.size() / 2; ++i) {
        if (zf[i] >= word.size() / 2) {
            REQUIRE(word.size() / 2 % i == 0, "Cannot split in equal blocks");
            answer = word.size() / 2 / i;
            break;
        }
    }
    std::cout << answer << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    solve();
    return 0;
}
