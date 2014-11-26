#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>
#include "jsonparser.h"

namespace Ui {
class SignupWindow;
}

class SignupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignupWindow(QWidget *parent = 0);
    ~SignupWindow();
    JsonParser *jsonParser;

private:
    Ui::SignupWindow *ui;

private slots:
    void registerNewAccount();
};

#endif // SIGNUPWINDOW_H
