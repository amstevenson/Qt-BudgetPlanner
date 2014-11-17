#include "accountsetupwindow.h"
#include "ui_accountsetupwindow.h"
#include <QInputDialog>

AccountSetupWindow::AccountSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AccountSetupWindow)
{
    ui->setupUi(this);

    // table + user operations
    tableOp = new TableOperations(this);

    // create the table model and set
    m_model = new QStandardItemModel(1,3,this);
    ui->tableView->setModel(m_model);

    // default state
    m_accountRegistrationState = tableOp->getState("INCOME");

    // default column headers
    m_columnHeaders << tr("Category") << tr("Amount") << tr("");
    updateTableSettings();
    updateTableButtons();

    // create connections with sockets
    connect(ui->btnAdd,      SIGNAL(clicked()), this, SLOT(addTableItem()));
    connect(ui->btnNext,     SIGNAL(clicked()), this, SLOT(getTableValues()));
    connect(ui->btnPrevious, SIGNAL(clicked()), this, SLOT(addAllTableItems()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateLabels()));

    // initial state is 'income' which is the first process, therefore some QObjects are invisible
    ui->btnPrevious->setVisible(false);
    ui->lblIncome->setVisible(false);
    ui->lblIncomeAmount->setVisible(false);
    ui->lblExpense->setVisible(false);
    ui->lblExpenseAmount->setVisible(false);
}

void AccountSetupWindow::getTableValues()
{
    if(m_model->rowCount() == 0){} // do nothing
    else
    {
        switch(m_accountRegistrationState)
        {
        case TABLESECTION::INCOME:
            if(m_userInformation.value("income_types").isEmpty() != true && m_userInformation.value("income_type_amounts").isEmpty() != true)
            {
                m_userInformation.remove("income_types");
                m_userInformation.remove("income_type_amounts");
            }

            m_userInformation.insert("income_types", tableOp->getRowInformation(m_model,0));
            m_userInformation.insert("income_type_amounts", tableOp->getRowInformation(m_model,1));

            m_model->clear();

            ui->btnPrevious->setVisible(true);

            // if we are progressing to the "expenses" page, and already have values existing in the system, retrieve them and display
            m_accountRegistrationState = tableOp->getState("BUDGET");
            addAllTableItems();

            // first step is completed, move on to second
            m_accountRegistrationState = tableOp->getState("EXPENSES");

            break;

        case TABLESECTION::EXPENSES:
            if(m_userInformation.value("expense_types").isEmpty() != true && m_userInformation.value("expense_type_amounts").isEmpty() != true)
            {
                m_userInformation.remove("expense_types");
                m_userInformation.remove("expense_type_amounts");
            }

            m_userInformation.insert("expense_types", tableOp->getRowInformation(m_model,0));
            m_userInformation.insert("expense_type_amounts", tableOp->getRowInformation(m_model,1));

            m_model->clear();

            // if we are progressing to the "budget" page, and already have values existing in the system, retrieve them and display
            m_accountRegistrationState = tableOp->getState("FINISH");
            addAllTableItems();

            // second step is completed, move on to third
            m_accountRegistrationState = tableOp->getState("BUDGET");
            break;

        default:
            break;
        }
    }

    updateLabels(); // we are on the next step - so we need new labels
}

