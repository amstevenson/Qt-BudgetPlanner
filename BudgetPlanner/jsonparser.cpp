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

/*
 * Creates a HTTPRequest using the QTNetwork modules classes.
 *
 * @param url    : the url where the script can be accessed from (QString).
 * @param method : either "GET" or "POST. The first for getting information and the second for inserting/changing.
 *                 both steps work in a very similar way, except for that some statements are skipped if the need for
 *                 inserting information is different. Afterall there is no point appending information to a url if
 *                 the need for it is not there.
 * @param params : appends information to the url based on a QMap of type QString, QString.
 * @see waitForFinish
 * @return A json object that contains information relating to the query that has been sent.
 *
 * */
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

        // Needs to use the read all function apparently to access data.
        getData = reply->readAll();

        // Create another bytearray structured as a json object
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(getData, &err);

        // Parse the QJsonDocument to a QJsonObject
        QJsonObject obj;
        if(doc.isObject() && err.error ==  QJsonParseError::NoError)
            obj = doc.object();

        delete manager;
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

        qDebug() << postData; // for debugging purposes

        // create another bytearray structured as a json object
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(postData, &err);
        QJsonObject obj;

        // convert to a jsonobject
        if(doc.isObject() && err.error ==  QJsonParseError::NoError)
            obj = doc.object();

        // return a jsonobject with "success" and "message" values
        delete manager;
        return obj;

    }
    else
    {
        // the method provided is not "get" or "post"
        QJsonObject obj;
        return obj;
    }
}

/*
 * A method that creates a connection between a QNetworkReply and a QEventLoop, in order
 * to avoid the continuation of the execution of code before the query has been processed
 * by the database and the reply collected.
 *
 * */
void JsonParser::waitForFinish(QNetworkReply *netReply)
{
    QEventLoop waitLoop;
    connect(netReply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
    waitLoop.exec();
}
