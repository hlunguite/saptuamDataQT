#include "cnewaccountdlg.h"
#include "ui_cnewaccountdlg.h"
#include <QDebug>
#include <QMessageBox>
#include "caccountmap.h"
#include "csdutils.h"
#include "cimportaccounttable.h"
CnewAccountDlg::CnewAccountDlg(bool isEdit, QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CnewAccountDlg),
    m_accountIDForEdit(0),
    m_accountData(nullptr),
    m_deptAccData(nullptr),
    m_remitData(nullptr),
    m_nameData(nullptr),
    m_pcData(nullptr),
    m_accountID(0),
    m_isImport(false)
{

    ui->setupUi(this);
    QStringList remitList = CremittanceNameTable::Object()->getAllRemittanceName();
    m_remitNameCompleter = new QCompleter(remitList, this);
    m_remitNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_remitNameCompleter->setWrapAround(true);
    m_remitNameCompleter->setFilterMode(Qt::MatchContains);
    ui->m_remittanceName->setCompleter(m_remitNameCompleter);
    QStringList accountList = CaccountMap::Object()->getAccountList();
    accountList.sort();
    m_accountNameCompleter = new QCompleter(accountList, this);
    m_accountNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_accountNameCompleter->setWrapAround(true);
    m_accountNameCompleter->setFilterMode(Qt::MatchContains);
    ui->m_accountName->setCompleter(m_accountNameCompleter);

    QVector<QString> accountType;
    CaccountUtils::Object()->getAccountTypesForNewAccountType(accountType);
    for (int i = 0; i < accountType.size(); ++i) {
         ui->m_accountTypeCombo->addItem(accountType.at(i));
    }

    ui->m_accountTypeCombo->setCurrentText(CaccountUtils::Object()->getAccountType(INCOME_ACCOUNT_TYPE));
    on_m_accountTypeCombo_currentTextChanged(CaccountUtils::Object()->getAccountType(INCOME_ACCOUNT_TYPE));
    populatePaymentAccount();
    populateDepartment();
    populateAccount();
    populateDepartmentAccountTable();
    resize();
}

CnewAccountDlg::~CnewAccountDlg()
{
    delete m_accountData;
    delete m_deptAccData;
    delete m_remitData;
    delete m_nameData;
    delete m_pcData;


    delete ui;
}

void CnewAccountDlg::on_m_useHqReceipt_clicked(bool checked)
{
    if (checked) {
        ui->m_remitBox->show();
    } else {
        ui->m_remitBox->hide();
    }
    showhidefield();
}

void CnewAccountDlg::populatePaymentAccount()
{
    QString condition;
    TobjectList values;
    if (CpaymentAccountTable::Object()->getTableValues(condition, values)) {

        int size = values.size();
        for (int i = 0; i < size; ++i) {
             const Tobjects& paymentObj = values.at(i);
             int id = paymentObj.at(PAYMENT_ACCOUNT_ID_IDX).toInt();
             //QString name = paymentObj.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString();
             //if (name.isEmpty() == false)
             {
                QString accountName = paymentObj.at(PAYMENT_ACCOUNT_NAME_IDX).toString();
                ui->m_paymentAccountCombo->addItem(accountName);
                m_paymentAccountIDNameMap.insert(id, accountName);
                m_paymentAccuntNameIDMap.insert(accountName, id);
             }
        }
    }
}

void CnewAccountDlg::populateDepartment()
{
    ui->m_deptCombo->addItem("");

    QString condition;
    TobjectList values;
    if (CdepartmentTable::Object()->getTableValues(condition, values)) {

        int size = values.size();
        for (int i = 0;  i < size; ++i) {
            const Tobjects& dept = values.at(i);
            int id = dept.at(DEPARTMENT_ID_IDX).toInt();
            QString name = dept.at(DEPARTMENT_NAME_IDX).toString();
            ui->m_deptCombo->addItem(name);
            m_deptNameIDMap.insert(name, id);
            m_deptIDNameMap.insert(id, name);
        }
    }


}

void CnewAccountDlg::populateAccount()
{
    QString condition;
    TobjectList values;
    if (CaccountTable::Object()->getTableValues(condition, values)) {

        int size = values.size();
        for (int i = 0; i < size; ++i) {
            const Tobjects& account = values.at(i);
            int id = account.at(ACCOUNT_ID_IDX).toInt();
            QString name = account.at(ACCOUNT_NAME_IDX).toString();
            m_accountNameIDMap.insert(name, id);
            m_accountIDNameMap.insert(id, name);
            QString nameinsmall = name.toLower();
            nameinsmall =  nameinsmall.simplified();
            nameinsmall = nameinsmall.replace(" ","");
            m_accountNameMap.insert(nameinsmall ,name);
        }
    }

}

