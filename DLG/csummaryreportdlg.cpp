#include "csummaryreportdlg.h"
#include "ui_csummaryreportdlg.h"
#include "ctransactiontable.h"
#include "cremittancesetuptable.h"
#include "caccountmap.h"
#include "ctransactionutils.h"
#include "ccontactmap.h"
#include "MISC/chtmlutils.h"
#include "csdutils.h"
#include "MISC/cprintUtils.h"
#include "MISC/CdlgDefine.h"

CsummaryReportDlg::CsummaryReportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CsummaryReportDlg)
{
    ui->setupUi(this);
    m_canpopulate = false;
    QDate date = QDate::currentDate();
    ui->m_fromDate->setDate(date);
    ui->m_toDate->setDate(date);

    m_canpopulate = true;

}

CsummaryReportDlg::~CsummaryReportDlg()
{
    delete ui;
}

void CsummaryReportDlg::on_m_close_clicked()
{
    emit reject();
}


void CsummaryReportDlg::on_m_savePDF_clicked()
{
    if (m_html.isEmpty() == false) {
        CprintUtils print(m_html, false);
        print.printPDF();
    }
    emit accept();
}

void CsummaryReportDlg::resize()
{

    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int x = XCORD;
    int y = YCORD;

    int btnWidth  = BUTTON_SIZE.width();
    int btnHeight = BUTTON_SIZE.height();

    ui->m_fromLBL->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width();
    ui->m_toLbl->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x = XCORD;
    y += GAP + DATE_SIZE.height();
    ui->m_fromDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width();
    ui->m_toDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    y += GAP + DATE_SIZE.height();
    x = XCORD;

    int width = windowwidth - 2*XCORD;
    int height = windowheight - y - GAP - btnHeight - YCORD;
    ui->m_textBox->setGeometry(x, y, width, height);
    y += GAP + height;
    x = windowwidth/2 - GAP/2 - btnWidth;
    ui->m_savePDF->setGeometry(x, y, btnWidth, btnHeight);
    x += btnWidth + GAP;
    ui->m_close->setGeometry(x, y, btnWidth, btnHeight);
}

void CsummaryReportDlg::populateTable()
{
    m_html = "";
    if (m_canpopulate == false) {
        return;
    }
    QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";

    QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";

    const std::set<int>& accounts =  CaccountMap::Object()->getAllAccount();
    QString accountFilter;
    std::map<int, std::pair<float, float>>      accountPct;

    bool first = true;
    for (auto id : accounts) {
        if (CaccountMap::Object()->getAccountType(id) == INCOME_ACCOUNT_TYPE) {
            SremittanceSetupTableData* pcsetup = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(id);
            if (pcsetup) {
                int percentid = pcsetup->m_percentageID;
                SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentid);
                std::pair<float, float>& pc = accountPct[id];
                pc.first = pcData->m_hqShare;
                pc.second = pcData->m_localShare;
                delete pcsetup;
                delete pcData;
            }
        }

        if (first) {
            accountFilter = " (";
        } else {
            accountFilter += " OR ";
        }
        first = false;
        accountFilter +=  CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(id);
    }
    if (accountFilter.isEmpty() == false) {
        accountFilter += ")";
        filter += " AND " + accountFilter;
    }


    QString startdate = ui->m_fromDate->date().toString("yyyy-MM-dd");
    QString enddate = ui->m_toDate->date().toString("yyyy-MM-dd");

    QString dateFilter = CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startdate +"\"";
    dateFilter += " AND \"" + enddate +"\"";

    filter += " AND (" + dateFilter + ")";
    query += filter;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    std::map<QString, double>   paymentAmount;
    std::map<QString, double> incomeAmount;
    std::map<QString, QPair<double,double> > bankAmount;
    std::map<QString, double> remitanceAmount;
    std::map<QString, double> requestAmount;
    std::map<QString, QPair<double,double> > loanAmount;

    for (auto data: results) {
        double amount = data->m_amount;
        int account = data->m_accountId;
        int transType = data->m_type;
        QString accountName = CaccountMap::Object()->getAccountName(account);
        //std::map<int, std::pair<float, float>>::iterator fn =   accountPct.find(account);
        EaccountType accountType = CaccountMap::Object()->getAccountType(account);
        int deptID = CaccountMap::Object()->getDeptForAccount(account);
        QString deptName = CaccountMap::Object()->getDeptName(deptID);
        if (accountType == INCOME_ACCOUNT_TYPE) {
            std::map<int, std::pair<float, float>>::iterator fn =  accountPct.find(account);
            QString name = accountName;
            if (fn != accountPct.end()) {
                double hq = fn->second.first;
                double local = fn->second.second;
                if (local > 99) {
                    name = deptName;
                }else {
                    if (local <1) {
                        continue;
                    }
                    amount = amount*(local/100);
                    name += " " +QString::number(local) + "%";
                }
            }
            incomeAmount[name] += amount;

        } else if (accountType == PAYMENT_ACCOUNT_TYPE) {
            QString paymentAccount =  CaccountMap::Object()->getPaymentAccountNameForAccountDepID(account);
            paymentAmount[paymentAccount] += amount;

        } else if (accountType == REMITTANCE_ACCOUNT_TYPE) {
            remitanceAmount[accountName] += amount;
        } else if (accountType == REQUEST_ACCOUNT_TYPE) {
            requestAmount[accountName] += amount;

        } else if (accountType == BANK_ACCOUNT_TYPE) {
            if (transType == BANK_CHARGES_TRANSACTION_TYPE) {
                QPair<double, double> & val = bankAmount[gBankChargesName];
                val.second += amount;
            } else if (transType == BANK_INTEREST_TRANSACTION_TYPE) {
               QPair<double, double> & val = bankAmount[gBankInterestName];
                val.first += amount;
            } else if (transType == BANK_CASH_DEPOSIT_TRANSACTION_TYPE) {
                QPair<double, double> & val = bankAmount[gBankCashDepositName];
                val.first += amount;
            }

        }else if (LOAN_ACCOUNT_TYPE == accountType){
            QString name = CcontactMap::Object()->getContanceName(data->m_fromId);
            QPair<double, double> & val = loanAmount[name];

            if (transType == LOAN_RECOVERY_TRANSACTION_TYPE) {
                val.first += amount;
            } else {//LOAN_DISBURSE_TRANSACTION_TYPE
                val.second += amount;
            }
        }

    }

    bool addPageBreak = false;
    QString html = addInTable(incomeAmount, "Income");
    if (html.isEmpty() == false) {
        m_html += html;
        addPageBreak = true;
    }
    html = addInTable(paymentAmount, "Payment");

    if (html.isEmpty() == false) {
        if (addPageBreak) {
            m_html += ChtmlUtils::getPageBreak();
        }
        m_html += html;
        addPageBreak = true;
    }

    html = addInTable(bankAmount, "Bank Account");

    if (html.isEmpty() == false) {
        if (addPageBreak) {
            m_html += ChtmlUtils::getPageBreak();
        }
        m_html += html;
        addPageBreak = true;
    }
    html = addInTable(remitanceAmount, gRemittanceAccountName);

    if (html.isEmpty() == false) {
        if (addPageBreak) {
            m_html += ChtmlUtils::getPageBreak();
        }
        m_html += html;
        addPageBreak = true;
    }
    html = addInTable(requestAmount, gRequestAccountName);

    if (html.isEmpty() == false) {
        if (addPageBreak) {
            m_html += ChtmlUtils::getPageBreak();
        }
        m_html += html;
        addPageBreak = true;
    }

    html = addInTable(loanAmount, gLoanAccountName);

    if (html.isEmpty() == false) {
        if (addPageBreak) {
            m_html += ChtmlUtils::getPageBreak();
        }
        m_html += html;
        addPageBreak = true;
    }


    ui->m_textBox->setHtml(m_html);

}

