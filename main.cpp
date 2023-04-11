#include "mainwindow.h"
#include <QApplication>
#include "frminput.h"
#include "frmnum.h"

bool appendFile(const QString &msg)
{
    QString fn = "/sdcard/process_log.txt";
    QFile file(fn);
    if(file.open(QIODevice::Append | QIODevice::Text)) {
        QString t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz\t");
        QByteArray array = t.toUtf8() + msg.toUtf8()+"\n";
        file.write(array);
    } file.close();

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //appendFile("monitor");
    MainWindow w;
    //w.showFullScreen();
    w.show();

    return a.exec();
}
