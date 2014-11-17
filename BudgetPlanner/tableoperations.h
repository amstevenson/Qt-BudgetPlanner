#ifndef TABLEOPERATIONS_H
#define TABLEOPERATIONS_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QModelIndex>
#include "qstringlist.h"

// Enum to determine the state of the setup ui form
namespace TABLESECTION
{
    enum state { INCOME, EXPENSES, BUDGET, FINISH };
}

class TableOperations : public QMainWindow
{
public:
    TableOperations(QMainWindow *parent);

    QStringList incomeType;

    void addTableItemFromList(QStandardItemModel *, QStringList);
    int getUserBudgetAmount(QStringList);
    QStringList getRowInformation(QStandardItemModel *, int);
    void setRowInformation(QStandardItemModel *, QStringList, int);

    TABLESECTION::state getState(QString);
};

#endif // TABLEOPERATIONS_H
