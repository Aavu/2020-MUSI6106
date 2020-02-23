//
// Created by Raghavasimhan Sankaranarayanan on 2/17/20.
//

/* Implementation inspired by wavetable synth JUCE tutorial */

#ifndef MUSI6106_LFO_H
#define MUSI6106_LFO_H

#include <iostream>
#include <cmath>
#include "RingBuffer.h"
#include "Vector.h"
#include "Synthesis.h"

template <class T>
class CLfo {
private:
    int m_iSize = 0;
    float m_fFreqInSamples = 0;
    float m_fCurrentIndex = 0;
    float m_fSampleDelta = 0;
    CRingBuffer<T>* m_buffer;

    Error_t generateSine() {
        if (!m_buffer)
            return kNotInitializedError;

        for (int t=0; t < m_iSize; t++)
            m_buffer->putPostInc((T)sin(2*M_PI*(1.0/m_iSize)*t));

        return kNoError;
    }

    void incrementCurrentIndex() {
        m_fCurrentIndex = std::fmod((m_fCurrentIndex + m_fSampleDelta), m_iSize); // float modulo
    }

    void updateSampleDelta() {
        m_fSampleDelta = m_iSize * m_fFreqInSamples;
//        std::cout << "updateSampleDelta " << m_fSampleDelta << std::endl;
    }

public:
    CLfo(int size): m_iSize(size)
    {
        m_buffer = new CRingBuffer<T>(m_iSize);
        generateSine();
    }

    ~CLfo() {
        delete m_buffer;
    }

    void setFrequency(float fFreqInSamples) {
        m_fFreqInSamples =  fFreqInSamples;
        updateSampleDelta();
    }

    T getPostInc() {
        auto val = m_buffer->get(m_fCurrentIndex);
        incrementCurrentIndex();
        return val;
    }
};

#endif //MUSI6106_LFO_H
