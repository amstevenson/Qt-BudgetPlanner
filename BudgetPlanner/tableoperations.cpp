#include "tableoperations.h"
#include "qdebug.h"
#include "qinputdialog.h"
#include "qmessagebox.h"
#include "qjsonobject.h"
#include "qjsonarray.h"

TableOperations::TableOperations(QMainWindow *parent) : QMainWindow(parent)
{
    // default constructor
}

/*
 * A method to initialise and populate a QStringList that is dependant on the QStandardItemModel provided
 * and the table row specified.
 *
 * @param tableModel      : a QStandardItemModel containing a list of QStandardItems.
 * @param colNumber       : the col number of the QStandardItemModel that will be iterated through.
 *
 * @return rowInformation : a StringList containing all the row information for a specified column.
 *
 * */
QStringList TableOperations::getRowInformation(QStandardItemModel *tableModel, int colNumber)
{
    QStringList rowInformation;

    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        QModelIndex index = tableModel->index(i,colNumber);

        // add information to the QStringList - if not null...
        if(tableModel->data(index).isNull() != true)
            rowInformation.append(tableModel->data(index).toString());
    }

    return rowInformation;
}

/*
 * Checks if any cells are null for a given item model, for the amount of columns specified.
 * An error warning is triggered if a null value is detected.
 *
 * @param tableModel      : a QStandardItemModel containing a list of QStandardItems.
 * @param numberOfCols    : specifies how many columns will be checked for each row.
 *
 * @return isRowCellEmpty : false if no cell is null, else true.
 *
 * */
bool TableOperations::checkForNullRowCells(QStandardItemModel *tableModel, int numberOfCols)
{
    bool isRowCellEmpty = false;

    for(int j = 0; j < numberOfCols; j++)
        for(int i = 0; i < tableModel->rowCount(); i++)
        {
            QModelIndex columnData = tableModel->index(i, j);

            if(tableModel->data(columnData).isNull())
            {
                isRowCellEmpty = true;

                QMessageBox::warning(this, tr("Qt Budget Planner"),
                                    tr("An empty row was detected. \n\n"
                                    "Please check all of the rows and ensure that any empty rows are filled"),
                                    QMessageBox::Ok);
            }
            else isRowCellEmpty = false;
        }

    return isRowCellEmpty;
}

/*
 * Adds each value found in the specified QStringList together.
 *
 * @param userBudgetAmount : the QStringList that contains numerical data.
 * @return budgetTotal     : every amount added together.
 *
 * */
int TableOperations::getUserBudgetAmount(QStringList userBudgetAmount)
{
    int budgetTotal = 0;

    for(int i = 0; i < userBudgetAmount.size(); i++)
        budgetTotal += userBudgetAmount.at(i).toInt();

    return budgetTotal;
}

/*
 * Sets the row information for the QStandardItemModel by either creating a new item or accessing and changing the values
 * of individual cells.
 *
 * @param tableModel     : a QStandardItemModel containing a list of QStandardItems.
 * @param rowInformation : the information that contains each rows cell data.
 * @param colNumber      : the column number that relates to the row cells that are to be inserted.
 *
 * */
