#include "audiorecoder.h"
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QDebug>
#include <QBuffer>

AudioRecoder::AudioRecoder()
{    
    QAudioFormat format;
    rate = 44100;
    numChanels = 2;
    bitRate = 16;
    sampleRateToEmit = 16384;

    format.setSampleRate(rate);
    format.setChannelCount(numChanels);
    format.setSampleSize(bitRate);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        format = info.nearestFormat(format);
    }

    audio = new QAudioInput(info, format, this);

    dev = new QBuffer(this);
    dev->open(QBuffer::ReadWrite);
}

void AudioRecoder::start()
{
    audio->start(dev);
    connect(dev, SIGNAL(bytesWritten(qint64)),
            SLOT(bufUpdated()));
}

void AudioRecoder::stop()
{
    audio->stop();
}

void AudioRecoder::bufUpdated()
{
    if (dev->size()>sampleRateToEmit*numChanels*bitRate/8)
    {
        QByteArray bytes = dev->data();

        qDebug()<<bytes.size()<<" "<<dev->size();
        if (bitRate == 16)
        {
            const unsigned short* data = reinterpret_cast<const unsigned short*>(dev->data().data());

            std::vector<quint32> vec;
            for (int i=0;i<sampleRateToEmit;i++)
                vec.push_back(data[i*numChanels]);

            dev->buffer().remove(0, dev->size());

            emit newData(vec);
        }
    }
}

int AudioRecoder::getRate() const
{
    return rate;
}
