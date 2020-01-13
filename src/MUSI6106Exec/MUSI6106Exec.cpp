
#include <iostream>
#include <ctime>
#include <fstream>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"

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
    long long int           iNumFrames = kBlockSize;

//    clock_t                 time = 0;

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();

    Error_t err = kNoError;
    
    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    sInputFilePath = argv[1];
    sOutputFilePath = argv[2];
    
    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    err = CAudioFileIf::create(phAudioFile);
    if (err != kNoError) {
        cout << "error: " << err << endl;
        return 1;
    }
    
    err = phAudioFile->openFile(sInputFilePath, CAudioFileIf::FileIoType_t::kFileRead);
    if (err != kNoError) {
        cout << "error: " << err << endl;
        return 1;
    }
    
    err = phAudioFile->getFileSpec(stFileSpec);
    if (err != kNoError) {
        cout << "error: " << err << endl;
        return 1;
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath, std::fstream::out | std::fstream::app);
    hOutputFile.precision(16);
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[2];
    for (int i=0; i < stFileSpec.iNumChannels; i++) {
        ppfAudioData[i] = new float[kBlockSize];
    }
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    
    while (iNumFrames >= kBlockSize) {
        err = phAudioFile->readData(ppfAudioData, iNumFrames);
        if (err != kNoError) {
            cout << "error: " << err << endl;
            return 1;
        }
        if (!hOutputFile.is_open()) {
            cout << "Text file could not be opened." << endl;
            return 1;
        }
        for (int i=0; i < iNumFrames; i++) {
            hOutputFile << std::dec << ppfAudioData[0][i] << "\t" << ppfAudioData[1][i] << endl;
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    hOutputFile.close();
    phAudioFile->closeFile();
    for (int i=0; i < stFileSpec.iNumChannels; i++) delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    
    // all done
    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

