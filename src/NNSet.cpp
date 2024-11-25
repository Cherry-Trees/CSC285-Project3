#include "NNSet.hpp"

/**
 * Default Constructor.
 * Set the max element to 4e+9.
 * Set each array element to zero.
 */
NNSet::NNSet() : 
    m_MaxElem(0), 
    m_ArraySize(0), 
    m_Elems(0) {}

void NNSet::reserve(unsigned int biggestElementPossible) {
    m_MaxElem = biggestElementPossible; 
    m_ArraySize = biggestElementPossible / UINT_SIZE + 1;
    m_Elems = new unsigned[biggestElementPossible / UINT_SIZE + 1];
    for (unsigned i = 0; i < m_ArraySize; i++)
        m_Elems[i] = 0;
}


/**
 * Constructor that takes one unsigned integer argument.
 * Set the max element to argument.
 * Set each array element to zero.
 */
NNSet::NNSet(const unsigned biggestElementPossible) : 
    m_MaxElem(biggestElementPossible), 
    m_ArraySize(biggestElementPossible / UINT_SIZE + 1), 
    m_Elems(new unsigned[biggestElementPossible / UINT_SIZE + 1]) {
        for (unsigned i = 0; i < m_ArraySize; i++)
            m_Elems[i] = 0;
    }

/**
 * Copy constructor.
 */
NNSet::NNSet(const NNSet &other) :
    m_MaxElem(other.m_MaxElem),
    m_ArraySize(other.m_ArraySize),
    m_Elems(new unsigned[other.m_ArraySize]) {
        for (unsigned i = 0; i < m_ArraySize; i++)
            m_Elems[i] = other.m_Elems[i];
    }

/**
 * Destructor.
 * Free the memory allocated for the array.
 */
NNSet::~NNSet() {delete[] m_Elems;}

/**
 * Check if an element is in the set.
 */
bool NNSet::contains(unsigned int lookupElement) const
{
    // Modulus the lookup element by 32 to get the actual value to look up.
    return CONTAINS(m_Elems[lookupElement / UINT_SIZE], lookupElement % UINT_SIZE);
}

/**
 * Add a new element to the set.
 */
bool NNSet::add(unsigned int newElement)
{
    // Add only if the new element is less than the max.
    if (newElement <= m_MaxElem)

        // Check if the set already contains the new element.
        if (!CONTAINS(m_Elems[newElement / UINT_SIZE], newElement))
        {
            ADD(m_Elems[newElement / UINT_SIZE], newElement);
            return 1;
        }

    return 0;
}

/**
 * Get the number of elements in the set.
 */
unsigned int NNSet::cardinality() const 
{
    unsigned cardinality = 0;
    for (unsigned i = 0; i < m_ArraySize; i++)
        for (unsigned j = 0; j < UINT_SIZE; j++)
            // Mask each individual bit. If the bit is a 1, add to the cardinality.
            if (MASK(m_Elems[i], 1 << j)) cardinality++;
    
    return cardinality;
}

/**
 * Assign this set to the rhs set.
 */
const NNSet& NNSet::operator=(const NNSet &rhs)
{
    m_MaxElem = rhs.m_MaxElem;
    m_ArraySize = rhs.m_ArraySize;

    // Free the memory of the array.
    delete[] m_Elems;
    m_Elems = new unsigned[rhs.m_ArraySize];
    for (unsigned i = 0; i < m_ArraySize; i++)
        m_Elems[i] = rhs.m_Elems[i];
    
    return *this;
}

/**
 * Get the union of this set and another set.
 */
NNSet NNSet::operator+(const NNSet &otherSet) const
{
    // Set the max element of the new set to the max of the two sets.
    NNSet retSet(MAX(m_MaxElem, otherSet.m_MaxElem));
    unsigned i = 0;

    // Iterate through the min size set.
    while (i < MIN(m_ArraySize, otherSet.m_ArraySize))
    {
        // Union each matching array element at index i.
        retSet.m_Elems[i] = UNION(m_Elems[i], otherSet.m_Elems[i]);
        i++;
    }

    // Assign the remaining elements of the larger set to the new set.
    if (m_ArraySize > otherSet.m_ArraySize)
        while (i < m_ArraySize)
        {
            retSet.m_Elems[i] = m_Elems[i];
            i++;
        }

    else
        while (i < otherSet.m_ArraySize)
        {
            retSet.m_Elems[i] = otherSet.m_Elems[i];
            i++;
        }
    
    return retSet;

}

/**
 * Get the difference between this set and another set.
 */
NNSet NNSet::operator-(const NNSet &rhsSet) const
{
    NNSet retSet(m_MaxElem);
    unsigned i = 0;

    // Iterate through the min size set.
    while (i < MIN(m_ArraySize, rhsSet.m_ArraySize))
    {
        retSet.m_Elems[i] = DIFFERENCE(m_Elems[i], rhsSet.m_Elems[i]);
        i++;
    }

    // Assign the remaining elements of the lhs set to the new set.
    if (m_ArraySize > rhsSet.m_ArraySize)
        while (i < m_ArraySize)
        {
            retSet.m_Elems[i] = m_Elems[i];
            i++;
        }


    return retSet;
}

/**
 * Get the complement of this set.
 */
NNSet NNSet::operator!() const
{
    NNSet retSet(m_MaxElem);
    for (unsigned i = 0; i < m_ArraySize; i++)
    {
        // Check if this is the last iteration of this loop.
        // If it is, take a string of one's (UINT_MAX), and bitshift it to the left by the remainder of 
        //      the max element and 32, the true max element of the set at index i.
        // If it isn't, then the mask will be a string of one's, so nothing will be cut off. 
        const unsigned mask = (i == m_ArraySize - 1) ? ~(UINT_MAX << (m_MaxElem % UINT_SIZE + 1)) : UINT_MAX;

        // Get the complement of the single set at index i and mask it.
        retSet.m_Elems[i] = MASK(COMPLEMENT(m_Elems[i]), mask);
    }
    
    return retSet;
}

/**
 * Check if the set is empty.
 */
bool NNSet::isEmpty() const 
{
    // Iterate through each element in the array.
    // If any of them is NOT zero, return false.
    for (unsigned i = 0; i < m_ArraySize; i++)
        if (m_Elems[i])
            return 0;

    return 1;
}

/**
 * Remove all elements in the set.
 */
void NNSet::clear() 
{
    for (unsigned i = 0; i < m_ArraySize; m_Elems[i] = 0, i++); 
}

/**
 * Output the set.
 */
std::ostream& operator<< (std::ostream &os, const NNSet &nns)
{
    os << "{ ";
    unsigned hits = 0;
    for (unsigned i = 0; i < nns.m_ArraySize; i++)
        for (unsigned j = 0; j < UINT_SIZE; j++)
        {
            if (CONTAINS(nns.m_Elems[i], j))
            {
                hits++;
                os << i * UINT_SIZE + j << ((hits == nns.cardinality()) ? " " : ", ");  
            }
        }

    os << '}';

    return os;
}