#include "cclosingcalculator.h"
#include "cclosingbalancetable.h"
#include "caccountmap.h"
#include "ctransactiontable.h"
#include "csdutils.h"
#include "ctransactionutils.h"
#include "cremittancetable.h"
CclosingCalculator::CclosingCalculator()
{
    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
     //std::set<int>       m_accountWithoutDept;
    const QStringList& accounts = CaccountMap::Object()->getAccountList();
     for (auto account: accounts){
        int id = CaccountMap::Object()->getAccountID(account);
         if (id > 0) {
            m_accountWithoutDept.insert(id);
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

    QString query = getQeryStr(fromDate, toDate);

    calculateClosing(fromDate, query);
}

void CclosingCalculator::calculateClosing(QDate fromDate, QString query)
{
    m_openingAndClosingBalance.clear();
    m_accountIncomeAndPayment.clear();
    m_cashOpening = 0;
    m_cashClosing = 0;
    m_bankOpening = 0;
    m_bankClosing = 0;
    QDate prevDate = CsdUtils::getPrevDay(fromDate);

    calculateClosing(prevDate);
    //std::map<int, std::pair<double, double> > processedDate;
    processQuery(query, m_accountIncomeAndPayment);


}

void CclosingCalculator::calculateClosing(QDate openingDate)
{

    SclosingData closingData = CclosingBalanceTable::Object()->getClosingForDate(openingDate, true);
    if (closingData.m_id > 0) { // found a closing
        QVector<SclosingDetailData*> closingDetail = CclosingBalanceDetailTable::Object()->getClosingDetailForID(closingData.m_id);
        for (auto detail : closingDetail) {
            int accountOrDepID = detail->m_accountOrDeptTableID;
            double amount = detail->m_amount;
            //double payment = detail->m_amountPayment;
            bool isAccount = detail->m_isAccount;
            QString cashOrBank = detail->m_cashOrBankName;
            if (cashOrBank.isEmpty() == false) {
                if (cashOrBank == gCashAccountName) {
                    m_cashOpening += amount;
                } else {
                    m_bankOpening += amount;
                }
                continue;
            }
            /*if (isAccount) {
                 //std::map<int, int>
                auto fn = m_accountDeptMap.find(accountOrDepID);
                if (fn != m_accountDeptMap.end()) {
                    accountOrDepID = fn->second;
                }
            }*/
            std::pair<double, double> & amounts = m_openingAndClosingBalance[accountOrDepID];
            amounts.first += amount;


        }
        QDate prevClosing = closingData.m_closingDate;
        if (prevClosing < openingDate) {
            prevClosing = CsdUtils::getNextDay(prevClosing);
            QString query = getQeryStr(prevClosing, openingDate);
            std::map<int, std::pair<double, double> > processedDate;
            processQuery(query, processedDate);
            for (auto data: processedDate) {
                qDebug()<<CaccountMap::Object()->getAccountName(data.first)<<" income:"<<data.second.first<<" payment:"<<data.second.second;
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

void CclosingCalculator::processQuery(QString query, std::map<int, std::pair<double, double> > &processedDate)
{
    //processedDate account total income and payment
    qDebug()<<"query "<<query;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    for (auto& transData: results) {
        int accountID = transData->m_accountId;
        double amount = transData->m_amount;
        EtransactionType type = (EtransactionType)transData->m_type;
        //int mode = transData->m_mode;
        EaccountType accType = CaccountMap::Object()->getAccountType(accountID);
        bool isIncome = ((accType == INCOME_ACCOUNT_TYPE) || (accType == REQUEST_ACCOUNT_TYPE));
        isIncome  |=  ((accType == BANK_ACCOUNT_TYPE) && (type == BANK_INTEREST_TRANSACTION_TYPE || type == BANK_CASH_DEPOSIT_TRANSACTION_TYPE));
        isIncome |= (accType == LOAN_ACCOUNT_TYPE && type ==LOAN_RECOVERY_TRANSACTION_TYPE);
        bool isPayment = false;
        if (isIncome == false) {
            isPayment = (accType == PAYMENT_ACCOUNT_TYPE);
            isPayment |= (accType == BANK_ACCOUNT_TYPE && type == BANK_CHARGES_TRANSACTION_TYPE);
            isPayment |= (accType == LOAN_ACCOUNT_TYPE && type == LOAN_DISBURSE_TRANSACTION_TYPE);
        }

        if (isIncome) {
            std::pair<double, double> & amt = processedDate[accountID];
            amt.first += amount;
        } else if (isPayment) {
            std::pair<double, double> & amt = processedDate[accountID];
            amt.second += amount;
        } else if (accType == REMITTANCE_ACCOUNT_TYPE) {
            std::pair<double, double> & amt = processedDate[accountID];
            amt.second += amount;
            int transID = transData->m_id;

            SremittanceData* remitData = CremittanceTable::Object()->getRemittanceDataFromTransID(transID);
            if (remitData) {
                int remitTableID = remitData->m_id;
                std::map<int, std::pair<double, double> > accountPct;
                QVector<SremittanceDetailData*>* vData = CremittanceDetailTable::Object()->getRemittanceDetailForTableID(remitTableID);
                if (vData) {
                    for (auto data : *vData) {
                        int	accountID = data->m_accuntTableID;
                        double	localShare = data->m_localShare;
                        double   hqShare = data->m_hqShare;
                        //QString accountName = CaccountMap::Object()->getAccountName(accountID);
                        accountPct[accountID] = std::make_pair(hqShare,localShare);
                        //nameID.insert({accountName, accountID});
                        delete data;
                    }
                    delete vData;
                }
                QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";
                QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";
                query += QString::number(remitTableID) +  ")";

                QVector<StransactionData*> remitTrans = CtransactionTable::Object()->getAllTransaction(query);
                //add remittance expense
                for (auto rtran: remitTrans) {
                    double amount = rtran->m_amount;
                    int accountID = rtran->m_accountId;
                    std::map<int, std::pair<double, double> >::iterator fn = accountPct.find(accountID);
                    double hqPC = 0;
                    double localPC = 100;
                    if (fn != accountPct.end()) {
                        hqPC = fn->second.first;
                        localPC = fn->second.second;
                    }
                    if (localPC < 100) {
                        //double local = (amount*localPC)/100;
                        double hq = (amount*hqPC)/100;
                        std::pair<double, double> & amt = processedDate[accountID];
                        amt.second += hq;
                    }
                    delete rtran;
                }

                QVector<SremittanceReconcileData*>* reconData = CremittanceReconcileTable::Object()->getRemittanceReconcileForTableID(remitTableID);
                if (reconData) {
                    for (auto data :*reconData) {
                        int id = data->m_accontOrDeptTableID;
                        QString	str = data->m_str;
                        SremitTransDetail detail(str);
                        double amount = detail.m_amount.toDouble();
                        double hqPC = 0;
                        double localPC = 100;
                        std::map<int, std::pair<double, double> >::iterator fn = accountPct.find(id);

                        if (fn != accountPct.end()) {
                            hqPC = fn->second.first;
                            localPC = fn->second.second;
                        }
                        if (localPC < 100) {
                            //double local = (amount*localPC)/100;
                            double hq = (amount*hqPC)/100;
                            std::pair<double, double> & amt = processedDate[id];
                            amt.second += hq;
                        }
                        delete data;

                    }
                    delete reconData;
                }
                delete remitData;
            }


        } else {
            qDebug()<<transData->m_id<<" "<<CaccountMap::Object()->getAccountName(accountID)<<" "<<amount<<" "<<transData->m_date.toString("dd MMM yyyy");
            Q_ASSERT(0);
        }

        delete transData;
    }
}

