
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CCombFilterIf::CCombFilterIf () :
    m_pCCombFilter((CCombFilterBase*)(this)),
    m_bIsInitialized(false),
    m_fSampleRate(0)
{
    // this never hurts
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}

int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}

const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create (CCombFilterIf*& pCCombFilter)
{
    pCCombFilter = new CCombFilterBase();
    if (!pCCombFilter)
        return kMemError;
    return kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = nullptr;

    return kNoError;
}

Error_t CCombFilterIf::init (CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{
    m_bIsInitialized = true;
    m_fSampleRate = fSampleRateInHz;
    return m_pCCombFilter->init(eFilterType, fSampleRateInHz, iNumChannels, fMaxDelayLengthInS);
}

Error_t CCombFilterIf::reset ()
{
    Error_t eErr = init(kCombFIR, 1, 44100, 2);
    if (eErr != kNoError)
        return eErr;

    return eErr;
}

Error_t CCombFilterIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    if (!ppfInputBuffer || !ppfInputBuffer[0] || !ppfOutputBuffer || !ppfOutputBuffer[0] || iNumberOfFrames < 0)
        return kFunctionInvalidArgsError;

    if (!m_bIsInitialized)
        return kNotInitializedError;

    return m_pCCombFilter->processIntern(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t CCombFilterIf::setParam (FilterParam_t eParam, float fParamValue)
{
    return m_pCCombFilter->setParamIntern(eParam, fParamValue);
}

float CCombFilterIf::getParam (FilterParam_t eParam) const
{
    return m_pCCombFilter->getParamIntern(eParam);
}
