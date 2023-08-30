#ifndef MB_UPDATE_H
#define MB_UPDATE_H

#include "mb_setting.h"

class Mb_Update : public Mb_Setting
{
public:
    explicit Mb_Update(QObject *parent = nullptr);
    void mbUpdates();
};

#endif // MB_UPDATE_H
