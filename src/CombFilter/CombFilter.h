//
// Created by Raghavasimhan Sankaranarayanan on 1/18/20.
//

#ifndef __CombFilter_hdr__
#define __CombFilter_hdr__

#include "CombFilterIf.h"
#include <functional>
#include <iostream>

//template <class T>
class RingBuffer {
private:
    float *buffer;
    int m_writeIndex;
    int m_readIndex;
    int max;
    bool bInitializsed;
    bool full;
    unsigned int capacity;
//    unsigned int m_channels;

public:
    explicit RingBuffer(unsigned int _size): capacity(_size), full(false), max(0) {

        buffer = new float [_size];
//        for (int i=0; i<channels; i++)
//            buffer[i] = new float [_size];

        m_writeIndex = 0;
        m_readIndex = 0;
        if (buffer)
            bInitializsed = true;
        fillZeros();
    }

    ~RingBuffer() {
        delete [] buffer;
        buffer = nullptr;
        m_writeIndex = 0;
        m_readIndex = 0;
        bInitializsed = false;
    }

    void write(float data) {
        buffer[m_writeIndex] = data;
        advancePointer();
    }

    float read() {
        if (!isEmpty()) {
            float data = buffer[m_readIndex];
            retreatPointer();
            return data;
        }
        return 0;
    }

    void setSize(unsigned int _size) {
        max = (int) _size;
    }

private:
    void advancePointer() {
        if (full)
            m_readIndex = (m_readIndex + 1) % max;

        m_writeIndex = (m_writeIndex + 1) % max;
        full = (m_writeIndex == m_readIndex);
    }

    void retreatPointer() {
        full = false;
        m_readIndex = (m_readIndex + 1) % max;
    }

    bool isEmpty() {
        return (!full && (m_writeIndex == m_readIndex));
    }

    size_t size() {
        size_t _size = max;
        if (!full) {
            if (m_writeIndex > m_readIndex) {
                _size = m_writeIndex - m_readIndex;
            } else {
                _size = (max + m_writeIndex - m_readIndex);
            }
        }
        return _size;
    }

    void fillZeros() {
        for (int i=0; i < max; i++)
            this->write(0);
    }
};

class CCombFilterBase: public CCombFilterIf {
private:
    float m_fGain;
    float m_fMaxDelayLength;
    float m_fDelay;
    float m_fSampleRate;
    int m_iNumChannels;
    const int numFilterParams = 2;
    CombFilterType_t filterType;
    float** delayLine;
//    RingBuffer** delayLine;
    int length;
    static Error_t zeros(float *arr, int length);

public:
    CCombFilterBase();
    ~CCombFilterBase() override;

//    std::function<Error_t (float **, float **, int )> processIntern;
    Error_t processIntern( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t processInternIIR( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t processInternFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t setParamIntern (FilterParam_t eParam, float fParamValue);
    float getParamIntern (FilterParam_t eParam) const;

    Error_t init(CombFilterType_t eFilterType,
                float sampleRate,
                 int numChannels,
                 float maxDelayLength);

};

#endif //__CombFilter_hdr__