void AccountSetupWindow::addAllTableItems()
{
    m_model->clear();       // clear first, then add more later on

    // switch progress enum - basically back one step, since it is the previous button and all
    // two switches in this method, one for determining the stage of the registration, and the other
    // for determining what information to retrieve.
    switch(m_accountRegistrationState)
    {
        case TABLESECTION::EXPENSES:
            ui->btnPrevious->setVisible(false);
            m_accountRegistrationState = TABLESECTION::INCOME;
            break;

        case TABLESECTION::BUDGET:
            ui->btnPrevious->setVisible(true);
            m_accountRegistrationState = TABLESECTION::EXPENSES;
            break;

        case TABLESECTION::FINISH:
            ui->btnPrevious->setVisible(true);
            m_accountRegistrationState = TABLESECTION::BUDGET;
            break;
        default:
            break;
    }

    // determine what values to get back
    switch (m_accountRegistrationState)
    {
        case TABLESECTION::INCOME:
            tableOp->setRowInformation(m_model, m_userInformation.value("income_types"), 0);
            tableOp->setRowInformation(m_model, m_userInformation.value("income_type_amounts"), 1);
            break;

        case TABLESECTION::EXPENSES:
            tableOp->setRowInformation(m_model, m_userInformation.value("expense_types"), 0);
            tableOp->setRowInformation(m_model, m_userInformation.value("expense_type_amounts"), 1);
            break;

        case TABLESECTION::BUDGET:
            tableOp->setRowInformation(m_model, m_userInformation.value("expense_types"), 0);
            tableOp->setRowInformation(m_model, m_userInformation.value("expense_type_amounts"), 1);
            break;
       default :
            break;
    }

    updateTableSettings();  // update the table settings to add in the new column headers
    updateTableButtons();
    updateLabels();
}

// adds a new item to the bottom of a specified table - from a dialog?
void AccountSetupWindow::addTableItem()
{
    QStringList qDialogItems;

    switch(m_accountRegistrationState)
    {
        case TABLESECTION::INCOME:
            qDialogItems << tr("Deposits") << tr("Interest") << tr("Investments") << tr("Monthly Income")
                  << tr("Other Income") << tr("Retirement") << tr("Royalties");

            tableOp->addTableItemFromList(m_model, qDialogItems);
            updateTableButtons();

            break;

        case TABLESECTION::EXPENSES:
            qDialogItems << tr("Automotive expenses") << tr("Bank Activity") << tr("Business") << tr("Debt")
                  << tr("Entertainment") << tr("Gift/Donations") << tr("Housing") << tr("Other Expenses")
                  << tr("Rent") << tr("Savings/Investments") << tr("Shopping") << tr("Utilities");

            tableOp->addTableItemFromList(m_model, qDialogItems);
            updateTableButtons();

            break;
    }
}

void AccountSetupWindow::removeTableItem(int itemIndex)
{
    m_model->removeRow(itemIndex);
    updateTableButtons();
}

void AccountSetupWindow::updateLabels()
{
    switch(m_accountRegistrationState)
    {

    case TABLESECTION::INCOME:

        ui->btnAdd->setText("New Income Type");
        ui->btnNext->setText("Next");
        ui->lblTitle->setText("What is your income?");
        ui->lblDetails->setText("As a budget revolves around a persons incomes, lets start by adding some information.\n\n"
                                "Please add some items and press 'next' to move on to the expenses section.");
        break;

    case TABLESECTION::EXPENSES:

        if(ui->lblIncomeAmount->isVisible() != false)
            ui->lblIncomeAmount->setVisible(false);

        if(ui->lblIncome->isVisible() != false)
            ui->lblIncome->setVisible(false);

        if(ui->lblExpenseAmount->isVisible() != false)
            ui->lblExpenseAmount->setVisible(false);

        if(ui->lblExpense->isVisible() != false)
            ui->lblExpense->setVisible(false);

        if(ui->btnAdd->isVisible() != true)
            ui->btnAdd->setVisible(true);

        ui->btnAdd->setText("New Expense Type");
        ui->btnNext->setText("Next");
        ui->lblTitle->setText("What do you spend your money on each month?");
        ui->lblDetails->setText("To create a budget, it is important to understand what the money\n is being spent on each month.");
        break;

    case TABLESECTION::BUDGET:

        if(ui->lblIncomeAmount->isVisible() != true)
            ui->lblIncomeAmount->setVisible(true);

        if(ui->lblIncome->isVisible() != true)
            ui->lblIncome->setVisible(true);

        if(ui->lblExpenseAmount->isVisible() != true)
            ui->lblExpenseAmount->setVisible(true);

        if(ui->lblExpense->isVisible() != true)
            ui->lblExpense->setVisible(true);

        ui->btnAdd->setVisible(false);
        ui->btnNext->setText("Finish");
        ui->lblTitle->setText("How much would you like to budget each month?");
        ui->lblDetails->setText("Please provide an a budget total for each expense.\n"
                                "You can refer to the income amount towards the bottom of the screen.");

        int totalIncome = tableOp->getUserBudgetAmount(m_userInformation.value("income_type_amounts"));
        int totalExpenses = tableOp->getUserBudgetAmount(m_userInformation.value("expense_type_amounts"));
        int totalAmount = totalIncome - totalExpenses;

        ui->lblIncomeAmount->setNum(totalIncome);
        ui->lblExpenseAmount->setNum(totalAmount);

        break;
    }
}

