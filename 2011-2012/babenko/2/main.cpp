#include <iostream>
#include <vector>
#include <algorithm>

typedef std::vector<int> IntSequence;

void readSequence(IntSequence* sequence)
{
    size_t sequenceSize = 0;
    std::cin >> sequenceSize;
    sequence->resize(sequenceSize);
    for (size_t i = 0; i < sequenceSize; ++i) {
        std::cin >> (*sequence)[i];
    }
}

void solve(const IntSequence& sequenceFirst,
           const IntSequence& sequenceSecond,
           std::vector<size_t>* longestLengths)
{
    // Denotes intermediate solution for prefix of the second sequence,
    // and each i-th element equals answer for i-th prefix of the first sequence.
    // Initially this is the solution, containing 0s, for empty second sequence.
    longestLengths->resize(sequenceFirst.size());
    // Iterate over elements of the second sequence, updating answer
    // for the next prefix.
    for (size_t secondIndex = 0; secondIndex < sequenceSecond.size(); ++secondIndex) {
        // This value is the longest common subsequence length for previous
        // prefixes of the first and the second sequences.
        size_t previousPrefixLongestLength = 0;
        for (size_t index = 0; index < sequenceFirst.size(); ++index) {
            // Just init current value by longestLengths[index], which is the answer
            // for index-th prefix of the first sequence and (secondIndex-1)-th prefix
            // of the second sequence.
            size_t currentLongestLength = longestLengths->at(index);
            // If there is previous non-empty prefix, try to update current
            // longest length value.
            if (index > 0) {
                currentLongestLength = std::max(currentLongestLength, longestLengths->at(index - 1));
            }
            // It is optimal to match last elements provided that they are equal.
            if (sequenceFirst[index] == sequenceSecond[secondIndex]) {
                currentLongestLength = std::max(currentLongestLength, previousPrefixLongestLength + 1);
            }
            previousPrefixLongestLength = longestLengths->at(index);
            (*longestLengths)[index] = currentLongestLength;
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    IntSequence sequenceFirst, sequenceSecond;
    readSequence(&sequenceFirst);
    readSequence(&sequenceSecond);

    std::vector<size_t> longestCommonLengths;
    solve(sequenceFirst, sequenceSecond, &longestCommonLengths);

    std::cout << longestCommonLengths.back() << std::endl;

    return 0;
}
