//
// Created by Raghavasimhan Sankaranarayanan on 1/18/20.
//

#include "CombFilter.h"

CCombFilterBase::CCombFilterBase():
                                m_fGain(0),
                                m_fDelay(0),
                                m_fSampleRate(0),
                                m_iNumChannels(0),
                                m_fMaxDelayLength(0),
                                delayLine(nullptr),
                                filterType(kCombFIR),
                                length(0)
//                                processIntern(nullptr)
{
//    processIntern = std::bind(&CCombFilterBase::processInternIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

CCombFilterBase::~CCombFilterBase() {
    for (int i=0; i<m_iNumChannels; i++)
        delete [] delayLine[i];
    delete [] delayLine;
    delayLine = nullptr;
//    processIntern = nullptr;
}

Error_t CCombFilterBase::processIntern(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    float **feedBack = (filterType == kCombFIR) ? ppfInputBuffer : ppfOutputBuffer;
    for (int c=0; c < m_iNumChannels; c++) {
        for (int i = 0; i < iNumberOfFrames; i++) {
            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * delayLine[c][length - 1]);
            for (int j = length - 1; j > 0; j--) // Better way is to use ring buffer.
                delayLine[c][j] = delayLine[c][j - 1];
            delayLine[c][0] = feedBack[c][i];
        }
    }
    return kNoError;
}

//Error_t CCombFilterBase::processIntern(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
//    auto length = (m_fDelay == 0) ? 1 : (int)(m_fDelay*m_fSampleRate);
//    float **feedBack = (filterType == kCombFIR) ? ppfInputBuffer : ppfOutputBuffer;
//    for (int c=0; c < m_iNumChannels; c++) {
//        for (int i = 0; i < iNumberOfFrames; i++) {
//            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * delayLine[c]->read());
////            for (int j = length - 1; j > 0; j--) // Better way is to use ring buffer.
////                delayLine[c][j] = delayLine[c][j - 1];
//            delayLine[c]->write(feedBack[c][i]);
//        }
//    }
//    return kNoError;
//}

Error_t CCombFilterBase::setParamIntern(CCombFilterIf::FilterParam_t eParam, float fParamValue) {
    switch (eParam) {
        case kParamGain:
            m_fGain = fParamValue;
            break;

        case kParamDelay:
            m_fDelay = std::min(fParamValue, m_fMaxDelayLength);
            length = (m_fDelay == 0) ? 1 : (int)(m_fDelay*m_fSampleRate);
//            for (int i=0; i<m_iNumChannels; i++)
//                delayLine[i]->setSize((unsigned int) (m_fDelay*m_fSampleRate));
            break;

        default:
            return kFunctionInvalidArgsError;
    }
    return kNoError;
}

float CCombFilterBase::getParamIntern(CCombFilterIf::FilterParam_t eParam) const {
    switch (eParam) {
        case kParamGain:
            return m_fGain;

        case kParamDelay:
            return m_fMaxDelayLength;

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
    delayLine = new float* [m_iNumChannels];
//    delayLine = new RingBuffer*[m_iNumChannels];
    Error_t err = kNoError;
    for (int i=0; i < m_iNumChannels; i++) {
        delayLine[i] = new float[_length];
        err = zeros(delayLine[i], _length);
    }
//    for (int i=0; i < m_iNumChannels; i++) {
//        delayLine[i] = new RingBuffer(length);
//    }

//    if (filterType == kCombFIR)
//        processIntern = std::bind(&CCombFilterBase::processInternFIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
//    else
//        processIntern = std::bind(&CCombFilterBase::processInternIIR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    return err;
}

Error_t CCombFilterBase::processInternIIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int c=0; c < m_iNumChannels; c++) {
        for (int i = 0; i < iNumberOfFrames; i++) {
            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * delayLine[c][length - 1]);
            for (int j = length - 1; j > 0; j--) // Better way is to use ring buffer.
                delayLine[c][j] = delayLine[c][j - 1];
            delayLine[c][0] = ppfOutputBuffer[c][i];
        }
    }
    return kNoError;
}

Error_t CCombFilterBase::processInternFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int c=0; c < m_iNumChannels; c++) {
        for (int i = 0; i < iNumberOfFrames; i++) {
            ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (m_fGain * delayLine[c][length - 1]);
            for (int j = length - 1; j > 0; j--) // Better way is to use ring buffer.
                delayLine[c][j] = delayLine[c][j - 1];
            delayLine[c][0] = ppfInputBuffer[c][i];
        }
    }
    return kNoError;
}
