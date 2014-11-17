#include "operations.h"

Operations::Operations(QMainWindow *parent) : QMainWindow(parent)
{
}

void Operations::displayInformativeMessage(QString setText, QString setInformativeMessage)
{
    QMessageBox msgBox;
    msgBox.setText(setText);
    msgBox.setInformativeText(setInformativeMessage);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    msgBox.exec();
}
