#include "cledgerreportdlg.h"
#include "ui_cledgerreportdlg.h"
#include "caccountmap.h"
#include "ccontactmap.h"
#include "CtransactionTable.h"
#include "MISC/chtmlutils.h"
#include "ctransactionutils.h"
#include "MISC/cprintUtils.h"
#include "csdutils.h"
#include "MISC/cclosingcalculator.h"
#include "MISC/CdlgDefine.h"
#include "MISC/caccountledger.h"
#include "MISC/cledger.h"

CledgerReportDlg::CledgerReportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CledgerReportDlg)
{
    ui->setupUi(this);
    m_isConstructor = true;
    QStringList minlist = CcontactMap::Object()->getNameList();


    if (minlist.size() > 0) {
        m_nameCompleter = new QCompleter(minlist, this);
        m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        m_nameCompleter->setWrapAround(true);
        m_nameCompleter->setFilterMode(Qt::MatchContains);
        ui->m_nameFilter->setCompleter(m_nameCompleter);
    }
    QDate date = QDate::currentDate();
    ui->m_fromDate->setDate(date);
    ui->m_toDate->setDate(date);
    ui->m_DeptBox->addItem("");
    ui->m_accountBox->addItem("");
    QStringList accounts = CaccountMap::Object()->getAccountList();
    accounts.sort();
    ui->m_accountBox->addItems(accounts);
    QStringList depts = CaccountMap::Object()->getDeptList();
    depts.sort();
    ui->m_DeptBox->addItems(depts);
    /*ui->m_individualLEdger->setChecked(false);
    ui->m_individualLEdger->setVisible(false);
    ui->m_individualLEdger->setCheckable(true);*/
    resize();
    m_isConstructor = false;

    populateTable();
}

CledgerReportDlg::~CledgerReportDlg()
{
    delete ui;
    clear();

}

void CledgerReportDlg::clear()
{

}

void CledgerReportDlg::resize()
{



    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int btnWidth = BUTTON_SIZE.width();
    int btnheight = BUTTON_SIZE.height();


    int x = XCORD;
    int y = YCORD;

    ui->m_fromDate->setGeometry(x , y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width() ;
    ui->m_toDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width();
    ui->m_DeptBox->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width(),  DEPT_ACCOUNT_COMBO_SIZE.height() );
    x += GAP +  DEPT_ACCOUNT_COMBO_SIZE.width();
    ui->m_accountBox->setGeometry(x, y,  DEPT_ACCOUNT_COMBO_SIZE.width(),  DEPT_ACCOUNT_COMBO_SIZE.height());
    x += GAP + DEPT_ACCOUNT_COMBO_SIZE.width();
    int width = DATE_SIZE.width()*1.3;
    ui->m_nameFilter->setGeometry(x, y, width, DATE_SIZE.height());
    x += GAP + width;
    ui->m_deptInCSV->setGeometry(x, y, width , DATE_SIZE.height());
    x += GAP + width;
    ui->m_individualLedger->setGeometry(x, y, width, DATE_SIZE.height());
    y += GAP +  DATE_SIZE.height();

    x = XCORD;
    width = windowwidth - 2*XCORD;
    int height = windowheight - y - GAP - YCORD - btnheight;



    ui->m_textBrowser->setGeometry(x, y, width, height);

    x  = windowwidth/2 - btnWidth - GAP - btnWidth/2;
    y = y + height + GAP;
    ui->m_pdfBtn->setGeometry(x, y, btnWidth, btnheight);
    x += btnWidth + GAP;
    ui->m_csvBtn->setGeometry(x, y, btnWidth, btnheight);
    x += btnWidth + GAP;
    ui->m_cancelBtn->setGeometry(x, y, btnWidth, btnheight);




}

