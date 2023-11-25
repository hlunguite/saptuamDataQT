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

    CclosingCalculator closingCalc;
    closingCalc.calculateClosing( ui->m_fromDate->date() ,query);
    populateForOpeningClosing(&closingCalc);

    //qDebug()<<"qeery is "<<query;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    //qDebug()<<"Amount Account, From/To Type";
    for (auto& transData: results) {
         int deptID = CaccountMap::Object()->getDeptForAccount(transData->m_accountId);
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
        int firstID = individualLedger ? transData->m_accountId : deptID;
        int secondID = individualLedger ? transData->m_fromId : transData->m_accountId;
        QMap<int, CaccountLedger*>& accountLedgerMap =  m_allAccountLedger[firstID];
        CaccountLedger* accountLedger = accountLedgerMap[secondID];
        if (accountLedger == nullptr) {
            accountLedger = new CaccountLedger;
        }

        accountLedger->addTransaction(transData);
        accountLedgerMap[secondID] = accountLedger;
    }
    bool isDeptInCSV = individualLedger == false ? ui->m_deptInCSV->isChecked() : false;
    QString replaceAccountName = individualLedger ? account : "";

    for (auto [firstID, accountLedgerMap] : m_allAccountLedger.asKeyValueRange()) {
        for (auto [secondID, accountLedger] : accountLedgerMap.asKeyValueRange()) {
            QString html = accountLedger->getHTMLTable(secondID, isDeptInCSV, individualLedger, replaceAccountName);

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

void CledgerReportDlg::populateForOpeningClosing(CclosingCalculator *calc)
{

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
            QString bgcolor;
            EhtmlCellBoarder borderType = HTML_NO_BORDER;
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

