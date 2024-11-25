/*******************************************************/
/* FILE:NNSet.hpp */
/* AUTHORS: S. Blythe, J. Miles :) */
/* DATE: 8/2024 */
/* PURPOSE: Header file for NNSet class for CSC28500 */
/*******************************************************/

#ifndef _NNSET_HPP_
#define _NNSET_HPP_

#include <iostream>

#define DEFAULT_MAX 4000000000
#define UINT_SIZE 32

/**
 * Helper macros for the class methods.
 */
#define MIN(x, y) ((x < y) ? (x) : (y))
#define MAX(x, y) ((x > y) ? (x) : (y))
#define CONTAINS(lhs, rhs) ((rhs < UINT_SIZE) ? (lhs & (1 << rhs)) : (0))
#define ADD(lhs, rhs) (lhs += (1 << rhs))
#define UNION(x, y) (x | y)
#define DIFFERENCE(lhs, rhs) (lhs - (lhs & rhs))
#define COMPLEMENT(x) (~x)
#define MASK(val, mask) (val & mask)

class NNSet
{
    private:
        unsigned *m_Elems;
        unsigned m_ArraySize;
        unsigned m_MaxElem;

    public:
        NNSet();
        NNSet(const unsigned biggestElementPossible);
        NNSet(const NNSet &other);
        ~NNSet();

        void reserve(unsigned int biggestElementPossible);
        const NNSet& operator=(const NNSet &rhs);
        bool add(unsigned int newElement);
        bool contains(unsigned int lookupElement) const;

        unsigned int cardinality() const;

        NNSet operator+(const NNSet &otherSet) const;
        NNSet operator-(const NNSet &rhsSet) const;
        NNSet operator!() const;

        bool isEmpty() const;
        void clear();

        friend std::ostream& operator<< (std::ostream &os, const NNSet &nns);
};

#endif