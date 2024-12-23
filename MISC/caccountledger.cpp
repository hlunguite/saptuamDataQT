#include "caccountledger.h"
#include "chtmlutils.h"
#include "caccountmap.h"
#include "ccontactmap.h"
#include "ctransactionutils.h"
#include "csdutils.h"
#include "cremittancetable.h"
#include "cremittancedetails.h"

CaccountLedger::CaccountLedger(bool isBankOrCash):m_isBankOrCashLedger(isBankOrCash),
    m_totalIncome(0),
    m_totalPayment(0)
{

}

CaccountLedger::~CaccountLedger()
{
    //QMap<QDate, QVector<StransactionData*> >;
    for(auto [key, value] :m_transactions.asKeyValueRange()) {
        for (auto data: value) {
            delete data;
        }
    }
    m_transactions.clear();
}

void CaccountLedger::addTransaction(StransactionData *trans)
{

    m_transactions[trans->m_date].push_back(trans);
}

QString CaccountLedger::getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName)
{
    QString html;
    QString csvLine;
    m_csvTrans.clear();
    if (m_transactions.isEmpty()) {
        return html;
    }

    ChtmlUtils htmlUtils;
    QString accountName = replaceAccountName;
    QString deptName;
    QString incomeStr("Income");
    QString paymentStr("Payment");

    QString line;
    QString htmlLine;
    if (m_isBankOrCashLedger == false && (indiviualAccount == false)) {
        accountName = CaccountMap::Object()->getAccountName(accountID);
        int deptID = CaccountMap::Object()->getDeptForAccount(accountID);
        if (deptID) {
            deptName = CaccountMap::Object()->getDeptName(deptID);
            line = deptName + "\n";
            htmlLine = deptName + "<br>";
        }
    } else if (indiviualAccount){
        if (replaceAccountName.isEmpty()) {
            accountName = CcontactMap::Object()->getContanceName(accountID);
        } else {
            accountName = accountName + ": " + CcontactMap::Object()->getContanceName(accountID);
        }
    }

    csvLine += line + "Ledger for " + accountName;
    htmlLine += "Ledger for " + accountName;
    htmlUtils.formatString(htmlLine, true);

    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(9);
    htmlUtils.addTableColSize(18);
    htmlUtils.addTableColSize(10);
    htmlUtils.addTableColSize(5);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(11);
    htmlUtils.addTableColSize(11);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);
    htmlUtils.openTable();

    htmlUtils.createRow();
    csvLine += "\n";
    QStringList headerList = {"Date", "Particular", "Receipt", "Mode", "Reference No", "Income", "Payment", "Total Income", "Total Payment"};
    bool first = true;
    for (auto header:  headerList) {
        htmlUtils.createCell(header, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        if (header == "Particular" && addDeptInCSV) {
            csvLine += ",Department, Account";
        }
        if (first == false) {
            csvLine += ",";
        }
        first = false;
        csvLine += header;
    }
    htmlUtils.closeRow();
    csvLine += "\n";
    m_csvTrans.push_back(csvLine);
    csvLine = "";
    int rowCount = 1;
    m_totalIncome = 0;
    m_totalPayment = 0;
    for (auto [date, transactionDatas] : m_transactions.asKeyValueRange()) {
        csvLine = "";
        double localIncome = 0;
        double localPayment = 0;
        QString dateStr = date.toString("dd MMM yyyy");
        int size = transactionDatas.size();
        int count = 0;
        for (auto transData: transactionDatas) {
            ++rowCount;
            ++count;
            EhtmlCellBoarder borderType = HTML_NO_BORDER;

            QString bgcolor;
            if(rowCount%2){
                bgcolor ="#efefef";
                //borderType = HTML_RIGHT_BORDER;
            }
            bool isIncome = true;
            bool isContra = false;
            switch((EtransactionType)transData->m_type) {
            case INCOME_TRANSACTION_TYPE:break;
            case PAYMENT_TRANSACTION_TYPE: isIncome = false; break;
            case LOAN_RECOVERY_TRANSACTION_TYPE: break;
            case LOAN_DISBURSE_TRANSACTION_TYPE: isIncome = false; break;
            case BANK_CHARGES_TRANSACTION_TYPE: isIncome = false; break;
            case BANK_INTEREST_TRANSACTION_TYPE: break;
            case BANK_CASH_DEPOSIT_TRANSACTION_TYPE: isIncome = true; isContra = true; break;
            case BANK_CASH_WITHDRAW_TRANSACTION_TYPE: isIncome = false; isContra = true; break;

            default: break;
            }
            QString mode = CtransactionUtils::Object()->getTransactionModeTwoTypeStr((EtransactionMode)transData->m_mode);
            QString receipt;
            if (transData->m_reeiptBook.isEmpty() == false && transData->m_receiptSlNo.isEmpty() == false) {
                receipt += transData->m_reeiptBook + ":" + transData->m_receiptSlNo;
            }
            QString incomeAmount;
            QString paymentAmount;
            if (isIncome) {
                incomeAmount =  CsdUtils::convertAmountToStringWithSign(transData->m_amount);
                localIncome += transData->m_amount;
            } else {
                paymentAmount = CsdUtils::convertAmountToStringWithSign(transData->m_amount);
                localPayment += transData->m_amount;
            }
            QString fromTo = CcontactMap::Object()->getContanceName(transData->m_fromId);
            if (m_isBankOrCashLedger) {
                fromTo += " (" + CaccountMap::Object()->getAccountName(transData->m_accountId) + ")";
            } else if (transData->m_particular.isEmpty() == false) {
                fromTo += "(" + transData->m_particular + ")";
            }
            if (isContra) {
                receipt = "(C)";
            }
            // qDebug()<<"inside mode "<<mode<<" "<<CaccountMap::Object()->getAccountName(transData->m_accountId)<<" "<<CcontactMap::Object()->getContanceName(transData->m_fromId);
            QString ref = transData->m_ref;
            if (ref.size() > 14) {
                ref = ref.right(14);
            }
            htmlUtils.createRow();
            htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            csvLine += dateStr + ",";
            if (addDeptInCSV) {
                csvLine +=deptName + ", " + accountName + ",";
            }
            csvLine += fromTo + ", " + receipt + ", " + mode +"," + ref + ",";
            if (isIncome) {
                csvLine += CsdUtils::converAmountToString(transData->m_amount,'f',2) + ",,";
            } else {
                csvLine += "," + CsdUtils::converAmountToString(transData->m_amount,'f',2) + ",";
            }

            if (count == size && (isContra == false)) {
                incomeAmount = "";
                paymentAmount = "";
                if (localIncome) {
                    incomeAmount = CsdUtils::convertAmountToStringWithSign(localIncome);
                }
                if (localPayment) {
                    paymentAmount = CsdUtils::convertAmountToStringWithSign(localPayment);
                }
                htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                csvLine += CsdUtils::converAmountToString(localIncome,'f',2) + ","  + CsdUtils::converAmountToString(localPayment,'f',2) +",";
            } else {
                htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                csvLine += ",,";
            }
            csvLine += "\n";
            htmlUtils.closeRow();

            if (isContra) {
                ++rowCount;
                bgcolor = "";
                borderType = HTML_NO_BORDER;
                if(rowCount%2){
                    bgcolor ="#efefef";
                    // borderType = HTML_RIGHT_BORDER;
                }
                //contra only in case of bank cash depost. interchange mode between bank and cash
                //Also interchange income amount and payment amount
                if (mode.contains("Bank", Qt::CaseInsensitive)) {
                    mode = "Cash";
                } else {
                    mode = "Bank";
                }
                QString ref = transData->m_ref;
                if (ref.size() > 14) {
                    ref = ref.right(14);
                }
                htmlUtils.createRow();
                htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);

                csvLine += dateStr + ",";
                if (addDeptInCSV) {
                    csvLine +=deptName + ", " + accountName + ",";
                }
                csvLine += fromTo + ", " + receipt + ", " + mode +",,";
                if (isIncome == false) {
                    csvLine += CsdUtils::converAmountToString(transData->m_amount,'f',2) + ",,";
                } else {
                    csvLine += "," + CsdUtils::converAmountToString(transData->m_amount,'f',2) + ",";
                }


                if (count  == size) {
                    if (isIncome) {
                        localPayment += transData->m_amount;
                    } else {
                        localIncome += transData->m_amount;

                    }
                    incomeAmount = "";
                    paymentAmount = "";
                    if (localIncome) {
                        incomeAmount = CsdUtils::convertAmountToStringWithSign(localIncome);
                    }
                    if (localPayment) {
                        paymentAmount = CsdUtils::convertAmountToStringWithSign(localPayment);
                    }
                    csvLine += CsdUtils::converAmountToString(localIncome,'f',2) + "," + CsdUtils::converAmountToString(localPayment,'f',2) + ",\n";
                    htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                    htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);



                } else {
                    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                    csvLine += ",,\n";

                }
                htmlUtils.closeRow();
            }

            m_csvTrans.push_back(csvLine);
            csvLine = "";
        }
        m_totalIncome += localIncome;
        m_totalPayment += localPayment;

    }
    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);// ref

    QString incomeAmount = CsdUtils::convertAmountToStringWithSign(m_totalIncome);
    QString paymentAmount = CsdUtils::convertAmountToStringWithSign(m_totalPayment);

    htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_DOUBLE_BOTTOM_STYLE);
    htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_DOUBLE_BOTTOM_STYLE);
    htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_DOUBLE_BOTTOM_STYLE);
    htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_DOUBLE_BOTTOM_STYLE);
    htmlUtils.closeRow();
    htmlUtils.closeTable();
    //{"Date", "Particular", "Receipt", "Mode", "Reference No", "Income", "Payment", "Total Income", "Total Payment"};

    csvLine = ",";
    if (addDeptInCSV) {
        csvLine += ",,";
    }
    //QString line;
    line = ", , , Total," + CsdUtils::converAmountToString(m_totalIncome,'f',2);
    line += "," +  CsdUtils::converAmountToString(m_totalPayment,'f',2);
    line += "," + CsdUtils::converAmountToString(m_totalIncome,'f',2);
    line += "," +  CsdUtils::converAmountToString(m_totalPayment,'f',2) + "\n";
    csvLine += line;
    m_csvTrans.push_back(csvLine);

    html = htmlUtils.getHtml();

    //qDebug()<<"HTML is "<<html;
    html += "<p></p>" + ChtmlUtils::getTwoSignator();
    html += ChtmlUtils::getPageBreak();

    return html;
}









