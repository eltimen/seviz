#ifndef WORDRANGE_H
#define WORDRANGE_H

#include <utility>
#include <cassert>

class WordRange : public std::pair<int, int> {
public:
    WordRange()
        : std::pair<int, int>(-1, -1) 
    {
    }

    WordRange(int from, int to)
        : std::pair<int, int>(from, to) 
    {
        // TODO подумать о замене на exception
        assert(from <= to);
        assert(from > 0);
        assert(to > 0);
    }

    bool operator==(const WordRange& other) const {
        return first == other.first && second == other.second;
    }

    bool operator!=(const WordRange& other) const {
        return !(*this==other);
    }

    bool operator<(int wordId) const {
        return second < wordId;
    }

    bool contains(int wordId) const {
        return first <= wordId && wordId <= second;
    }

    bool isOutsideOf(const WordRange& range) const {
        const WordRange& a = *this;
        const std::pair<int, int>& b = range;

        assert(a.first <= a.second);
        assert(b.first <= b.second);

        //b внутри a
        return b.first >= a.first&& b.second <= a.second && *this!=range;
    }

    bool isInsideOf(const WordRange& range) const {
        const WordRange& a = *this;
        const WordRange& b = range;

        assert(a.first <= a.second);
        assert(b.first <= b.second);

        //a внутри b
        return a.first >= b.first && a.second <= b.second && *this != range;
    }
};
#endif // WORDRANGE_H
