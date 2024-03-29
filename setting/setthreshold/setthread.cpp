#include "setthread.h"
//#include "snmp/snmpthread.h"


SetThread::SetThread(QObject *parent) : QThread(parent)
{
    isRun = true;
    mSetShm = new SetShm(this);
    mNetCmd = SetNetCmd::bulid(this);
    mRtuCmd = new SetRtuCmd(this);
}

SetThread::~SetThread()
{
    isRun = false;
    wait();
}

SetThread *SetThread::bulid(QObject *parent)
{
    static SetThread* sington = NULL;
    if(sington == NULL) {
        sington = new SetThread(parent);
    }
    return sington;
}


void SetThread::workDown()
{
    if(mItems.size()) {
        gReadWriteflag = 2;
        sThresholdItem item = mItems.first();
        //bool ret = mNetCmd->send(item);
        //if(!ret) mRtuCmd->send(item);//V2.5
        bool ret = false;
        if(!ret){
            if(gVerflag == 2){
                if(item.box == 0) mRtuCmd->sendStartV3(item);
                else mRtuCmd->sendPlugV3(item);
            }
//            if(gVerflag == 3){
//                gReadWriteflag = 2;
//                emit sendSetSnmpSig(&item);
//            }
        }

        mSetShm->setItem(item);
        mItems.removeFirst();
        sleep(1);
        gReadWriteflag = 1;
    }
}

void SetThread::run()
{
    while (isRun) {
        workDown();
        msleep(1);
    }
}
