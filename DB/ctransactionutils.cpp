#include "ctransactionutils.h"
#include "caccounttable.h"

QStringList CtransactionUtils::getTransactionModeList()
{
    return m_modeList;
}

EtransactionMode CtransactionUtils::getTransactionMode(const QString &modeStr)
{
    EtransactionMode modeInt = CASH_TRANSACTION_MODE;
    QMap<QString, EtransactionMode>::iterator fn = m_modeStrTypeMap.find(modeStr);
    if (fn != m_modeStrTypeMap.end()) {
        modeInt = fn.value();
    }
    return modeInt;
}

QString CtransactionUtils::getTransactionModeStr(EtransactionMode mode)
{
    QString modeStr = "Cash";
    QMap<EtransactionMode, QString >::iterator fn = m_modeModeStrMap.find(mode);
    if (fn != m_modeModeStrMap.end()) {
        modeStr = fn.value();
    }
    return modeStr;
}

QString CtransactionUtils::getTransactionModeTwoTypeStr(EtransactionMode mode)
{
    QString modeStr = "Cash";
    QMap<EtransactionMode, QString >::iterator fn = m_modeModeTwoTypeStrMap.find(mode);
    if (fn != m_modeModeTwoTypeStrMap.end()) {
        modeStr = fn.value();
    }
    return modeStr;
}

bool CtransactionUtils::isModeRequireRef(const QString &mode)
{
    bool isRequireRef = false;
    if (mode.contains("Cheque") || mode.contains("Internet")) {
        isRequireRef = true;
    }
    return isRequireRef;
}

QStringList CtransactionUtils::getTransectionModeForTransactionType(EtransactionType type)
{
     QStringList transModes;
    switch(type) {
    case INCOME_TRANSACTION_TYPE:
    case  PAYMENT_TRANSACTION_TYPE:
    case LOAN_RECOVERY_TRANSACTION_TYPE:
    case LOAN_DISBURSE_TRANSACTION_TYPE:
        transModes.push_back(m_modeModeStrMap[CASH_TRANSACTION_MODE]);
        transModes.push_back(m_modeModeStrMap[CHEQUE_TRANSACTION_MODE]);
        transModes.push_back(m_modeModeStrMap[INTERNET_TRANSACTION_MODE]);
        break;
    case BANK_CHARGES_TRANSACTION_TYPE:
        transModes.push_back(m_modeModeStrMap[BANK_CHARGES_TRANSACTION_MODE]);
        break;
    case BANK_INTEREST_TRANSACTION_TYPE:
        transModes.push_back(m_modeModeStrMap[BANK_INTEREST_TRANSACTION_MODE]);
        break;
    case BANK_CASH_DEPOSIT_TRANSACTION_TYPE:
        transModes.push_back(m_modeModeStrMap[BANK_CASH_DEPOSIT_TRANSACTION_MODE]);

        break;
    default:
        break;
    }

    return transModes;


}