void CledgerReportDlg::populateTable()
{
    if (m_isConstructor) {
        return;
    }
    clear();
    m_html.clear();
    m_csvTrans.clear();
    m_htmlList.clear();

    ui->m_textBrowser->setHtml(m_html);

    QString startDate = ui->m_fromDate->date().toString("yyyy-MM-dd");
    QString toDate = ui->m_toDate->date().toString("yyyy-MM-dd");

    QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName();
    query += " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND (";
    query += CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDate  + "\" AND \"";
    query += toDate +  "\")";

    QString dept = ui->m_DeptBox->currentText().trimmed();
    QString account = ui->m_accountBox->currentText().trimmed();

    QSet<int> accountList;

    bool hasAccount = false;
    if (account.isEmpty() == false) {
        int accountID = CaccountMap::Object()->getAccountID(account);
        if (accountID > 0) {
            accountList.insert(accountID);
            hasAccount = true;
        }

    } else {
        int deptID = CaccountMap::Object()->getDeptId(dept);
        QSet<int> accounts;
        CaccountMap::Object()->getAccounsForDept(deptID, accounts);
        if (accounts.isEmpty() == false) {
            for (int id : accounts) {

                accountList.insert(id);

            }
        }

    }

    QString name = ui->m_nameFilter->text().trimmed().simplified();



     bool individualLedger = false;
    if ((ui->m_individualLedger->isChecked() == true && hasAccount == true)) {
        individualLedger = true;
    }
    Cledger     m_ledger;
    m_ledger.populateLedger(startDate, toDate, accountList, name, individualLedger);
    const QMap<int, QMap<int, CaccountLedger *> >& allAccountLedger = m_ledger.allAccountLedger();

    CaccountLedger bankAccount(true);
    bool isAll = ui->m_accountBox->currentText().isEmpty() && ui->m_DeptBox->currentText().isEmpty() && ui->m_nameFilter->text().isEmpty();
    if (allAccountLedger.isEmpty() == false && individualLedger == false && isAll) {
        CclosingCalculator closingCalc;
        closingCalc.calculateClosing( ui->m_fromDate->date() ,query);
        printForOpeningClosing(&closingCalc);
    }

    //qDebug()<<"qeery is "<<query;
    //qDebug()<<"Amount Account, From/To Type";


    bool isDeptInCSV = individualLedger == false ? ui->m_deptInCSV->isChecked() : false;
    QString replaceAccountName = individualLedger ? account : "";

    for (auto [accountOrDeptID, accountLedgerMap] : allAccountLedger.asKeyValueRange()) {
        for (auto [fromOrAccountID, accountLedger] : accountLedgerMap.asKeyValueRange()) {
            QString html = accountLedger->getHTMLTable(fromOrAccountID, isDeptInCSV, individualLedger, replaceAccountName);

            QStringList csvs = accountLedger->getCSVTransList();
            m_htmlList.push_back(html);
            foreach(const QString& line, csvs) {
                m_csvTrans.push_back(line);
            }
        }
    }
    if (individualLedger == false){
        QString html = m_ledger.bankAccount().getHTMLTable(0, isDeptInCSV, false, "Bank");
        QStringList csvs = m_ledger.bankAccount().getCSVTransList();
        m_htmlList.push_back(html);
        foreach(const QString& line, csvs) {
            m_csvTrans.push_back(line);
        }
    }
    if (individualLedger == false){
        QString html = m_ledger.cashAccount().getHTMLTable(0, isDeptInCSV, false, "Cash");
        QStringList csvs = m_ledger.cashAccount().getCSVTransList();
        m_htmlList.push_back(html);
        foreach(const QString& line, csvs) {
            m_csvTrans.push_back(line);
        }
    }


    ChtmlUtils htmlUtils;
    m_html = htmlUtils.getHtmlPage(m_htmlList);

    ui->m_textBrowser->setHtml(m_html);

}

void CledgerReportDlg::printForOpeningClosing(CclosingCalculator *calc)
{
   // printForSummary(calc);
   // printForClosingSummary(calc);
    printDeptSummary(calc);

    //qDebug()<<openingCash<<" "<<openingBank<<" "<<closingCash<<" "<<closingBank;
/*
 *     double cashOpening() const;

    double cashClosing() const;

    double bankOpening() const;

    double bankClosing() const;
    const accountDeptAmountType& getClosing() { return m_closingValues;}
    const accountDeptAmountType& getOpening() { return m_openingValues;}

    QDate lastTransDate() const;

    accountTwoAmountType accountIncomeAndPayment() const;*/
}

