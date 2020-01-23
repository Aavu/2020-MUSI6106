
#include <ctime>
#include <cmath>
#include <iostream>
#include <vector>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;
using std::string;

// local function declarations
void showClInfo();

void runTests();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char *argv[]) {
    std::string sInputFilePath,  //!< file paths
            sOutputFilePath;

    static int kBlockSize;

    clock_t time = 0;

    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;

    CAudioFileIf *phInputAudioFile = nullptr;
    CAudioFileIf *phOutputAudioFile = nullptr;
    CCombFilterIf *filter = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;

    float gain = 0.99;
    float delay = 0.01;

    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;

    bool testMode;

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2) {
        testMode = true;
        runTests();
        return 0;
    } else {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
    }

    std::string progName = argv[0];
    testMode = progName.substr(0, 16) == "MUSI6106ExecTEST";

    auto testNum = progName.substr(12, 6);
    if (testNum == "TEST_4" || testNum == "TEST_2") {
        if (strcmp(argv[3], "IIR") == 0)
            filterType = CCombFilterIf::kCombIIR;
    }

    if (testNum == "TEST_3")
        kBlockSize = (int) strtol(argv[4], nullptr, 10);
    else
        kBlockSize = 1024;

    if (testNum == "TEST_1") {
        gain = -1;
        delay = 0.01;
    }
    else if (testNum == "TEST_5") {
        gain = 0;
        delay = 0;
    }
    else {
        gain = 0.99;
        delay = 0.01;
    }

    if(!testMode)
        showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phInputAudioFile);
    CAudioFileIf::create(phOutputAudioFile);
    CCombFilterIf::create(filter);

    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        cout << "Input Wave file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wave file
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputAudioFile->isOpen()) {
        cout << "Output Wave file open error!";
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

    filter->init(filterType, 5, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    filter->setParam(CCombFilterIf::kParamGain, gain); //0.99
    filter->setParam(CCombFilterIf::kParamDelay, delay);

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phInputAudioFile->isEof()) {
        long long iNumFrames = kBlockSize;
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);

        Error_t err = filter->process(ppfInputAudioData, ppfOutputAudioData, (int)iNumFrames);
        if (err != kNoError)
            return -1;

//        if(!testMode) {
//            cout << "\r"
//                 << "reading and writing...";
//        }
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    if (!testMode) {
        cout << "\nreading/writing done in: \t" << (double)(clock() - time) / CLOCKS_PER_SEC << " seconds."
             << endl; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    }

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);

    CCombFilterIf::destroy(filter);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete [] ppfOutputAudioData[i];
    }

    delete[] ppfInputAudioData;
    delete [] ppfOutputAudioData;

    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;

    return 0;
}

std::string getString(bool s) {
    return (s ? "SUCCESS" : "FAILED");
}

void TEST_1() {
    std::string progName = "MUSI6106ExecTEST_1";
    std::string in = "tests/sine.wav";
    std::string out = progName + ".wav";
    std::string ft = "FIR";

    CAudioFileIf *phAudioFile = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;
    static const int kBlockSize = 1024;
    float **ppfAudioData = nullptr;

    CAudioFileIf::create(phAudioFile);

    char *argv[] = {(char*)progName.c_str(), (char*)in.c_str(), (char*)out.c_str(), (char*)ft.c_str()};
    main(4, argv);

    phAudioFile->openFile(out, CAudioFileIf::kFileRead);

    if (!phAudioFile->isOpen()) {
        cout << "Input Wave file open error!";
        return;
    }
    phAudioFile->getFileSpec(stFileSpec);

    ppfAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfAudioData[i] = new float[kBlockSize];
    }

    float err = 0;

    phAudioFile->setPosition((long long)kBlockSize);
    while (!phAudioFile->isEof()) {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++)
            for (int i = 0; i < iNumFrames; i++) {
                err += ppfAudioData[c][i];
            }
    }
//    cout << err << endl;
    cout << "Test 1 (FIR) : " << getString((std::abs(err) < 1e-3)) << endl;
    CAudioFileIf::destroy(phAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = nullptr;
}

