#include "accountsetupwindow.h"
#include "ui_accountsetupwindow.h"

AccountSetupWindow::AccountSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AccountSetupWindow)
{
    ui->setupUi(this);

    // Table and user operations
    tableOp = new TableOperations(this);

    // Create the table model and set
    m_model = new QStandardItemModel(1,3,this);
    ui->tableView->setModel(m_model);

    // Default state
    m_accountRegistrationState = tableOp->getState("INCOME");

    // Default column headers
    m_columnHeaders << tr("Category") << tr("Amount") << tr("");
    updateTableSettings();
    updateTableButtons();

    // Create signal connections for UI objects.
    connect(ui->btnAdd,      SIGNAL(clicked()), this, SLOT(addTableItem()));
    connect(ui->btnNext,     SIGNAL(clicked()), this, SLOT(getTableValues()));
    connect(ui->btnPrevious, SIGNAL(clicked()), this, SLOT(addAllTableItems()));
    connect(m_model,         SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateLabels()));

    // Initial state is 'income' which is the first process, therefore some QObjects are invisible
    ui->btnPrevious->setVisible(false);
    ui->lblIncome->setVisible(false);
    ui->lblIncomeAmount->setVisible(false);
    ui->lblExpense->setVisible(false);
    ui->lblExpenseAmount->setVisible(false);
}

/*
 * When the 'next' buttons connect signal is triggered, this method stores the information about the user
 * into the QMap StringList that is linked to it. If the finish button is pressed however, we send a call
 * to the method responsible for sending the information to the database, which results in the end of the
 * registration steps.
 *
 * @see TableOperations.getState          : retrieve a specific state
 * @see TableOperations.checkForNullRows  : checks to see if any cells on a table are null or not.
 * @see TableOperations.getRowInformation : adds all information from each row, per column submitted.
 * @see JsonParser.makeHTTPRequest        : performs a database query based on the script and data passed.
 *
 * */
