//
// Created by Raghavasimhan Sankaranarayanan on 2/17/20.
//

#ifndef MUSI6106_LFO_H
#define MUSI6106_LFO_H

#include <iostream>
#include "RingBuffer.h"
#include "Vector.h"
#include "Synthesis.h"

template <class T>
class CLfo {
private:
    float m_fSampleRate = 0;
    int m_iLength = 0;
    int m_iCapacity = 0;
    float m_fFreqInSamples = 0;
    CRingBuffer<T>* m_buffer;

    Error_t generateSine() {
        if (!m_buffer)
            return kNotInitializedError;

        for (int t=0; t < m_iLength; t++)
            m_buffer->putPostInc((T)sin(2*M_PI*m_fFreqInSamples*t));

        return kNoError;
    }

public:
    CLfo(float sampleRate, float fFreqInSamples, int maxLength): m_fSampleRate(sampleRate),
                                                            m_fFreqInSamples(fFreqInSamples),
                                                            m_iCapacity(maxLength),
                                                            m_iLength(maxLength)
    {
        m_buffer = new CRingBuffer<T>(m_iCapacity);
        generateSine();
    }

    ~CLfo() {

    }

    void setFrequency(float fFreqInSamples) {
        m_fFreqInSamples =  fFreqInSamples;
        m_iLength = (int) (1/m_fFreqInSamples);
        m_buffer->setLength(m_iLength);
        generateSine();
    }

    T getPostInc() {
        return m_buffer->getPostInc();
    }

};

#endif //MUSI6106_LFO_H
