#include "cpaymentaccountdlg.h"
#include "ui_cpaymentaccountdlg.h"

CpaymentAccountDlg::CpaymentAccountDlg(QWidget *parent) :
    CdlgBase(parent),

    ui(new Ui::CpaymentAccountDlg)
{
    ui->setupUi(this);
}

CpaymentAccountDlg::~CpaymentAccountDlg()
{
    delete ui;
}
