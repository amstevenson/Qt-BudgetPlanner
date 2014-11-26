#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "qmessagebox.h"
#include "qmap.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qstringlist.h"

SignupWindow::SignupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignupWindow)
{
    ui->setupUi(this);

    // window design variables
    // Background from: http://wallpaperswide.com/simple_blue_background-wallpapers.html
    ui->lblBackground->setStyleSheet("background-image: url(:/images/loginBackground.jpg);");
    setFixedSize(800,454);

    connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(registerNewAccount()));

    // Action from the lecture practical - text editor
    connect (ui->actionAbout, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

/*
 * Register a users account using the email address and password provided. The account is only created if
 * the user provides details that match, are above a certain length and if the email address is not currently in use
 * by another user.
 *
 * @see JsonParser.makeHTTPRequest
 *
 * */
void SignupWindow::registerNewAccount()
{
    // Collect user information
    QString initialEmail, initialEmailRepeat, initialPassword, initialPasswordRepeat;
    initialEmail = ui->txtEmailAddressInitial->toPlainText();
    initialEmailRepeat = ui->txtEmailAddressRepeat->toPlainText();
    initialPassword = ui->txtPasswordInitial->toPlainText();
    initialPasswordRepeat = ui->txtPasswordRepeat->toPlainText();

    // Validate if objects are empty or not
    if(initialEmail.isEmpty() || initialEmailRepeat.isEmpty() || initialPassword.isEmpty() || initialPasswordRepeat.isEmpty())
    {
        QMessageBox::warning(this, tr("Qt Budget Planner"),
                            tr("Your account cannot be created.\n\n"
                            "You have one or more fields that currently contain no value."),
                            QMessageBox::Ok);
    }

    // Validate if user information is of the right size, and if the email addresses and passwords match
    if(initialEmail == initialEmailRepeat && initialPassword == initialPasswordRepeat)
        if(initialEmail.size() > 3 && initialEmailRepeat.size() > 3 && initialPassword.size() > 3 && initialPasswordRepeat.size() > 3)
        {
            //*********************************************************//
            //                   Create the new account                //
            //*********************************************************//
            jsonParser = new JsonParser(this);

            QMap<QString, QString> queryData;

            queryData.insert("user_email", initialEmail);
            queryData.insert("user_password",initialPassword);

            // Take the type of query, post and the url and collect or input data into
            // the relevant database. Need to check how to do the isset stuff on the php end
            // still.
            // convert to a jsonArray and build stringLists to store the data
            QString url_insert = "http://www.amstevenson.net/middleware/qtcreator/create_new_user.php";

            QJsonObject postResponse = jsonParser->makeHTTPRequest(url_insert, "POST", queryData);

            // convert to a jsonArray and build stringLists to store the data
            QJsonArray postDataToArray = postResponse["response"].toArray();

            int success = 0;
            QString message;

            // will return a success and message value for the insert
            foreach (const QJsonValue & value, postDataToArray)
            {
                QJsonObject dbInformation = value.toObject();

                success = dbInformation["success"].toInt();
                message = dbInformation["message"].toString();
            }

            // Determine if the user has been added to the database or not.
            // Note: there has been an attempt to populate the message information of the
            // QMessageBox 's with the variable "message" above, but after about two hours of working with
            // trying to convert strings to const wchar *'s it seemed more time effective to move on.
            switch(success)
            {
                case 0:
                    QMessageBox::warning(this, tr("Qt Budget Planner"),
                                        tr("Error processing request.\n\n"
                                        "Technical error: please contact addstevenson@hotmail.com \n"
                                           "It could either be that the email address is already in use by another user, "
                                           "or there is a networking error."),
                                        QMessageBox::Ok);
                    this->close();
                    break;

                case 1:        
                    QMessageBox::information(this, tr("Qt Budget Planner"),
                                        tr("Account has been created! \n\n"
                                        "Please login using the email address and password that you signed up with."),
                                        QMessageBox::Ok);
                    this->close();
                    break;

                default:
                    break;
            }

        }
        else
        {
            QMessageBox::warning(this, tr("Qt Budget Planner"),
                                tr("Your account cannot be created.\n\n"
                                "Your passwords need to be atleast 3 or more characters long,"
                                   " and your email addresses need to be valid."),
                                QMessageBox::Ok);
        }
    else
        {  
            QMessageBox::warning(this, tr("Qt Budget Planner"),
                                tr("Your account cannot be created.\n\n"
                                "Your email addresses and passwords need to match"),
                                QMessageBox::Ok);
        }
}


SignupWindow::~SignupWindow()
{
    delete ui;
}

