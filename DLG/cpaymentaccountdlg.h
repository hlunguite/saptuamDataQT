#ifndef CPAYMENTACCOUNTDLG_H
#define CPAYMENTACCOUNTDLG_H

#include <QDialog>
#include "cdlgbase.h"
namespace Ui {
class CpaymentAccountDlg;
}

class CpaymentAccountDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CpaymentAccountDlg(QWidget *parent = nullptr);
    ~CpaymentAccountDlg();

private:
    Ui::CpaymentAccountDlg *ui;
};

#endif // CPAYMENTACCOUNTDLG_H
