#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>

/*! \brief implement a circular buffer of type T
*/
template <class T> 
class CRingBuffer
{
public:
    explicit CRingBuffer (int iBufferLengthInSamples) : m_iBuffLength(iBufferLengthInSamples),
                                                        m_writeIndex(0),
                                                        m_readIndex(0)
    {
        assert(iBufferLengthInSamples > 0);
        m_buffer = new T[m_iBuffLength];
        reset();
        // allocate and init
    }

    virtual ~CRingBuffer ()
    {
        reset();
        //No need to delete m_buffer as it uses smart pointers
        // free memory
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc (T tNewValue)
    {
        put(tNewValue);
        updateWriteIdx();
    }

    /*! add new values of type T to write index and increment write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void putPostInc (const T* ptNewBuff, int iLength)
    {
        assert(iLength > 0 && iLength < m_iBuffLength);
        for (int i=0; i < iLength; i++)
            putPostInc(ptNewBuff[i]);
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_buffer[getWriteIdx()] = tNewValue;
    }

    /*! add new values of type T to write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void put(const T* ptNewBuff, int iLength)
    {
        assert(iLength > 0 && iLength < m_iBuffLength);
        for (int i=0; i < iLength; i++)
            put(ptNewBuff[i]);
    }
    
    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc ()
    {
        T val = m_buffer[m_readIndex];
        updateReadIndex();
        return val;
    }

    /*! return the values starting at the current read index and increment the read pointer
    \param ptBuff: pointer to where the values will be written
    \param iLength: number of values
    \return void
    */
    void getPostInc (T* ptBuff, int iLength)
    {
        assert(iLength > 0 && iLength < m_iBuffLength);
        for (int i=0; i<iLength; i++)
            ptBuff[i] = getPostInc();
    }

    /*! return the value at the current read index
    \param fOffset: read at offset from read index
    \return float the value from the read index
    */
    T get (float fOffset = 0.f) const
    {
        T val = interpolate(fOffset);
        return val;
    }

    /*! return the values starting at the current read index
    \param ptBuff to where the values will be written
    \param iLength: number of values
    \return void
    */
    void get (T* ptBuff, int iLength) const
    {
        assert(iLength > 0 && iLength < m_iBuffLength);
        for (int i=0; i <iLength; i++)
            ptBuff[i] = get();
    }
    
    /*! set buffer content and indices to 0
    \return void
    */
    void reset ()
    {
        m_readIndex = 0;
        m_writeIndex = 0;
        zeroBuffer();
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx () const
    {
        return m_writeIndex;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx (int iNewWriteIdx)
    {
        m_writeIndex = getAdjustedIndex(iNewWriteIdx);
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx () const
    {
        return m_readIndex;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx (int iNewReadIdx)
    {
        m_readIndex = getAdjustedIndex(iNewReadIdx);
    }

    int getAdjustedIndex (int idx) {
        return idx < 0 ? (std::abs(m_iBuffLength - idx) % m_iBuffLength) : (idx % m_iBuffLength);
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer () const
    {
        if (m_writeIndex < m_readIndex)
            return m_iBuffLength - (m_readIndex - m_writeIndex);
        return m_writeIndex - m_readIndex;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength () const
    {
        return m_iBuffLength;
    }

    void updateWriteIdx () {
        m_writeIndex = getAdjustedIndex(++m_writeIndex);
    }

    void updateReadIndex () {
        m_readIndex = getAdjustedIndex(++m_readIndex);;
    }

    T interpolate(float offset) {
        assert(offset < m_iBuffLength);
        return (m_buffer[m_readIndex] * (1 - offset)) + (m_buffer[getAdjustedIndex(++m_readIndex)] * offset);
    }

    CRingBuffer () = delete;

private:
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    std::unique_ptr<T> m_buffer;
    int m_writeIndex;
    int m_readIndex;

    void zeroBuffer() const {
        for (int i=0; i < m_iBuffLength; i++)
            m_buffer[i] = 0;
    }
};
#endif // __RingBuffer_hdr__
