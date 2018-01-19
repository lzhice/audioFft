#ifndef AUDIORECODER_H
#define AUDIORECODER_H

#include <QObject>

class QAudioInput;
class QBuffer;


class AudioRecoder: public QObject
{
    Q_OBJECT
public:
    AudioRecoder();
    void start();
    void stop();
    int getRate() const;

public slots:
    void bufUpdated();
signals:
    void newData(std::vector<quint32> vec);
private:
    QBuffer* dev;
    QAudioInput* audio;
    int rate;
    int numChanels;
    int bitRate;
    int sampleRateToEmit;
};

#endif // AUDIORECODER_H
