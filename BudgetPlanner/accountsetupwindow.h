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
#include "tableoperations.h"

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

    QString m_userID;

    QMap<QString, QStringList> m_userInformation;

    QStringList m_columnHeaders;

    QStandardItemModel *m_model;

    void updateTableSettings();
    void updateTableButtons();

    QStandardItemModel getTableIndexItem(QStringList);

    TableOperations *tableOp;

    TABLESECTION::state m_accountRegistrationState;
};

#endif // ACCOUNTSETUPWINDOW_H