void TEST_2() {
    std::string progName = "MUSI6106ExecTEST_2";
    std::string in = "tests/sine.wav";
    std::string out = progName + ".wav";
    std::string ft = "IIR";

    CAudioFileIf *phAudioFile = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;
    static const int kBlockSize = 1024;
    float **ppfAudioData = nullptr;

    CAudioFileIf::create(phAudioFile);

    char *argv[] = {(char*)progName.c_str(), (char*)in.c_str(), (char*)out.c_str(), (char*)ft.c_str()};
    main(4, argv);

    phAudioFile->openFile(out, CAudioFileIf::kFileRead);

    if (!phAudioFile->isOpen()) {
        cout << "Input Wave file open error!";
        return;
    }
    phAudioFile->getFileSpec(stFileSpec);

    long long int length = 0;
    phAudioFile->getLength(length);

    int numBlocks = length/kBlockSize + 1;
    float **amp = new float *[stFileSpec.iNumChannels];
    ppfAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfAudioData[i] = new float[kBlockSize];
        amp[i] = new float[numBlocks];
    }

    int frameCount = 0;

    while (!phAudioFile->isEof()) {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) {
            amp[c][frameCount] = 0;
            for (int i = 1; i < iNumFrames; i++) {
                amp[c][frameCount] += (std::abs(ppfAudioData[c][i]) / iNumFrames);
//                amp += (std::abs(ppfAudioData[c][i]) - std::abs(ppfAudioData[c][i - 1]));
            }
        }
        frameCount++;
    }

    bool passed = true;

    for (int c = 0; c < stFileSpec.iNumChannels; c++)
        for (int i=1; i < frameCount; i++) {
            if ((amp[c][i-1] - amp[c][i]) > 0.01) {
                cout << amp[c][i] << " < " << amp[c][i - 1] << endl;
                passed = false;
            }
        }
    
    cout << "Test 2 (IIR) : " << getString(passed) << endl;
    CAudioFileIf::destroy(phAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfAudioData[i];
        delete [] amp[i];
    }
    delete[] ppfAudioData;
    delete [] amp;
    ppfAudioData = nullptr;
}

void TEST_3() {
    string progName = "MUSI6106ExecTEST_3";
    string in = "tests/noise.wav";
    string ft[2] = {"FIR", "IIR"};
    string outName[2];
    CAudioFileIf *phAudioFile = nullptr;
    CAudioFileIf *phAudioFile2 = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;
    float **ppfAudioData = nullptr;
    float **ppfAudioData2 = nullptr;
    int kBlockSize = 1024;
    CAudioFileIf::create(phAudioFile);
    CAudioFileIf::create(phAudioFile2);

    for(auto &t: ft) {
        for (int i = 0; i < 2; i++) {
            string out = progName + "_" + std::to_string(i) + ".wav";
            outName[i] = out;
            kBlockSize = 512 * (i + 1);
            string s = std::to_string(kBlockSize);
            char *argv[] = {(char *) progName.c_str(), (char *) in.c_str(), (char *) out.c_str(),
                            (char *) t.c_str(),
                            (char *) s.c_str()};
            main(5, argv);
        }

        phAudioFile->openFile(outName[0], CAudioFileIf::kFileRead);
        phAudioFile2->openFile(outName[1], CAudioFileIf::kFileRead);

        if (!phAudioFile->isOpen() || !phAudioFile2->isOpen()) {
            cout << "Input Wave(s) file open error!";
            return;
        }

        phAudioFile->getFileSpec(stFileSpec);

        ppfAudioData = new float *[stFileSpec.iNumChannels];
        ppfAudioData2 = new float *[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            ppfAudioData[i] = new float[kBlockSize];
            ppfAudioData2[i] = new float[kBlockSize];
        }

        bool passed = true;

        while (!phAudioFile->isEof()) {
            long long iNumFrames = kBlockSize;
            phAudioFile->readData(ppfAudioData, iNumFrames);
            phAudioFile2->readData(ppfAudioData2, iNumFrames);
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
                for (int i = 0; i < iNumFrames; i++) {
                    if (ppfAudioData[c][i] != ppfAudioData2[c][i]) {
                        passed = false;
                        goto output;
                    }
                }
        }

        output:
        cout << "Test 3 (" << t << ") : " << getString(passed) << " | ";
    }
    cout << "\b\b" << endl;

    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(phAudioFile2);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfAudioData2[i];
        delete[] ppfAudioData[i];
    }
    delete[] ppfAudioData;
    delete[] ppfAudioData2;
    ppfAudioData = nullptr;
    ppfAudioData2 = nullptr;
}

