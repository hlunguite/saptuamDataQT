#include "cdlgbase.h"
#include <QMessageBox>

CdlgBase::CdlgBase(QWidget *parent):
    QDialog(parent)
{

}

void CdlgBase::message(QString msg)
{
    QMessageBox::warning(this, "SaptuamData",
                         msg);
}
