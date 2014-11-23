#ifndef ACCOUNTBUDGETWINDOW_H
#define ACCOUNTBUDGETWINDOW_H

#include <QMainWindow>
#include "accountsetupwindow.h"
#include "qstringlist.h"
#include "qmap.h"
#include "qstandarditemmodel.h"
#include "tableoperations.h"

namespace Ui {
class AccountBudgetWindow;
}

class AccountBudgetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountBudgetWindow(QWidget *parent = 0);
    ~AccountBudgetWindow();

    void setUserID(QString);

private slots:

    void changeUserBudget();

private:

    // Information specific to the user
    QMap<QString, QStringList> m_userInformation;
    QString m_userID;

    // Column Headers and table models
    QStringList m_incomeColumnHeaders, m_expenseColumnHeaders;
    QStandardItemModel *m_incomeModel, *m_expensesModel;

    // @see tableOperations.h /.cpp
    TableOperations *tableOp;

    void getCategoryItems();
    void tableSettings();
    void setTableValues();
    void updateLabels();

    Ui::AccountBudgetWindow *ui;
};

#endif // ACCOUNTBUDGETWINDOW_H
