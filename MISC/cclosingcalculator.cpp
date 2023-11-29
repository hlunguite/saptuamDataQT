#include "cclosingcalculator.h"
#include "cclosingbalancetable.h"
#include "caccountmap.h"
#include "ctransactiontable.h"
#include "csdutils.h"
#include "ctransactionutils.h"
#include "cremittancetable.h"
#include "cremittancedetails.h"
CclosingCalculator::CclosingCalculator()
{
    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
    m_remittanceID = 0;
    m_requestID = 0;
    m_lastTransDate = QDate::currentDate();
     //std::set<int>       m_accountWithoutDept;
    const QStringList& accounts = CaccountMap::Object()->getAccountList();
     for (auto account: accounts){
        int id = CaccountMap::Object()->getAccountID(account);
         if (id > 0) {
            m_accountWithoutDept.insert(id);
            EaccountType accType = CaccountMap::Object()->getAccountType(id);
            if (accType == REMITTANCE_ACCOUNT_TYPE) {
                m_remittanceID = id;
            } else if (accType == REQUEST_ACCOUNT_TYPE) {
                m_requestID = id;
            }

         }
    }
    const QStringList& depList = CaccountMap::Object()->getDeptList();
    for (auto dept : depList) {
        int deptID = CaccountMap::Object()->getDeptId(dept);
        QSet<int> accounts;
        if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)){
            for (auto accountID : accounts) {
                m_accountDeptMap[accountID] = deptID;
                m_accountWithoutDept.equal_range(accountID);
            }
        } else {

        }

    }

}

void CclosingCalculator::calculateClosing(QDate fromDate, QDate toDate)
{
    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
    QString query = getQeryStr(fromDate, toDate);

    calculateClosing(fromDate, query);
}

void CclosingCalculator::calculateClosing(QDate fromDate, QString query)
{
    m_closingValues.clear();
    m_openingValues.clear();
    m_accountIncomeAndPayment.clear();
    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
    QDate prevDate = CsdUtils::getPrevDay(fromDate);
    accountDeptAmountType openingValue;
    accountCashORBankAmountType cashBankValue;
    calculateClosingAsOnInternal(prevDate, m_openingValues, cashBankValue);
    m_cashOpening = cashBankValue[gCashAccountName];
    m_bankOpening = cashBankValue[gBankAccountName];

    for (auto values : m_openingValues) {
        int id = values.first;
        double amt = values.second;
        m_closingValues[id] += amt;
    }

    accountCashORBankAmountType cashBankValue1;
    getAccountIncomePaymentForQuery(query, m_accountIncomeAndPayment, cashBankValue1);


    for (auto keyValue : m_accountIncomeAndPayment) {
        int accountID = keyValue.first;
        double incomeAmt = keyValue.second.first;
        double paymentAmt = keyValue.second.second;
        auto fn =  m_accountDeptMap.find(accountID);
        int idToUse = accountID;
        if (fn != m_accountDeptMap.end()) {
            idToUse = fn->second;
        }
        m_closingValues[idToUse] += incomeAmt;
        m_closingValues[idToUse] -= paymentAmt;
    }

    m_cashClosing = cashBankValue1[gCashAccountName] + m_cashOpening;
    m_bankClosing = cashBankValue1[gBankAccountName] + m_bankOpening;

}

void CclosingCalculator::calculateClosingAsOn(QDate date)
{

    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
    m_closingValues.clear();
    m_openingValues.clear();
    m_accountIncomeAndPayment.clear();
    //accountDeptAmountType closingValue;
    accountCashORBankAmountType cashBankValue;
    calculateClosingAsOnInternal(date, m_closingValues, cashBankValue);
    m_bankClosing = cashBankValue[gBankAccountName];
    m_cashClosing = cashBankValue[gCashAccountName];

}

double CclosingCalculator::cashOpening() const
{
    return m_cashOpening;
}

double CclosingCalculator::cashClosing() const
{
    return m_cashClosing;
}

double CclosingCalculator::bankOpening() const
{
    return m_bankOpening;
}

double CclosingCalculator::bankClosing() const
{
    return m_bankClosing;
}

QDate CclosingCalculator::lastTransDate() const
{
    return m_lastTransDate;
}

