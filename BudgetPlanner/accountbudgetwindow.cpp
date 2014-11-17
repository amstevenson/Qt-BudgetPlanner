#include "accountbudgetwindow.h"
#include "ui_accountbudgetwindow.h"

AccountBudgetWindow::AccountBudgetWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AccountBudgetWindow)
{
    ui->setupUi(this);

    //QMainWindow::showFullScreen();



}

AccountBudgetWindow::~AccountBudgetWindow()
{
    delete ui;
}