void CnewAccountDlg::populateDepartmentAccountTable()
{
    /*QString condition;
    TobjectList values;
    if (CdepartmentAccountTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            const Tobjects& deptAccObj = values.at(i);
            int id = deptAccObj.at(DEPARTMENT_ACCOUNT_ID_IDX).toInt();
            int depAccID = deptAccObj.at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
            m_deptAccountTableMap.insert(id, depAccID);
        }

    }*/
}

bool CnewAccountDlg::populateDataMember()
{
    QString accountName = ui->m_accountName->text();
    if (accountName.isEmpty()) {
        message("Empty Account Name");
        return false;
    }
    return populateDataMemberForNonSubAccount();
}


bool CnewAccountDlg::populateDataMemberForNonSubAccount()
{
    if (m_accountData == nullptr) {
        m_accountData = new SaccountData;
        m_accountData->m_id = 0;
        m_accountData->m_isActiveAccount = true;
        m_accountData->m_isUseHqReceipt = false;
    }
    if (validateAccountName() == false) {
        return false;
    }

     QString accountType = ui->m_accountTypeCombo->currentText();
     m_accountData->m_accountType = CaccountUtils::Object()->getAccountTypeId(accountType);
    bool isIncomeOrPayment = false;
     if (accountType.contains("income", Qt::CaseInsensitive) ) {
         m_accountData->m_isUseHqReceipt  = ui->m_useHqReceipt->isChecked();
         isIncomeOrPayment = true;
     } else if (accountType.contains("payment", Qt::CaseInsensitive)) {
         isIncomeOrPayment = true;
          m_accountData->m_isUseHqReceipt = false;
         QString paymentAccountName = ui->m_paymentAccountCombo->currentText();
          if (paymentAccountName.isEmpty()) {
            m_accountData->m_paymentAccountID = 0;
          } else {
            m_accountData->m_paymentAccountID = m_paymentAccuntNameIDMap[paymentAccountName];
         }

     } else {

          m_accountData->m_isUseHqReceipt = false;
     }
     QString bankAccountNo = ui->m_bankAccountNo->text();
     m_accountData->m_bankAccountName = bankAccountNo;
    if (m_deptAccData == nullptr) {
        m_deptAccData = new SdepartmentAccountData;
        m_deptAccData->m_id = 0;
        m_deptAccData->m_isDept = false;

    }

     QString department = isIncomeOrPayment ?  ui->m_deptCombo->currentText() : "";
     if (department.isEmpty() == false) {
         int deptID = m_deptNameIDMap[department];
         SdepartmentAccountData* da = CdepartmentAccountTable::Object()->getDeptAccountDataForDepOrAccID(deptID);
         if (da){
            m_deptAccData->m_parentID = da->m_id;
            delete da;
         } else {
             Q_ASSERT(0);
         }
     } else {
         m_deptAccData->m_parentID =0;
     }


     return validateRemitSetup();

}

bool CnewAccountDlg::validateAccountName()
{
    QString accountName = CsdUtils::ucfirst(ui->m_accountName->text().simplified());
    int id = CaccountMap::Object()->getAccountID(accountName);
    if ((id > 0) &&( (m_accountIDForEdit == 0 || (m_accountIDForEdit > 0 && (m_accountIDForEdit != id)) )) ) {
        message("Account Name already exist");
        return false;
    }

    m_accountData->m_accountName = CsdUtils::ucfirst(accountName);

    return true;
}

