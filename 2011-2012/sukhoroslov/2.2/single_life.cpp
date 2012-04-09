#include <cassert>
#include <iostream>
#include <vector>
#include <string>

typedef std::vector<std::string> Field;

const int NUM_NEIGHBOURS = 8;
const int dx[NUM_NEIGHBOURS] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int dy[NUM_NEIGHBOURS] = {0, -1, -1, -1, 0, 1, 1, 1};

Field next(const Field& field)
{
    if (field.empty()) {
        return field;
    }
    int numRows = field.size();
    int numColumns = field.back().size();
    Field nextf(numRows, std::string(numColumns, '0'));
    for (int x = 0; x < numRows; ++x) {
        for (int y = 0; y < numColumns; ++y) {
            int numActiveNeighbours = 0;
            for (int i = 0; i < NUM_NEIGHBOURS; ++i) {
                int xx = (x + dx[i] + numRows) % numRows;
                int yy = (y + dy[i] + numColumns) % numColumns;
                if (field[xx][yy] == '1') {
                    ++numActiveNeighbours;
                }
            }
            if (numActiveNeighbours == 3) {
                nextf[x][y] = '1';
            } else if (numActiveNeighbours == 2) {
                nextf[x][y] = field[x][y];
            }
        }
    }
    return nextf;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    int numRows, numColumns, numIterations;
    std::cin >> numRows >> numColumns >> numIterations;
    Field field(numRows);
    for (int i = 0; i < numRows; ++i) {
        std::cin >> field[i];
        assert(field[i].size() == static_cast<size_t>(numColumns));
    }

    for (int it = 0; it < numIterations; ++it) {
        field = next(field);
    }

    std::cout << numRows << " " << numColumns << std::endl;
    for (int i = 0; i < numRows; ++i) {
        std::cout << field[i] << std::endl;
    }
}
