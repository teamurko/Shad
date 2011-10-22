#include <iostream>
#include <vector>

void readSeq(std::vector<int>* sequence)
{
    size_t seqSize = 0;
    std::cin >> seqSize;
    sequence->resize(seqSize);
    for (size_t i = 0; i < seqSize; ++i) {
        std::cin >> (*sequence)[i];
    }
}

size_t bitCount(size_t n)
{
    size_t ret = 0;
    while (n > 0) {
        ret += n & 1;
        n >>= 1;
    }
    return ret;
}

int main()
{
    std::vector<int> a, b;
    readSeq(&a);
    readSeq(&b);
    size_t ans = 0;
    for (size_t ma = 0; ma < (1 << a.size()); ++ma) {
        for (size_t mb = 0; mb < (1 << b.size()); ++mb) {
            if (bitCount(ma) == bitCount(mb)) {
                std::vector<int> c(bitCount(ma));
                size_t i = 0;
                for(size_t j = 0; j < a.size(); ++j) {
                    if (ma & (1 << j)) {
                        c[i++] = a[j];
                    }
                }
                i = 0;
                bool ok = true;
                for(size_t j = 0; j < b.size(); ++j) {
                    if (mb & (1 << j)) {
                        if (c[i++] != b[j]) {
                            ok = false;
                            break;
                        }
                    }
                }
                if (ok) {
                    ans = std::max(ans, bitCount(ma));
                }
            }
        }
    }
    std::cout << ans << std::endl;

}
