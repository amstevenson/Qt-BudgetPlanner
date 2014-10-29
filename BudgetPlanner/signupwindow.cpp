#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "qmessagebox.h"
#include "jsonparser.h"
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

    // check to see if the user has left any textboxes empty or not
    if(initialEmail.isEmpty() || initialEmailRepeat.isEmpty() || initialPassword.isEmpty() || initialPasswordRepeat.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Your account cannot be created");
        msgBox.setInformativeText("You have one or more fields that currently contain no value.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int res = msgBox.exec();

        switch(res){

            case QMessageBox::Ok:
              //  this->close();

            default:
                break;
        }
    }
    // check to see if the email addresses and passwords match
    // as well as ensuring that the size of both is above three for now (until I find a regular expression to check the email address)
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

            QStringList success, message;

            // will return a success and message value for the insert
            foreach (const QJsonValue & value, postDataToArray)
            {
                 QJsonObject obj2 = value.toObject();

                 success.append(obj2["success"].toString());
                 message.append(obj2["message"].toString());

            }

            //******************************************************************//
            //                   Display the results to the user                //
            //******************************************************************//
            QMessageBox msgBox;
            msgBox.setInformativeText(message.at(0));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int res = msgBox.exec();

            switch(res){

                case QMessageBox::Ok:
                    // after confirming message, close the form
                    this->close();
                default:
                    break;
            }

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Your account cannot be created");
            msgBox.setInformativeText("Your passwords need to be atleast 3 or more characters long, and your email addresses need to be valid.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int res = msgBox.exec();

            switch(res){

                case QMessageBox::Ok:


                default:
                    break;
            }
        }
    else
        {
            QMessageBox msgBox;
            msgBox.setText("Your account cannot be created");
            msgBox.setInformativeText("Your email addresses and passwords need to match");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int res = msgBox.exec();

            switch(res){

                case QMessageBox::Ok:


                default:
                    break;
            }
        }
}
