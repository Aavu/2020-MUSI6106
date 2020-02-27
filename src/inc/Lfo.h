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

/*! \brief The LFO contains the wavetable buffer filled with 1 cycle of sine wave. If the user changes the frequency,
 * the buffer length is updated to accomodate 1 cycle of the wave.
*/
template <class T>
class CLfo {
private:
    int m_iSize = 0;
    float m_fFreqInSamples = 0;
    float m_fCurrentIndex = 0;
    float m_fSampleDelta = 0;
    CRingBuffer<T>* m_buffer;

    /*! Fills the buffer with sine wave
    \return Error_t
    */
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
    }

public:
    /*! Constructor for LFO. Allocates memory for the internal buffer and fills it up with sine wave.
    \param sampleRate: Sample rate of the audio samples
    \param fFreqInSamples: Desired Frequency of the LFO in samples
    \param maxLength: Maximum allowed length of the internal buffer
    */
    CLfo(int size): m_iSize(size)
    {
        m_buffer = new CRingBuffer<T>(m_iSize);
        generateSine();
    }

    ~CLfo() {
        delete m_buffer;
    }

    /*! Sets the frequency (in samples) of the LFO. This will also update the buffer size of LFO to fill 1 cycle.
    \param fFreqInSamples: Desired frequency (in samples)
    \return void
    */
    void setFrequency(float fFreqInSamples) {
        m_fFreqInSamples =  fFreqInSamples;
        updateSampleDelta();
    }

    /*! Get value from the current read index of the buffer.
    \return last read value from buffer
    */
    T getPostInc() {
        auto val = m_buffer->get(m_fCurrentIndex);
        incrementCurrentIndex();
        return val;
    }
};

#endif //MUSI6106_LFO_H
