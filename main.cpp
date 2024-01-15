#include "mainwindow.h"
#include <QApplication>
#include "common/keyboard/frminput.h"
#include "common/keyboard/frmnum.h"

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
//    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg , true);
    if(QString::fromUtf8(argv[1]) == "d"){
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg , true);
    }
    QApplication a(argc, argv);

    frmInput::Instance()->Init("bottom" , "black" , 10 , 10);
    frmInput::Instance()->hide();
    //appendFile("monitor");
    MainWindow w;
    //w.showFullScreen();
    w.show();

    return a.exec();
}
