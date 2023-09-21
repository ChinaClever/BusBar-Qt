/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mb_update.h"

Mb_Update::Mb_Update(QObject *parent)
    : Mb_Setting{parent}
{


}

void Mb_Update::mbUpdates()
{
    sDataPacket *shm = get_share_mem(); // 获取共享内存
        for(int i = 0 ; i < BUS_NUM ; i++){
            for(int j = 0 ; j < BOX_NUM-1 ; j++){
            sBusData *data = &(shm->data[i]);
            if(j == 0){
                upMasterDevInfo(data , i , j);
                upMasterDevRange(data , i , j);
                //env_update();
                //mbSetUpdate();
            }else{
                upSlaveDevInfo(data , i , j);
                upSlaveDevRange(data , i , j);
            }
        }
    }
}
