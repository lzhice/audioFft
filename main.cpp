#include <QCoreApplication>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "wavereader.h"
#include "audiorecoder.h"
#include "audioFft.h"

using namespace std;


int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    qDebug()<<argc<<"!"<<argv[1]<<"!";
    if (argc == 1)
    {
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "-file") == 0)
    {
        const int size_N = 16384;

        QString filePath("72.wav");
        WaveReader reader;

        reader.verbose = true;
        reader.ReadFile(filePath);

        int rate = reader.getSamplesPerSec();

        qDebug() <<"MAIN:: read have error:"<<reader.isError();

        quint32 samples[size_N];

        reader.setLenBufData(size_N);
        qDebug()<<"read Next";
        if (!reader.NextData(samples[0]))
        {
            qDebug()<<"read data fail";
            return 1;
        }

        vector<quint32> a0;
        for (int i = 0; i < size_N; i++)
            a0.push_back(samples[i]);

        AudioFft f;
        f.setRate(rate);
        f.calculate(a0);
    }


    if (argc == 2 && strcmp(argv[1], "-mic") == 0)
    {
        qDebug()<<"mic";
        AudioRecoder ar;
        AudioFft f;
        f.setRate(ar.getRate());

        QObject::connect(&ar, SIGNAL(newData(std::vector<quint32>)),
                         &f, SLOT(calculate(std::vector<quint32>)));

        ar.start();

        qDebug()<<"OK";
    }
    return a.exec();
}
