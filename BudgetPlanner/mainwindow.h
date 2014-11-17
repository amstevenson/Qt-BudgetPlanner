#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include "jsonparser.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "accountsetupwindow.h"
#include "accountbudgetwindow.h"
#include "signupwindow.h"
#include "operations.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void displayInformativeMessage(QString, QString);

private:
    Ui::MainWindow *ui;

private slots:
    void createNewWindow(QString);
};

#endif // MAINWINDOW_H
