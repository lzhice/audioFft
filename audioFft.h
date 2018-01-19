#ifndef FFT_H
#define FFT_H

#include <QObject>
#include <QBuffer>
#include <complex>
#include <QVector>
#include <QPair>

typedef std::complex<double> base;
typedef QPair<double, qint8> fm;// Frequency and midi of Note

class AudioFft: public QObject
{
    Q_OBJECT
public:
    AudioFft();
    void setRate(int rate);
private:
    void fft(std::vector<base> & a);
    int findIndexOfMax(std::vector<base> arr,int startInd, int endInd);
    void readFreqTable();
    qint8 findMidinotaByFrequency(double freq);

    int rate;
    bool fromBuffer;
    unsigned short* buf;
    double frequency;
    QVector<QPair<double, qint8> > FreqTable;
    const int size_N = 16384;
public slots:
    void calculate(std::vector<quint32> a);

};

#endif // FFT_H
