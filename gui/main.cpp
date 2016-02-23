#include <QApplication>
#include <QTextEdit>
#include "version.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    QTextEdit textEdit;
    textEdit.setText(QString("Olá mundo! Bem-vindo(a) à versão %1").arg(FSDK_VERSION));
    textEdit.show();
    
    return app.exec();
}