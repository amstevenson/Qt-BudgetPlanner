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

    // Table and user operations
    tableOp = new TableOperations(this);

    // Signal connections
    connect(ui->btnChangeValues, SIGNAL(clicked()), this, SLOT(changeUserBudget()));
    connect(ui->actionLogout, SIGNAL(triggered()), this, SLOT(logout()));
    connect(ui->btnLogout, SIGNAL(clicked()), this, SLOT(logout()));

    // Practical actions
    connect (ui->actionAbout, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
 * */
void AccountBudgetWindow::getCategoryItems()
{
    // Initialise the QMap<QString, QStringList> that will be used to contain all of the information
    // relating to the income and expenses belonging to the users.
    jsonParser = new JsonParser(this);

    m_userInformation = tableOp->getCategoryItems(m_userID);

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

/*
 * Return to the main page - logout
 *
 * */
void AccountBudgetWindow::logout()
{
    MainWindow *main = new MainWindow();
    main->show();
    this->close();
}

AccountBudgetWindow::~AccountBudgetWindow()
{
    delete ui;
}
