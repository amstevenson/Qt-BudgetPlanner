#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include "qmap.h"

class JsonParser : public QMainWindow
{
    Q_OBJECT
public:
    // Default constructor
    JsonParser(QMainWindow *parent);

    // The http request either gets, inserts, or changes information in the database.
    QJsonObject makeHTTPRequest(QString, QString, QMap<QString, QString>);

public slots:

private:

    // wait for finish works in conjunction with makeHTTPRequest.
    void waitForFinish(QNetworkReply *);

    // To allow us to perform networking operations.
    QNetworkAccessManager *manager;
};

#endif // JSONPARSER_H
