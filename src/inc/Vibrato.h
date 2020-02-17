//
// Created by Raghavasimhan Sankaranarayanan on 2/16/20.
//

#ifndef MUSI6106_VIBRATO_H
#define MUSI6106_VIBRATO_H

#include <iostream>
#include "RingBuffer.h"
#include "Synthesis.h"
#include "Lfo.h"

class CVibrato {
private:
    CRingBuffer<float>** m_delayLine;
    CLfo<float>* m_sineLUT;
    float m_sampleRate = 0;
    float m_modFreq = 0;
    int m_width = 0;
    int m_delay = 0;
    int m_length = 0;
    int m_channels = 0;
    bool b_initialized = false;

    CVibrato();
    ~CVibrato();

public:
    static void create(CVibrato*& pCVibrato);
    Error_t init(float sampleRate, int iNumChannels, float modFreq, float width);
    static void destroy(CVibrato* vibrato);

    enum FilterParam_t
    {
        kParamModFreq,
        kParamWidth
    };

    Error_t setParam (FilterParam_t eParam, float fParamValue);
    float   getParam (FilterParam_t eParam) const;

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

    void reset();

};


#endif //MUSI6106_VIBRATO_H