bool CnewAccountDlg::validateRemitSetup()
{
    if (m_accountData->m_isUseHqReceipt == false) {
        if (m_nameData) {
            m_nameData->m_name = "";
        }
        return true;
    }

    if (m_remitData == nullptr) {
        m_remitData = new SremittanceSetupTableData;
        m_remitData->m_id = 0;
    }
    if (m_nameData == nullptr) {
        m_nameData = new SremittanceNameData;
        m_nameData->m_id = 0;
    }
    if (m_pcData == nullptr) {
        m_pcData = new SremitrancePercentData;
        m_pcData->m_id = 0;
    }

    bool ok;
    int hqShare = 0;
    if (ui->m_hqShare->text().isEmpty() == false) {
        hqShare = ui->m_hqShare->text().simplified().toInt(&ok);
        if (ok == false) {
            message("HQ Share is not a number");
            return false;
        }
    }
    int localShare = 0;
    if (ui->m_localShare->text().isEmpty() == false) {
        localShare = ui->m_localShare->text().simplified().toInt(&ok);
        if (ok == false) {
            message ("Local Share is not a number");
            return false;
        }
    }

    if (localShare == 0 && hqShare == 0 && ui->m_remittanceName->text().trimmed().isEmpty() == false) {
        message("Remittance name specify but HQ or Local share not specify");
        return false;
    }
    if (((localShare > 0) || hqShare > 0) && ui->m_remittanceName->text().trimmed().isEmpty()) {
        message("Remittance name not specify but HQ or Local share is specify");
        return false;
    }
    if (localShare == 0 && hqShare == 0) {
        message("No remittance percentage");
        return false;
    }
    if ((localShare + hqShare != 100)) {
        message("Remittance local and hq share shuld be 100");
        return false;
    }

    QString remittanceName =  CsdUtils::ucfirst(ui->m_remittanceName->text().simplified());
    m_nameData->m_name = remittanceName;
    m_pcData->m_hqShare = hqShare;
    m_pcData->m_localShare = localShare;

    return true;
}

void CnewAccountDlg::addPaymentAccountComboBox()
{

    //m_paymentAccuntNameIDMap
    SdepartmentData* deptData = CdepartmentTable::Object()->getDepartmentDataByName(ui->m_deptCombo->currentText());
    if (deptData) {
        int paymentAccount = deptData->m_paymentAccuntID;
        if (paymentAccount > 0) {
            QString paymentAccountName = m_paymentAccountIDNameMap[paymentAccount];
            if (paymentAccountName.isEmpty() == false) {
                ui->m_paymentAccountCombo->addItem(paymentAccountName);
                ui->m_paymentAccountCombo->setCurrentText(paymentAccountName);
                return;
            }
        }
    }
    for (auto itr = m_paymentAccuntNameIDMap.cbegin(), end = m_paymentAccuntNameIDMap.cend(); itr != end; ++itr) {
        ui->m_paymentAccountCombo->addItem(itr.key());
    }

}

int CnewAccountDlg::accountID() const
{
    return m_accountID;
}

void CnewAccountDlg::on_m_cancelButton_clicked()
{
    m_accountID = 0;
    emit reject();
}

void CnewAccountDlg::on_m_okButton_clicked()
{
    m_accountID = 0;
    if (m_isImport) {
        QString accountName = CsdUtils::ucfirst(ui->m_accountName->text().simplified());
        int id = CaccountMap::Object()->getAccountID(accountName);
        if (id > 0) {
            m_accountID = id;
            emit accept();
            return;
        }
    }
    if (populateDataMember() == false ) {
        return;
    }


    if (m_accountData->m_accountName.isEmpty()) {
        message("Account Name Empty");
        return;
    }
    m_accountID = m_accountData->save();
    m_deptAccData->m_deptOrAccountID = m_accountID;
    m_deptAccData->save();
    if (m_nameData && m_nameData->m_id > 0 && m_nameData->m_name.isEmpty() && m_remitData) {
        m_remitData->deleteObj();
    } else if (m_accountData->m_isUseHqReceipt && m_remitData) {

        m_remitData->m_accountID = m_accountID;
        m_remitData->m_nameID =  m_nameData->save();
        m_remitData->m_percentageID = m_pcData->save();
        m_remitData->save();
    }

    //account type and parent account type mismatch
    emit accept();
}


