#include "cpaymentaccountdlg.h"
#include "ui_cpaymentaccountdlg.h"
#include "MISC/CdlgDefine.h"
#include "cpaymentaccounttable.h"
CpaymentAccountDlg::CpaymentAccountDlg(QWidget *parent) :
    CdlgBase(parent),

    ui(new Ui::CpaymentAccountDlg)
{
    m_idToEdit = 0;
    ui->setupUi(this);
}

CpaymentAccountDlg::~CpaymentAccountDlg()
{
    delete ui;
}

bool CpaymentAccountDlg::setIDToEdit(int idtoedit)
{
    bool ret = false;
    m_idToEdit = idtoedit;

    SpaymentAccountData*  data = CpaymentAccountTable::Object()->getPaymentAccountDataForId(m_idToEdit);
    if (data) {
        ret = true;
        ui->m_accountName->setText(data->m_accountName);
        ui->m_paymentAccountName->setText(data->m_paymentAccountName);
        ui->m_natureOFExpense->setText(data->m_natureOfExpense);
        delete data;
    }



    return ret;

}

void CpaymentAccountDlg::on_m_okBtn_clicked()
{
    SpaymentAccountData*  data = nullptr;
    if (m_idToEdit > 0) {
        data = CpaymentAccountTable::Object()->getPaymentAccountDataForId(m_idToEdit);
    } else {
        data = new SpaymentAccountData;
    }
    QString accountName = ui->m_accountName->text().simplified();
    QString paymentAccountName = ui->m_paymentAccountName->text().simplified();
    QString natureofexpense = ui->m_natureOFExpense->text().simplified();
    bool success = true;
    SpaymentAccountData* nameData = CpaymentAccountTable::Object()->getPaymentAccountDataForPaymentAccountName(paymentAccountName);
    if (nameData) {
        if (m_idToEdit) {
            if (m_idToEdit != nameData->m_id) {
                message("Payment account already exist");
                success = false;
            }
        } else {
            message("Payment account already exist");
            success = false;
        }

        delete nameData;
    }
    if (success) {
        data->m_accountName = accountName;
        data->m_paymentAccountName = paymentAccountName;
        data->m_natureOfExpense = natureofexpense;
        data->save();
    }

    delete data;
    if (success) {
        emit accept();
    }

}


void CpaymentAccountDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}

void CpaymentAccountDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int width = windowwidth - DEPT_ACCOUNT_COMBO_SIZE.width()*2;
    int height = windowheight - ( DEFAULT_HEIGHT*3 + GAP*3 + DEFAULT_HEIGHT + GAP);

    int x = width/2;
    int y = height/2;
    width = DEPT_ACCOUNT_COMBO_SIZE.width()*2;
    ui->m_accountName->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    ui->m_paymentAccountName->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    ui->m_natureOFExpense->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    x = windowwidth/2 - GAP/2 - BUTTON_SIZE.width();
    ui->m_okBtn->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += BUTTON_SIZE.width() + GAP;
    ui->m_cancelBtn->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());


}