void TEST_4() {
    string progName = "MUSI6106ExecTEST_4";
    string in = "tests/silence.wav";
    string out = progName + ".wav";
    string ft[2] = {"FIR", "IIR"};

    CAudioFileIf *phAudioFile = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;
    static const int kBlockSize = 1024;
    float **ppfAudioData = nullptr;

    CAudioFileIf::create(phAudioFile);

    for (auto & t : ft) {
        char *argv[] = {(char*)progName.c_str(), (char*)in.c_str(), (char*)out.c_str(), (char*)t.c_str()};
        main(4, argv);

        phAudioFile->openFile(out, CAudioFileIf::kFileRead);

        if (!phAudioFile->isOpen()) {
            cout << "Input Wave file open error!";
            return;
        }
        phAudioFile->getFileSpec(stFileSpec);

        ppfAudioData = new float *[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            ppfAudioData[i] = new float[kBlockSize];
        }

        float err = 0;

        while (!phAudioFile->isEof()) {
            long long iNumFrames = kBlockSize;
            phAudioFile->readData(ppfAudioData, iNumFrames);
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
                for (int i = 0; i < iNumFrames; i++) {
                    err += ppfAudioData[c][i];
                }
        }
        cout << "Test 4 (" << t << ") : " << getString(err == 0) << " | ";
    }
    cout << "\b\b" << endl;
    CAudioFileIf::destroy(phAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = nullptr;
}

void TEST_5() {
    std::string progName = "MUSI6106ExecTEST_5";
    std::string in = "tests/noise.wav";
    std::string out = progName + ".wav";
    std::string ft[2] = {"FIR", "IIR"};

    CAudioFileIf *phAudioFile = nullptr;
    CAudioFileIf *phAudioFile2 = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;
    float **ppfAudioData = nullptr;
    float **ppfAudioData2 = nullptr;
    int kBlockSize = 1024;
    CAudioFileIf::create(phAudioFile);
    CAudioFileIf::create(phAudioFile2);

    for (auto & t : ft) {
        char *argv[] = {(char*)progName.c_str(), (char*)in.c_str(), (char*)out.c_str(), (char*)t.c_str()};
        main(4, argv);

        phAudioFile->openFile(in, CAudioFileIf::kFileRead);
        phAudioFile2->openFile(out, CAudioFileIf::kFileRead);

        if (!phAudioFile->isOpen() || !phAudioFile2->isOpen()) {
            cout << "Input Wave file open error!";
            return;
        }

        phAudioFile->getFileSpec(stFileSpec);

        ppfAudioData = new float *[stFileSpec.iNumChannels];
        ppfAudioData2 = new float *[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            ppfAudioData[i] = new float[kBlockSize];
            ppfAudioData2[i] = new float[kBlockSize];
        }

        bool passed = true;

        while (!phAudioFile->isEof()) {
            long long iNumFrames = kBlockSize;
            phAudioFile->readData(ppfAudioData, iNumFrames);
            phAudioFile2->readData(ppfAudioData2, iNumFrames);
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
                for (int i = 0; i < iNumFrames; i++) {
                    if (ppfAudioData[c][i] != ppfAudioData2[c][i])
                        passed = false;
                }
        }

        cout << "Test 5 (" << t << ") : " << getString(passed) << " | ";
    }
    cout << "\b\b" << endl;
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(phAudioFile2);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfAudioData2[i];
        delete[] ppfAudioData[i];
    }
    delete[] ppfAudioData;
    delete[] ppfAudioData2;
    ppfAudioData = nullptr;
    ppfAudioData2 = nullptr;
}

void runTests() {
    showClInfo();
    cout << "Running tests... \n";
    TEST_1();
    TEST_2();
    TEST_3();
    TEST_4();
    TEST_5();
}

void showClInfo() {
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout << endl;
}