void CnewAccountDlg::setAccountIDForEdit(int accountIDForEdit)
{
    m_accountIDForEdit = accountIDForEdit;

    if (m_accountIDForEdit > 0) {
        m_accountData = CaccountTable::Object()->getAccountDataForId(m_accountIDForEdit);
        if (m_accountData) {
            ui->m_accountName->setText(m_accountData->m_accountName);
            ui->m_accountTypeCombo->setCurrentText(CaccountUtils::Object()->getAccountType(m_accountData->m_accountType));
            on_m_accountTypeCombo_currentTextChanged(ui->m_accountTypeCombo->currentText());
            ui->m_useHqReceipt->setChecked(m_accountData->m_isUseHqReceipt);
            int paymentAccountID = m_accountData->m_paymentAccountID;
            if (paymentAccountID > 0) {
                ui->m_paymentAccountCombo->setCurrentText(m_paymentAccountIDNameMap[paymentAccountID]);
            }

            ui->m_bankAccountNo->setText(m_accountData->m_bankAccountName);
        }
        int deptID = CaccountMap::Object()->getDeptForAccount(m_accountIDForEdit);
        if (deptID) {
            QString dept = CaccountMap::Object()->getDeptName(deptID);
            ui->m_deptCombo->setCurrentText(dept);
        }
        m_deptAccData = CdepartmentAccountTable::Object()->getDeptAccountDataForDepOrAccID(m_accountIDForEdit);
        /*if (m_deptAccData) {
            int parentID = m_deptAccData->m_parentID;
            //bool isDept = deptAccData->m_isDept;

            //qDebug()<<"found dept account "<<parentID<<" is dept "<<isDept;
            int deptORAccID = m_deptAccountTableMap[parentID];

            if (m_deptIDNameMap.find(deptORAccID) != m_deptIDNameMap.end()) {
                QString dept = m_deptIDNameMap[m_deptAccountTableMap[parentID]];
                ui->m_deptCombo->setCurrentText(dept);
            }
        }*/
        if (m_accountData && m_accountData->m_isUseHqReceipt) {
            m_remitData = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(m_accountIDForEdit);
            if (m_remitData) {
                int nameID = m_remitData->m_nameID;
                int percentID = m_remitData->m_percentageID;

                m_nameData = CremittanceNameTable::Object()->getRemittanceNameData(nameID);
                if (m_nameData) {
                    QString name = m_nameData->m_name;

                    ui->m_remittanceName->setText(name);
                }
                m_pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);
                if (m_pcData) {
                    QString localShare = QString::number(m_pcData->m_localShare);
                    QString hqShare = QString::number(m_pcData->m_hqShare);
                    ui->m_localShare->setText(localShare);
                    ui->m_hqShare->setText(hqShare);

                }

            }
        }

    }
    showhidefield();
    resize();

}

void CnewAccountDlg::setAccountToImport(SimportAccountData *importAccount)
{
    m_isImport = true;
    m_accountID = 0;
    QString name = importAccount->m_accountName;
    int type = importAccount->m_accountType;
    ui->m_accountName->setText(name);
    ui->m_accountTypeCombo->setCurrentText(CaccountUtils::Object()->getAccountType(type));
    on_m_accountTypeCombo_currentTextChanged(ui->m_accountTypeCombo->currentText());

    showhidefield();
    resize();
}



void CnewAccountDlg::resizeEvent(QResizeEvent *)
{
    resize();
}

void CnewAccountDlg::resize()
{
   /* int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();*/

}

void CnewAccountDlg::showhidefield()
{
    ui->m_remitBox->hide();
    ui->m_paymentAccountCombo->hide();
    ui->m_paymentAccuntLbl->hide();
    ui->m_bankAccountNo->hide();
    ui->m_bankAccountNoLbl->hide();
    ui->m_useHqReceipt->hide();

    ui->m_deptCombo->show();
    ui->m_deptLbl->show();
    QString accountType = ui->m_accountTypeCombo->currentText().simplified();
    ui->m_paymentAccountCombo->clear();
    //qDebug()<<"account type "<<m_accountType;

    if (accountType.compare("income account", Qt::CaseInsensitive) == 0) {
        ui->m_useHqReceipt->show();
        if (ui->m_useHqReceipt->isChecked()) {
            ui->m_remitBox->show();
        }

    } else if (accountType.compare("payment account", Qt::CaseInsensitive) == 0) {
        ui->m_paymentAccountCombo->show();
        ui->m_paymentAccuntLbl->show();
        addPaymentAccountComboBox();
    } else {
        ui->m_deptCombo->hide();
        ui->m_deptLbl->hide();
        if (accountType.contains("bank account", Qt::CaseInsensitive)) {
            ui->m_bankAccountNo->show();
            ui->m_bankAccountNoLbl->show();
        }
    }

}

void CnewAccountDlg::on_m_accountTypeCombo_currentTextChanged(const QString &arg1)
{

    showhidefield();
    resize();
}





void CnewAccountDlg::on_m_deptCombo_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    showhidefield();
}

