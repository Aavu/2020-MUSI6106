//
// Created by Raghavasimhan Sankaranarayanan on 1/31/20.
//

#ifndef MUSI6106_RINGBUFFER_H
#define MUSI6106_RINGBUFFER_H

#include <iostream>

class RingBuffer {
private:
    float **buffer = nullptr;
    int m_writeIndex = 0;
    int m_readIndex = 0;
    int m_max = 0;
    bool m_bInitializsed = false;
    bool full = false;
    unsigned int m_capacity = 0;
    unsigned int m_channels = 0;
    float* m_data = nullptr;

public:
    RingBuffer() = default;
    RingBuffer(unsigned int _size, unsigned int _channels):     m_capacity(_size),
                                                                full(false),
                                                                m_max(0),
                                                                m_channels(_channels) {
        buffer = new float* [m_channels];
        for (int i=0; i<m_channels; i++)
            buffer[i] = new float [m_capacity];

        m_data = new float [m_channels];

        m_writeIndex = 0;
        m_readIndex = 0;
        if (buffer)
            m_bInitializsed = true;
        fillZeros();
    }

    ~RingBuffer() {
        for (int c=0; c < m_channels; c++)
            delete[] buffer[c];
        delete[] buffer;
        delete [] m_data;
        buffer = nullptr;
        m_data = nullptr;
        m_writeIndex = 0;
        m_readIndex = 0;
        m_bInitializsed = false;
    }

    static void destroy(RingBuffer* rBuf) {
        delete rBuf;
    }

    static RingBuffer* create(unsigned int _size, unsigned int _channels) {
        auto* rBuf = new RingBuffer(_size, _channels);
        return rBuf;
    }

    Error_t write(float** data, int idx) {
        if (!m_bInitializsed)
            return kNotInitializedError;
        for (int c=0; c < m_channels; c++)
            buffer[c][m_writeIndex] = data[c][idx];
        advancePointer();
        return kNoError;
    }

    float* read() {
        for (int i=0; i< m_channels; i++)
            m_data[i] = buffer[i][m_readIndex];
        retreatPointer();
        return m_data;
    }

    void setSize(unsigned int _size) {
        m_max = (int) _size;
    }

    size_t getSize() {
        size_t _size = m_max;
        if (!full) {
            if (m_writeIndex > m_readIndex) {
                _size = m_writeIndex - m_readIndex;
            } else {
                _size = (m_max + m_writeIndex - m_readIndex);
            }
        }
        return _size;
    }

private:
    void advancePointer() {
        if (full)
            m_readIndex = (m_readIndex + 1) % m_max;

        m_writeIndex = (m_writeIndex + 1) % m_max;
        full = (m_writeIndex == m_readIndex);
    }

    void retreatPointer() {
        full = false;
        m_readIndex = (m_readIndex + 1) % m_max;
    }

    bool isEmpty() {
        return (!full && (m_writeIndex == m_readIndex));
    }

    void fillZeros() {
        for (int c=0; c < m_channels; c++) {
            for (int i=0; i < m_capacity; i++)
                buffer[c][i] = 0;
        }
    }
};

#endif //MUSI6106_RINGBUFFER_H