accountTwoAmountType CclosingCalculator::accountIncomeAndPayment() const
{
    return m_accountIncomeAndPayment;
}
void CclosingCalculator::calculateClosingAsOnInternal(QDate openingDate,
                                                      accountDeptAmountType &closingValue,
                                                      accountCashORBankAmountType& cashBankValue)
{
    SclosingData closingData = CclosingBalanceTable::Object()->getClosingForDate(openingDate, true);
    if (closingData.m_id > 0) { // found a closing
        m_lastTransDate = closingData.m_closingDate;
        QVector<SclosingDetailData*> closingDetail = CclosingBalanceDetailTable::Object()->getClosingDetailForID(closingData.m_id);
        double prevRemittance = 0;
        double prevRequest = 0;
        for (auto detail : closingDetail) {
            int accountOrDepID = detail->m_accountOrDeptTableID;
            double amount = detail->m_amount;
            //double payment = detail->m_amountPayment;
            //bool isAccount = detail->m_isAccount;
            QString cashOrBank = detail->m_cashOrBankName;
            if (cashOrBank.isEmpty() == false) {
                if (cashOrBank == gCashAccountName) {
                    cashBankValue[gCashAccountName] += amount;
                } else {
                    cashBankValue[gBankAccountName] += amount;
                }
                continue;
            }
            if (accountOrDepID == m_remittanceID) {
                prevRemittance +=  amount;
            } else if (accountOrDepID == m_requestID) {
                prevRequest += amount;
            }
            closingValue[accountOrDepID] += amount;

        }

        QDate prevClosing = closingData.m_closingDate;
        if (prevClosing < openingDate) {
            prevClosing = CsdUtils::getNextDay(prevClosing);
            QString query = getQeryStr(prevClosing, openingDate);
            double diff = abs(prevRemittance + prevRequest);
            if (diff < 1) {
                closingValue[m_remittanceID] = 0;
                closingValue[m_requestID] = 0;
            }

            accountTwoAmountType accountIncomeAndPayment;
            getAccountIncomePaymentForQuery(query, accountIncomeAndPayment, cashBankValue);
            for (auto keyValue : accountIncomeAndPayment) {
                int accountID = keyValue.first;
                double incomeAmt = keyValue.second.first;
                double paymentAmt = keyValue.second.second;
                auto fn =  m_accountDeptMap.find(accountID);
                int idToUse = accountID;
                if (fn != m_accountDeptMap.end()) {
                    idToUse = fn->second;
                }
                closingValue[idToUse] += incomeAmt;
                closingValue[idToUse] -= paymentAmt;
            }


        } else {

        }
    }
}


QString CclosingCalculator::getQeryStr(QDate fromDate, QDate toDate)
{

    QString startDateStr = fromDate.toString("yyyy-MM-dd");
    QString toDateStr = toDate.toString("yyyy-MM-dd");

    QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName();
    query += " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND (";
    query += CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDateStr  + "\" AND \"";
    query += toDateStr +  "\")";

    return query;
}

void CclosingCalculator::getAccountIncomePaymentForQuery(QString query,
                                                         accountTwoAmountType &processedDate,
                                                         accountCashORBankAmountType& cashBankValue)