void AccountSetupWindow::getTableValues()
{
    // Set a boolean to check if any rows are empty
    bool rowCellEmpty = false;

    // Check to see if there are any rows
    if(m_model->rowCount() == 0){

        // Do nothing
        QMessageBox::warning(this, tr("My Application"),
                            tr("Empty table detected.\n\n"
                            "If you have no income or expenses, please add an item and enter 0 as a value!"),
                            QMessageBox::Ok);
    }
    else
    {
        // If there are rows, perform the data gathering that is specific to the current table state: income, expenses and budget.
        switch(m_accountRegistrationState)
        {
        case TABLESECTION::INCOME:

            //-----------------------------------------------------------------------------------------------//
            //                                      INCOME STATE                                             //
            //-----------------------------------------------------------------------------------------------//
            // Check for any empty row cells for a given amount of columns
            rowCellEmpty = tableOp->checkForNullRowCells(m_model, 2);

            if(rowCellEmpty == false)
            {
                // If populated already, remove information (for when previous button is pressed)
                if(m_userInformation.value("income_types").isEmpty() != true
                        && m_userInformation.value("income_type_amounts").isEmpty() != true)
                {
                    m_userInformation.remove("income_types");
                    m_userInformation.remove("income_type_amounts");
                }

                // Retrive the information from the table model and store it in the specific StringList within the QMap
                m_userInformation.insert("income_types", tableOp->getRowInformation(m_model,0));
                m_userInformation.insert("income_type_amounts", tableOp->getRowInformation(m_model,1));

                m_model->clear();

                ui->btnPrevious->setVisible(true);

                // If values already exist in the system, the addAllTableItems method will retrieve them.
                // Needed when the user goes back from the third stage, to the first, and then progresses to the second.
                m_accountRegistrationState = tableOp->getState("BUDGET");
                addAllTableItems();

                // First step is completed, move on to second
                m_accountRegistrationState = tableOp->getState("EXPENSES");
            }

            break;

        case TABLESECTION::EXPENSES:

            //-----------------------------------------------------------------------------------------------//
            //                                      EXPENSES STATE                                           //
            //-----------------------------------------------------------------------------------------------//
            // Check for any empty row cells for a given amount of columns
            rowCellEmpty = tableOp->checkForNullRowCells(m_model, 2);

            if(rowCellEmpty == false)
            {
                // If populated already, remove information (for when previous button is pressed)
                if(m_userInformation.value("expense_types").isEmpty() != true
                        && m_userInformation.value("expense_type_amounts").isEmpty() != true)
                {
                    m_userInformation.remove("expense_types");
                    m_userInformation.remove("expense_type_amounts");
                }

                // Retrive the information from the table model and store it in the specific StringList within the QMap
                m_userInformation.insert("expense_types", tableOp->getRowInformation(m_model,0));
                m_userInformation.insert("expense_type_amounts", tableOp->getRowInformation(m_model,1));

                m_model->clear();

                // If we are progressing to the "budget" page, and already have values existing in the system, retrieve them and display
                m_accountRegistrationState = tableOp->getState("FINISH");
                addAllTableItems(); // Retrieve the income information - budgets are based on income items (or so I believe).
                                    // If, when in the addAllTableItems the state is 'FINISH', the 'delete' buttons are not created which allows us to
                                    // make room for the "budget" column, where the user can enter that information for each income item.

                // Second step is completed, move on to third
                m_accountRegistrationState = tableOp->getState("BUDGET");
            }

            break;

        case TABLESECTION::BUDGET:

            //-----------------------------------------------------------------------------------------------//
            //                                      BUDGET STATE                                             //
            //-----------------------------------------------------------------------------------------------//
            // Check for any empty cells for a given amount of columns - column numbers are three instead of two
            rowCellEmpty = tableOp->checkForNullRowCells(m_model, 3);

            if(rowCellEmpty == false)
            {
                // If populated already, remove information (for when previous button is pressed)
                if(m_userInformation.value("income_types").isEmpty() != true && m_userInformation.value("income_type_amounts").isEmpty() != true)
                {
                    m_userInformation.remove("income_types");
                    m_userInformation.remove("income_type_amounts");
                }

                // Retrive the information from the table model and store it in the specific StringList within the QMap
                m_userInformation.insert("income_types", tableOp->getRowInformation(m_model,0));
                m_userInformation.insert("income_type_amounts", tableOp->getRowInformation(m_model,1));
                m_userInformation.insert("income_budget_amounts", tableOp->getRowInformation(m_model,2));

                if(m_userInformation.value("income_types").isEmpty() || m_userInformation.value("income_type_amounts").isEmpty()
                        || m_userInformation.value("income_budget_amounts").isEmpty()){
                    // Do nothing
                }
                else
                {
                    // Add all of the rows to the database: income, expenses and budget.
                    JsonParser jsonParser = new JsonParser(this);

                    // Initialise QMap responsible for query
                    QMap<QString, QString> categoryItem;

                    // Add to database - income items
                    for(int i = 0; i < m_userInformation.value("income_types").size(); i++)
                    {
                        categoryItem.insert("user_id", m_userID);
                        categoryItem.insert("category_name", m_userInformation.value("income_types").at(i));
                        categoryItem.insert("category_amount", m_userInformation.value("income_type_amounts").at(i));
                        categoryItem.insert("category_type", "income");
                        categoryItem.insert("category_budget", m_userInformation.value("income_budget_amounts").at(i));

                        // Create and send POST query
                        jsonParser.makeHTTPRequest("http://www.amstevenson.net/middleware/qtcreator/create_new_category_item.php",
                                                   "POST",categoryItem);
                        categoryItem.clear();
                    }

                    // Add to database - expense items
                    for(int i = 0; i < m_userInformation.value("expense_types").size(); i++)
                    {
                        categoryItem.insert("user_id", m_userID);
                        categoryItem.insert("category_name", m_userInformation.value("expense_types").at(i));
                        categoryItem.insert("category_amount", m_userInformation.value("expense_type_amounts").at(i));
                        categoryItem.insert("category_type", "expense");
                        categoryItem.insert("category_budget", "0");

                        // Create and send POST query
                        jsonParser.makeHTTPRequest("http://www.amstevenson.net/middleware/qtcreator/create_new_category_item.php",
                                                   "POST",categoryItem);
                        categoryItem.clear();
                    }

                    // Update the user in the database and move to the account budget form.
                    categoryItem.insert("user_id", m_userID);
                    jsonParser.makeHTTPRequest("http://www.amstevenson.net/middleware/qtcreator/user_registration_complete.php",
                                               "POST",categoryItem);

                    // Once the rows have started to be added to the database, notify the user and return them to the main screen
                    QMessageBox::information(this, tr("Qt Budget Planner"),
                                        tr("You will now be directed to your summary page, please return to this setup in the future"
                                           " should you wish to change any of your information."
                                        ),
                                        QMessageBox::Ok);

                    AccountBudgetWindow *summaryWindow = new AccountBudgetWindow();
                    summaryWindow->setUserID(m_userID);
                    summaryWindow->show();
                    this->close();
                }
            }
            break;

        default:
            break;
        }
    }

    updateLabels(); // We are on the next step - so we need new labels
}

/*
 * If the previous button is pressed, retrieve information relating to the user for the step before (refer to getTableValues)
 * in order to facilitate the need to display information from previous steps.
 *
 * @see TableOperations.setRowInformation : sets the cell information for a table with the info in the passed StringList for
 * the rows of the passed column number.
 * */
void AccountSetupWindow::addAllTableItems()
{
    m_model->clear();       // clear first, then add more later on

    // Switch progress enum - basically back one step, since it is the previous button and all
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

    // Determine what values to get back
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
        tableOp->setRowInformation(m_model, m_userInformation.value("income_types"), 0);
        tableOp->setRowInformation(m_model, m_userInformation.value("income_type_amounts"), 1);
        break;
    default:
        break;
    }

    updateTableSettings();  // Update the table settings to add in the new column headers.
    updateTableButtons();   // If the state is income or expenses, add delete buttons, else leave it be.
    updateLabels();         // Collect the labels that apply to the state returned to.
}

/* This method determines what QStandardItem gets added to the table model. After it has been added
 * a delete button is mapped to the new rows third column (if the state is either income or expenses).
 *
 * @see TableOperations.addTableItemFromList : adds the QStandardItem selected from the list, to the model.
 *
 * */
