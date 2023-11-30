#ifndef CDBDEFINE
#define CDBDEFINE
#include <QString>
#include <QVector>
//db primary key start number
#define SETTING_TABLE_START_NO 1

#define PAYMENT_ACCOUNT_TABLE_START_NO 100
#define DEPARTMENT_TABLE_START_NO 300
#define ACCOUNT_TABLE_START_NO  500
#define DEPARTMENT_ACCOUNT_TABLE_START_NO 1200


#define PERSON_TABLE_START_NO 2500
#define OTHER_CONTACT_TABLE_START_NO 10000
#define CONTACT_TABLE_START_NO 17000

#define   REMITTANCE_NAME_TABLE_START_NO 30000
#define   REMITTANCE_PERCENT_TABLE_START_NO 30100
#define    REMITTANCE_SETUP_TABLE_START_NO 30200

#define   REMITTANCE_TABLE_START_NO 35000
#define   REMITTANCE_DETAIL_TABLE_START_NO 135000
#define   REMITTANCE_RECONCILE_TABLE_START_NO 100000

#define   RECEIPT_BOOK_TABLE_START_NO 235000
#define   RECEIPT_TABLE_START_NO 300000
#define   RECEIPT_ACCOUNT_RECEIPT_BOOK_TABLE_START_NO 500000

#define   IMPORT_BANK_TRANSACTION_TABLE_START_NO 600000

#define   BANK_TRANSACTION_TABLE_START_NO 650000
#define   TRANSACTION_TABLE_START_NO 1000000

#define IMPORT_TABLE_START_NO 1
#define CLOSING_TABLE_START_NO 2000
#define CLOSING_DETAIL_TABLE_START_NO 10000



enum EtableType {
    ACCOUNT_TABLE,
    DEPARTMENT_TABLE,
    DEPARTMENT_ACCOUNT_TABLE,
    PAYMENT_ACCOUNT_TABLE,
    PERSON_TABLE,
    OTHER_CONTACT_TABLE,
    CONTACT_TABLE,
    TRANSACTION_TABLE,
    RECEIPT_BOOK_TABLE,
    RECEIPT_TABLE,
    RECEIPT_ACCOUNT_RECEIPT_BOOK_TABLE,
    IMPORT_BANK_TRANSACTION_TABLE,
    BANK_TRANSACTION_TABLE,
    REMITTANCE_PERCENT_TABLE,
    REMITTANCE_NAME_TABLE,
    REMITTANCE_SETUP_TABLE,
    REMITTANCE_TABLE,
    REMITTANCE_DETAIL_TABLE,
    REMITTANCE_RECONCILE_TABLE,
    SETTING_TABLE,
    MIN_IMPORT_TABLE,
    ACCOUNT_IMPORT_TABLE,
    TRANS_IMPORT_TABLE,
    CLOSING_TABLE,
    CLOSING_DETAIL_TABLE,
    MAX_TABLE_TYPE
};

const QVector<QString> GtableName(QVector<QString>()<< "accountTable"
                                  << "departmentTable"
                                  << "departmentAccountTable"
                                  << "paymentAccountTable"
                                  << "personTable"
                                  << "otherContactTable"
                                  << "contactTable"
                                  << "transctionTable"
                                  << "receiptBookTable"
                                  << "receiptTable"
                                  << "accountReceiptBook"
                                  << "importBankTransactionTable"
                                  << "bankTransactionTable"
                                  << "remittancePercentTable"
                                  << "remittanceNameTable"
                                  << "remittaceSetupTable"
                                  << "remittanceTable"
                                  << "remittanceDetailTable"
                                  << "remittanceReconcileTable"
                                  << "settingTable"
                                  << "minImportTable"
                                  <<"importAccountTable"
                                  <<"transImportTable"
                                  <<"closingTable"
                                  <<"closingDetailTable"
                                  <<"");


const QString gBankCashDepositName = "Bank Cash Deposit";
const QString gBankInterestName = "Bank Interest";
const QString gBankChargesName = "Bank Charges";
const QString gRemittanceAccountName = "Remittance";
const QString gRequestAccountName = "Request";
const QString gBankAccountName = "Bank Account";
const QString gCashAccountName = "Cash Account";
const QString gLoanAccountName = "Loan Account";
const QString gReceipSeprator = ";";
#endif // CDBDEFINE