void CledgerReportDlg::printForSummary(CclosingCalculator *calc)
{
    double openingCash = calc->cashOpening();
    double openingBank = calc->bankOpening();
    double closingCash = calc->cashClosing();
    double closingBank = calc->bankClosing();
    double totalIncome = 0;
    double totalPayment = 0;
    accountTwoAmountType incomeAndPayment = calc->accountIncomeAndPayment();
    for (auto iandp : incomeAndPayment) {
        totalIncome += iandp.second.first;
        totalPayment += iandp.second.second;
    }



    ChtmlUtils htmlUtils;
    QString htmlLine = "SUMMARY: " + ui->m_fromDate->date().toString("d MMM yyyy") + " TO " + ui->m_toDate->date().toString("d MMM yyyy") + "</br>";
    htmlUtils.formatString(htmlLine, true, true);

    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(3);
    htmlUtils.addTableColSize(15);
    htmlUtils.addTableColSize(7);
    htmlUtils.addTableColSize(15);
    htmlUtils.addTableColSize(20);


    htmlUtils.openTable();
    //add opening balance
    htmlUtils.createRow();
    htmlUtils.createCell("1", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Opening Balance", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("(a)", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Cash", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    QString cashOpningStr = CsdUtils::convertAmountToStringWithSign(openingCash);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(openingCash);
    QString line = "1,Opening Balance, (a), Cash, " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);

    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("(b)", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Saving Account", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(openingBank);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(openingBank);
    line = ",, (b), Saving Account, " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);


    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Total", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(openingBank + openingCash);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(openingBank + openingCash);
    line = ",Total, , , " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);

    //Add total receipt
    htmlUtils.createRow();
    htmlUtils.createCell("2", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Total Receipt", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(totalIncome);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(totalIncome);
    line = "2,Total Receipt, , , " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);
    //Add total payment
    htmlUtils.createRow();
    htmlUtils.createCell("3", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Total Payment", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(totalPayment);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(totalPayment);
    line = "3,Total Payment, , , " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);
    // add closing balance

    htmlUtils.createRow();
    htmlUtils.createCell("4", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Closing Balance", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("(a)", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Cash", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(closingCash);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(closingCash);
    line = "4,Closing Balance, (a) , Cash, " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);

    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("(b)", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Saving Account", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(closingBank);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(closingBank);
    line = ",, (b) , Saving Account, " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);


    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Total", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    cashOpningStr = CsdUtils::convertAmountToStringWithSign(closingBank + closingCash);
    htmlUtils.createCell(cashOpningStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    cashOpningStr = CsdUtils::converAmountToString(closingBank + closingCash);
    line = ",Total,  , , " + cashOpningStr +"\n";
    m_csvTrans.push_back(line);

    htmlUtils.closeTable();
    double diff = (openingCash + openingBank + totalIncome - totalPayment) - (closingCash + closingBank);
    if (diff != 0) {
        QString htmlLine = "*Mismatch in calculation";
        htmlUtils.formatString(htmlLine, true, true);
        m_csvTrans.push_back(htmlLine + "\n");

    }
    m_csvTrans.push_back("\n");

    QString html = htmlUtils.getHtml() + ChtmlUtils::getPageBreak();

    m_htmlList.push_back(html);
}

