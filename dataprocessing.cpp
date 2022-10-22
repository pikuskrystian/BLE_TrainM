#include "dataprocessing.h"

DataProcessing::DataProcessing(QObject *parent)
    : QObject{parent}
{
    server = "https://danielkrol.pwsztar.edu.pl/iot/dev";
    control.resize(16);

    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &DataProcessing::managerFinished);
}

void DataProcessing::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        if(reply->error() == QNetworkReply::ContentNotFoundError) {
            // onSendValue(0,0); // create json array if not exist
            // onRequest();
        }
    }
    emit statusChange(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());

    answer = reply->readAll();
    //    qDebug() << answer.size();
    //    qDebug() << answer;
}

void DataProcessing::onRequest()
{
    QString requeststring=server+QString().asprintf("%02d", deviceid)+"/data1.dat";
    request.setUrl(QUrl(requeststring));
    manager->get(request);
}

void DataProcessing::onCreateData(QByteArray &ch1, QByteArray &ch2, QByteArray &ch3, QByteArray &ch4, QByteArray &ch5)
{
    qDebug()<<"CreateData"<<ch1.size()<<ch2.size()<<ch3.size()<<ch4.size()<<ch5.size();

    QByteArray ba;
    ba.resize(5*sizeof(quint32));
    quint32 *pba = reinterpret_cast<quint32*>(ba.data());

    pba[0]=ch1.size();
    pba[1]=ch2.size();
    pba[2]=ch3.size();
    pba[3]=ch4.size();
    pba[4]=ch5.size();


    QByteArray pack;

    pack.clear();
    pack.append(ba);
    pack.append(ch1);
    pack.append(ch2);
    pack.append(ch3);
    pack.append(ch4);
    pack.append(ch5);

    onSendValue(pack);

//    qDebug()<<"Pack size:"<<pack.size()<<ba.size()<<pba[0];
}

void DataProcessing::onSendValue(QByteArray &data)
{

    QByteArray postDataSize = QByteArray::number(data.size());
    QUrl serviceURL(server+QString().asprintf("%02d", deviceid)+"/");
    QNetworkRequest request(serviceURL);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", postDataSize);
    manager->post(request, data);
}