CtransactionUtils::CtransactionUtils()
{
    //m_modeList <<"Cash"<<"Cheque"<<"Internet Transfer"<<"Bank Cash Deposit"<<"Bank Charges"<<"Bank Interest"<<"Contra";
    m_modeModeStrMap.insert(CASH_TRANSACTION_MODE, "Cash");
    m_modeModeStrMap.insert(CHEQUE_TRANSACTION_MODE, "Cheque");
    m_modeModeStrMap.insert(INTERNET_TRANSACTION_MODE, "Internet Transfer");
    m_modeModeStrMap.insert(BANK_CASH_DEPOSIT_TRANSACTION_MODE, "Bank Cash Deposit");
    m_modeModeStrMap.insert(BANK_CHARGES_TRANSACTION_MODE, "Bank Charges");
    m_modeModeStrMap.insert(BANK_INTEREST_TRANSACTION_MODE, "Bank Interest");

    //m_modeModeTwoTypeStrMap
    m_modeModeTwoTypeStrMap.insert(CASH_TRANSACTION_MODE, "Cash");
    m_modeModeTwoTypeStrMap.insert(CHEQUE_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(INTERNET_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_CASH_DEPOSIT_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_CHARGES_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_INTEREST_TRANSACTION_MODE, "Bank");


    QMapIterator<EtransactionMode, QString > iterator1(m_modeModeStrMap);
    while (iterator1.hasNext()) {
        iterator1.next();
        EtransactionMode type = iterator1.key();
        QString typeStr = iterator1.value();
        m_modeList<<typeStr;
        m_modeStrTypeMap.insert(typeStr, type);
    }

    m_transTypeMap.insert(INCOME_TRANSACTION_TYPE, gIncomeTransactionType);
    m_transTypeMap.insert(PAYMENT_TRANSACTION_TYPE, gExpenseTransactionType);
    m_transTypeMap.insert(LOAN_RECOVERY_TRANSACTION_TYPE, gLoanRecoverTransactionType);
    m_transTypeMap.insert(LOAN_DISBURSE_TRANSACTION_TYPE, gLoanDisburseTransactionType);
    m_transTypeMap.insert(BANK_CHARGES_TRANSACTION_TYPE, gBankChargeTransactionType);
    m_transTypeMap.insert(BANK_INTEREST_TRANSACTION_TYPE, gBankInterestTransactionType);
    m_transTypeMap.insert(BANK_CASH_DEPOSIT_TRANSACTION_TYPE, gBankCashDepositTransactionType);

    QMapIterator<EtransactionType, QString> iterator2(m_transTypeMap);

    while (iterator2.hasNext()) {
        iterator2.next();
        EtransactionType type = iterator2.key();
        QString typeStr = iterator2.value();
        m_transTypeStrMap.insert(typeStr, type);
    }
}

CtransactionUtils::~CtransactionUtils()
{

}

EtransactionType CtransactionUtils::getTransactionType(const QString &typeStr)
{
    EtransactionType type = INVALID_TRANSACTION_TYPE;
    QMap<QString, EtransactionType>::iterator fn = m_transTypeStrMap.find(typeStr);
    if (fn != m_transTypeStrMap.end()) {
        type = fn.value();
    }
    return type;
}

QStringList CtransactionUtils::getAllTransctionTypeStr()
{
    QStringList types;
    for (auto type : m_transTypeMap) {
        types.push_back(type);
    }

    return types;

}

QStringList CtransactionUtils::getTransactionTypesForAccountType(int accountType)
{
    QStringList transTypes;
    switch((EaccountType)accountType) {
    case INCOME_ACCOUNT_TYPE:
    case REQUEST_ACCOUNT_TYPE: transTypes.push_back(getTransactionTypeStr(INCOME_TRANSACTION_TYPE));
        break;
    case PAYMENT_ACCOUNT_TYPE:
    case REMITTANCE_ACCOUNT_TYPE: transTypes.push_back(getTransactionTypeStr(PAYMENT_TRANSACTION_TYPE));
        break;
    case LOAN_ACCOUNT_TYPE: transTypes.push_back(getTransactionTypeStr(LOAN_RECOVERY_TRANSACTION_TYPE));
        transTypes.push_back(getTransactionTypeStr(LOAN_DISBURSE_TRANSACTION_TYPE));
        break;
    case BANK_ACCOUNT_TYPE:transTypes.push_back(getTransactionTypeStr(BANK_CHARGES_TRANSACTION_TYPE));
        transTypes.push_back(getTransactionTypeStr(BANK_INTEREST_TRANSACTION_TYPE));
        transTypes.push_back(getTransactionTypeStr(BANK_CASH_DEPOSIT_TRANSACTION_TYPE));
        break;
    default: break;
    }
    transTypes.sort();
    return transTypes;
}

bool CtransactionUtils::getIsTransactionIncome(EtransactionType type)
{
    bool ret = false;
    switch(type) {
    case INCOME_TRANSACTION_TYPE:
    case LOAN_RECOVERY_TRANSACTION_TYPE:
    case BANK_INTEREST_TRANSACTION_TYPE:
    case BANK_CASH_DEPOSIT_TRANSACTION_TYPE: ret = true; break;
    default: ret = false; break;
    }

    return ret;
}

bool CtransactionUtils::getIsTransactionPayment(EtransactionType type)
{

    bool ret = false;
    switch(type) {
    case PAYMENT_TRANSACTION_TYPE:
    case LOAN_DISBURSE_TRANSACTION_TYPE:
    case BANK_CHARGES_TRANSACTION_TYPE:
    /*case BANK_CASH_DEPOSIT_TRANSACTION_TYPE:*/ ret = true; break;
    default:ret = false; break;
    }

    return ret;
}

void CtransactionUtils::serialize(QDataStream &out)
{
    int size = m_modeModeStrMap.size();
    out<<size;
    for (auto keyvalue : m_modeModeStrMap.asKeyValueRange()) {
        out<<(int)keyvalue.first<<(QString)keyvalue.second;
    }
    size = m_transTypeMap.size();
    out<<size;
    for (auto keyvalue: m_transTypeMap.asKeyValueRange()) {
        out<<(int)keyvalue.first<<(QString)keyvalue.second;

    }
}

void CtransactionUtils::desserialize(QDataStream &in)
{
    int size;
    in>>size;
    for (int i = 0; i < size; ++i) {
        int mode;
        QString modestr;
        in>>mode>>modestr;
    }
    in>>size;
    for (int i = 0; i < size; ++i) {
        int mode;
        QString modestr;
        in>>mode>>modestr;
    }




}

QString CtransactionUtils::getTransactionTypeStr(EtransactionType type)
{
    QString typeStr = "";
    QMap<EtransactionType, QString >::iterator fn = m_transTypeMap.find(type);
    if (fn != m_transTypeMap.end()) {
        typeStr = fn.value();
    }
    return typeStr;
}
