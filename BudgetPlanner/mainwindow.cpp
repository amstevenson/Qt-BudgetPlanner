#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the background image and the fixed size of the window
    ui->lblBackground->setStyleSheet("background-image: url(:/images/loginBackground.jpg);");
    setFixedSize(814,454);

    // Create a signal mapper and add the register and login buttons.
    QSignalMapper *m = new QSignalMapper(this);

    QList<QPushButton *> blist;

    blist.append(ui->btnRegister);
    blist.append(ui->btnLogin);

    connect(blist[0], SIGNAL(clicked()), m, SLOT(map()));
    m->setMapping(blist[0], "register");

    connect(blist[1], SIGNAL(clicked()), m, SLOT(map()));
    m->setMapping(blist[1], "login");

    connect(m, SIGNAL(mapped(QString)), this, SLOT(createNewWindow(QString)));
}

/*
 * Dependant on which button is pressed on the main windows UI, another window is opened.
 * In the case that a user chooses to login, a HTTPRequest will query whether s/he is registered or not,
 * if so then they will be send to the budget window, and if not, the registration window.
 *
 * @see JsonParser.makeHTTPRequest
 *
 * */
void MainWindow::createNewWindow(QString windowName)
{
    if(windowName == "register")
    {
        SignupWindow *signupWindow = new SignupWindow();
        signupWindow->show();
    }
    else if(windowName == "login")
    {
        // Collect user input
        QString userEmail, userPassword;
        userEmail = ui->txtEmail->toPlainText();
        userPassword = ui->txtPassword->toPlainText();

        if(userEmail.isEmpty() != true && userPassword.isEmpty() != true)
        {
            // Determine if the email and password entered is correct or not.
            JsonParser *jsonParser;
            jsonParser = new JsonParser(this);

            QMap<QString, QString> queryData;

            queryData.insert("user_email", userEmail);
            queryData.insert("user_password",userPassword);

            QString url_insert = "http://www.amstevenson.net/middleware/qtcreator/validate_login.php";

            QJsonObject postResponse = jsonParser->makeHTTPRequest(url_insert, "POST", queryData);

            // Convert to a jsonArray and build stringLists to store the data
            QJsonArray postDataToArray = postResponse["response"].toArray();

            // Certain ints are QStrings: this is because the database returns them as strings, even though
            // they are specified as numbers within the database. A shame really.
            int success;
            QString message, userID, userRegistered;

            // will return a success and message value for the insert
            foreach (const QJsonValue & value, postDataToArray)
            {
                QJsonObject dbInformation = value.toObject();

                userID          = dbInformation["user_id"].toString();
                userRegistered  = dbInformation["user_registered"].toString();
                success         = dbInformation["success"].toInt();
                message         = dbInformation["message"].toString();

            }

            // Initialisation of classes
            AccountSetupWindow *accountSetup = new AccountSetupWindow();
            AccountBudgetWindow *accountBudget = new AccountBudgetWindow();

            // validate login
            switch(success)
            {
                case 0: // login failed
                    QMessageBox::warning(this, tr("Qt Budget Planner"),
                                        tr("Login has failed. \n\n"
                                        "Please check that you have entered in the correct email address and password."),
                                        QMessageBox::Ok);
                    break;

                case 1: // login successful

                    if(userRegistered == "0")
                    {
                        // go to registration form
                        accountSetup->setUserID(userID, userRegistered.toInt());
                        accountSetup->show();
                        this->close();
                    }
                    else if(userRegistered == "1")
                    {
                        // go to summary form
                        accountBudget->setUserID(userID);
                        accountBudget->show();
                        this->close();
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            // If either of the fields are left blank
            QMessageBox::warning(this, tr("Qt Budget Planner"),
                                tr("Error processing request.\n\n"
                                "Please check to see if the email and password fields are empty."
                                " If so, please enter valid information."),
                                QMessageBox::Ok);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
