#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Vibrato.h"
#include "Synthesis.h"
#include "Vector.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData():  m_pCVibrato(nullptr),
                        m_ppfInputData(nullptr),
                        m_ppfOutputData(nullptr),
                        m_iDataLength(35131),
                        m_iBlockLength(171),
                        m_iNumChannels(3),
                        m_fSampleRate(8000),
                        m_fWidth(.05f),
                        m_fModFreq(2.f)
        {
            CVibrato::create(m_pCVibrato);
            m_ppfInputData  = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp   = new float*[m_iNumChannels];
            m_ppfOutputTmp  = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i]  = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);
            }
        }

        ~VibratoData()
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;

            CVibrato::destroy(m_pCVibrato);
        }

        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c]   = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pCVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        void TestProcessInplace()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pCVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        CVibrato  *m_pCVibrato;
        float   **m_ppfInputData,
                **m_ppfOutputData,
                **m_ppfInputTmp,
                **m_ppfOutputTmp;
        int     m_iDataLength;
        int     m_iBlockLength;
        int     m_iNumChannels;
        float   m_fSampleRate;
        float   m_fWidth,
                m_fModFreq;
    };

    TEST_FIXTURE(VibratoData, ZeroInput)
    {
        m_pCVibrato->init(m_fSampleRate, m_iNumChannels, m_fModFreq, m_fWidth);
        TestProcess();

        for (int c=0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);

        m_pCVibrato->reset();
    }

    TEST_FIXTURE(VibratoData, DCInput)
    {
        m_pCVibrato->init(m_fSampleRate, m_iNumChannels, m_fModFreq, m_fWidth);

        for (int i = 0; i < m_iNumChannels; i++)
            CVectorFloat::setValue(m_ppfInputData[i], 0.5, m_iDataLength);

        TestProcess();

        auto width = (int) std::roundf(m_fWidth * m_fSampleRate);
        auto length = (int)(2 + width + width*2);
        for (int c=0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c] + length, m_ppfOutputData[c] + length, m_iDataLength - length, 1e-3);

        m_pCVibrato->reset();
    }

}

#endif //WITH_TESTS