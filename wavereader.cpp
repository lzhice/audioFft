#include "wavereader.h"
#include <string>
#include <QDebug>


WaveReader::WaveReader()
{
    //file = nullptr;
    lenBufData = 1024;
}

WaveReader::~WaveReader()
{
    closeFile();
}

void WaveReader::ReadFile(QString filePath)
{
    initReader();

    if (!openFile(filePath))
        return;

    qDebug()<<"Open File:"<<filePath<<"OK";

    ChunkHeader cHeader;

    while (cHeader.getName() != "data")
    {

        cHeader = readChunkHeader();
        if (!ok)
        {
            return;
        }
        cout<<"read chunk: '"<<cHeader.getName()<<"' OK size="<<cHeader.ChunkSize<<endl;

        if (cHeader.getName() == "RIFF")
        {
            readHeader();
            seekChunk += wavHeader.Subchunk1Size;
        }
        else
            seekChunk += cHeader.ChunkSize;
    }
    seekData = seekChunk - cHeader.ChunkSize;
}

bool WaveReader::isError()
{
    return !ok;
}

void WaveReader::setLenBufData(int len)
{
    lenBufData = len;
}

bool WaveReader::NextData(quint32 &buf)
{
    file.seek(seekData);
    qint64 countBytes = lenBufData*getChannelCount()*getBitPerSample()/8;
    QByteArray ba = file.read(countBytes);
    qDebug()<<"seek="<<seekData << " size bits ="<<ba.size();
    if (ba.size() != countBytes)
        return false;

    seekData += countBytes;
    quint32* arr = &buf;


    int bitInSample = this->getBitPerSample();
    if (bitInSample == 16)
    {
        const unsigned short* data = reinterpret_cast<const unsigned short*>(ba.data());

        for (int nSample =0; nSample < lenBufData; nSample++)
            arr[nSample] = data[nSample*this->getChannelCount()];
    }


    return true;
}

int WaveReader::getSamplesPerSec()
{
    return wavHeader.SamplesPerSec;
}

int WaveReader::getChannelCount()
{
    return wavHeader.NumOfChan;
}

int WaveReader::getBitPerSample()
{
    return wavHeader.bitsPerSample;
}

bool WaveReader::openFile(QString filePath)
{
    file.setFileName(filePath);
    file.open(QFile::ReadOnly);
    qDebug() <<"Open File start" <<filePath;

    if (!file.isOpen())
    {
        ok = false;
        return false;
    }
    return true;
}


void WaveReader::closeFile()
{
    ok = false;
    file.close();
}

void WaveReader::initReader()
{
    seekData = 0;
    seekChunk = 0;
    ok = true;
}

ChunkHeader WaveReader::readChunkHeader()
{
    int headerSize = sizeof(ChunkHeader);
    ChunkHeader header;
    file.seek(seekChunk);
    QByteArray byteArr = file.read(sizeof(ChunkHeader));
    header = *reinterpret_cast<ChunkHeader*>(byteArr.data());

    seekChunk += headerSize;

    return header;
}

void WaveReader::readHeader()
{

    int headerSize = sizeof(wavHeader), filelength = 0;

    //Read the header
    QByteArray byteArr = file.read(headerSize);
    wavHeader = *reinterpret_cast<WavHeader*>(byteArr.data());

    //size_t bytesRead = fread(&wavHeader, 1, headerSize, file);
    wavHeader.Subchunk1Size += 12;// add 12 byte for subheader
    //cout << "Header Read " << bytesRead << " bytes." << endl;
    //if (bytesRead > 0)
    {
        //Read the data

        filelength = getFileSize();

        if (verbose)
        {

            cout << "File is                    :" << filelength << " bytes." << endl;
            cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
            cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;

            // Display the sampling Rate from the header
            cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
            cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
            cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
            cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
            //cout << "Data length                :" << header.Subchunk2Size << endl;
            cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
            // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

            cout << "Block align                :" << wavHeader.blockAlign << endl;
        }
    }
}

int WaveReader::getFileSize()
{
    return file.size();
}



ChunkHeader::ChunkHeader()
{
    ChunkName[0] = 0;
}

string ChunkHeader::getName()
{
    string s;
    for (int i=0;i<4;i++)
        s.push_back(char(this->ChunkName[i]));
    return s;
}
