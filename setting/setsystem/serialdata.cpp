#include "serialdata.h"
#include "ui_serialdata.h"

Serialdata::Serialdata(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Serialdata)
{
    ui->setupUi(this);
}

Serialdata::~Serialdata()
{
    delete ui;
}