QString CsummaryReportDlg::addInTable(std::map<QString, double> &amounts, QString str)
{
    ChtmlUtils htmlUtils;
    if (str.isEmpty() == false) {
        htmlUtils.formatString(str, true);
    }
    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(40);
    htmlUtils.addTableColSize(60);
    htmlUtils.openTable();
    htmlUtils.createRow();
    htmlUtils.createCell("Account Head", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
    htmlUtils.createCell("Amount", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
    htmlUtils.closeRow();
    double totalAmount = 0;
    int rowCount = 0;
    for (auto keyvalue : amounts) {
       ++rowCount;
        QString bgcolor;
        if(rowCount%2 == 0){
            bgcolor ="#efefef";
        }
        htmlUtils.createRow();
        htmlUtils.createCell(keyvalue.first , HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        QString amountStr = CsdUtils::convertAmountToStringWithSign(keyvalue.second);

        htmlUtils.createCell(amountStr,
                           HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
        totalAmount += keyvalue.second;
    }
    if (totalAmount > 0) {
        ++rowCount;
        QString bgcolor;
        if(rowCount%2 == 0){
            bgcolor ="#efefef";
        }
        htmlUtils.createRow();
        htmlUtils.createCell("Total" , HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        QString amountStr = CsdUtils::convertAmountToStringWithSign(totalAmount);

        htmlUtils.createCell(amountStr,
                            HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
    }

    htmlUtils.closeTable();
    QString html;
    if (rowCount > 0) {
        html = htmlUtils.getHtml();
    }

    return html;
}

QString CsummaryReportDlg::addInTable(std::map<QString, QPair<double, double> > &amounts, QString str)
{
    ChtmlUtils htmlUtils;
    if (str.isEmpty() == false) {
        htmlUtils.formatString(str, true);
    }
    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(30);
    htmlUtils.addTableColSize(35);
    htmlUtils.addTableColSize(35);

    htmlUtils.openTable();
    htmlUtils.createRow();
    htmlUtils.createCell("Account Head", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
    htmlUtils.createCell("Income Amount", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
    htmlUtils.createCell("Payment Amount", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
    htmlUtils.closeRow();
    int rowCount = 0;
    for (auto keyvalue : amounts) {
       ++rowCount;
        QString bgcolor;
        if(rowCount%2 == 0){
            bgcolor ="#efefef";
        }
        htmlUtils.createRow();
        htmlUtils.createCell(keyvalue.first , HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        QString incomeAmtStr;
        QString paymentAmtStr;
        double income = keyvalue.second.first;
        double payment = keyvalue.second.second;
        if (income > 0) {
            incomeAmtStr = CsdUtils::convertAmountToStringWithSign(income);
        }
        if (payment > 0) {
            paymentAmtStr = CsdUtils::convertAmountToStringWithSign(payment);
        }
        htmlUtils.createCell(incomeAmtStr,
                           HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);

        htmlUtils.createCell(paymentAmtStr,
                             HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
    }
    htmlUtils.closeTable();
    QString html;
    if (rowCount > 0) {
        html = htmlUtils.getHtml();
    }

    return html;
}

void CsummaryReportDlg::resizeEvent(QResizeEvent *event)
{
    resize();
}


void CsummaryReportDlg::on_m_fromDate_dateChanged(const QDate &date)
{
    populateTable();
}


void CsummaryReportDlg::on_m_toDate_dateChanged(const QDate &date)
{
    populateTable();

}

