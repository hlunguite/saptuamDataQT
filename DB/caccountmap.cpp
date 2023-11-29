#include "caccountmap.h"
#include "cdepartmentaccounttable.h"
#include "cdepartmenttable.h"
#include "cpaymentaccounttable.h"
CaccountMap::CaccountMap()
{
    m_defaultDept = 0;
    populateAccountMap();
}

CaccountMap::~CaccountMap()
{

}
void CaccountMap::populateAccountMap()
{
    populateAccountMapInt();
    populateAccountDeptMap();

}

void CaccountMap::populateAccountMapInt()
{
    m_defaultDept = 0;
    m_accountIdNameMap.clear();
    m_accountNameIdMap.clear();
    m_accountList.clear();
    m_accountListWithHqReceipt.clear();
    m_accountIDListWithHqReceipt.clear();
    m_allAccount.clear();
    m_paymentAccountIDNameMap.clear();
    m_accountDepPaymentAccountMap.clear();
    TobjectList values;
    QString condition;
    if (CpaymentAccountTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int id = values.at(i).at(PAYMENT_ACCOUNT_ID_IDX).toInt();
            QString accuntName = values.at(i).at(PAYMENT_ACCOUNT_NAME_IDX).toString();
            m_paymentAccountIDNameMap[id] = accuntName;
            //qDebug()<<id<<" payment account "<<accuntName;
        }
    }


    values.clear();
    condition.clear();
    if (CaccountTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int id = values.at(i).at(ACCOUNT_ID_IDX).toInt();
            m_allAccount.insert(id);
            QString name = values.at(i).at(ACCOUNT_NAME_IDX).toString();
            EaccountType type = (EaccountType)values.at(i).at(ACCOUNT_TYPE_IDX).toInt();
            if (type == INCOME_ACCOUNT_TYPE || type == REQUEST_ACCOUNT_TYPE) {
                m_incomeAccountList.push_back(name);
            } else if (type == PAYMENT_ACCOUNT_TYPE || type == REMITTANCE_ACCOUNT_TYPE ){
                m_paymentAccountList.push_back(name);
            } else if (type == BANK_ACCOUNT_TYPE || type == LOAN_ACCOUNT_TYPE) {
                m_incomeAccountList.push_back(name);
                m_paymentAccountList.push_back(name);
            }


            m_accountIdNameMap.insert(id, name);
            m_accountNameIdMap.insert(name, id);
            m_accountList.push_back(name);
            m_accountNameTypeMap.insert(name, type);
            if (values.at(i).at(ACCOUNT_USE_HQ_RECEIPT_IDX).toBool()) {
                m_accountListWithHqReceipt.push_back(name);
                m_accountIDListWithHqReceipt.insert(id);
            }

            m_accountTypes.insert(id, type);
            int paymentAccountID = values.at(i).at(ACCOUNT_PAYMENT_ACCOUNT_ID_IDX).toInt();
            if (paymentAccountID > 0) {
                m_accountDepPaymentAccountMap[id] = paymentAccountID;
            }
        }
        m_accountList.sort();
        m_accountListWithHqReceipt.sort();
        m_incomeAccountList.sort();
        m_paymentAccountList.sort();
    }



}

