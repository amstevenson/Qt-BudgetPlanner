#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jsonparser.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "signupwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    defineEntitySettings(); //changes to user interface - design wise

    connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(createNewSignupForm()));
}

void MainWindow::defineEntitySettings()
{

    // set the background for the login page
    ui->lblBackground->setStyleSheet("background-image: url(:/images/loginBackground.jpg);");
    setFixedSize(814,454);
}

void MainWindow::createNewSignupForm()
{

    SignupWindow *signupWindow = new SignupWindow();

    // create new window and close current form
    signupWindow->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
