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
#include "MISC/cremittancedetails.h"
#include "cremittancetable.h"
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
    for (auto [key, value]: m_allAccountLedger.asKeyValueRange()) {
        for (auto [id, account] : value.asKeyValueRange()) {
            delete account;
        }
    }
    m_allAccountLedger.clear();
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
    ui->m_nameFilter->setGeometry(x, y, DATE_SIZE.width() + btnWidth, DATE_SIZE.height());
    x += GAP + DATE_SIZE.width() + btnWidth;

    ui->m_deptInCSV->setGeometry(x, y, DATE_SIZE.width() + btnWidth, DATE_SIZE.height());
    x += GAP + DATE_SIZE.width() + btnWidth;
    ui->m_individualLedger->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    y += GAP +  DATE_SIZE.height();

    x = XCORD;
    int width = windowwidth - 2*XCORD;
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

    bool hasAccount = false;
    if (account.isEmpty() == false) {
        int accountID = CaccountMap::Object()->getAccountID(account);
        if (accountID > 0) {
            hasAccount = true;
            query +=" AND (" + CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(accountID) + ")";
        }

    } else {
        int deptID = CaccountMap::Object()->getDeptId(dept);
        QSet<int> accounts;
        CaccountMap::Object()->getAccounsForDept(deptID, accounts);
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

    }

    QString name = ui->m_nameFilter->text().trimmed().simplified();
    if (name.isEmpty() == false) {
        int id = CcontactMap::Object()->getContactID(name);
        query += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_FROM_IDX) + "=" + QString::number(id) + ")";


    }
     bool individualLedger = false;
    if ((ui->m_individualLedger->isChecked() == true && hasAccount == true)) {
        individualLedger = true;
    }
    CaccountLedger cashAccount(true);
    CaccountLedger bankAccount(true);
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    bool isAll = ui->m_accountBox->currentText().isEmpty() && ui->m_DeptBox->currentText().isEmpty() && ui->m_nameFilter->text().isEmpty();
    if (results.isEmpty() == false && individualLedger == false && isAll) {
        CclosingCalculator closingCalc;
        closingCalc.calculateClosing( ui->m_fromDate->date() ,query);
        printForOpeningClosing(&closingCalc);
    }
    int loanAccountID = 0;
    //qDebug()<<"qeery is "<<query;
    //qDebug()<<"Amount Account, From/To Type";
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
                bankAccount.addTransaction(bank);
                cashAccount.addTransaction(cash);

            } else {

                if (mode == "Bank") {
                    bankAccount.addTransaction(new StransactionData(*transData));
                } else {
                    cashAccount.addTransaction(new StransactionData(*transData));
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
        if (individualLedger == false && (ui->m_accountBox->currentText().isEmpty() || ui->m_accountBox->currentText() == gLoanAccountName)) {
            CloanLedger* loanLedger = new CloanLedger();
            int id = CaccountMap::Object()->getAccountID(gLoanAccountName);
            if (loanLedger->processTransForLoanAcc()) {
                 QMap<int, CaccountLedger*>& accountLedgerMap =  m_allAccountLedger[50000 /*deptID*/];
                //qDebug()<<"loan account name "<<gLoanAccountName<<" id "<<id;
                accountLedgerMap[id] = loanLedger;
            } else  {
                delete loanLedger;
            }
        }
    }

    bool isDeptInCSV = individualLedger == false ? ui->m_deptInCSV->isChecked() : false;
    QString replaceAccountName = individualLedger ? account : "";

    for (auto [accountOrDeptID, accountLedgerMap] : m_allAccountLedger.asKeyValueRange()) {
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
        QString html = bankAccount.getHTMLTable(0, isDeptInCSV, false, "Bank");
        QStringList csvs = bankAccount.getCSVTransList();
        m_htmlList.push_back(html);
        foreach(const QString& line, csvs) {
            m_csvTrans.push_back(line);
        }
    }
    if (individualLedger == false){
        QString html = cashAccount.getHTMLTable(0, isDeptInCSV, false, "Cash");
        QStringList csvs = cashAccount.getCSVTransList();
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
    printForSummary(calc);
    printForClosingSummary(calc);
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
    htmlUtils.addTableColSize(2);
    htmlUtils.addTableColSize(10);
    htmlUtils.addTableColSize(2);
    htmlUtils.addTableColSize(10);
    htmlUtils.addTableColSize(13);


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
    htmlUtils.addTableColSize(20);
    htmlUtils.addTableColSize(15);
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
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(18);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(9);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(12);
    htmlUtils.addTableColSize(12);
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

            htmlUtils.createRow();
            htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(transData->m_ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            csvLine += dateStr + ",";
            if (addDeptInCSV) {
                csvLine +=deptName + ", " + accountName + ",";
            }
            csvLine += fromTo + ", " + receipt + ", " + mode +"," + transData->m_ref + ",";
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
                htmlUtils.createRow();
                htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(transData->m_ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
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







void CledgerReportDlg::on_m_individualLedger_clicked()
{
    populateTable();
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
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(18);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(9);
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
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(18);
    htmlUtils.addTableColSize(8);
    htmlUtils.addTableColSize(9);
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
             htmlUtils.createRow();
             htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
             htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
             htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
             htmlUtils.createCell(transData->m_ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
             htmlUtils.createCell(amountStr, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
             csvLine = dateStr +"," + fromTo + ", " + mode +", " + transData->m_ref + "," + amountStrCSV +", ";
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

    return CaccountLedger::getHTMLTable(accountID, addDeptInCSV, indiviualAccount, replaceAccountName);


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

            htmlUtils.createRow();
            htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(transData->m_ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(incomeAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            htmlUtils.createCell(paymentAmount, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
            csvLine += dateStr + ",";
            if (addDeptInCSV) {
                csvLine +=deptName + ", " + accountName + ",";
            }
            csvLine += fromTo + ", " + receipt + ", " + mode +"," + transData->m_ref + ",";
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
                htmlUtils.createRow();
                htmlUtils.createCell(dateStr, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(fromTo, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(receipt, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(mode, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
                htmlUtils.createCell(transData->m_ref, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
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