void CaccountMap::populateAccountDeptMap()
{
    m_deptIDNameMap.clear();
    m_deptNameIDMap.clear();
    m_deptAccountsMap.clear();
    m_accountDeptMap.clear();
    m_departmentList.clear();
    m_deptAccountTableMap.clear();

    QString condition;
    TobjectList values;
    if (CdepartmentAccountTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            const Tobjects& deptAccObj = values.at(i);
            int id = deptAccObj.at(DEPARTMENT_ACCOUNT_ID_IDX).toInt();
            int depAccID = deptAccObj.at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
            m_deptAccountTableMap.insert(id, depAccID);
        }

    }


    values.clear();
    condition.clear();
    if (CdepartmentTable::Object()->getTableValues(condition, values)) {

        int size = values.size();
        for (int i = 0;  i < size; ++i) {
            const Tobjects& dept = values.at(i);
            int id = dept.at(DEPARTMENT_ID_IDX).toInt();
            QString name = dept.at(DEPARTMENT_NAME_IDX).toString();
            if (name.contains("upa committee", Qt::CaseInsensitive)) { // Could there be better way to check
                m_defaultDept = id;
            }
            //ui->m_deptCombo->addItem(name);
            m_deptIDNameMap.insert(id, name);
            m_deptNameIDMap.insert(name, id);
            m_departmentList.push_back(name);
            int paymentAccountID = dept.at(DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX).toInt();
            if (paymentAccountID > 0) {
                m_accountDepPaymentAccountMap[id] = paymentAccountID;

            }

        }
    }
    m_departmentList.sort();

    values.clear();
    condition.clear();
    //qDebug()<<"populate for dept account mapping\n";
    if (CdepartmentAccountTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int accountOrDeptID = values.at(i).at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
            int isDepartment = values.at(i).at(DEPARTMENT_ACCOUNT_IS_DEPT).toInt();
            int parent = values.at(i).at(DEPARTMENT_ACCOUNT_PARENT_ID).toInt();

            int deptORAccParentID = m_deptAccountTableMap[parent];

            //qDebug()<<"account id "<<accountOrDeptID <<" parent "<<parent<<" is dept "<<isDepartment;
            if (isDepartment == 0) {
                //qDebug()<<"Accountname: "<<getAccountName(accountOrDeptID);
                if (m_deptIDNameMap.find(deptORAccParentID) != m_deptIDNameMap.end()) {
                    m_accountDeptMap.insert(accountOrDeptID, deptORAccParentID);
                    QSet<int>& accounts = m_deptAccountsMap[deptORAccParentID];
                    accounts.insert(accountOrDeptID);
                    //qDebug()<<"Add mappint for dept "<<getDeptName(deptORAccParentID)<<" with account "<<getAccountName(accountOrDeptID);
                } else {
                    //qDebug()<<"account parent ";
                    //accountParent.insert(accountOrDeptID, deptORAccParentID);
                }
            } else {
                //qDebug()<<"Dept name "<<getDeptName(accountOrDeptID);
            }
        }
    }

}


QString CaccountMap::getAccountName(int id) const
{
    QMap<int, QString>::const_iterator it = m_accountIdNameMap.find(id);
    if (it != m_accountIdNameMap.end()) {
        return it.value();
    }
    return "";
}

int CaccountMap::getAccountID(const QString& accontName) const
{
    QMap<QString, int>::const_iterator it = m_accountNameIdMap.find(accontName);
    if (it != m_accountNameIdMap.end()) {
        return it.value();
    }
    return 0;
}

int CaccountMap::getAccountType(const QString &accountName) const
{
    QMap<QString, int>::const_iterator it = m_accountNameTypeMap.find(accountName);
    if (it != m_accountNameTypeMap.end()) {
        return it.value();
    }
    return INVALID_ACOUNT_TYPE;
}

QString CaccountMap::getDeptName(int id) const
{
    QMap<int, QString>::const_iterator it = m_deptIDNameMap.find(id);
    if (it != m_deptIDNameMap.end()) {
        return it.value();
    }
    return "";
}

int CaccountMap::getDeptId(const QString &deptName) const
{
    QMap<QString, int>::const_iterator it  = m_deptNameIDMap.find(deptName);
    if (it != m_deptNameIDMap.end()) {
        return it.value();
    }
    return 0;
}

int CaccountMap::getDeptForAccount(int accountID) const
{
    QMap<int, int>::const_iterator it = m_accountDeptMap.find(accountID);
    if (it != m_accountDeptMap.end()) {
        return it.value();
    }
    return 0;
}

bool CaccountMap::getAccounsForDept(int deptID, QSet<int> &accounts) const
{
    accounts.clear();
    bool ret = false;
    QMap<int, QSet<int> >::const_iterator fn = m_deptAccountsMap.find(deptID);
    if (fn != m_deptAccountsMap.end()) {
        const QSet<int>& accountOfDept = fn.value();
        for (auto id : accountOfDept) {
            accounts.insert(id);
            ret = true;
        }
    }

    return ret;
}

EaccountType CaccountMap::getAccountType(int accountID)
{
    EaccountType type = INVALID_ACOUNT_TYPE;
    auto fn = m_accountTypes.constFind(accountID);
    if (fn != m_accountTypes.end() ) {
        type = fn.value();
    }
    return type;
}

QString CaccountMap::getPaymentAccountNameForAccountDepID(int id)
{
    QString name;
    int paymentID = 0;
    if (id > 0) {
        QMap<int, int>::iterator fn = m_accountDepPaymentAccountMap.find(id);
        if (fn != m_accountDepPaymentAccountMap.end()) {
            paymentID = fn.value();

        } else {
            QMap<int, QString>::iterator fn = m_accountIdNameMap.find(id);
            if (fn != m_accountIdNameMap.end()) { // is account
                //get dept
                id = getDeptForAccount(id);
                name = getPaymentAccountNameForAccountDepID(id);
            }
        }
        if (paymentID) {
            QMap<int, QString>::iterator f =  m_paymentAccountIDNameMap.find(paymentID);
            if (f != m_paymentAccountIDNameMap.end()) {
                name = f.value();
            }
        }
    }
    return name;
}

