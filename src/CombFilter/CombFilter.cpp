//
// Created by Raghavasimhan Sankaranarayanan on 1/18/20.
//

#include "CombFilter.h"

CCombFilterBase::CCombFilterBase()
{
//    processIntern = std::bind(&CCombFilterBase::processInternIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

CCombFilterBase::~CCombFilterBase() {
    RingBuffer::destroy(delayLine);
    delayLine = nullptr;
}

Error_t CCombFilterBase::setParamIntern(CCombFilterIf::FilterParam_t eParam, float fParamValue) {
    switch (eParam) {
        case kParamGain:
            m_fGain = std::max(std::min(fParamValue, 1.f), -1.f);
            return kNoError;

        case kParamDelay:
            m_fDelay = std::min(fParamValue, m_fMaxDelayLength);
            length = (m_fDelay == 0) ? 1 : (int)(m_fDelay*m_fSampleRate);
            delayLine->setSize(length);
            return (delayLine->getSize() > 0) ? kNoError : kUnknownError;

        default:
            return kFunctionInvalidArgsError;
    }
}

float CCombFilterBase::getParamIntern(CCombFilterIf::FilterParam_t eParam) const {
    switch (eParam) {
        case kParamGain:
            return m_fGain;

        case kParamDelay:
            return m_fDelay;

        case kNumFilterParams:
            return (float)numFilterParams;

        default:
            return kFunctionInvalidArgsError;
    }
}

Error_t CCombFilterBase::zeros(float *arr, int length) {
    if (!arr || length < 1)
        return kFunctionInvalidArgsError;

    for (int i=0; i < length; i++) arr[i] = 0;

    return kNoError;
}

Error_t CCombFilterBase::init(CombFilterType_t eFilterType, float sampleRate, int numChannels, float maxDelayLength) {
    if (sampleRate < 0 || numChannels < 0 || maxDelayLength < 0 || (eFilterType != kCombFIR && eFilterType != kCombIIR))
        return kFunctionInvalidArgsError;

    filterType = eFilterType;
    m_fSampleRate = sampleRate;
    m_iNumChannels = numChannels;
    m_fMaxDelayLength = maxDelayLength;

    auto _length = (int)(maxDelayLength*m_fSampleRate);
    delayLine = RingBuffer::create(_length, m_iNumChannels);
    processFunc = (filterType == kCombFIR) ? &CCombFilterBase::processInternFIR : &CCombFilterBase::processInternIIR;

    return kNoError;
}

Error_t CCombFilterBase::processIntern(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    return (this->*processFunc)(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t CCombFilterBase::processInternIIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    Error_t err = kNoError;
    for (int i = 0; i < iNumberOfFrames; i++) {
        auto* sample = delayLine->read();
        for (int c=0; c < m_iNumChannels; c++)
            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * sample[c]);
        err = delayLine->write(ppfOutputBuffer, i);
    }
    return err;
}

Error_t CCombFilterBase::processInternFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    Error_t err = kNoError;
    for (int i = 0; i < iNumberOfFrames; i++) {
        auto* sample = delayLine->read();
        for (int c=0; c < m_iNumChannels; c++)
            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * sample[c]);
        err = delayLine->write(ppfInputBuffer, i);
    }
    return err;
}