void TableOperations::setRowInformation(QStandardItemModel *tableModel, QStringList rowInformation, int colNumber)
{
    for(int i = 0; i < rowInformation.size(); i++)
    {
        QStandardItem *rowItem;
        QModelIndex   rowIndex;

        // "0" appends a new row and "1" sets the second columns information;
        // we do not need a case for the third column, as that is only viable in the budget step,
        // and after that step, the user will be sent to the budget window once everything is processed completely.
        // So therefore it is not necessary.
        switch(colNumber)
        {
        case 0:
            // create and center the item
            rowItem = new QStandardItem(QString(rowInformation.at(i)));
            tableModel->appendRow(rowItem);

            rowIndex = tableModel->index(i,colNumber);
            tableModel->setData(rowIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
            break;

        case 1:
            // enters the data for the second row
            rowItem = new QStandardItem(QString(rowInformation.at(i)));
            tableModel->setItem(i, colNumber, rowItem);

            rowIndex = tableModel->index(i,colNumber);
            tableModel->setData(rowIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
            break;

        case 2:

            // enters the data for the third row
            rowItem = new QStandardItem(QString(rowInformation.at(i)));
            tableModel->setItem(i, colNumber, rowItem);

            rowIndex = tableModel->index(i,colNumber);
            tableModel->setData(rowIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
            break;

        default:
            break;
        }
    }
}

/*
 * Adds a QStandardItem to a QStandardItemModel (a list of standard items).
 *
 * @param tableModel      : a QStandardItemModel containing a list of QStandardItems.
 * @param listInformation : a QStringList that contains the name of each possible item that can be added.
 *
 * */
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

/*
 * Retrieves all of the income and expenses items that belong to a user.
 *
 * @param userID : the identifer for the user for which the QMap will return the information of.
 *
 * @return A QMap<QString, QStringList> object that contains all information belonging to the user.
 *
 * */
QMap<QString,QStringList> TableOperations::getCategoryItems(QString userID)
{
    jsonParser = new JsonParser(this);

    // Add the userID that will be sent off as the parameter for the HTTPRequest
    QMap<QString,QString> qmapUserID;
    qmapUserID.insert("user_id", userID);

    QJsonObject postResponse = jsonParser->makeHTTPRequest("http://www.amstevenson.net/middleware/qtcreator/get_all_category_items.php",
                                                   "POST",qmapUserID);

    // Set up a QMap<QString, QStringList> that will be sent back containing all of the category item information.
    QMap<QString,QStringList> m_userInformation;

    // Category items array
    QJsonArray categoryItemsToArray = postResponse["categoryitems"].toArray();

    // Success of the HTTPRequest
    int success = postResponse["success"].toInt();

    // Define QStringLists to hold the category information
    QStringList categoryName, categoryAmount, categoryType, categoryBudget;

    // Seperate the values to eliminate ambiguity in terms of incomes and expenses.
    QStringList incomeNames,  expenseNames, incomeAmounts, expenseAmounts, budgetAmounts;

    // If we get a success of 0, notify user of the error
    // If we get a success of 1, store the values of category items in the QMap of StringLists
    switch(success)
    {
        case 0:

            // the user has not registered yet, so we do not need to do anything else.
            break;

        case 1:

            // Insert the StringList information that pertains to the category items collected from the database.
            foreach (const QJsonValue & value, categoryItemsToArray)
            {
                QJsonObject dbInformation = value.toObject();

                categoryName.append(dbInformation["category_name"].toString());
                categoryAmount.append(dbInformation["category_amount"].toString());
                categoryType.append(dbInformation["category_type"].toString());
                categoryBudget.append(dbInformation["category_budget"].toString());

                // QMap inserts for the StringLists
                m_userInformation.insert("category_name", categoryName);
                m_userInformation.insert("category_amount", categoryAmount);
                m_userInformation.insert("category_type", categoryType);
                m_userInformation.insert("category_budget", categoryBudget);
            }

            for(int i = 0; i < m_userInformation.value("category_type").size(); i++)
            {
                if(m_userInformation.value("category_type").at(i) == "income")
                {
                    incomeNames.append(m_userInformation.value("category_name").at(i));
                    incomeAmounts.append(m_userInformation.value("category_amount").at(i));
                    budgetAmounts.append(m_userInformation.value("category_budget").at(i));
                }
                else if(m_userInformation.value("category_type").at(i) == "expense")
                {
                    expenseNames.append(m_userInformation.value("category_name").at(i));
                    expenseAmounts.append(m_userInformation.value("category_amount").at(i));
                }
            }

            // Clear the previous QMap of QStringLists
            m_userInformation.clear();

            // Repopulate with sorted values
            // Income items
            m_userInformation.insert("income_types", incomeNames);
            m_userInformation.insert("income_type_amounts", incomeAmounts);
            m_userInformation.insert("income_budget_amounts", budgetAmounts);

            // Expense items
            m_userInformation.insert("expense_types", expenseNames);
            m_userInformation.insert("expense_type_amounts", expenseAmounts);
            break;

        default:
            break;
    }

    return m_userInformation;
}

/*
 * Enumerators to determine the state of the registration process.
 * There is a particular emphasis on the table/QStandardItemModel with regards to this particularly
 * in the forms that relate to the registration process and also the display of the budget information
 * and status.
 *
 * @param state                : a QString that determines the enum that is returned.
 *
 * @return TABLESECTION::state : a state is returned dependant on the parameter sent to the method.
 *
 * */
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
