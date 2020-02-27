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

/*! \brief A class to store the filter parameters
*/
class CVibratoParams {
public:
    float width;
    float modFreq;

    /*! friend function to print the parameters to console
    \return ostream
    */
    friend std::ostream& operator<<(std::ostream& os, const CVibratoParams& obj)
    {
        os << "width: " << obj.width << std::endl
           << "Mod freq: " << obj.modFreq;
        return os;
    }
};

/*! \brief The Vibrato filter is first initialized with the init method with the sampling rate, number of channels
 *  and the maximum allowed delay. Various filter parameters are set using the setParam method.
 *  The process method can then be called to actually process the incoming audio samples block-wise.
 *
 *  The Vibrato instance is created and destroyed using the create and destroy methods respectively.
*/
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

    /*! Initializes the LFO
    \return void
    */
    void initLFO();

    /*! Initializes the internal buffer
    \return void
    */
    void initBuffer();

    /*! Calculates and sets the length of the internal buffer based on the delay and width
    \return void
    */
    void setBufferLength();

public:
    /*! Creates a CVibrato instance
    \param pCVibrato: the pointer to store the created instance's address
    \return void
    */
    static void create(CVibrato*& pCVibrato);

    /*! Initializes the filter
    \param fSampleRate: Sample rate of the audio
    \param iNumChannels: Number of channels of audio
    \param fMaxDelayLengthInS: Maximum allowed delay (in seconds)
    \return Error_t
    */
    Error_t init(float fSampleRate, int iNumChannels, float fMaxDelayLengthInS);

    /*! Destroys the CVibrato instance
    \param vibrato: the instance to be destroyed
    \return void
    */
    static void destroy(CVibrato* vibrato);

    enum FilterParam_t
    {
        kParamModFreq,
        kParamWidth,
        kParamDelay,

        kNumFilterParams
    };

    /*! Sets the various filter parameters.
    \param eParam: Filter parameter to set
    \param fParamValue: Desired value
    \return Error_t
    */
    Error_t setParam (FilterParam_t eParam, float fParamValue);

    /*! Get the value of various filter parameters
    \param eParam: Desired parameter
    \return value of that parameter
    */
    float   getParam (FilterParam_t eParam) const;

    /*! Get the length of the internal buffer
    \return length of the internal buffer
    */
    int getLength();

    /*! Processes the incoming audio samples in blocks
    \param ppfInputBuffer: pointer to the input audio
    \param ppfOutputBuffer: pointer to where the output is to be stored
    \param iNumberOfFrames: number of frames to process
    \return Error_t
    */
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

    /*! Resets all the variables to its initial state
    \return void
    */
    void reset();

};


#endif //MUSI6106_VIBRATO_H
