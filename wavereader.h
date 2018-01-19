#ifndef WAVEREADER_H
#define WAVEREADER_H

#include <cstdint>
#include <QFile>
#include <iostream>
#include <memory>
using namespace std;

struct ChunkHeader
{
    ChunkHeader();
    uint8_t         ChunkName[4];
    uint32_t        ChunkSize;
    string getName();
};

struct WavHeader
{
    uint8_t         WAVE[4];        // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample
};

class WaveReader
{
public:
    WaveReader();
    ~WaveReader();
    bool verbose = false;
    void ReadFile(QString filePath);
    bool isError();
    void setLenBufData(int len);

    bool NextData(quint32 &buf);
    int getSamplesPerSec();
    int getChannelCount();
    int getBitPerSample();
private:
    bool openFile(QString filePath);
    void closeFile();

    void initReader();
    ChunkHeader readChunkHeader();
    void readHeader();
    int getFileSize();

    WavHeader wavHeader;
    QFile file;
    bool ok;
    int seekData;
    int seekChunk;
    int lenBufData;
};

#endif // WAVEREADER_H
