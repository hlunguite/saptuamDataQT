#ifndef CTRANSACTIONUTILS_H
#define CTRANSACTIONUTILS_H
#include <QStringList>
#include "cdbdefine.h"
#include "csdsingleton.h"
#include <QMap>
//number of element and order in EtableType and GtableName should match
#define FALSE 0
#define TRUE 1



enum EtransactionType {
    INVALID_TRANSACTION_TYPE = 0,
    INCOME_TRANSACTION_TYPE, //INCOME_ACCOUNT_TYPE, REQUEST_ACCOUNT_TYPE
    PAYMENT_TRANSACTION_TYPE, //PAYMENT_ACCOUNT_TYPE , REMITTANCE_ACCOUNT_TYPE
    LOAN_RECOVERY_TRANSACTION_TYPE, //LOAN_ACCOUNT_TYPE,
    LOAN_DISBURSE_TRANSACTION_TYPE, // LOAN_ACCOUNT_TYPE
    BANK_CHARGES_TRANSACTION_TYPE, //BANK_ACCOUNT_TYPE
    BANK_INTEREST_TRANSACTION_TYPE, // BANK_ACCOUNT_TYPE
    BANK_CASH_DEPOSIT_TRANSACTION_TYPE, // BANK_ACCOUNT_TYPE
};

enum EtransactionMode {
    CASH_TRANSACTION_MODE = 1,
    CHEQUE_TRANSACTION_MODE,
    INTERNET_TRANSACTION_MODE,
    BANK_CASH_DEPOSIT_TRANSACTION_MODE,
    BANK_CHARGES_TRANSACTION_MODE,
    BANK_INTEREST_TRANSACTION_MODE,
    CONTRA_TRANSACTION_MODE
};
class CtransactionUtils : public CsdSingleton<CtransactionUtils>
{
    friend class CsdSingleton<CtransactionUtils>;

public:
    QStringList getTransactionModeList();
    EtransactionMode getTransactionMode(const QString& mode);
    QString getTransactionModeStr(EtransactionMode mode);
    QString getTransactionModeTwoTypeStr(EtransactionMode mode);

    bool isModeRequireRef(const QString& mode);
    QStringList getTransectionModeForTransactionType(EtransactionType type);

    QString getTransactionTypeStr(EtransactionType type);
    EtransactionType     getTransactionType(const QString& type);
    QStringList getAllTransctionTypeStr();
    QStringList getTransactionTypesForAccountType(int accountType);
    bool getIsTransactionIncome(EtransactionType type);
    bool getIsTransactionPayment(EtransactionType type);
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
private:
    CtransactionUtils();
    ~CtransactionUtils();

    QStringList 			  		m_modeList;
    QMap<EtransactionMode, QString> m_modeModeStrMap;
    QMap<EtransactionMode, QString> m_modeModeTwoTypeStrMap;

    QMap<QString, EtransactionMode> m_modeStrTypeMap;

    QMap<EtransactionType, QString> m_transTypeMap;
    QMap<QString, EtransactionType> m_transTypeStrMap;

};

#endif // CTRANSACTIONUTILS_H