{




    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    //qDebug()<<"number of transaction "<<results.size();
    for (auto& transData: results) {
        int accountID = transData->m_accountId;
        double amount = transData->m_amount;
        if (m_lastTransDate < transData->m_date) {
            m_lastTransDate = transData->m_date;
        }
        EtransactionType type = (EtransactionType)transData->m_type;
        EtransactionMode mode = (EtransactionMode)transData->m_mode;
        EaccountType accType = CaccountMap::Object()->getAccountType(accountID);
        bool isIncome = (accType == INCOME_ACCOUNT_TYPE);
        isIncome |= (accType == LOAN_ACCOUNT_TYPE && type == LOAN_RECOVERY_TRANSACTION_TYPE);
        bool isPayment = false;
        if (isIncome == false) {
            isPayment = (accType == PAYMENT_ACCOUNT_TYPE);
            isPayment |= (accType == LOAN_ACCOUNT_TYPE && type == LOAN_DISBURSE_TRANSACTION_TYPE);
        }
        if (accType == REQUEST_ACCOUNT_TYPE) {
            std::pair<double, double> & amt = processedDate[accountID];
            amt.first += amount;
            if (mode == CASH_TRANSACTION_MODE) {
                cashBankValue[gCashAccountName] += amount;
            } else {
                cashBankValue[gBankAccountName] += amount;
            }

        } else if (type == BANK_CASH_DEPOSIT_TRANSACTION_TYPE || mode == BANK_CASH_DEPOSIT_TRANSACTION_MODE) {
             cashBankValue[gCashAccountName] -= amount;
             cashBankValue[gBankAccountName] += amount;
        } else if (type == BANK_INTEREST_TRANSACTION_TYPE || mode == BANK_INTEREST_TRANSACTION_MODE){
             cashBankValue[gBankAccountName] += amount;
        } else if (type == BANK_CHARGES_TRANSACTION_TYPE || mode == BANK_CHARGES_TRANSACTION_MODE){
             cashBankValue[gBankAccountName] -= amount;
        } else if (isIncome) {
            std::pair<double, double> & amt = processedDate[accountID];
            amt.first += amount;
            if (mode == CASH_TRANSACTION_MODE) {
                cashBankValue[gCashAccountName] += amount;
            } else {
                cashBankValue[gBankAccountName] += amount;
            }
        } else if (isPayment) {
            if (mode == CASH_TRANSACTION_MODE) {
                cashBankValue[gCashAccountName] -= amount;
            } else {
                cashBankValue[gBankAccountName] -= amount;
            }
            std::pair<double, double> & amt = processedDate[accountID];
            amt.second += amount;
        } else if (accType == REMITTANCE_ACCOUNT_TYPE) {
            if (mode == CASH_TRANSACTION_MODE) {
                cashBankValue[gCashAccountName] -= amount;
            } else {
                cashBankValue[gBankAccountName] -= amount;
            }
            double remittance = 0;
            int transID = transData->m_id;

            SremittanceData* remitData = CremittanceTable::Object()->getRemittanceDataFromTransID(transID);
            if (remitData) {
                int remitTableID = remitData->m_id;
                std::map<int, std::pair<double, double> > accountPct;
                //qDebug()<<"Remit Table id "<<remitTableID;
                QVector<SremittanceDetailData*>* vData = CremittanceDetailTable::Object()->getRemittanceDetailForTableID(remitTableID);
                if (vData) {
                    for (auto data : *vData) {
                        int	accountTableID = data->m_accuntTableID;
                        double	localShare = data->m_localShare;
                        double   hqShare = data->m_hqShare;
                        //QString accountName = CaccountMap::Object()->getAccountName(accountID);
                        //qDebug()<<accountName<<" hq pc "<<hqShare<<" local pc "<<localShare;
                        accountPct[accountTableID] = std::make_pair(hqShare,localShare);
                        //nameID.insert({accountName, accountID});
                        delete data;
                    }
                    delete vData;
                }
                QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";
                QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";
                query += CtransactionTable::Object()->getColName(TRANSACTION_REMITTANCE_ID_IDX) + "=" + QString::number(remitTableID) ;
                QVector<StransactionData*> remitTrans = CtransactionTable::Object()->getAllTransaction(query);
                //add remittance expense
                for (auto rtran: remitTrans) {
                    double amount = rtran->m_amount;
                    int transAccountID = rtran->m_accountId;
                    std::map<int, std::pair<double, double> >::iterator fn = accountPct.find(transAccountID);
                    double hqPC = 0;
                    double localPC = 100;
                    if (fn != accountPct.end()) {
                        hqPC = fn->second.first;
                        localPC = fn->second.second;
                    } else {
                        //QString accountName = CaccountMap::Object()->getAccountName(transAccountID);
                    }
                    if (localPC < 100) {
                        double hq = (amount*hqPC)/(double)100;
                        std::pair<double, double> & amt = processedDate[transAccountID];
                        amt.second += hq;
                        remittance += (amount*localPC)/(double)100;
                    } else {
                        remittance += amount;
                    }
                    delete rtran;
                }

                QVector<SremittanceReconcileData*>* reconData = CremittanceReconcileTable::Object()->getRemittanceReconcileForTableID(remitTableID);
                if (reconData) {
                    for (auto data :*reconData) {
                        int id = data->m_accontOrDeptTableID;
                        QString	str = data->m_str;
                        SremitTransDetail detail(str);
                        double amount = detail.m_amount;
                        double hqPC = 0;
                        double localPC = 100;
                        std::map<int, std::pair<double, double> >::iterator fn = accountPct.find(id);

                        if (fn != accountPct.end()) {
                            hqPC = fn->second.first;
                            localPC = fn->second.second;
                        }
                        if (localPC < 100) {
                            //double local = (amount*localPC)/100;
                            double hq = (amount*hqPC)/(double)100;
                            std::pair<double, double> & amt = processedDate[id];
                            amt.second += hq;
                            remittance += (amount*localPC)/(double)100;
                        } else {
                            remittance += amount;

                        }
                        delete data;

                    }
                    delete reconData;
                }
                delete remitData;
            }
            if (remittance > 0) {
                std::pair<double, double> & amt = processedDate[accountID];
                amt.second += remittance;
            }

        } else {
            qDebug()<<transData->m_id<<" "<<CaccountMap::Object()->getAccountName(accountID)<<" "<<amount<<" "<<transData->m_date.toString("dd MMM yyyy");
            Q_ASSERT(0);
        }

        delete transData;
    }


}

