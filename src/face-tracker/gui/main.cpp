#include <QApplication>
#include <QTextEdit>
#include "version.h"

#include "test.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    QTextEdit textEdit;
    textEdit.setText(QString("Olá mundo! Bem-vindo à versão %1! A resposta para tudo é: %2").arg(FSDK_VERSION).arg(answerToAll()));
    textEdit.show();
    
    return app.exec();
}