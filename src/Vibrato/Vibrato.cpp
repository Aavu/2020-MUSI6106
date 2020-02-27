//
// Created by Raghavasimhan Sankaranarayanan on 2/16/20.
//

#ifndef MUSI6106_VIBRATO_CPP
#define MUSI6106_VIBRATO_CPP

#include <Vibrato.h>

#endif //MUSI6106_VIBRATO_CPP

CVibrato::CVibrato() {
    reset();
}

CVibrato::~CVibrato() {
    reset();
}

void CVibrato::create(CVibrato*& pCVibrato) {
    pCVibrato = new CVibrato();
}

void CVibrato::destroy(CVibrato* vibrato) {
    delete vibrato;
}

void CVibrato::reset() {
    m_fSampleRate = 0;
    m_fModFreqInSamples = 0;
    m_iWidth = 0;
    m_iDelay = 0;
    m_iLength = 0;
    for (int c=0; c < m_iNumChannels; c++)
        m_delayLine[c]->reset();
    m_iNumChannels = 0;
    m_bInitialized = false;
}

Error_t CVibrato::init(float fSampleRate, int iNumChannels, float fMaxDelayLengthInS) {
    if (fSampleRate <= 0 || iNumChannels <=0 ||fMaxDelayLengthInS < 0)
        return kFunctionInvalidArgsError;

    reset();

    m_fSampleRate = fSampleRate;
    m_iNumChannels = iNumChannels;
    m_iMaxDelayLength = CUtil::float2int<int>(fMaxDelayLengthInS * m_fSampleRate);

    initLFO();
    initBuffer();
    setBufferLength();
    m_bInitialized = true;
    return kNoError;
}

Error_t CVibrato::setParam(CVibrato::FilterParam_t eParam, float fParamValue) {
    if (!m_bInitialized)
        return kNotInitializedError;

    if (eParam == kParamWidth) {
        m_iWidth = CUtil::float2int<int>(fParamValue * m_fSampleRate);
//        for (int c = 0; c < m_iNumChannels; c++)
//            m_delayLine[c]->setReadIdx(std::ceil(fParamValue * m_fSampleRate / 2.0)); //TODO: check
    } else if (eParam == kParamModFreq) {
        m_fModFreqInSamples = fParamValue / m_fSampleRate;
        m_lfo->setFrequency(m_fModFreqInSamples);
    } else if (eParam == kParamDelay) {
        m_iDelay = std::min(CUtil::float2int<int>(fParamValue * m_fSampleRate), m_iMaxDelayLength);
    } else
        return kFunctionInvalidArgsError;

//    setBufferLength();
    return kNoError;
}

float CVibrato::getParam(CVibrato::FilterParam_t eParam) const {
    //TODO
    return 0;
}

Error_t CVibrato::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int i=0; i<iNumberOfFrames; i++) {
        auto mod = (1 + m_lfo->getPostInc())*0.5f;
//        auto tap = 1 + (float)m_iDelay + ((float)m_iWidth * mod);
        float tap = (float) m_iWidth * mod; //m_lfo->getPostInc();
        for (int c = 0; c < m_iNumChannels; c++) {
            m_delayLine[c]->putPostInc(ppfInputBuffer[c][i]);
            ppfOutputBuffer[c][i] = m_delayLine[c]->get(tap);
            m_delayLine[c]->getPostInc();
        }
    }
    return kNoError;
}

void CVibrato::initLFO() {
    m_lfo = new CLfo<float>(2048);
    m_lfo->setFrequency(m_fModFreqInSamples);
}

void CVibrato::setBufferLength() {
    m_iLength = m_iDelay + CUtil::float2int<int>((float) m_iWidth/2.0f) + 1;
    for (int c = 0; c < m_iNumChannels; c++)
        m_delayLine[c]->setLength(m_iLength);
}

int CVibrato::getLength() {
    return m_iLength;
}

void CVibrato::initBuffer() {
    m_delayLine = new CRingBuffer<float>*[m_iNumChannels];
    std::unique_ptr<float> zeros(new float[m_iMaxDelayLength]);
    for (int c = 0; c < m_iNumChannels; c++) {
        m_delayLine[c] = new CRingBuffer<float>(m_iMaxDelayLength);
        m_delayLine[c]->put(zeros.get(), m_iMaxDelayLength);
    }
}
