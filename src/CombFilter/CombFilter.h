//
// Created by Raghavasimhan Sankaranarayanan on 1/18/20.
//

#ifndef __CombFilter_hdr__
#define __CombFilter_hdr__

#include "CombFilterIf.h"
#include "RingBuffer.h"
#include <functional>
#include <iostream>

class CCombFilterBase: public CCombFilterIf {
private:
    float m_fGain = 0;
    float m_fMaxDelayLength = 0;
    float m_fDelay = 0;
    float m_fSampleRate = 44100;
    int m_iNumChannels = 1;
    const int numFilterParams = 2;
    CombFilterType_t filterType = kCombFIR;
    RingBuffer* delayLine = nullptr;
    int length = 0;
    static Error_t zeros(float *arr, int length);

public:
    CCombFilterBase();
    ~CCombFilterBase() override;

//    std::function<Error_t (float **, float **, int )> processIntern;
    Error_t (CCombFilterBase::*processFunc)(float**, float**, int) = nullptr;
    Error_t processIntern( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t processInternIIR( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t processInternFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t setParamIntern (FilterParam_t eParam, float fParamValue);
    float getParamIntern (FilterParam_t eParam) const;

    Error_t init(CombFilterType_t eFilterType, float sampleRate, int numChannels, float maxDelayLength);

};

#endif //__CombFilter_hdr__
