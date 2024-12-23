#include "cledger.h"
#include "ccontactmap.h"
#include "caccountmap.h"
#include "ctransactionutils.h"

Cledger::Cledger():m_cashAccount(true),m_bankAccount(true) {}

Cledger::~Cledger()
{
    for (auto [key, value]: m_allAccountLedger.asKeyValueRange()) {
        for (auto [id, account] : value.asKeyValueRange()) {
            delete account;
        }
    }
    m_allAccountLedger.clear();
}

void Cledger::clear()
{
    m_allAccountLedger.clear();
    m_bankAccount.clear();
    m_cashAccount.clear();
}

void Cledger::populateLedger(QString startDate,
                             QString toDate,
                             QSet<int> &accounts,
                             QString name,
                             bool individualLedger)
{
    clear();
    QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName();
    query += " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND (";
    query += CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDate  + "\" AND \"";
    query += toDate +  "\")";

    if (accounts.isEmpty() == false) {
        query +=" AND (";
        int first = true;
        for (int id : accounts) {
            if (first == false) {
                query += " OR ";
            }
            first = false;
            query +=  CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(id);
        }
        query += ")";
    }

    if (name.isEmpty() == false) {
        int id = CcontactMap::Object()->getContactID(name);
        query += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_FROM_IDX) + "=" + QString::number(id) + ")";
    }

    int loanAccountID = 0;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    for (auto& transData: results) {
        int deptID = CaccountMap::Object()->getDeptForAccount(transData->m_accountId);
        if (deptID == 0) {
            deptID = 50000;
        }
        QString mode = CtransactionUtils::Object()->getTransactionModeTwoTypeStr((EtransactionMode)transData->m_mode);
        if (individualLedger == false) {
            if (BANK_CASH_DEPOSIT_TRANSACTION_TYPE == transData->m_type) {
                StransactionData* bank = new StransactionData(*transData);
                bank->m_type = INCOME_TRANSACTION_TYPE;
                bank->m_mode = INTERNET_TRANSACTION_MODE;
                StransactionData* cash = new StransactionData(*transData);
                cash->m_type =PAYMENT_TRANSACTION_TYPE;
                cash->m_mode = CASH_TRANSACTION_MODE;
                if (bank->m_ref.isEmpty()) {
                    bank->m_ref = gBankCashDepositName;
                    cash->m_ref = gBankCashDepositName;
                }
                m_bankAccount.addTransaction(bank);
                m_cashAccount.addTransaction(cash);

            } else if (BANK_CASH_WITHDRAW_TRANSACTION_TYPE == transData->m_type) {
                StransactionData* bank = new StransactionData(*transData);
                bank->m_type = PAYMENT_TRANSACTION_TYPE;
                bank->m_mode = INTERNET_TRANSACTION_MODE;
                StransactionData* cash = new StransactionData(*transData);
                cash->m_type = INCOME_TRANSACTION_TYPE;
                cash->m_mode = CASH_TRANSACTION_MODE;
                if (bank->m_ref.isEmpty()) {
                    bank->m_ref = gBankCashDepositName;
                    cash->m_ref = gBankCashDepositName;
                }
                m_bankAccount.addTransaction(bank);
                m_cashAccount.addTransaction(cash);

            }
            else {

                if (mode == "Bank") {
                    m_bankAccount.addTransaction(new StransactionData(*transData));
                } else {
                    m_cashAccount.addTransaction(new StransactionData(*transData));
                }
            }
        }
        int accountOrDeptID = individualLedger ? transData->m_accountId : deptID;
        int fromOrAccountID = individualLedger ? transData->m_fromId : transData->m_accountId;
        int accountToCheck = individualLedger ? 0 : transData->m_accountId;
        QMap<int, CaccountLedger*>& accountLedgerMap =  m_allAccountLedger[accountOrDeptID];
        CaccountLedger* accountLedger = accountLedgerMap[fromOrAccountID];
        if (accountLedger == nullptr) {
            if (accountToCheck) {
                EaccountType accType = CaccountMap::Object()->getAccountType(accountToCheck);
                if (accType == REMITTANCE_ACCOUNT_TYPE) {
                    accountLedger = new CremitttanceLedger;

                } else if (accType == LOAN_ACCOUNT_TYPE) {
                    accountLedger = new CloanLedger;
                    loanAccountID = accountToCheck;
                } else {
                    accountLedger = new CaccountLedger;

                }
            } else {
                accountLedger = new CaccountLedger;
            }
        }

        accountLedger->addTransaction(transData);
        accountLedgerMap[fromOrAccountID] = accountLedger;
    }

    if (loanAccountID == 0 && results.size() > 0) {
        int loanindex = CaccountMap::Object()->getAccountID(gLoanAccountName);
        bool hasLoanAccount = (accounts.size() == 1) && (accounts.contains(loanindex));
        if (individualLedger == false && (accounts.isEmpty() || hasLoanAccount)) {
            CloanLedger* loanLedger = new CloanLedger();
            if (loanLedger->processTransForLoanAcc()) {
                QMap<int, CaccountLedger*>& accountLedgerMap =  m_allAccountLedger[50000 /*deptID*/];
                //qDebug()<<"loan account name "<<gLoanAccountName<<" id "<<id;
                accountLedgerMap[loanindex] = loanLedger;
            } else  {
                delete loanLedger;
            }
        }
    }
}

const QMap<int, QMap<int, CaccountLedger *> >& Cledger::allAccountLedger() const
{
    return m_allAccountLedger;
}

CaccountLedger& Cledger::cashAccount()
{
    return m_cashAccount;
}

CaccountLedger& Cledger::bankAccount()
{
    return m_bankAccount;
}

