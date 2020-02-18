//
// Created by Raghavasimhan Sankaranarayanan on 2/16/20.
//

#ifndef MUSI6106_VIBRATO_H
#define MUSI6106_VIBRATO_H

#include <iostream>
#include "RingBuffer.h"
#include "Synthesis.h"
#include "Lfo.h"
#include <algorithm>

class CVibrato {
private:
    CRingBuffer<float>** m_delayLine;
    CLfo<float>* m_lfo;
    float m_fSampleRate = 0;
    float m_fModFreqInSamples = 0;
    int m_iWidth = 0;
    int m_iDelay = 0;
    int m_iLength = 0;
    int m_iNumChannels = 0;
    int m_iMaxDelayLength = 0;
    bool m_bInitialized = false;

    CVibrato();
    ~CVibrato();

    void initLFO();
    void initBuffer();

    void calcLength();

public:
    static void create(CVibrato*& pCVibrato);
    Error_t init(float fSampleRate, int iNumChannels, float fMaxDelayLengthInS);
    static void destroy(CVibrato* vibrato);

    enum FilterParam_t
    {
        kParamModFreq,
        kParamWidth,
        kParamDelay,

        kNumFilterParams
    };

    Error_t setParam (FilterParam_t eParam, float fParamValue);
    float   getParam (FilterParam_t eParam) const;

    int getLength();

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

    void reset();

};


#endif //MUSI6106_VIBRATO_H
