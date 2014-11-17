#ifndef ACCOUNTBUDGETWINDOW_H
#define ACCOUNTBUDGETWINDOW_H

#include <QMainWindow>

namespace Ui {
class AccountBudgetWindow;
}

class AccountBudgetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountBudgetWindow(QWidget *parent = 0);
    ~AccountBudgetWindow();

private:
    Ui::AccountBudgetWindow *ui;
};

#endif // ACCOUNTBUDGETWINDOW_H
