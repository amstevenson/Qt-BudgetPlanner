#include "tableoperations.h"
#include "qdebug.h"
#include "qinputdialog.h"

TableOperations::TableOperations(QMainWindow *parent) : QMainWindow(parent)
{
    // default constructor
}

QStringList TableOperations::getRowInformation(QStandardItemModel *tableModel, int rowNumber)
{
    QStringList rowInformation;

    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        QModelIndex index = tableModel->index(i,rowNumber);

        // add information to the QStringList - if not null...
        if(tableModel->data(index).isNull() != true)
            rowInformation.append(tableModel->data(index).toString());
    }

    return rowInformation;
}

int TableOperations::getUserBudgetAmount(QStringList userBudgetAmount)
{
    int budgetTotal = 0;

    for(int i = 0; i < userBudgetAmount.size(); i++)
        budgetTotal += userBudgetAmount.at(i).toInt();

    return budgetTotal;
}

void TableOperations::setRowInformation(QStandardItemModel *tableModel, QStringList rowInformation, int rowNumber)
{
    for(int i = 0; i < rowInformation.size(); i++)
    {
        QStandardItem *rowItem;
        QModelIndex   rowIndex;

        // 0 appends a new row and 1 sets the second rows information
        switch(rowNumber)
        {
        case 0:
            // create and center the item
            rowItem = new QStandardItem(QString(rowInformation.at(i)));
            tableModel->appendRow(rowItem);

            rowIndex = tableModel->index(i,rowNumber);
            tableModel->setData(rowIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
            break;

        case 1:
            // enters the data for the second row
            rowItem = new QStandardItem(QString(rowInformation.at(i)));
            tableModel->setItem(i, rowNumber, rowItem);

            rowIndex = tableModel->index(i,rowNumber);
            tableModel->setData(rowIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
            break;

        default:
            break;
        }
    }
}

void TableOperations::addTableItemFromList(QStandardItemModel *tableModel, QStringList listInformation)
{
    bool ok;
    QString resultString = QInputDialog::getItem(this, tr("Enter in a type of income, or specify 'other'"),
                                                 tr("Type of income:"), listInformation, 0, false, &ok);

    QStandardItem *newItem = new QStandardItem(QString(resultString));
    tableModel->appendRow(newItem);

    // center the new row
    int newRowIndex = tableModel->rowCount() -1;

    QModelIndex index = tableModel->index(newRowIndex,0);
    tableModel->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
    index = tableModel->index(newRowIndex,1);
    tableModel->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
}

TABLESECTION::state TableOperations::getState(QString state)
{
    if (state == "INCOME")
        return TABLESECTION::INCOME;
    if (state == "EXPENSES")
        return TABLESECTION::EXPENSES;
    if (state == "BUDGET")
        return TABLESECTION::BUDGET;
    if (state == "FINISH")
        return TABLESECTION::FINISH;

    // Default value
    return TABLESECTION::INCOME;
}
