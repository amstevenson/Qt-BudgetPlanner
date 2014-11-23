#include "accountbudgetwindow.h"
#include "ui_accountbudgetwindow.h"
#include "jsonparser.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qmessagebox.h"

AccountBudgetWindow::AccountBudgetWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AccountBudgetWindow)
{
    ui->setupUi(this);

    // Signal connections
    connect(ui->btnChangeValues, SIGNAL(clicked()), this, SLOT(changeUserBudget()));

}

/*
 * Opens a new instance of the account setup window to allow the user to change
 * previous values pertaining to income, budget and expense information.
 *
 * */
void AccountBudgetWindow::changeUserBudget()
{
    AccountSetupWindow *changeBudgetInfo = new AccountSetupWindow();
    changeBudgetInfo->setUserID(m_userID, 1);
    changeBudgetInfo->show();
    this->close();
}

/*
 * Sets the user identifier which will be used by other methods in this source file.
 *
 * @param userID : the id of the user that will be used in database queries.
 *
 * */
void AccountBudgetWindow::setUserID(QString userID)
{
    m_userID = userID;
    getCategoryItems();
}

/*
 * Retrieve all of the category items that are attributed to a specific user and populates a list of strings
 * dependant on the results - if the response from the database does not indicate an error.
 *
 * NOTE: I have made a method in TableOperations.cpp as this code was duplicated in the setup process as well as
 * in this form. It works when I use it within the setupWindow, but gives me read/write access violation on this
 * form, so I have had to leave it as it was before, instead of making it a more readable "one line" initialisation
 * for the QMap.
 *
 * */
void AccountBudgetWindow::getCategoryItems()
{
    // Initialise the QMap<QString, QStringList> that will be used to contain all of the information
    // relating to the income and expenses belonging to the users.
    JsonParser jsonParser = new JsonParser(this);

    QMap<QString,QString> qmapUserID;
    qmapUserID.insert("user_id", m_userID);

    QJsonObject postResponse = jsonParser.makeHTTPRequest("http://www.amstevenson.net/middleware/qtcreator/get_all_category_items.php",
                                                   "POST",qmapUserID);

    // Category items array
    QJsonArray categoryItemsToArray = postResponse["categoryitems"].toArray();

    // Success of the HTTPRequest
    int success;

    success = postResponse["success"].toInt();

    // Define QStringLists to hold the category information
    QStringList categoryName, categoryAmount, categoryType, categoryBudget;

    // String lists used afterwards to seperate the values to eliminate ambiguity in terms of incomes and expenses.
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
            }

            // QMap inserts for the StringLists
            m_userInformation.insert("category_name", categoryName);
            m_userInformation.insert("category_amount", categoryAmount);
            m_userInformation.insert("category_type", categoryType);
            m_userInformation.insert("category_budget", categoryBudget);

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

    // Initialise the tables and attach the models.
    tableSettings();
}

/*
 * Create the models for the two respective tables; name and resize the columns for each,
 * using the information collected regarding the user.
 *
 * */
