#include "jsonparser.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "QtNetwork/QNetworkRequest"
#include "QBuffer"
#include "QJsonDocument"
#include "QEventLoop"

JsonParser::JsonParser(QMainWindow *parent) : QMainWindow(parent)
{
    // default constructor
}

QJsonObject JsonParser::makeHTTPRequest(QString url, QString method, QMap<QString, QString> params)
{

    if(method == "GET")
    {
        QByteArray getData;
        manager = new QNetworkAccessManager(this); // if not specified, r/w access violations can occur

        QNetworkRequest request;
        request.setUrl(QUrl(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-httpd-php");

        QNetworkReply *reply = manager->get(request);
        waitForFinish(reply);

        // needs to use the read all function apparently to access data.
        getData = reply->readAll();

        // create another bytearray structured as a json object
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(getData, &err);

        // Parse the QJsonDocument to a QJsonObject
        QJsonObject obj;
        if(doc.isObject() && err.error ==  QJsonParseError::NoError)
            obj = doc.object();

        return obj;

    } else if(method == "POST")
    {
        manager = new QNetworkAccessManager(this); // if not specified, r/w access violations can occur

        QNetworkRequest request;
        request.setUrl(QUrl(url)); // set the url to be that as defined in the method constructor
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QByteArray postData;

        // for each key in the params list <string, string>, add the post parameters to the QByteArray
        for(int i = 0; i < params.size(); i++)
        {
            QString paramData1 = params.keys().at(i);                   // key name
            QString paramData2 = params.value(params.keys().at(i));     // key value

            postData.append(paramData1).append("=").append(paramData2).append("&");
        }

        // Submit the query to the php script
        QNetworkReply *reply = manager->post(request, postData);

        // Wait until the network query has finished being processed
        waitForFinish(reply);

        // Clear QByteArray
        postData.clear();

        // Check the redirect and retrieve all of the data from the middleware script
        postData = reply->readAll();

        qDebug() << postData;

        // create another bytearray structured as a json object
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(postData, &err);
        QJsonObject obj;

        // convert to a jsonobject
        if(doc.isObject() && err.error ==  QJsonParseError::NoError)
            obj = doc.object();

        // return a jsonobject with "success" and "message" values
        return obj;

    }
    else
    {
        // the method provided is not "get" or "post"
        QJsonObject obj;
        return obj;
    }
}

void JsonParser::waitForFinish(QNetworkReply *reply)
{
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}