void AccountSetupWindow::updateTableSettings()
{
    switch(m_accountRegistrationState)
    {
    case TABLESECTION::INCOME:
        m_columnHeaders.clear();
        m_columnHeaders << tr("Category") << tr("Income Amount") << tr("");
        break;
    case TABLESECTION::EXPENSES:
        m_columnHeaders.clear();
        m_columnHeaders << tr("Category") << tr("Expense Amount") << tr("");
        break;
    case TABLESECTION::BUDGET:
        m_columnHeaders.clear();
        m_columnHeaders << tr("Category") << tr("Expense Amount") << tr("Budget Amount");
        break;
    }

    for(int i = 0; i < m_columnHeaders.size(); i++)
    {
        m_model->setHorizontalHeaderItem(i, new QStandardItem(QString(m_columnHeaders.at(i))));

        // fit columns to fit the size of the table - has to come after model has been set for table
        QSize   totalWidthQSize = ui->tableView->size();
        int     totalWidth      = totalWidthQSize.width();
        int     lastColumn      = (totalWidth / 100) * 26;
        int     previousColumn  = (totalWidth / 100) * 40;

        if(i < m_columnHeaders.size() -1)
            ui->tableView->horizontalHeader()->resizeSection(i, previousColumn);
        else
            ui->tableView->horizontalHeader()->resizeSection(i, lastColumn);
    }

    // if there are no table items that have values
    QStandardItem *defaultItem;
    QModelIndex index = m_model->index(0,0);
    switch (m_accountRegistrationState)
    {
        case TABLESECTION::INCOME:
            if(m_model->data(index).toString() == "")
            {
                qDebug() << "in the blank section: income";

                m_model->setData(index, "Monthly Income");
                m_model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);

                index = m_model->index(0,1);
                m_model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
            }
            break;
        case TABLESECTION::EXPENSES:
            if(m_model->rowCount() == 0)
            {
                qDebug() << "in the create section: expenses";

                defaultItem = new QStandardItem(QString("Rent"));
                m_model->appendRow(defaultItem);

                index = m_model->index(0,0);
                m_model->setData(index, "Rent");
                m_model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);

                index = m_model->index(0,1);
                m_model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
            }
            break;
        default:
            break;
    }

}

void AccountSetupWindow::updateTableButtons()
{
    updateTableSettings(); // if columns are not centered (as a result of adding a new item)

    // for every row we have, add a push button and map it
    QSignalMapper *m = new QSignalMapper(this);
    QList<QPushButton *> deleteBList;

    if(m_accountRegistrationState != TABLESECTION::BUDGET)
    {
        for(int i = 0; i < m_model->rowCount(); i++)
        {
            QPushButton *deleteButton = new QPushButton();
            deleteButton->setText("Delete");
            ui->tableView->setIndexWidget(ui->tableView->model()->index(i,2), deleteButton);

            deleteBList.append(deleteButton);
        }

        for(int i = 0; i < deleteBList.length(); i++)
        {
            connect(deleteBList[i], SIGNAL(clicked()), m, SLOT(map()));
            m->setMapping(deleteBList[i],i);
        }

        connect(m, SIGNAL(mapped(int)), this, SLOT(removeTableItem(int)));
    }
}

void AccountSetupWindow::setUserID(QString setIDNumber)
{
    m_userID = setIDNumber;
}

AccountSetupWindow::~AccountSetupWindow()
{
    delete ui;
}
