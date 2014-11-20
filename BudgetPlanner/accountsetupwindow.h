#ifndef ACCOUNTSETUPWINDOW_H
#define ACCOUNTSETUPWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QtGui>
#include <QDebug>
#include <QSignalMapper>
#include <QMap>
#include <QInputDialog>
#include "tableoperations.h"
#include "jsonparser.h"

namespace Ui {
class AccountSetupWindow;
}

class AccountSetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountSetupWindow(QWidget *parent = 0);

    ~AccountSetupWindow();
    void setUserID(QString);

private slots:
    void removeTableItem(int);
    void addTableItem();
    void getTableValues();
    void addAllTableItems();
    void updateLabels();

private:
    Ui::AccountSetupWindow *ui;

    QMap<QString, QStringList> m_userInformation;
    QStringList m_columnHeaders;
    QStandardItemModel *m_model;
    QStandardItemModel getTableIndexItem(QStringList);
    QString m_userID;

    // @see tableOperations.h /.cpp
    TableOperations *tableOp;

    // Enum to control the state of the account setup form
    TABLESECTION::state m_accountRegistrationState;

    void updateTableSettings();
    void updateTableButtons();
};

#endif // ACCOUNTSETUPWINDOW_H