void AccountBudgetWindow::tableSettings()
{
    // Create the table model and set
    m_incomeModel = new QStandardItemModel(0,3,this);
    ui->tblIncome->setModel(m_incomeModel);

    // Default column headers
    m_incomeColumnHeaders << tr("Category") << tr("Income Amount") << "Budget Amount";
    m_expenseColumnHeaders << tr("Category") << tr ("Expense Amount");

    for(int i = 0; i < m_incomeColumnHeaders.size(); i++)
    {
        m_incomeModel->setHorizontalHeaderItem(i, new QStandardItem(QString(m_incomeColumnHeaders.at(i))));

        // fit columns to fit the size of the table - has to come after model has been set for table
        QSize   totalWidthQSize = ui->tblIncome->size();
        int     totalWidth      = totalWidthQSize.width();
        int     lastColumn      = (totalWidth / 100) * 34;
        int     previousColumn  = (totalWidth / 100) * 34;

        if(i < m_incomeColumnHeaders.size() -1)
            ui->tblIncome->horizontalHeader()->resizeSection(i, previousColumn);
        else
            ui->tblIncome->horizontalHeader()->resizeSection(i, lastColumn);
    }

    // Create the table model and set
    m_expensesModel = new QStandardItemModel(0,2,this);
    ui->tblExpenses->setModel(m_expensesModel);

    for(int i = 0; i < m_expenseColumnHeaders.size(); i++)
    {
        m_expensesModel->setHorizontalHeaderItem(i, new QStandardItem(QString(m_expenseColumnHeaders.at(i))));

        // fit columns to fit the size of the table - has to come after model has been set for table
        QSize   totalWidthQSize = ui->tblExpenses->size();
        int     totalWidth      = totalWidthQSize.width();
        int     lastColumn      = (totalWidth / 100) * 35;
        int     previousColumn  = (totalWidth / 100) * 35;

        if(i < m_incomeColumnHeaders.size() -1)
            ui->tblExpenses->horizontalHeader()->resizeSection(i, previousColumn);
        else
            ui->tblExpenses->horizontalHeader()->resizeSection(i, lastColumn);
    }

    //Insert the information for each table
    setTableValues();
}

/*
 * Iterates through both tables models and populates each specific cell using the information
 * retrieved from the database.
 *
 * @see TableOperations.setRowInformation : changes the value of each specific cell with the information
 *                                          found in the passed QStringList.
 *
 * */
void AccountBudgetWindow::setTableValues()
{
    // Set the values for the cells in the income table
    tableOp->setRowInformation(m_incomeModel, m_userInformation.value("income_types"), 0);
    tableOp->setRowInformation(m_incomeModel, m_userInformation.value("income_type_amounts"), 1);
    tableOp->setRowInformation(m_incomeModel, m_userInformation.value("income_budget_amounts"), 2);

    // Set the values for the cells in the expenses table
    tableOp->setRowInformation(m_expensesModel, m_userInformation.value("expense_types"), 0);
    tableOp->setRowInformation(m_expensesModel, m_userInformation.value("expense_type_amounts"), 1);

    // Update the labels depending on the income/budget and expense amounts.
    updateLabels();
}


/*
 * Update the forms labels.
 *
 * @see TableOperations.getUserBudgetAmount : add up each amount specific to the income or expense amounts of the user.
 *
 * */
void AccountBudgetWindow::updateLabels()
{
    // Define the integers that will store the total amounts regarding incomes, budgets and expenses.
    int totalIncomeAmount, totalExpenseAmount, totalBudgetAmount, incomeDifference;
    totalIncomeAmount = totalExpenseAmount = totalBudgetAmount = incomeDifference = 0;

    // Use TableOperations.getUserBudgetAmount to total up each section.
    totalIncomeAmount  = tableOp->getUserBudgetAmount(m_userInformation.value("income_type_amounts"));
    totalBudgetAmount  = tableOp->getUserBudgetAmount(m_userInformation.value("income_budget_amounts"));
    totalExpenseAmount = tableOp->getUserBudgetAmount(m_userInformation.value("expense_type_amounts"));

    // Display the total amounts
    ui->lblTotalIncomeAmount->setNum(totalIncomeAmount);
    ui->lblTotalSavingsAmount->setNum(totalBudgetAmount);
    ui->lblTotalExpensesAmount->setNum(totalExpenseAmount);

    // Determine the difference between the income and budgeted amount
    incomeDifference = totalIncomeAmount - totalBudgetAmount;

    // Display a summary header dependant on the totals
    if(incomeDifference - totalExpenseAmount > 0)
        ui->lblSummaryComment->setText("You are saving money wisely. Keep up the good work!");
    else if (incomeDifference - totalExpenseAmount < 0)
        ui->lblSummaryComment->setText("Once your planned savings are deducted from your expenses, you are left in the red"
                                       "\n Please revise and be more careful!");
}

AccountBudgetWindow::~AccountBudgetWindow()
{
    delete ui;
}
