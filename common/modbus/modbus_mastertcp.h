#ifndef MODBUS_MASTERTCP_H
#define MODBUS_MASTERTCP_H
#include <QModbusTcpClient>
#include "modbus_masterrtu.h"

class Modbus_MasterTcp : public Modbus_MasterObj
{
    Q_OBJECT
public:
    explicit Modbus_MasterTcp(QObject *parent = nullptr);
    bool connectModbusTcp(const QString &url, int port);
};

#endif // MODBUS_MASTERTCP_H