void AccountSetupWindow::addTableItem()
{
    QStringList qDialogItems;

    switch(m_accountRegistrationState)
    {
        case TABLESECTION::INCOME:
            qDialogItems << tr("Deposits") << tr("Interest") << tr("Investments") << tr("Monthly Income")
                  << tr("Other Income") << tr("Retirement") << tr("Royalties");

            tableOp->addTableItemFromList(m_model, qDialogItems);
            updateTableButtons(); // Add a delete button.

            break;

        case TABLESECTION::EXPENSES:
            qDialogItems << tr("Automotive expenses") << tr("Bank Activity") << tr("Business") << tr("Debt")
                  << tr("Entertainment") << tr("Gift/Donations") << tr("Housing") << tr("Other Expenses")
                  << tr("Rent") << tr("Savings/Investments") << tr("Shopping") << tr("Utilities");

            tableOp->addTableItemFromList(m_model, qDialogItems);
            updateTableButtons(); // Add a delete button.

            break;
    }
}

/*
 * Removes the table row/item (whichever the preferred semantic is). This method is called when the a signal
 * that has been attached to a delete button is triggered.
 *
 * */
void AccountSetupWindow::removeTableItem(int itemIndex)
{
    m_model->removeRow(itemIndex);
    updateTableButtons();
}

/*
 * Update the labels for the UI based on the current state of the registration process.
 * Income and expenses are fairly similar. The budget state totals up the amounts from both
 * the income and expenses states and displays relevant totals.
 *
 * @see TableOperations.getUserBudgetAmount : total up the specific amounts in the passed list.
 * */
void AccountSetupWindow::updateLabels()
{
    switch(m_accountRegistrationState)
    {

    case TABLESECTION::INCOME:

        // Change UI labels.
        ui->btnAdd->setText("New Income Type");
        ui->btnNext->setText("Next");
        ui->lblTitle->setText("What is your income?");
        ui->lblDetails->setText("As a budget revolves around a persons incomes, lets start by adding some information.\n\n"
                                "Please add some items and press 'next' to move on to the expenses section.");
        break;

    case TABLESECTION::EXPENSES:


        // Change UI label visibility.
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


        // Change UI labels.
        ui->btnAdd->setText("New Expense Type");
        ui->btnNext->setText("Next");
        ui->lblTitle->setText("What do you spend your money on each month?");
        ui->lblDetails->setText("To create a budget, it is important to understand what the money\n is being spent on each month.");
        break;

    case TABLESECTION::BUDGET:


        // Change UI label visibility.
        if(ui->lblIncomeAmount->isVisible() != true)
            ui->lblIncomeAmount->setVisible(true);

        if(ui->lblIncome->isVisible() != true)
            ui->lblIncome->setVisible(true);

        if(ui->lblExpenseAmount->isVisible() != true)
            ui->lblExpenseAmount->setVisible(true);

        if(ui->lblExpense->isVisible() != true)
            ui->lblExpense->setVisible(true);

        ui->btnAdd->setVisible(false);


        // Change UI labels.
        ui->btnNext->setText("Finish");
        ui->lblTitle->setText("How much would you like to budget each month?");
        ui->lblDetails->setText("Please provide an a budget total for each expense.\n"
                                "You can refer to the income amount towards the bottom of the screen.");

        // Add up the totals for the amounts held in both income and expenses, and then display.
        int totalIncome = tableOp->getUserBudgetAmount(m_userInformation.value("income_type_amounts"));
        int totalExpenses = tableOp->getUserBudgetAmount(m_userInformation.value("expense_type_amounts"));
        int totalAmount = totalIncome - totalExpenses;

        ui->lblIncomeAmount->setNum(totalIncome);
        ui->lblExpenseAmount->setNum(totalAmount);

        break;
    }
}

/*
 * This method is concerned with: the layout of the columns, and their sizes; in addition to
 * the default items that are displayed for each state, which are centralised within the table
 * with the exception of the those that pertain to the "budget" state, which requires no default
 * item - although the third row is centralised.
 *
 * */
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
        m_columnHeaders << tr("Category") << tr("Income Amount") << tr("Budget Amount");

        // in the next part of the method (column headers), since we are not going to be adding a default item
        // as a result of keeping the same information from before, we need to centralise the third
        // columns rows here.
        QModelIndex index = m_model->index(0,2);
        m_model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
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

/*
 * For every row that is on the table, this method adds a delete button to it, and adds its signal
 * to the QSignalMapper. This is only done for the first two steps of the registration process.
 *
 * */
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

/*
 * Retrieves the information relating to the user for updating/registration purposes.
 *
 * */
void AccountSetupWindow::setUserID(QString setIDNumber, int userRegistered)
{
    m_userID = setIDNumber;

    switch(userRegistered)
    {
        case 0:

            // Do nothing
            break;

        case 1:

            m_userInformation = tableOp->getCategoryItems(m_userID);
            addAllTableItems();
            break;
    }
}

AccountSetupWindow::~AccountSetupWindow()
{
    delete ui;
}
