#include "cnewdeptdlg.h"
#include "ui_cnewdeptdlg.h"
#include "DB/cdepartmenttable.h"
#include "DB/cpaymentaccounttable.h"
#include <QMessageBox>
#include "csdutils.h"
CnewDeptDlg::CnewDeptDlg(bool isEdit, QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CnewDeptDlg),
    m_deptTableID(0),
    m_deptData(nullptr),
    m_deptAccountData(nullptr)
{
    ui->setupUi(this);
    ui->m_paymentAccountCombo->addItem("");
    m_paymentAccountNameIDMap.insert("", 0);
    m_paymentAccountIDNameMap.insert(0, "");
    TobjectList values;
    QString condition;
    if (CpaymentAccountTable::Object()->getTableValues(condition, values)){
        int size = values.size();
        QMap<int, QString> otherPaymentAccount;
        int idx = 1;
        for (int i = 0; i < size; ++i) {
             const Tobjects& paymentObj = values.at(i);
             int id = paymentObj.at(PAYMENT_ACCOUNT_ID_IDX).toInt();
             QString name = paymentObj.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString();
             QString accountName = paymentObj.at(PAYMENT_ACCOUNT_NAME_IDX).toString();
//             qDebug()<<"payment name:"<<name<<" accountn ame "<<accountName;
             if (name.isEmpty() == true) {
                 ++idx;
                ui->m_paymentAccountCombo->addItem(accountName);
                m_paymentAccountNameIDMap.insert(accountName, id);
                m_paymentAccountIDNameMap.insert(id, accountName);

             } else {
                otherPaymentAccount.insert(id, name);
             }
        }
        ui->m_paymentAccountCombo->insertSeparator(idx);
        for(auto [id, accountName]: otherPaymentAccount.asKeyValueRange()) {

             ui->m_paymentAccountCombo->addItem(accountName);
             m_paymentAccountNameIDMap.insert(accountName, id);
             m_paymentAccountIDNameMap.insert(id, accountName);
        }
    }
}

CnewDeptDlg::~CnewDeptDlg()
{
    delete m_deptAccountData;
    delete m_deptData;
    delete ui;
}


bool CnewDeptDlg::setDeptTableID(int deptTableID)
{
    bool ret = false;
    m_deptTableID = deptTableID;
    if (m_deptTableID > 0) {
        m_deptData = CdepartmentTable::Object()->getDepartmentDataByID(deptTableID);
        ret = true;
        if (m_deptData){
            ui->m_deptName->setText(m_deptData->m_deptName);
            ui->m_deptShortName->setText(m_deptData->m_deptShortName);
            int paymentID = m_deptData->m_paymentAccuntID;
            if (paymentID > 0) {
                QString paymantAccName = m_paymentAccountIDNameMap[paymentID];
                ui->m_paymentAccountCombo->setCurrentText(paymantAccName);
            }
            m_deptAccountData = CdepartmentAccountTable::Object()->getDeptAccountDataForDepOrAccID(deptTableID);
        }
    }

    return ret;
}


void CnewDeptDlg::on_m_cancelBtn_clicked()
{
   emit reject();
}

void CnewDeptDlg::on_pushButton_clicked()
{
    QString deptName = CsdUtils::ucfirst(ui->m_deptName->text());
    if (deptName.isEmpty()) {
        message("Empty Department Name");
        return;
    }
    QString deptShortName = CsdUtils::ucfirst(ui->m_deptShortName->text());
    if (deptShortName.isEmpty()) {
        deptShortName = CsdUtils::abbravate(deptName);
    }
    SdepartmentData* dept = CdepartmentTable::Object()->getDepartmentDataByName(deptName);
    if (dept) {
        if (m_deptTableID == 0|| (m_deptTableID > 0 && (dept->m_id != m_deptTableID))) {
            QString msg = "Department Name \'" + deptName + "\' already Exist";
            message(msg);
            delete dept;
            return;
        }
        delete dept;
    }
    dept = CdepartmentTable::Object()->getDepartmentDataByShortName(deptShortName);
    if (dept) {
        if (m_deptTableID == 0|| (m_deptTableID > 0 && (dept->m_id != m_deptTableID))) {
            QString msg = "Department Short Name \'" + deptShortName + "\' already Exist";
            message(msg);
            delete dept;
            return;
        }
        delete dept;
    }

    if (m_deptData == nullptr) {
        m_deptData = new SdepartmentData;
        m_deptData->m_id = 0;
    }
    m_deptData->m_deptName = deptName;
    m_deptData->m_deptShortName = deptShortName;
    if (ui->m_paymentAccountCombo->currentText().isEmpty()) {
        m_deptData->m_paymentAccuntID = 0;
    } else {
        m_deptData->m_paymentAccuntID =  m_paymentAccountNameIDMap[ui->m_paymentAccountCombo->currentText()];
    }
    int id = m_deptData->save();

    if (m_deptAccountData == nullptr) {
        m_deptAccountData = new SdepartmentAccountData;
        m_deptAccountData->m_isDept = true;
        m_deptAccountData->m_parentID = 0;
        m_deptAccountData->m_id = 0;
    }
    m_deptAccountData->m_deptOrAccountID = id;
    m_deptAccountData->save();

    emit accept();
}
