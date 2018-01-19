#include "audioFft.h"
#include <QDebug>
#include <QFile>
#include <iostream>
#include <vector>

int AudioFft::findIndexOfMax(std::vector<base> arr,int startInd, int endInd)
{
    int indMax = startInd;
    double max = abs(arr[startInd]);
    for (int i=startInd+1;i<endInd;i++)
        if (abs(arr[i])>max)
        {
            max = abs(arr[i]);
            indMax = i;
        }

    return indMax;
}

void AudioFft::readFreqTable()
{
    QFile f("spectr_notes.txt");
    f.open(QFile::ReadOnly|QFile::Text);
    if (!f.isOpen())
    {
        qDebug()<<"error read Freaq Table";
        return;
    }

    while (!f.atEnd())
    {
        QString line = f.readLine();
        auto sp = line.split('\t');
        qint8 i = sp.first().toInt();
        double d = sp.last().toDouble();
        FreqTable.append(fm(d,i));
    }
    f.close();
}




qint8 AudioFft::findMidinotaByFrequency(double freq)
{
    if (FreqTable.size() == 0)
        readFreqTable();


    int N = FreqTable.size();
    if (N==0)
        return -1;

    int l =0;
    int r = N - 1;
    qint8 midi;
    while (r > l+1)
    {
        int m = (l+r)/2;
        if (freq>FreqTable[m].first)
            l = m;
        else
            r = m;
    }

    auto midFreq = (FreqTable[l].first + FreqTable[r].first)/2;

    if (midFreq>freq)
        midi = FreqTable[l].second;
    else
        midi = FreqTable[r].second;
    return midi;
}

AudioFft::AudioFft()
{
    fromBuffer = true;
    rate = 0;
}

void AudioFft::setRate(int rate)
{
    this->rate = rate;
}
template <typename T>
void writeAbsArrayToFile(const T array, int size, const char* filename)
{

    QFile f(filename);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream writeStream(&f);


    for (int i=0;i<size;i++)
        writeStream << abs(array[i])<<endl;

    f.close();
}


void AudioFft::calculate(std::vector<quint32> arr)
{
    qDebug()<<"calculate";
    std::vector<base> a0;
    for (int i=0;i<size_N;i++)
        a0.push_back(1.0*arr[i]/size_N);

    //writeAbsArrayToFile(a0, int(size_N*0.9), "input.txt");
    fft(a0);
    //writeAbsArrayToFile(a0, int(size_N*0.9), "output.txt");

    int ind = findIndexOfMax(a0, 10, 0.9*a0.size());
    frequency = 1.0*ind*rate/size_N;
    uint midi = findMidinotaByFrequency(frequency);
    qDebug() << "frec ="<< frequency<<"midi ="<<midi;
}

void AudioFft::fft (std::vector<base> & a) {
    int n = a.size();
    if (n == 1) return;

    std::vector<base> a0 (n/2),  a1 (n/2);
    for (int i=0, j=0; i<n; i+=2, ++j) {
        a0[j] = a[i];
        a1[j] = a[i+1];
    }
    fft (a0);
    fft (a1);

    double ang = 2*M_PI/n;
    base w (1),  wn (cos(ang), sin(ang));
    for (int i=0; i<n/2; i++) {
        auto w2 = w * a1[i];
        a[i] = a0[i] + w2;
        a[i+n/2] = a0[i] - w2;
        w *= wn;
    }
}
