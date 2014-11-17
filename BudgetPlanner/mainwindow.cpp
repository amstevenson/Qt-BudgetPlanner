#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lblBackground->setStyleSheet("background-image: url(:/images/loginBackground.jpg);");
    setFixedSize(814,454);

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

void MainWindow::createNewWindow(QString windowName)
{
    if(windowName == "register")
    {
        SignupWindow *signupWindow = new SignupWindow();
        signupWindow->show();
    }
    else if(windowName == "login")
    {
        QString userEmail, userPassword;
        userEmail = ui->txtEmail->toPlainText();
        userPassword = ui->txtPassword->toPlainText();

        if(userEmail.isEmpty() != true && userPassword.isEmpty() != true)
        {
            JsonParser *jsonParser;
            jsonParser = new JsonParser(this);

            QMap<QString, QString> queryData;

            queryData.insert("user_email", userEmail);
            queryData.insert("user_password",userPassword);

            QString url_insert = "http://www.amstevenson.net/middleware/qtcreator/validate_login.php";

            QJsonObject postResponse = jsonParser->makeHTTPRequest(url_insert, "POST", queryData);

            // convert to a jsonArray and build stringLists to store the data
            QJsonArray postDataToArray = postResponse["response"].toArray();

            int success, userRegistered;
            QString message, userID;

            // will return a success and message value for the insert
            foreach (const QJsonValue & value, postDataToArray)
            {
                QJsonObject dbInformation = value.toObject();

                userID          = dbInformation["user_id"].toString();
                userRegistered  = dbInformation["user_registered"].toInt();
                success         = dbInformation["success"].toInt();
                message         = dbInformation["message"].toString();
            }

            // initialisation of classes
            Operations *operations = new Operations(this);
            AccountSetupWindow *accountSetup = new AccountSetupWindow();
            AccountBudgetWindow *accountBudget = new AccountBudgetWindow();

            // validate login
            switch(success)
            {
                case 0: // login failed
                    operations->displayInformativeMessage("Qt Budget Planner \n", message);
                    break;

                case 1: // login successful
                    switch(userRegistered)
                    {
                        case 0: // user has NOT registered before
                            accountSetup->setUserID(userID);
                            accountSetup->show();
                            this->close();
                            break;

                        case 1: // user has registered before
                            accountBudget->show();
                            this->close();
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else
        {
            QMessageBox::warning(this, tr("My Application"),
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
