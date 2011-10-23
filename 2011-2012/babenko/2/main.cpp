#include <iostream>
#include <vector>
#include <algorithm>

void readSeq(std::vector<int>* sequence)
{
    size_t seqSize = 0;
    std::cin >> seqSize;
    sequence->resize(seqSize);
    for (size_t i = 0; i < seqSize; ++i) {
        std::cin >> (*sequence)[i];
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::vector<int> seqA, seqB;
    readSeq(&seqA);
    readSeq(&seqB);
    std::vector<size_t> longestCommonSubseq(seqA.size());
    for (size_t seqBIndex = 0; seqBIndex < seqB.size(); ++seqBIndex) {
        size_t oneStepBackValue = 0;
        for (size_t index = 0; index < seqA.size(); ++index) {
            size_t longestValue = longestCommonSubseq[index];
            if (index > 0) {
                longestValue = std::max(longestValue, longestCommonSubseq[index - 1]);
            }
            if (seqA[index] == seqB[seqBIndex]) {
                longestValue = std::max(longestValue, oneStepBackValue + 1);
            }
            oneStepBackValue = longestCommonSubseq[index];
            longestCommonSubseq[index] = longestValue;
        }
    }
    std::cout << longestCommonSubseq.back() << std::endl;
    return 0;
}
