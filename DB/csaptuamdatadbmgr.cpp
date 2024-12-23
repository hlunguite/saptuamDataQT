#include "csaptuamdatadbmgr.h"
#include "caccounttable.h"
#include "cpersontable.h"
#include "cothercontacttable.h"
#include "ccontacttable.h"
#include "cdepartmenttable.h"
#include "cimportbanktransactiontable.h"
#include "cpaymentaccounttable.h"
#include "creceiptbooktable.h"
#include "cremittancetable.h"
#include "ctransactiontable.h"
#include "csdutils.h"
#include "csettingtable.h"
#include "cdepartmentaccounttable.h"
#include "caccountreceiptbook.h"
#include "cbanktransactiontable.h"
#include "cremittancesetuptable.h"
#include "cimportmintable.h"
#include "cimportaccounttable.h"
#include "cimporttransactiontable.h"
#include "cclosingbalancetable.h"

void CsaptuamDataDBMgr::addRemitRequestBankAccount()
{
    SaccountData accountData;
    accountData.m_id = 0;
    accountData.m_accountName = gRemittanceAccountName;
    accountData.m_accountType = REMITTANCE_ACCOUNT_TYPE;
    accountData.m_isUseHqReceipt = false;
    accountData.m_paymentAccountID = 0;
    accountData.m_isActiveAccount = 0;
    accountData.m_bankAccountName = "";

    SaccountData* accData = CaccountTable::Object()->getAccountDataForName(accountData.m_accountName);
    if (accData == nullptr) {
        int id = accountData.save();
        SdepartmentAccountData deptAcc;
        deptAcc.m_id = 0;
        deptAcc.m_deptOrAccountID = id;
        deptAcc.m_isDept = false;
        deptAcc.m_parentID = 0;
        deptAcc.save();
    } else {
        delete accData;
    }

    accountData.m_id = 0;
    accountData.m_accountName = gRequestAccountName;
    accountData.m_accountType = REQUEST_ACCOUNT_TYPE;
    accountData.m_isUseHqReceipt = false;
    accountData.m_paymentAccountID = 0;
    accountData.m_isActiveAccount = 0;
    accountData.m_bankAccountName = "";

    accData = CaccountTable::Object()->getAccountDataForName(accountData.m_accountName);
    if (accData == nullptr) {
        int id = accountData.save();
        SdepartmentAccountData deptAcc;
        deptAcc.m_id = 0;
        deptAcc.m_deptOrAccountID = id;
        deptAcc.m_isDept = false;
        deptAcc.m_parentID = 0;
        deptAcc.save();
    } else {
        delete accData;
    }

    accountData.m_id = 0;
    accountData.m_accountName = gBankAccountName;
    accountData.m_accountType = BANK_ACCOUNT_TYPE;
    accountData.m_isUseHqReceipt = false;
    accountData.m_paymentAccountID = 0;
    accountData.m_isActiveAccount = 0;
    accountData.m_bankAccountName = "";

    accData = CaccountTable::Object()->getAccountDataForName(accountData.m_accountName);
    if (accData == nullptr) {
        int id = accountData.save();
        SdepartmentAccountData deptAcc;
        deptAcc.m_id = 0;
        deptAcc.m_deptOrAccountID = id;
        deptAcc.m_isDept = false;
        deptAcc.m_parentID = 0;
        deptAcc.save();
    } else {
        delete accData;
    }


    accountData.m_id = 0;
    accountData.m_accountName = gLoanAccountName;
    accountData.m_accountType = LOAN_ACCOUNT_TYPE;
    accountData.m_isUseHqReceipt = false;
    accountData.m_paymentAccountID = 0;
    accountData.m_isActiveAccount = 0;
    accountData.m_bankAccountName = "";

    accData = CaccountTable::Object()->getAccountDataForName(accountData.m_accountName);
    if (accData == nullptr) {
        int id = accountData.save();
        SdepartmentAccountData deptAcc;
        deptAcc.m_id = 0;
        deptAcc.m_deptOrAccountID = id;
        deptAcc.m_isDept = false;
        deptAcc.m_parentID = 0;
        deptAcc.save();
    } else {
        delete accData;
    }
}

void CsaptuamDataDBMgr::addDefaultName()
{

    SotherContactData* other = CotherContactTable::Object()->findName(gBankCashDepositName);
    if (other == nullptr) {
        other = new SotherContactData;
        other->m_idx = 0;
        other->m_name = gBankCashDepositName;
        other->m_phone = "";
        other->save();
    }
    delete other;

    other = CotherContactTable::Object()->findName(gBankInterestName);
    if (other == nullptr) {
        other = new SotherContactData;
        other->m_idx = 0;
        other->m_name = gBankInterestName;
        other->m_phone = "";
        other->save();
    }
    delete other;


    other = CotherContactTable::Object()->findName(gBankChargesName);
    if (other == nullptr) {
        other = new SotherContactData;
        other->m_idx = 0;
        other->m_name = gBankChargesName;
        other->m_phone = "";
        other->save();
    }
    delete other;

    QString saptuamShortName = CsdUtils::getSaptuamShortName();
    other = CotherContactTable::Object()->findName(saptuamShortName);
    if (other == nullptr) {
        other = new SotherContactData;
        other->m_idx = 0;
        other->m_name = saptuamShortName;
        other->m_phone = "";
        other->save();
    }
    delete other;
    QStringList remitList = CremittanceNameTable::Object()->getAllRemittanceName();
    for (int i = 0; i < remitList.size(); ++i) {
        other = CotherContactTable::Object()->findName(remitList.at(i));
        if (other == nullptr) {
            other = new SotherContactData;
            other->m_idx = 0;
            other->m_name = remitList.at(i);
            other->m_phone = "";
            other->save();
        }
        delete other;
        other = nullptr;
    }
    other = CotherContactTable::Object()->findName(gBankCashWithdrawalName);
    if (other == nullptr) {
        other = new SotherContactData;
        other->m_idx = 0;
        other->m_name = gBankCashWithdrawalName;
        other->m_phone = "";
        other->save();
    }
    delete other;


}

