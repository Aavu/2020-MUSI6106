
#include <iostream>
#include <ctime>
#include <unistd.h>
#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"
//#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfInputAudioData = 0;
    float                   **ppfOutputAudioData = 0;

    CAudioFileIf            *phInputAudioFile = 0;
    CAudioFileIf            *phOutputAudioFile = 0;

    CAudioFileIf::FileSpec_t stFileSpec;

    CVibrato *pCVibrato = nullptr;

    CVibratoParams cVibratoParams = {0, 0};

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments

    int options;
    while ((options = getopt(argc, argv, "i:o:w:f:")) != -1) {
        switch (options) {
            case 'i':
                sInputFilePath = optarg;
                break;
            case 'o':
                sOutputFilePath = optarg;
                break;
            case 'w':
                cVibratoParams.width = atof(optarg);
                break;
            case 'f':
                cVibratoParams.modFreq = atof(optarg);
                break;
        }
    }

    if (sInputFilePath.empty())
    {
        cout << "Missing audio input path!";
        return -1;
    }

    if (sOutputFilePath.empty()) {
        sOutputFilePath = sInputFilePath + "\b\b\b\b_out.wav";
    }

    cout << "inputpath " << sInputFilePath << endl;
    cout << "outputpath " << sOutputFilePath << endl << endl;
    cout << cVibratoParams << endl;

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);

    phInputAudioFile->getFileSpec(stFileSpec);

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);

    if (!phInputAudioFile->isOpen() || !phOutputAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfInputAudioData = new float *[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    CVibrato::create(pCVibrato);

    auto err = pCVibrato->init(stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels, 0.1);

    pCVibrato->setParam(CVibrato::kParamDelay, .02);
    pCVibrato->setParam(CVibrato::kParamModFreq, cVibratoParams.modFreq);
    pCVibrato->setParam(CVibrato::kParamWidth, cVibratoParams.width);

    if (err != kNoError)
        return -1;

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phInputAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);

        err = pCVibrato->process(ppfInputAudioData, ppfOutputAudioData, iNumFrames);
        if (err != kNoError)
            return -1;
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);

    CVibrato::destroy(pCVibrato);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;
    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}