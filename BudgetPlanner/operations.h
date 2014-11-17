#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QMainWindow>
#include <QMessageBox>
#include "qicon.h"
#include "qstyle.h"
#include <QTableView>
#include <QSignalMapper>

class Operations : public QMainWindow
{
public:
    Operations(QMainWindow *parent);

    void displayInformativeMessage(QString, QString);
    void displayWarningMessage(QString, QString);
};

#endif // OPERATIONS_H
