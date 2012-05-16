#include <numeric>
#include <algorithm>
#include <set>
#include <queue>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#define REQUIRE(cond, message) \
            do { \
                if (!(cond)) { \
                    std::cerr << message << std::endl; \
                    assert(false); \
                } \
            } while (false)

#define NDEBUG

typedef std::vector<size_t> Ids;


size_t charId(char c)
{
    static const char MIN_CHAR = 'a';
    static const char MAX_CHAR = 'z';
    REQUIRE(MIN_CHAR <= c && c <= MAX_CHAR,
            "Cannot get id of unknown char " << c);
    return c - MIN_CHAR;
}

bool containsGreaterThan(const Ids& array, size_t value)
{
    for (size_t index = 0; index < array.size(); ++index) {
        if (array[index] > value) return true;
    }
    return false;
}

Ids countSort(const Ids& classId, const Ids& positions)
{
    const size_t size = classId.size();
    REQUIRE(!containsGreaterThan(classId, size - 1),
            "Too big ids, you should normalize");

    if (size == 0) {
        return Ids();
    }

    Ids count(size);
    for (size_t index = 0; index < size; ++index) {
        ++count.at(classId.at(index));
    }

    std::rotate(count.begin(), count.begin() + size - 1, count.end());
    count[0] = 0;
    std::partial_sum(count.begin(), count.end(), count.begin());

    Ids result(size);
    for (size_t index = 0; index < size; ++index) {
        size_t id = positions.at(index);
        result.at(count.at(classId.at(id))++) = id;
    }
    return result;
}

Ids range(size_t size)
{
    Ids result(size, 1);
    result.at(0) = 0;
    std::partial_sum(result.begin(), result.end(), result.begin());
    return result;
}

size_t rebuildClassIds(const Ids& classId, const Ids& positions,
                       size_t length, Ids* result)
{
    const size_t size = positions.size();
    REQUIRE(size == classId.size(), "Ids args should have the same size");

    result->assign(size, 0);
    size_t numEqClasses = 0;

    if (size > 0) {
        result->at(positions.at(0)) = numEqClasses;
    }
    for (size_t index = 1; index < size; ++index) {
        size_t currentId = positions.at(index);
        size_t shiftedCurrentId = (currentId + length / 2) % size;
        size_t previousId = positions.at(index - 1);
        size_t shiftedPreviousId = (previousId + length / 2) % size;
        if (classId.at(currentId) > classId.at(previousId) ||
                (classId[currentId] == classId[previousId] &&
                classId.at(shiftedCurrentId) >
                                    classId.at(shiftedPreviousId))) {
            ++numEqClasses;
        }
        result->at(positions.at(index)) = numEqClasses;
    }
    return 1 + numEqClasses;
}

Ids normalized(Ids sequence)
{
    Ids result(sequence);
    std::sort(sequence.begin(), sequence.end());
    sequence.erase(
            std::unique(sequence.begin(), sequence.end()), sequence.end());
    for (size_t index = 0; index < result.size(); ++index) {
        result[index] = std::lower_bound(sequence.begin(), sequence.end(),
                                         result[index]) - sequence.begin();
    }
    return result;
}

Ids buildSuffixArray(const Ids& array)
{
    const size_t size = array.size();

    Ids positions = countSort(normalized(array), range(size));
    Ids classId;
    size_t numEqClasses =
                rebuildClassIds(normalized(array), positions, 1, &classId);

    for (size_t length = 2; length < size; length <<= 1) {
        for (size_t index = 0; index < size; ++index) {
            positions.at(index) =
                            (positions.at(index) + size - length / 2) % size;
        }
        positions = countSort(classId, positions);

        Ids nextClassId;
        numEqClasses =
                rebuildClassIds(classId, positions, length, &nextClassId);
        classId = nextClassId;
    }
    return positions;
}

#ifdef DEBUG
bool isConsistent(const Ids& array, const Ids& positions)
{
    const size_t size = array.size();
    REQUIRE(size == positions.size(), "Inconsistency");
    for (size_t i = 1; i < size; ++i) {
        size_t firstId = positions[i - 1];
        size_t secondId = positions[i];
        for (size_t j = 0; j < size; ++j) {
            if (array[firstId] < array[secondId]) break;
            if (array[firstId] > array[secondId]) return false;
            firstId = (firstId + 1) % size;
            secondId = (secondId + 1) % size;
        }
    }
    return true;
}

#endif

std::string burrowsWheelerTransform(const std::string& text)
{
    size_t size = text.size();
    Ids suffixArray = buildSuffixArray(Ids(text.begin(), text.end()));
#ifdef DEBUG
    REQUIRE(isConsistent(sequence, suffixArray), "Incorrect suffix array");
#endif
    std::string result;
    for (size_t index = 0; index < size; ++index) {
        size_t cyclicShiftLastNumberPosition =
                                    (suffixArray[index] + size - 1) % size;
        result += text.at(cyclicShiftLastNumberPosition);
    }
    return result;
}

std::string readData()
{
    std::string text;
    std::cin >> text;
    return text;
}

void outData(const std::string& text)
{
    std::cout << text << std::endl;
}

#ifdef DEBUG
void testRandomString()
{
    for (size_t it = 0; it < 100; ++it) {
        std::cerr << "test "  << it << std::endl;
        size_t length = rand() % 1000 + 30;
        std::string str;
        for (size_t i = 0; i < length; ++i) {
            str += static_cast<char>(rand() % 26 + 'a');
        }
        std::cerr << str << std::endl;
        std::cerr << burrowsWheelerTransform(str) << std::endl;
    }
    exit(0);
}

#endif

int main()
{
    std::ios_base::sync_with_stdio(false);
#ifdef DEBUG
    testRandomString();
#endif
    outData(burrowsWheelerTransform(readData()));
    return 0;
}
