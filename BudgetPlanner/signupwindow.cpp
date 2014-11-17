#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "qmessagebox.h"
#include "jsonparser.h"
#include "qmap.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qstringlist.h"
#include "operations.h"

SignupWindow::SignupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignupWindow)
{
    ui->setupUi(this);

    // window design variables
    ui->lblBackground->setStyleSheet("background-image: url(:/images/loginBackground.jpg);");
    setFixedSize(814,454);

    connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(registerNewAccount()));
}

SignupWindow::~SignupWindow()
{
    delete ui;
}

void SignupWindow::registerNewAccount()
{
    QString initialEmail, initialEmailRepeat, initialPassword, initialPasswordRepeat;
    initialEmail = ui->txtEmailAddressInitial->toPlainText();
    initialEmailRepeat = ui->txtEmailAddressRepeat->toPlainText();
    initialPassword = ui->txtPasswordInitial->toPlainText();
    initialPasswordRepeat = ui->txtPasswordRepeat->toPlainText();

    if(initialEmail.isEmpty() || initialEmailRepeat.isEmpty() || initialPassword.isEmpty() || initialPasswordRepeat.isEmpty())
    {
        QMessageBox::warning(this, tr("Qt Budget Planner"),
                            tr("Your account cannot be created.\n\n"
                            "You have one or more fields that currently contain no value."),
                            QMessageBox::Ok);
    }

    if(initialEmail == initialEmailRepeat && initialPassword == initialPasswordRepeat)
        if(initialEmail.size() > 3 && initialEmailRepeat.size() > 3 && initialPassword.size() > 3 && initialPasswordRepeat.size() > 3)
        {
            //*********************************************************//
            //                   Create the new account                //
            //*********************************************************//
            JsonParser *jsonParser;
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

            int success;
            QString message;

            // will return a success and message value for the insert
            foreach (const QJsonValue & value, postDataToArray)
            {
                QJsonObject dbInformation = value.toObject();

                success = dbInformation["success"].toInt();
                message = dbInformation["message"].toString();
            }

            Operations *o = new Operations(this);

            switch(success)
            {
                case 0:
                    o->displayInformativeMessage("Qt Budget Planner \n", message);
                    this->close();
                    break;

                case 1:        
                    o->displayInformativeMessage("Qt Budget Planner \n", message);
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
