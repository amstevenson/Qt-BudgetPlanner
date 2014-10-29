#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include "QMap.h"

class JsonParser : public QMainWindow
{
    Q_OBJECT
public:
    JsonParser(QMainWindow *parent);
    QJsonObject makeHTTPRequest(QString, QString, QMap<QString, QString>);

public slots:

private:

    void waitForFinish(QNetworkReply *reply);
    QNetworkAccessManager *manager;
};

#endif // JSONPARSER_H