CsaptuamDataDBMgr::CsaptuamDataDBMgr(QString saptuamMin):
    m_saptuamMin(saptuamMin)
{
    //1. creat table data
    QVector<CdbTableBase *> dbTables(MAX_TABLE_TYPE, nullptr);

    dbTables[ACCOUNT_TABLE] = CaccountTable::Object();
    dbTables[DEPARTMENT_TABLE] = CdepartmentTable::Object();
    dbTables[DEPARTMENT_ACCOUNT_TABLE] = CdepartmentAccountTable::Object();
    dbTables[PAYMENT_ACCOUNT_TABLE] = CpaymentAccountTable::Object();
    dbTables[CONTACT_TABLE] = CcontactTable::Object();
    dbTables[PERSON_TABLE] = CpersonTable::Object();
    dbTables[OTHER_CONTACT_TABLE] = CotherContactTable::Object();
    dbTables[TRANSACTION_TABLE] = CtransactionTable::Object();
    dbTables[RECEIPT_BOOK_TABLE] = CreceiptBookTable::Object();

    dbTables[RECEIPT_ACCOUNT_RECEIPT_BOOK_TABLE] = CaccountReceiptBook::Object();
    dbTables[IMPORT_BANK_TRANSACTION_TABLE] = CimportBankTransactionTable::Object();
    dbTables[BANK_TRANSACTION_TABLE] = CbankTransactionTable::Object();
    dbTables[REMITTANCE_PERCENT_TABLE] = CremittancePercentTable::Object();
    dbTables[REMITTANCE_NAME_TABLE] = CremittanceNameTable::Object();
    dbTables[REMITTANCE_SETUP_TABLE] = CremittanceSetupTable::Object();
    dbTables[REMITTANCE_TABLE] = CremittanceTable::Object();
    dbTables[REMITTANCE_DETAIL_TABLE] = CremittanceDetailTable::Object();
    dbTables[REMITTANCE_RECONCILE_TABLE] = CremittanceReconcileTable::Object();
    dbTables[SETTING_TABLE] = CsettingTable::Object();
    dbTables[MIN_IMPORT_TABLE] = CimportMinTable::Object();
    dbTables[ACCOUNT_IMPORT_TABLE] = CimportAccountTable::Object();
    dbTables[TRANS_IMPORT_TABLE] = CimportTransactionTable::Object();
    dbTables[CLOSING_TABLE] = CclosingBalanceTable::Object();
    dbTables[CLOSING_DETAIL_TABLE] = CclosingBalanceDetailTable::Object();

    /*
    dbTables[PASSWORD_TABLE] = CpassWordTable::Object();

    */

    //2. db setup
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    dataPath.append("/");
    dataPath.append(QCoreApplication::applicationName());
    QDir myDir(dataPath);
    if (!myDir.exists()) {
        myDir.mkpath(dataPath);
    }

    m_dbName = "saptuamdatabase";
    dataPath.append("/saptuamNew.db");
    QFile::setPermissions(dataPath,QFile::WriteOwner|QFile::ReadOwner);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_dbName);
    db.setDatabaseName(dataPath);
    db.setHostName("");
    db.setPort(0);
    if (!db.open("","")){
        QSqlDatabase::removeDatabase(m_dbName);
        return ;//db.lastError();
    }

    //3. Create table in db
    m_dbName =  db.connectionName();
    CsdUtils::setDbName(m_dbName);



    QStringList tables = db.tables();
    for(int i = 0; i < MAX_TABLE_TYPE; ++i){
        CdbTableBase * dbTable = dbTables[i];
        if (dbTable == nullptr) {
            continue;
        }

        QString tablename = dbTable->getTableName();
        if (tables.contains(tablename, Qt::CaseInsensitive)){
            continue;
        }
        QSqlError error = dbTable->createTable();
        if(error.isValid()){
            qDebug()<<"Error:"<<error;
        }
    }

    addRemitRequestBankAccount();
    addDefaultName();



}

CsaptuamDataDBMgr::~CsaptuamDataDBMgr()
{
    CaccountTable::Destroy();
    CcontactTable::Destroy();
    //CpassWordTable::Destroy();
    CpersonTable::Destroy();
    CotherContactTable::Destroy();
    CdepartmentTable::Destroy();
    CdepartmentAccountTable::Destroy();
    CimportBankTransactionTable::Destroy();
    CimportBankTransactionTable::Destroy();
    CpaymentAccountTable::Destroy();
    CreceiptBookTable::Destroy();
    //CreceiptTable::Destroy();
    CaccountReceiptBook::Destroy();
    CtransactionTable::Destroy();
    CremittancePercentTable::Destroy();
    CremittanceNameTable::Destroy();
    CremittanceSetupTable::Destroy();
    CremittanceTable::Destroy();
    CremittanceDetailTable::Destroy();
    CremittanceReconcileTable::Destroy();
    CsettingTable::Destroy();
    CimportMinTable::Destroy();
    CimportAccountTable::Destroy();
    CimportTransactionTable::Destroy();
    CclosingBalanceTable::Destroy();
    CclosingBalanceDetailTable::Destroy();
    /*for (unsigned int i = 0; i < dbTables.size(); ++i) {
        if (dbTables[i]) {
            delete dbTables[i];
        }
    }*/
}

QSqlDatabase CsaptuamDataDBMgr::getDataBase()
{
    return QSqlDatabase::database(m_dbName);
}

