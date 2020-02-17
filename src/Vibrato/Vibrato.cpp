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
    vibrato = nullptr;
}

void CVibrato::reset() {
    m_sampleRate = 0;
    m_modFreq = 0;
    m_width = 0;
    m_delay = 0;
    m_length = 0;
    for (int c=0; c < m_channels; c++)
        m_delayLine[c]->reset();
    m_channels = 0;
    delete m_sineLUT;
    b_initialized = false;
}

Error_t CVibrato::init(float sampleRate, int iNumChannels, float modFreq, float width) {
    if (sampleRate <= 0 || modFreq < 0 || width < 0)
        return kFunctionInvalidArgsError;
    m_sampleRate = sampleRate;
    m_modFreq = modFreq / m_sampleRate;
    m_width = (int) std::roundf(width * m_sampleRate);
    m_delay = m_width;
    m_length = 2 + m_delay + m_width*2;
    m_channels = iNumChannels;

    m_sineLUT = new CLfo<float>(m_sampleRate, modFreq, (int)(1/m_modFreq));

    m_delayLine = new CRingBuffer<float>*[m_channels];
    for (int c = 0; c < m_channels; c++)
        m_delayLine[c] = new CRingBuffer<float>(m_length);

    b_initialized = true;
    return kNoError;
}

Error_t CVibrato::setParam(CVibrato::FilterParam_t eParam, float fParamValue) {
    if (!b_initialized)
        return kNotInitializedError;

    if (eParam == kParamWidth)
        m_width = (int) (fParamValue * m_sampleRate);
    else if (eParam == kParamModFreq)
        m_modFreq = fParamValue * m_sampleRate;
    else
        return kFunctionInvalidArgsError;

    return kNoError;
}

float CVibrato::getParam(CVibrato::FilterParam_t eParam) const {
    //TODO
    return 0;
}

Error_t CVibrato::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int i=0; i<iNumberOfFrames; i++) {
        auto mod = m_sineLUT->getPostInc();
        auto tap = 1.f + (float)m_delay + ((float)m_width * mod);
        for (int c = 0; c < m_channels; c++) {
            m_delayLine[c]->putPostInc(ppfInputBuffer[c][i]);
            ppfOutputBuffer[c][i] = m_delayLine[c]->get(tap);
            m_delayLine[c]->getPostInc();
        }
    }

    return kNoError;
}