void CledgerReportDlg::printForClosingSummary(CclosingCalculator *calc)
{

    const accountDeptAmountType& closing = calc->getClosing();
    ChtmlUtils htmlUtils;
    QString htmlLine  = "Department's Balance in Local Pool as on " + ui->m_toDate->date().toString("d MMM yyyy");
    m_csvTrans.push_back(htmlLine + "\n");
    htmlUtils.formatString(htmlLine, true, true);
    std::map<QString, double> deptAmount;
    std::map<QString, double> accAmount;

    for (auto accDept : closing) {
        int id = accDept.first;
        EaccountType accType = CaccountMap::Object()->getAccountType(id);
        if (accType == REMITTANCE_ACCOUNT_TYPE || REQUEST_ACCOUNT_TYPE == accType) {
            continue;
        }
        if (accType == BANK_ACCOUNT_TYPE || accType == LOAN_ACCOUNT_TYPE) {
            id = CaccountMap::Object()->getDefaultDept();
        }

        double amount = accDept.second;
        QString name = CaccountMap::Object()->getDeptName(id);
        if (name.isEmpty() == false) {
            deptAmount.insert({name, amount});
        } else {
            name = CaccountMap::Object()->getAccountName(id);
            if (name.isEmpty() == false) {
                accAmount.insert({name, amount});
            } else {
                Q_ASSERT(0);
            }
        }
    }
    int slno = 0;
    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(5);
    htmlUtils.addTableColSize(25);
    htmlUtils.addTableColSize(25);
    htmlUtils.openTable();
    double total = 0;
    for (auto dept :deptAmount ) {
        ++slno;
        htmlUtils.createRow();
        QString slNo = QString::number(slno);
        htmlUtils.createCell(slNo, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.createCell(dept.first, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        QString amountStr = CsdUtils::convertAmountToStringWithSign(dept.second);
        htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.closeRow();
        amountStr = CsdUtils::converAmountToString(dept.second);
        QString csvLine = slNo + ", " + dept.first + "," + amountStr + "\n";
        m_csvTrans.push_back(csvLine);
        total += dept.second;
    }

    for (auto acc : accAmount ) {
        ++slno;
        htmlUtils.createRow();
        QString slNo = QString::number(slno);
        htmlUtils.createCell(slNo, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.createCell(acc.first, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        QString amountStr = CsdUtils::convertAmountToStringWithSign(acc.second);
        htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.closeRow();
        amountStr = CsdUtils::converAmountToString(acc.second);
        QString csvLine = slNo + ", " + acc.first + "," + amountStr + "\n";
        m_csvTrans.push_back(csvLine);
        total += acc.second;
    }

    htmlUtils.createRow();
    htmlUtils.createCell("", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.createCell("Total", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    QString amountStr = CsdUtils::convertAmountToStringWithSign(total);
    htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
    htmlUtils.closeRow();
    amountStr = CsdUtils::converAmountToString(total);
    QString csvLine = ", Total," + amountStr + "\n";
    m_csvTrans.push_back(csvLine);
    m_csvTrans.push_back("\n");
    htmlUtils.closeTable();

    QString html = htmlUtils.getHtml() + ChtmlUtils::getPageBreak();

    m_htmlList.push_back(html);

}

void CledgerReportDlg::printDeptSummary(CclosingCalculator *calc)
{
    m_deptStatement.clear();
    const accountDeptAmountType& opening = calc->getOpening();
    const accountDeptAmountType& closing = calc->getClosing();
    accountTwoAmountType incomeAndPayment = calc->accountIncomeAndPayment();

    for (auto accDept : opening) {
        int id = accDept.first;
        EaccountType accType = CaccountMap::Object()->getAccountType(id);
        if (accType == REMITTANCE_ACCOUNT_TYPE || REQUEST_ACCOUNT_TYPE == accType) {
            continue;
        }
        if (accType == BANK_ACCOUNT_TYPE || accType == LOAN_ACCOUNT_TYPE) {
            id = CaccountMap::Object()->getDefaultDept();
        }

        double amount = accDept.second;
        QString name = CaccountMap::Object()->getDeptName(id);
        if (name.isEmpty()) {
            name = CaccountMap::Object()->getAccountName(id);
            if (name.isEmpty()) {
                Q_ASSERT(0);
            }
        }

        SdeptStatement& statement = m_deptStatement[name];
        statement.m_openintBalance += amount;
    }

    for (auto accDept : closing) {
        int id = accDept.first;
        EaccountType accType = CaccountMap::Object()->getAccountType(id);
        if (accType == REMITTANCE_ACCOUNT_TYPE || REQUEST_ACCOUNT_TYPE == accType) {
            continue;
        }
        if (accType == BANK_ACCOUNT_TYPE || accType == LOAN_ACCOUNT_TYPE) {
            id = CaccountMap::Object()->getDefaultDept();
        }

        double amount = accDept.second;
        QString name = CaccountMap::Object()->getDeptName(id);
        if (name.isEmpty()) {
            name = CaccountMap::Object()->getAccountName(id);
            if (name.isEmpty()) {
                Q_ASSERT(0);
            }
        }

        SdeptStatement& statement = m_deptStatement[name];
        statement.m_closingBalance += amount;
    }

    for (auto accountdpt: incomeAndPayment) {
        int id = accountdpt.first;
        double incomeAmt = accountdpt.second.first;
        double paymentAmt = accountdpt.second.second;
        int deptID = 0;
        EaccountType accType = CaccountMap::Object()->getAccountType(id);
        if (accType == REMITTANCE_ACCOUNT_TYPE || REQUEST_ACCOUNT_TYPE == accType) {
            continue;
        }
        if (accType == BANK_ACCOUNT_TYPE || accType == LOAN_ACCOUNT_TYPE) {
            deptID = CaccountMap::Object()->getDefaultDept();
        } else {
            deptID = CaccountMap::Object()->getDeptForAccount(id);
        }
        QString deptName = CaccountMap::Object()->getDeptName(deptID);
        QString accountName = CaccountMap::Object()->getAccountName(id);
        SdeptStatement& statement = m_deptStatement[deptName];
        std::pair<double, double>& accountAmt = statement.m_incomePaymentAmt[accountName];
        accountAmt.first += incomeAmt;
        accountAmt.second += paymentAmt;
        /*if (deptName.isEmpty()) {
            qDebug()<<id<<" Empty dept for account "<<accountName;
        }*/

    }


    ChtmlUtils htmlUtils;
    QString htmlLine  = "Department's summary statement: " + ui->m_fromDate->date().toString("d MMM yyyy")+ " to " + ui->m_toDate->date().toString("d MMM yyyy");
    m_csvTrans.push_back(htmlLine + "\n");
    htmlUtils.formatString(htmlLine, true, true);
    bool first = true;
    for (auto deptstatement: m_deptStatement) {
        QString deptName = deptstatement.first;
        const SdeptStatement& statement = deptstatement.second;
        if (first == false) {
            htmlUtils.formatString("<br>", false, false);
            m_csvTrans.push_back("\n");
        }
        first = false;
        htmlUtils.setIsTableHeader(false);
        htmlUtils.addTableColSize(5);
        htmlUtils.addTableColSize(20);
        htmlUtils.addTableColSize(20);
        htmlUtils.addTableColSize(15);
        htmlUtils.addTableColSize(15);
        htmlUtils.openTable();

        double income = 0;
        double payment = 0;
        htmlUtils.createRow();
        htmlUtils.createCell("Sl No", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        htmlUtils.createCell("Department", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        htmlUtils.createCell("Account Head", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        htmlUtils.createCell("Receipt", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        htmlUtils.createCell("Payment", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        htmlUtils.closeRow();
        QString line = "Sl No, Department, Account Head, Receipt, Payment";
        m_csvTrans.push_back(line + "\n");
        int rowCount = 0;
        QString bgcolor;
        if(rowCount%2){
            bgcolor ="#efefef";

        }

        htmlUtils.createRow();
        QString slno = QString::number(rowCount + 1);
        htmlUtils.createCell(slno, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell(deptName, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("Opening Balance Local Pool", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        QString receiptStr = CsdUtils::convertAmountToStringWithSign(statement.m_openintBalance);
        htmlUtils.createCell(receiptStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
        receiptStr = CsdUtils::converAmountToString(statement.m_openintBalance);
        line = slno + "," + deptName + ",Opening Balance Local Pool," + receiptStr + ",\n";
        m_csvTrans.push_back(line);
        income += statement.m_openintBalance;
        ++rowCount;

        for (auto accountamt : statement.m_incomePaymentAmt) {
            QString accountName = accountamt.first;
            double accountIncome = accountamt.second.first;
            double accountPayment = accountamt.second.second;
            QString bgcolor;
            if(rowCount%2){
                bgcolor ="#efefef";

            }
            htmlUtils.createRow();
            QString slno = QString::number(rowCount + 1);
            htmlUtils.createCell(slno, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(deptName, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(accountName, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            QString receiptStr = CsdUtils::convertAmountToStringWithSign(accountIncome);
            QString paymentStr = CsdUtils::convertAmountToStringWithSign(accountPayment);
            htmlUtils.createCell(receiptStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(paymentStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.closeRow();
            receiptStr = CsdUtils::converAmountToString(accountIncome);
            paymentStr = CsdUtils::converAmountToString(accountPayment);
            line = slno + "," + deptName + ","+ accountName +"," + receiptStr +","+ paymentStr + ",\n";
            m_csvTrans.push_back(line);
            income += accountIncome;
            payment += accountPayment;
            ++rowCount;
        }


        bgcolor.clear();
        if(rowCount%2){
            bgcolor ="#efefef";

        }


        htmlUtils.createRow();
        htmlUtils.createCell("", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("<stong>TOTAL</strong>", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        receiptStr = "<strong>" + CsdUtils::convertAmountToStringWithSign(income) + "</strong>";
        QString paymentStr = "<strong>"  + CsdUtils::convertAmountToStringWithSign(payment) + "</strong>";
        htmlUtils.createCell(receiptStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell(paymentStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
        receiptStr = CsdUtils::converAmountToString(income);
        paymentStr = CsdUtils::converAmountToString(payment);
        line =   ",,Total," + receiptStr +"," + paymentStr + ",\n";
        m_csvTrans.push_back(line);
        ++rowCount;

        bgcolor.clear();
        if(rowCount%2){
            bgcolor ="#efefef";

        }

        QString closingString = "Closing Balance Local Pool";
        double diff = (income - payment) - statement.m_closingBalance;
        if (diff != 0) {
            closingString += "*";
        }
        htmlUtils.createRow();
        htmlUtils.createCell("", HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell(closingString, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        receiptStr = CsdUtils::convertAmountToStringWithSign(statement.m_closingBalance);
        htmlUtils.createCell(receiptStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.createCell("", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_TOP_BOTTOM_BORDER, HTML_NORMAL_BORDER_STYLE, bgcolor);
        htmlUtils.closeRow();
        receiptStr = CsdUtils::converAmountToString(statement.m_closingBalance);
        line =   ",,"+closingString+"," + receiptStr + ",\n";
        m_csvTrans.push_back(line);


        htmlUtils.closeTable();
    }


    QString html = htmlUtils.getHtml() + ChtmlUtils::getPageBreak();

    m_htmlList.push_back(html);
}

void CledgerReportDlg::resizeEvent(QResizeEvent *event)
{
    resize();
}

void CledgerReportDlg::on_m_fromDate_dateChanged(const QDate &date)
{
   // qDebug()<<"from date change";
    populateTable();
}


void CledgerReportDlg::on_m_toDate_dateChanged(const QDate &date)
{

    populateTable();
}


void CledgerReportDlg::on_m_DeptBox_currentTextChanged(const QString &arg1)
{
    //qDebug()<<"Dept:"<<arg1;
    bool isConstructor =  m_isConstructor;
    m_isConstructor = true;
    /*ui->m_individualLEdger->setChecked(false);
    ui->m_individualLEdger->setVisible(false);
    ui->m_individualLEdger->setCheckable(true);*/
    if (arg1.isEmpty() == false) {
        ui->m_accountBox->clear();
        ui->m_accountBox->addItem("");
        int deptID = CaccountMap::Object()->getDeptId(arg1);
        QSet<int> accountIds;
        if (CaccountMap::Object()->getAccounsForDept(deptID, accountIds)) {
            QStringList accounts;
            for (int id : accountIds) {
                QString accountName = CaccountMap::Object()->getAccountName(id);
                accounts.push_back(accountName);
                //qDebug()<<"accountname "<<accountName;
                //ui->m_accountBox->addItem(accountName);
            }
            accounts.sort();
            ui->m_accountBox->addItems(accounts);
        }
    } else {
        QStringList accounts = CaccountMap::Object()->getAccountList();
        accounts.sort();
        ui->m_accountBox->addItems(accounts);
    }

    m_isConstructor = isConstructor;
    populateTable();
}


void CledgerReportDlg::on_m_accountBox_currentTextChanged(const QString &arg1)
{
    bool isConstructor =  m_isConstructor;
    m_isConstructor = true;
   /* ui->m_individualLEdger->setChecked(false);
    ui->m_individualLEdger->setVisible(false);

    if (arg1.isEmpty() == false) {
        ui->m_individualLEdger->setVisible(true);
    }
    ui->m_individualLEdger->setCheckable(true);*/

    m_isConstructor = isConstructor;

    populateTable();
}


void CledgerReportDlg::on_m_nameFilter_editingFinished()
{
    int id = CcontactMap::Object()->getContactID(ui->m_nameFilter->text().trimmed().simplified());
    if (id > 0) {

        populateTable();
    }

}
void CledgerReportDlg::on_m_deptInCSV_clicked()
{
    populateTable();

}



void CledgerReportDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CledgerReportDlg::on_m_csvBtn_clicked()
{
    CprintUtils csv(m_csvTrans);
    csv.printCSV();

    emit accept();
}


void CledgerReportDlg::on_m_pdfBtn_clicked()
{

    CprintUtils htmltopdf(m_html, true);
    htmltopdf.printPDF();
    emit accept();
}


void CledgerReportDlg::on_m_individualLedger_clicked()
{
    populateTable();
}