bool CloanLedger::processTransForLoanAcc()
{
    // qDebug()<<"processing loan account\n";
    if (m_loanAccountProcessed) {
        return m_loanTransaction.empty() ? false : true;
    }
    for(auto [key, value] :m_transactions.asKeyValueRange()) {
        for (auto data: value) {
            delete data;
        }
    }
    for (auto trans:m_loanTransaction) {
        for (auto todel : trans.second) {
            delete todel;
        }
    }
    m_transactions.clear();
    m_loanAccountProcessed = true;
    int id = CaccountMap::Object()->getAccountID(gLoanAccountName);
    QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName();
    query += " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND (";
    query += CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(id)  +") ";
    query += "ORDER BY " +  CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " ASC";
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);

    std::map<int, std::pair<double, double> > loanAmount;
    m_loanTransaction.clear();
    for (auto trans: results) {
        int amount = trans->m_amount;
        double income = 0;
        double payment = 0;
        if (trans->m_type == LOAN_RECOVERY_TRANSACTION_TYPE) {
            income += amount;
        } else {
            payment += amount;
        }

        std::pair<double, double>& amt = loanAmount[trans->m_fromId];
        amt.first += income;
        amt.second += payment;
        std::vector<StransactionData*>& loan = m_loanTransaction[trans->m_fromId];

        if (amt.first != amt.second) {
            loan.push_back(trans);
        } else {
            delete trans;
            for (auto l : loan) {
                delete l;
            }
            loanAmount.erase(trans->m_fromId);
            m_loanTransaction.erase(trans->m_fromId);
        }
    }

    return m_loanTransaction.empty() ? false : true;

}

