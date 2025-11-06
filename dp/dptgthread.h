#ifndef DPTGTHREAD_H
#define DPTGTHREAD_H

#include <QObject>
#include "common/common.h"
#include "sql/dbsystem.h"

class DpTgThread : public QThread
{
    Q_OBJECT
public:
    explicit DpTgThread(QObject *parent = 0);
    ~DpTgThread();

signals:

protected:
    void run();

    int averData(ushort *data, int len=LINE_NUM);
    void lineTgObj(sObjData *obj, sLineTgObjData *tg, int addr);
    void dcLineTgObj(sObjData *obj, sLineTgObjData *tg, int line, int len);
//    void loopTgObj(int loop , int phase , sObjData *obj, sLoopTgObjData *tg);

    void tgObj(sObjData *obj, sTgObjData *tg);

    void tgBox(sBoxData *box, int addr);
    void tgBus(sBusData *bus);


protected slots:
    void timeoutDone();

private:
     bool isRun;
     QTimer *timer;
     sDataPacket *shm;
};

#endif // DPTGTHREAD_H
