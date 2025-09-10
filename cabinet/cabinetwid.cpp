#include "cabinetwid.h"
#include "ui_cabinetwid.h"

CabinetWid::CabinetWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetWid)
{
    ui->setupUi(this);
    mTableWid = new CabinetTableWidget(ui->widget);
    connect(this, SIGNAL(cabColChangedSig(int)), mTableWid, SLOT(cabColChangeSlot(int)));
}


CabinetWid::~CabinetWid()
{
    delete ui;
}