CloanLedger::~CloanLedger()
{
    for (auto trans:m_loanTransaction) {
        for (auto todel : trans.second) {
            delete todel;
        }
    }
}

QString CloanLedger::getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName)
{
    if (processTransForLoanAcc() == false) {
        return "";
    }

    QString html;
    QString csvLine;
    m_csvTrans.clear();
    if (m_loanTransaction.empty()) {
        return html;
    }

    ChtmlUtils htmlUtils;
    QString accountName = replaceAccountName;
    QString deptName;
    QString incomeStr("Income");
    QString paymentStr("Payment");

    QString line;
    QString htmlLine;
    if (m_isBankOrCashLedger == false && (indiviualAccount == false)) {
        accountName = CaccountMap::Object()->getAccountName(accountID);
        int deptID = CaccountMap::Object()->getDeptForAccount(accountID);
        if (deptID) {
            deptName = CaccountMap::Object()->getDeptName(deptID);
            line = deptName + "\n";
            htmlLine = deptName + "<br>";
        }
    } else if (indiviualAccount){
        if (replaceAccountName.isEmpty()) {
            accountName = CcontactMap::Object()->getContanceName(accountID);
        } else {
            accountName = accountName + ": " + CcontactMap::Object()->getContanceName(accountID);
        }
    }
    //qDebug()<<"for laon accunt "<<accountName;
    csvLine += line + "Ledger for " + accountName;
    htmlLine += "Ledger for " + accountName;
    htmlUtils.formatString(htmlLine, true);

    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(9);
    htmlUtils.addTableColSize(18);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(10);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);

    htmlUtils.openTable();

    htmlUtils.createRow();
    csvLine += "\n";
    QStringList headerList = {"Date", "Particular",  "Mode", "Reference No", "Loan Disburse", "Loan Recover",  "Balance"};
    bool first = true;
    for (auto header:  headerList) {
        htmlUtils.createCell(header, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");

        if (first == false) {
            csvLine += ",";
        }
        first = false;
        csvLine += header;
    }

    htmlUtils.closeRow();
    csvLine += "\n";
    m_csvTrans.push_back(csvLine);
    csvLine = "";
    int rowCount = 1;
    m_totalIncome = 0;
    m_totalPayment = 0;
    for (auto nameIDTransactions: m_loanTransaction) {
        double totalIncome = 0;
        double totalPayment = 0;
        //int size = nameIDTransactions.second.size();
        //int count = 0;
        for (auto transData : nameIDTransactions.second) {
            csvLine = "";
            QString dateStr = transData->m_date.toString("dd MMM yyyy");
            QString fromTo = CcontactMap::Object()->getContanceName(transData->m_fromId);
            QString mode = CtransactionUtils::Object()->getTransactionModeTwoTypeStr((EtransactionMode)transData->m_mode);
            QString ref = transData->m_ref;

            if (ref.size() > 14) {
                ref = ref.right(14);
            }
            QString disburseStr = "";
            QString recoverStr = "";
            QString disburseStrCSV = "";
            QString recoverStrCSV = "";
            if((EtransactionType)transData->m_type == LOAN_DISBURSE_TRANSACTION_TYPE) {
                totalPayment += transData->m_amount;
                disburseStr = CsdUtils::convertAmountToStringWithSign(transData->m_amount);
                disburseStrCSV = CsdUtils::converAmountToString(transData->m_amount);
            } else {
                totalIncome += transData->m_amount;
                recoverStr = CsdUtils::convertAmountToStringWithSign(transData->m_amount);
                recoverStrCSV = CsdUtils::converAmountToString(transData->m_amount);
            }
            double balance = totalPayment - totalIncome;
            QString balanceStr = CsdUtils::convertAmountToStringWithSign(balance);
            QString balanceStrCSV = CsdUtils::converAmountToString(balance);
            ++rowCount;
            QString bgcolor;
            if(rowCount%2){
                bgcolor ="#efefef";
            }
            htmlUtils.createRow();
            htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(disburseStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(recoverStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(balanceStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.closeRow();

            csvLine = dateStr + "," + fromTo + "," + mode + "," + ref + ", " + disburseStrCSV + "," + recoverStrCSV + "," + balanceStrCSV + "\n";
            m_csvTrans.push_back(csvLine);

        }
    }
    htmlUtils.closeTable();

    html = htmlUtils.getHtml();
    m_csvTrans.push_back("\n");
    //qDebug()<<"HTML is "<<html;
    html += "<p></p>" + ChtmlUtils::getTwoSignator();
    html += ChtmlUtils::getPageBreak();

    return html;
}

QString CremitttanceLedger::getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName)
{
    QString html;
    QString csvLine;
    m_csvTrans.clear();
    if (m_transactions.isEmpty()) {
        return html;
    }

    ChtmlUtils htmlUtils;
    QString accountName = replaceAccountName;
    QString deptName;
    QString incomeStr("Income");
    QString paymentStr("Payment");

    QString line;
    QString htmlLine;
    if (m_isBankOrCashLedger == false && (indiviualAccount == false)) {
        accountName = CaccountMap::Object()->getAccountName(accountID);
        int deptID = CaccountMap::Object()->getDeptForAccount(accountID);
        if (deptID) {
            deptName = CaccountMap::Object()->getDeptName(deptID);
            line = deptName + "\n";
            htmlLine = deptName + "<br>";
        }
    } else if (indiviualAccount){
        if (replaceAccountName.isEmpty()) {
            accountName = CcontactMap::Object()->getContanceName(accountID);
        } else {
            accountName = accountName + ": " + CcontactMap::Object()->getContanceName(accountID);
        }
    }

    csvLine += line + "Ledger for " + accountName;
    htmlLine += "Ledger for " + accountName;
    htmlUtils.formatString(htmlLine, true);

    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(9);
    htmlUtils.addTableColSize(25);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(10);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);
    htmlUtils.openTable();

    htmlUtils.createRow();
    csvLine += "\n";
    QStringList headerList = {"Date", "Particular",  "Mode", "Reference No", "Amount", "Account Remit Amount", "To Received"};
    bool first = true;
    for (auto header:  headerList) {
        htmlUtils.createCell(header, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        if (header == "Particular" && addDeptInCSV) {
            csvLine += ",Department, Account";
        }
        if (first == false) {
            csvLine += ",";
        }
        first = false;
        csvLine += header;
    }
    htmlUtils.closeRow();
    csvLine += "\n";
    m_csvTrans.push_back(csvLine);
    csvLine = "";
    int rowCount = 1;
    m_totalIncome = 0;
    m_totalPayment = 0;
    double totalRemittance = 0;
    for (auto [date, transactionDatas] : m_transactions.asKeyValueRange()) {

        for (auto transData: transactionDatas) {
            totalRemittance += transData->m_amount;
            ++rowCount;
            EhtmlCellBoarder borderType = HTML_NO_BORDER;

            QString bgcolor;
            if(rowCount%2){
                bgcolor ="#efefef";
            }
            QString dateStr = transData->m_date.toString("dd MMM yyyy");
            QString fromTo = CcontactMap::Object()->getContanceName(transData->m_fromId);
            QString mode = CtransactionUtils::Object()->getTransactionModeTwoTypeStr((EtransactionMode)transData->m_mode);
            QString amountStr =  CsdUtils::convertAmountToStringWithSign(transData->m_amount);
            QString amountStrCSV = CsdUtils::converAmountToString(transData->m_amount);

            QString csvLine = "";
            QString ref = transData->m_ref;
            if (ref.size() > 14) {
                ref = ref.right(14);

            }
            htmlUtils.createRow();
            htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            csvLine = dateStr +"," + fromTo + ", " + mode +", " + ref + "," + amountStrCSV +", ";
            bool closeRow = false;
            //htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);

            SremittanceData* remitData = CremittanceTable::Object()->getRemittanceDataFromTransID(transData->m_id);
            if (remitData) {
                int remitTableID = remitData->m_id;
                CremittanceDetails dtl (remitTableID);
                delete remitData;
                const std::map<int, std::pair<double, double> >& accontLocalDepAmt = dtl.getAccountDeptLocalAndDeptAmt();
                if (accontLocalDepAmt.size() == 1) {
                    double hq = accontLocalDepAmt.begin()->second.second;
                    QString amountStr =  CsdUtils::convertAmountToStringWithSign(hq);
                    QString amountStrCSV = CsdUtils::converAmountToString(hq);
                    htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                    csvLine +=    amountStrCSV + ",";


                    double local = accontLocalDepAmt.begin()->second.first;
                    amountStr =  CsdUtils::convertAmountToStringWithSign(local);
                    amountStrCSV = CsdUtils::converAmountToString(local);
                    htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                    csvLine +=    amountStrCSV + ",";
                    htmlUtils.closeRow();
                    m_csvTrans.push_back(csvLine + "\n");
                    closeRow = true;

                } else if (accontLocalDepAmt.size() > 1){
                    if (closeRow == false) {
                        closeRow = true;
                        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.closeRow();
                        csvLine += ",,";
                        m_csvTrans.push_back(csvLine + "\n");
                    }
                    double toReceived = 0;
                    for (auto localDepAmt: accontLocalDepAmt) {
                        toReceived += localDepAmt.second.first;
                        if (localDepAmt.second.second) {
                            ++rowCount;
                            QString bgcolor;
                            if(rowCount%2){
                                bgcolor ="#efefef";
                            }
                            htmlUtils.createRow();
                            fromTo = "Remittance - " + CaccountMap::Object()->getAccountName(localDepAmt.first);
                            htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            QString amountStr =  CsdUtils::convertAmountToStringWithSign(localDepAmt.second.second);
                            QString amountStrCSV = CsdUtils::converAmountToString(localDepAmt.second.second);
                            htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                            htmlUtils.closeRow();
                            csvLine = "," + fromTo + ",,,," + amountStrCSV + ",";
                            m_csvTrans.push_back(csvLine + "\n");

                        }
                    }
                    if (toReceived) {
                        ++rowCount;
                        QString bgcolor;
                        if(rowCount%2){
                            bgcolor ="#efefef";
                        }
                        htmlUtils.createRow();
                        fromTo = "To Received";
                        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        QString amountStr =  CsdUtils::convertAmountToStringWithSign(toReceived);
                        QString amountStrCSV = CsdUtils::converAmountToString(toReceived);
                        htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                        htmlUtils.closeRow();
                        csvLine = "," + fromTo + ",,,,," + amountStrCSV;
                        m_csvTrans.push_back(csvLine + "\n");
                    }

                }
            }
            if (closeRow == false) {
                htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);

                htmlUtils.closeRow();
                m_csvTrans.push_back(csvLine + "\n");
            }
        }
    }

    if (totalRemittance) {
        ++rowCount;
        QString bgcolor;
        if(rowCount%2){
            bgcolor ="#efefef";
        }
        EhtmlCellBoarder borderType = HTML_NO_BORDER;
        htmlUtils.createRow();
        QString fromTo = "Total ";
        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell(fromTo, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        QString amountStr =  CsdUtils::convertAmountToStringWithSign(totalRemittance);
        QString amountStrCSV = CsdUtils::converAmountToString(totalRemittance);
        htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
        csvLine = "," + fromTo + ",,," + amountStrCSV + ",,,";
        m_csvTrans.push_back(csvLine + "\n");

    }
    htmlUtils.closeTable();
    m_csvTrans.push_back("\n");
    //qDebug()<<"HTML is "<<html;

    html = htmlUtils.getHtml();
    html += "<p></p>" + ChtmlUtils::getTwoSignator();
    html += ChtmlUtils::getPageBreak();

    return html;

}
