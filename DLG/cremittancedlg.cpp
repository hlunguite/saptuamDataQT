#include <QMenu>
#include "cremittancedlg.h"
#include "ui_cremittancedlg.h"
#include "cremittancesetuptable.h"
#include "caccountmap.h"
#include <QVBoxLayout>
#include "ctransactiontable.h"
#include "ccontactmap.h"
#include "csdutils.h"
#include "MISC/cprintUtils.h"
#include "ccustomdeligateforremittance.h"
#include <QMessageBox>
#include "ctransactionutils.h"
#include "cremittancetable.h"
#include "cselectremittance.h"
//#include <QRegularExpression>
#include "MISC/CdlgDefine.h"
CremittanceDlg::CremittanceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CremittanceDlg)
{
    ui->setupUi(this);
    m_constructor = true;
    m_allHq = true;
    m_selectRow = -1;
    m_deleteRowAction = new QAction("Delete Row", this);
    m_addRowAction = new QAction("Add Row", this);
    connect(m_deleteRowAction, SIGNAL(triggered()), this, SLOT(deleteRow()));
    connect(m_addRowAction, SIGNAL(triggered()), this, SLOT(addRow()));
    QDate date = QDate::currentDate();
    ui->m_periodFrom->setDate(date);
    ui->m_periodTo->setDate(date);
    ui->m_transDate->setDate(date);
    ui->m_submitDate->setDate(date);
    m_customDeligateForRemittance = new CcustomDeligateForRemittance;
    m_remittanceAmount = 0;
    m_remittanceIndex = 0;

    QStringList remitList = CremittanceNameTable::Object()->getAllRemittanceName();
    ui->m_remittanceNameCombo->addItems(remitList);
    ui->m_table->setColumnHidden(0, true);
    ui->m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->m_table->setItemDelegate(m_customDeligateForRemittance);

    m_constructor = false;
    populateAccount();
    populateTable();



}

CremittanceDlg::~CremittanceDlg()
{
    delete ui;
}

void CremittanceDlg::populateAccount()
{
    if (m_constructor) {
        return;
    }
    QVBoxLayout *vbox = nullptr;
    m_accountPct.clear();
    if (ui->m_accountBox->layout()) {
       vbox = (QVBoxLayout*)ui->m_accountBox->layout();
       QLayoutItem *item;
       while ((item = vbox->takeAt(0)) != nullptr) {
           if (QWidget* widget = item->widget()) {
               widget->deleteLater();
           }
           delete item;
       }
       delete ui->m_accountBox->layout();
    }
    vbox = new QVBoxLayout;

    QString remitName = ui->m_remittanceNameCombo->currentText();
    m_accountCheckBox.clear();
    m_accontIDs.clear();
    QStringList accountList;
    std::map<QString, int> nameID;

    if (m_remittanceIndex > 0) {
       QVector<SremittanceDetailData*>* vData = CremittanceDetailTable::Object()->getRemittanceDetailForTableID(m_remittanceIndex);
       if (vData) {
           for (auto data : *vData) {
               int	accountID = data->m_accuntTableID;
               double	localShare = data->m_localShare;
               double   hqShare = data->m_hqShare;
               QString accountName = CaccountMap::Object()->getAccountName(accountID);
               m_accountPct[accountID] = std::make_pair(hqShare,localShare);
               nameID.insert({accountName, accountID});

               delete data;
           }
           delete vData;
       }

    } else {
       QVector<SremittanceSetupTableData*>* accounts = CremittanceSetupTable::Object()->getRemitanceSetupForName(remitName);

       if (accounts) {
           int size = accounts->size();
           for (int i = 0; i < size; ++i) {
               SremittanceSetupTableData* data = (*accounts)[i];
               int account = data->m_accountID;
               QString accountName = CaccountMap::Object()->getAccountName(account);
               nameID.insert({accountName, account});
               SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(data->m_percentageID);
               m_accountPct[account] = std::make_pair(pcData->m_hqShare, pcData->m_localShare);

               delete pcData;
               delete data;
           }
           delete accounts;

       }
    }
    for (auto keyValue : nameID) {
       QString accountName = keyValue.first;
       int accountID = keyValue.second;
       accountList.push_back(accountName);
       QCheckBox * cb = new QCheckBox(accountName);
       cb->setChecked(true);
       m_accountCheckBox.push_back(cb);
       m_accontIDs.push_back(accountID);
       vbox->addWidget(cb);
       connect(cb, SIGNAL(clicked()), this, SLOT(accountCheckBoxClicked()));

    }
    vbox->addStretch(1);
    ui->m_accountBox->setLayout(vbox);
    accountList.sort();
    m_customDeligateForRemittance->setAccountList(accountList);


}

void CremittanceDlg::populateTable()
{
    ui->m_textBrowser->setText("");
    if (m_constructor) {
        return;
    }
    //m_accountRemittance.clear();
    //m_deptRemittance.clear();
    m_allHq = true;
    m_html.clear();
    m_csvList.clear();
    if (populateTableUsingTransaction() == false) {
        ui->m_textBrowser->setHtml("");
        return;
    }
    calculateTotalAndPopulateHtmlTable();


}
void CremittanceDlg::populateHTMLTable() {
    m_csvList.clear();
    QStringList headers = CsdUtils::getHeaderText();
    m_csvList.push_back(headers.at(0) + "\n");
    m_csvList.push_back(headers.at(1) + "\n");
    m_csvList.push_back(headers.at(2) + "\n");
    m_csvList.push_back("\n\n");
    m_html = ChtmlUtils::getHeader();
    QString addressText = ui->m_toAddressBox->toPlainText();

    QString submitDate = ui->m_submitDate->date().toString("d MMM yyyy");

    if (addressText.isEmpty() == false) {
        QStringList split = addressText.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
        for (int i = 0; i < split.size(); ++i) {
            if (i == 0) {
               m_csvList.push_back(split.at(i) + ",,,,\n");
            }else {
               m_csvList.push_back(split.at(i) + ",,,,\n");
            }
        }
        m_csvList.push_back("");

        addressText.replace(QRegularExpression("[\r\n]"), "<br>");

        ChtmlUtils htmlUtils;
        htmlUtils.setIsTableHeader(false);
        htmlUtils.addTableColSize(50);
        htmlUtils.addTableColSize(50);
        htmlUtils.openTable();
        htmlUtils.createRow();
        htmlUtils.createCell(addressText, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.createCell(submitDate,
                             HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE);
        htmlUtils.closeRow();
        htmlUtils.closeTable();
        m_html += htmlUtils.getHtml(false, false);
    }

    QString submitText = ui->m_submitByBox->toPlainText();
    QString sumitTextHtml;
    QString subMitTextCSV = submitText;
    if (submitText.isEmpty() == false) {
        ChtmlUtils htmlutil;

        htmlutil.formatString(submitText, true);
        sumitTextHtml = htmlutil.getHtml(false, false);

        //QStringList split = submitText.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

    }
    QString htmlSummary;
    QStringList headerList = {"Sl No", "Head of Account", "Receipt Book & Sl No", "Total Amount", "Local Fund", "For Hqtrs"};
    double totalAll =0;
    double local = 0;
    double hq = 0;
    ChtmlUtils htmlUtils;
    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(6); //sl
    htmlUtils.addTableColSize(20); //account name
    htmlUtils.addTableColSize(20); // receipt book
    htmlUtils.addTableColSize(18); // total
    htmlUtils.addTableColSize(18); // local
    htmlUtils.addTableColSize(18); // hq
    htmlUtils.openTable();
    htmlUtils.createRow();

    QString csvline;
    for (auto header:  headerList) {
        htmlUtils.createCell(header, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        csvline += header + ",";
    }
    htmlUtils.closeRow();
    csvline += "\n";
    if (m_allHq == false) {
        m_csvList.push_back(csvline);
    }

    int rowCount = 0;
    if (m_allHq) {
        QString accountName;
        for (auto keyvalue: m_accountRemittance) {
            if (accountName.isEmpty() == false){
               accountName += ", ";
            }
            accountName += CaccountMap::Object()->getAccountName(keyvalue.first);

        }
        QString text = "Hiai a nuai a bang in "+ accountName + " " + ui->m_modeCombo->currentText();
        if(ui->m_referene->text().trimmed().isEmpty() == false) {
            text += " " + ui->m_referene->text().trimmed();
        }
        text += ", " + ui->m_transDate->date().toString("d MMM yyyy") + " ni in ka hon khak hi.";
        ChtmlUtils htmlutil;
        htmlutil.formatString(text, true);
        m_csvList.push_back(text + "\n\n");
        m_html += htmlutil.getHtml(false, false);



    } else {

        QString text = "Hiai a nuai a bang in " + ui->m_modeCombo->currentText();
        if(ui->m_referene->text().trimmed().isEmpty() == false) {
            text += " " + ui->m_referene->text().trimmed();
        }
        text += ", " + ui->m_transDate->date().toString("d MMM yyyy") + " ni in ka hon khak hi.";
        ChtmlUtils htmlutil;
        htmlutil.formatString(text, true);
        m_html += htmlutil.getHtml(false, false);
        m_csvList.push_back(text + "\n");



    }



    std::map<int, StransForRemittance>::iterator itr = m_accountRemittance.begin();
    std::map<int, StransForRemittance>::iterator end = m_accountRemittance.end();

    while(itr != end) {
        populateSummaryTable(htmlUtils, m_csvList, itr, rowCount, true, totalAll, local, hq);
        ++itr;
    }

    itr = m_deptRemittance.begin();
    end = m_deptRemittance.end();

    while(itr != end) {
        populateSummaryTable(htmlUtils, m_csvList, itr, rowCount, false, totalAll, local, hq);
        ++itr;
    }
    ++rowCount;

    QString bgcolor;
    EhtmlCellBoarder borderType = HTML_NO_BORDER;
    if(rowCount%2 == 0){
        bgcolor ="#efefef";
    }
    htmlUtils.createRow();


    QString csvLine;
    QString data = "";
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    htmlUtils.createCell(data, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    data = "Total";
    borderType = HTML_TOP_BOTTOM_BORDER;
    htmlUtils.createCell(data, HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    QString totalStr = CsdUtils::convertAmountToStringWithSign(totalAll);
    QString totalStrCSV = CsdUtils::converAmountToString(totalAll,'f',2);

    QString localStr = CsdUtils::convertAmountToStringWithSign(local);
    QString localStrCSV = CsdUtils::converAmountToString(local,'f',2);

    QString hqStr = CsdUtils::convertAmountToStringWithSign(hq);
    QString hqStrCSV = CsdUtils::converAmountToString(hq,'f',2);

    htmlUtils.createCell(totalStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += totalStrCSV + ",";

    htmlUtils.createCell(localStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += localStrCSV + ",";

    htmlUtils.createCell(hqStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += hqStrCSV + ",\n";
    htmlUtils.closeRow();




    htmlUtils.closeTable();
    htmlSummary = htmlUtils.getHtml(false, false);
    ui->m_textBrowser->setHtml(htmlSummary);
    if (m_allHq == false) {
        m_csvList.push_back(csvLine  + "\n\n");

        m_html += htmlSummary;
        m_html += "<br>" + sumitTextHtml;

        m_csvList.push_back(subMitTextCSV + "\n\n");
        //m_html += ChtmlUtils::getTwoSignator();
        m_html += ChtmlUtils::getPageBreak();
    }



    bool first = true;
    for (auto& keyValue : m_accountRemittance) {
        if (first == false) {
            m_html+= ChtmlUtils::getPageBreak();
        }
        populateTransacltionListTable(keyValue.first, keyValue.second, true);

        first = false;
    }

    for (auto& keyValue : m_deptRemittance) {
        if (first == false) {

            m_html+= ChtmlUtils::getPageBreak();
        }
        populateTransacltionListTable(keyValue.first, keyValue.second, false);

        first = false;
    }


    if (m_allHq) {
        m_html += sumitTextHtml;
        m_csvList.push_back( subMitTextCSV +"\n");
        if (submitDate.isEmpty() == false) {
            m_csvList.push_back(submitDate);
        }

    }

    //ui->m_textBrowser->setText(m_html);
}

void CremittanceDlg::populateSummaryTable(ChtmlUtils& htmlUtils,
                                          QStringList& csvList,
                                          std::map<int, StransForRemittance>::iterator & keyvalue,
                                          int &rowCount,
                                          bool isAccount,
                                          double& totalAll,
                                          double& local,
                                          double& hq)
{

    ++rowCount;

    QString bgcolor;
    EhtmlCellBoarder borderType = HTML_NO_BORDER;
    if(rowCount%2 == 0){
        bgcolor ="#efefef";
    }
    htmlUtils.createRow();
    QString csvLine;
    QString data = QString::number(rowCount);
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    data = isAccount ? CaccountMap::Object()->getAccountName(keyvalue->first) :
               CaccountMap::Object()->getDeptName(keyvalue->first);
    htmlUtils.createCell(data, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    data = "";
    for (auto receipts : keyvalue->second.m_receipts) {
        if (data.isEmpty() == false) {
            data += "; ";
        }
        data += QString::number(receipts.first);
        QString number = CsdUtils::makeReceiptString(receipts.second);
        if (number.isEmpty() == false) {
            data += "- " + number;
        }
    }
    htmlUtils.createCell(data, HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    double total = keyvalue->second.m_hqShare +  keyvalue->second.m_localShare;
    totalAll += total;
    local +=  keyvalue->second.m_localShare;
    hq += keyvalue->second.m_hqShare;
    QString totalStr = CsdUtils::convertAmountToStringWithSign(total);
    QString totalStrCSV = CsdUtils::converAmountToString(total,'f',2);

    QString localStr = CsdUtils::convertAmountToStringWithSign(keyvalue->second.m_localShare);
    QString localStrCSV = CsdUtils::converAmountToString(keyvalue->second.m_localShare,'f',2);

    QString hqStr = CsdUtils::convertAmountToStringWithSign(keyvalue->second.m_hqShare);
    QString hqStrCSV = CsdUtils::converAmountToString(keyvalue->second.m_hqShare,'f',2);

    htmlUtils.createCell(totalStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += totalStrCSV + ",";

    htmlUtils.createCell(localStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += localStrCSV + ",";

    htmlUtils.createCell(hqStr + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += hqStrCSV + "\n";
    htmlUtils.closeRow();
    csvList.push_back(csvLine);

}

void CremittanceDlg::populateTransacltionListTable(int accountDept, const StransForRemittance &transForRemittance, bool isAccount)
{



    QStringList headerList = {"Sl No", "Receipt Book No", "Receipt Sl No", "Min", "Piak San", "Amount"};
    ChtmlUtils htmlUtils;
    htmlUtils.setIsTableHeader(false);
    htmlUtils.addTableColSize(6); //sl
    htmlUtils.addTableColSize(14); //Receipt Book No
    htmlUtils.addTableColSize(14); // Receipt Sl No
    htmlUtils.addTableColSize(24); // Min
    htmlUtils.addTableColSize(24); // Piak Chan
    htmlUtils.addTableColSize(18); // Amount
    htmlUtils.openTable();
    htmlUtils.createRow();


    QString csvline;
    for (auto header:  headerList) {
        htmlUtils.createCell(header, HTML_CENTER_ALIGN, HTML_V_MIDDLE_ALIGN, HTML_NO_BORDER, HTML_NORMAL_BORDER_STYLE, "#E2E2E2");
        csvline += header + ",";
    }
    htmlUtils.closeRow();

    csvline += "\n";
    m_csvList.push_back(csvline);
    int rowCount = 0;
    /*if (isAccount == false) {
        qDebug()<<"Dept is "<<CaccountMap::Object()->getDeptName(accountDept)<<" "<<accountDept;
    }*/

    QString accountDeptName = isAccount ? CaccountMap::Object()->getAccountName(accountDept) : CsdUtils::abbravate(CaccountMap::Object()->getDeptName(accountDept));
    double total = 0;
    for (auto & trans : transForRemittance.m_allTrans) {
        ++rowCount;
        QString bgcolor;
        EhtmlCellBoarder borderType = HTML_NO_BORDER;
        if(rowCount%2 == 0){
            bgcolor ="#efefef";
        }
        htmlUtils.createRow();
        QString csvLine;
        QString data = QString::number(rowCount);
        htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += data + ",";
        data = trans.m_receiptBook;
        htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += data + ",";
        data = trans.m_receiptNo;
        htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += data + ",";
        data = trans.m_from;
        htmlUtils.createCell(data + " ", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += data + ",";
        data = accountDeptName;
        htmlUtils.createCell(data + " ", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += data + ",";

        data = trans.m_amount.isEmpty() ? "" :  CsdUtils::convertAmountToStringWithSign(trans.m_amount.toDouble());
        total += trans.m_amount.isEmpty() ? 0 : trans.m_amount.toDouble();
        htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
        csvLine += trans.m_amount + ",\n";
        m_csvList.push_back(csvLine);

        htmlUtils.closeRow();

    }
    ++rowCount;
    QString bgcolor;
    EhtmlCellBoarder borderType = HTML_NO_BORDER;
    if(rowCount%2 == 0){
        bgcolor ="#efefef";
    }
    htmlUtils.createRow();
    QString csvLine;
    QString data ="";
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    htmlUtils.createCell(data + " ", HTML_LEFT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    data = "Total";
    borderType = HTML_TOP_BOTTOM_BORDER;
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    csvLine += data + ",";
    data =  CsdUtils::convertAmountToStringWithSign(total);
    htmlUtils.createCell(data + " ", HTML_RIGHT_ALIGN, HTML_V_MIDDLE_ALIGN, borderType, HTML_NORMAL_BORDER_STYLE, bgcolor);
    data =  CsdUtils::converAmountToString(total,'f',2);
    csvLine += data + "\n";
    m_csvList.push_back(csvLine + "\n\n");


    htmlUtils.closeRow();


    m_csvList.push_back("");

    htmlUtils.closeTable();
    if (m_allHq) {
        m_html += htmlUtils.getHtml(false, false);
    } else {
        m_html += htmlUtils.getHtml(true, true);
        m_html += "<p></p>" + ChtmlUtils::getTwoSignator();


    }

}
QString CremittanceDlg::getQueryString(int remittanceID , bool isEqual)
{
    int count = m_accountCheckBox.size();
    QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";
    QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";

    if (remittanceID == 0) {
        QString accountFilter;
        bool first = true;
        for (int idx = 0; idx < count; ++idx) {
            QCheckBox * cb = m_accountCheckBox.at(idx);
            int accIdx = m_accontIDs.at(idx);
            //qDebug()<<"Account name "<<CaccountMap::Object()->getAccountName(accIdx)<<" is checked "<<cb->isChecked();
            if (cb->isChecked() == false) {
               continue;
            }
            if (first) {
               accountFilter = " (";
            } else {
               accountFilter += " OR ";
            }
            first = false;
            accountFilter +=  CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(accIdx);
        }
        if (accountFilter.isEmpty() == false) {
            accountFilter += ")";
        } else {
            return "";
        }
        if (accountFilter.isEmpty() == false) {
            filter += " AND " + accountFilter;
        }
        QString startdate = ui->m_periodFrom->date().toString("yyyy-MM-dd");
        QString enddate = ui->m_periodTo->date().toString("yyyy-MM-dd");

        QString dateFilter = CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startdate +"\"";
        dateFilter += " AND \"" + enddate +"\"";

        filter += " AND (" + dateFilter + ")";
    }

    query += filter;
    if (isEqual) {
        query += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_REMITTANCE_ID_IDX) + "=";
    } else {
        query += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_REMITTANCE_ID_IDX) + ">";

    }
    query += QString::number(remittanceID) +  ")";



    return query;

}
bool CremittanceDlg::populateTableUsingTransaction()
{
    m_remittanceIndex = 0;
    m_selectRow = -1;
    deleteAllRow();
    QString query = getQueryString(0, true);
    QString queryRemittace = getQueryString(0, false);
    //qDebug()<<"qeury for transacton "<<query;
    //qDebug()<<"query for remittance "<<queryRemittace;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    QVector<StransactionData*> resultsRemittance = CtransactionTable::Object()->getAllTransaction(queryRemittace);
    disableEnable(true);
    bool ret = false;
    if (resultsRemittance.isEmpty()) {
        return populateTransaction(results);
    } else {
        std::set<int> remittanceTableID;

        for (auto transData: resultsRemittance) {
            remittanceTableID.insert(transData->m_remittanceID);


        }
        bool populateUsingRemittance = false;
        if ( remittanceTableID.size() == 1 &&  results.isEmpty()) {
            m_remittanceIndex = *remittanceTableID.begin();
            populateUsingRemittance = true;


        } else {
             if (results.isEmpty() == false) {
               remittanceTableID.insert(0);
             }
             CselectRemittance selectRemit;

             selectRemit.setAllID(remittanceTableID);
             if (selectRemit.exec()) {

               int select = selectRemit.selectID();
               if (select == 0) {
                    ret = populateTransaction(results);
                    results.clear();
               } else if (select > 0) {
                    m_remittanceIndex = select;
                    populateUsingRemittance = true;
               }
            }
        }
        if (populateUsingRemittance) {
            queryRemittace = getQueryString(m_remittanceIndex, true);
            QVector<StransactionData*> remitToDislay = CtransactionTable::Object()->getAllTransaction(queryRemittace);
            disableEnable(false);
            populateAccount();
            ret = populateTransaction(remitToDislay);
            setFieldBaseOnremittanceID();
           //populate reconcile
            //qDebug()<<"to reconcile "<<m_remittanceIndex;
            QVector<SremittanceReconcileData*>* vData = CremittanceReconcileTable::Object()->getRemittanceReconcileForTableID(m_remittanceIndex);
            if (vData) {
              // qDebug()<<"reconcile found";
               for (auto data :*vData) {
                    int	 accontDepID = data->m_accontOrDeptTableID;
                    bool isAccount = data->m_isAccount;
                    QString	str = data->m_str;
                    SremitTransDetail detail(str);
                    //qDebug()<<"str is "<<str;
                    float hqPC = 0;
                    float localPC = 100;
                    std::map<int, std::pair<float, float> >::iterator fn = m_accountPct.find(accontDepID);
                    double amount = detail.m_amount.toDouble();

                    //qDebug()<<"local amount "<<local<<" hq "<<hq;
                    if (fn != m_accountPct.end()) {
                        hqPC = fn->second.first;
                        localPC = fn->second.second;
                    }
                    double local = (amount*localPC)/100;
                    double hq = (amount*hqPC)/100;
                    if (isAccount) {
                        StransForRemittance& transRemit = m_accountRemittance[accontDepID];
                        transRemit.m_allTrans.push_back(detail);
                        transRemit.m_hqShare += hq;
                        transRemit.m_localShare += local;
                    } else {
                        StransForRemittance& transRemit = m_deptRemittance[accontDepID];
                        transRemit.m_allTrans.push_back(detail);
                        transRemit.m_hqShare += hq;
                        transRemit.m_localShare += local;
                    }
                    int rowCount = ui->m_table->rowCount();
                    ui->m_table->insertRow(rowCount);

                    //set min
                    QTableWidgetItem *itemMin = new QTableWidgetItem();
                    itemMin->setFlags(itemMin->flags() & ~Qt::ItemIsEditable);
                    itemMin->setText(detail.m_from);
                    ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_MIN, itemMin);

                    QString accountName = CaccountMap::Object()->getAccountName(detail.m_accountID);
                    QTableWidgetItem *itemAccount = new QTableWidgetItem();
                    itemAccount->setFlags(itemAccount->flags() & ~Qt::ItemIsEditable);
                    itemAccount->setText(accountName);
                    ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_PIAKSAN, itemAccount);


                    QTableWidgetItem *itemAmount = new QTableWidgetItem();
                    itemAmount->setFlags(itemAmount->flags() & ~Qt::ItemIsEditable);
                    itemAmount->setText(detail.m_amount);
                    ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_AMOUNT, itemAmount);

                    QString amountLocalStr = CsdUtils::converAmountToString(local,'f',2);
                    QTableWidgetItem *itemAmountLocal = new QTableWidgetItem();
                    itemAmountLocal->setFlags(itemAmountLocal->flags() & ~Qt::ItemIsEditable);
                    itemAmountLocal->setText(amountLocalStr);
                    ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_LOCAL_SHARE, itemAmountLocal);


                    QString amountHqStr = CsdUtils::converAmountToString(hq,'f',2);
                    QTableWidgetItem *itemAmountHq = new QTableWidgetItem();
                    itemAmountHq->setFlags(itemAmountHq->flags() & ~Qt::ItemIsEditable);
                    itemAmountHq->setText(amountHqStr);
                    ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_HQ_SHARE, itemAmountHq);
               }

               delete vData;
            }

        }
        for (auto todel : results) {
            delete todel;
        }
        for (auto todel : resultsRemittance) {
            delete todel;
        }
    }



    return ret;
}
 bool CremittanceDlg::populateTransaction( QVector<StransactionData*>&results)
 {
    m_selectRow = -1;
    deleteAllRow();
    if (results.size() == 0) {
        return false;
    }
    bool constructor = m_constructor;
    m_constructor = true;
    for (auto transData: results ) {

        int accountID = transData->m_accountId;
        int id = transData->m_id;
        float hqPC = 0;
        float localPC = 100;
        std::map<int, std::pair<float, float> >::iterator fn = m_accountPct.find(accountID);
        if (fn != m_accountPct.end()) {
             hqPC = fn->second.first;
             localPC = fn->second.second;
        }

        QString receiptBook = transData->m_reeiptBook;
        QString receiptSl = transData->m_receiptSlNo;
        if (receiptBook.isEmpty() || receiptSl.isEmpty()) {
            receiptSl.clear();
            receiptBook.clear();
        }
        QString min = CcontactMap::Object()->getContanceName(transData->m_fromId);
        double amount = transData->m_amount;
        double local = (amount*localPC)/100;
        double hq = (amount*hqPC)/100;

        QString amountStr = CsdUtils::converAmountToString(amount,'f',2);


        int rowCount = ui->m_table->rowCount();
        ui->m_table->insertRow(rowCount);
        //set id
        QTableWidgetItem *itemID = new QTableWidgetItem();
        itemID->setFlags(itemID->flags() & ~Qt::ItemIsEditable);
        itemID->setText(QString::number(id));
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_ID, itemID);
        //set sl no
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        itemDate->setText(transData->m_date.toString("d MMM yyyy"));
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_DATE, itemDate);

        //set receipt bookno
        QTableWidgetItem *itemBook = new QTableWidgetItem();
        itemBook->setFlags(itemBook->flags() & ~Qt::ItemIsEditable);
        itemBook->setText(receiptBook);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_RECEIPT_BOOK_NO, itemBook);
        //set receipt sl
        QTableWidgetItem *itemBookSl = new QTableWidgetItem();
        itemBookSl->setFlags(itemBookSl->flags() & ~Qt::ItemIsEditable);
        itemBookSl->setText(receiptSl);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_REEIPT_SL_NO, itemBookSl);

        //set min
        QTableWidgetItem *itemMin = new QTableWidgetItem();
        itemMin->setFlags(itemMin->flags() & ~Qt::ItemIsEditable);
        itemMin->setText(min);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_MIN, itemMin);

        QString accountName = CaccountMap::Object()->getAccountName(accountID);
        QTableWidgetItem *itemAccount = new QTableWidgetItem();
        itemAccount->setFlags(itemAccount->flags() & ~Qt::ItemIsEditable);
        itemAccount->setText(accountName);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_PIAKSAN, itemAccount);


        QTableWidgetItem *itemAmount = new QTableWidgetItem();
        itemAmount->setFlags(itemAmount->flags() & ~Qt::ItemIsEditable);
        itemAmount->setText(amountStr);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_AMOUNT, itemAmount);

        QString amountLocalStr = CsdUtils::converAmountToString(local,'f',2);
        QTableWidgetItem *itemAmountLocal = new QTableWidgetItem();
        itemAmountLocal->setFlags(itemAmountLocal->flags() & ~Qt::ItemIsEditable);
        itemAmountLocal->setText(amountLocalStr);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_LOCAL_SHARE, itemAmountLocal);


        QString amountHqStr = CsdUtils::converAmountToString(hq,'f',2);
        QTableWidgetItem *itemAmountHq = new QTableWidgetItem();
        itemAmountHq->setFlags(itemAmountHq->flags() & ~Qt::ItemIsEditable);
        itemAmountHq->setText(amountHqStr);
        ui->m_table->setItem(rowCount, REMIT_DLG_TABLE_HQ_SHARE, itemAmountHq);


        delete transData;

    }
    results.clear();
    m_constructor = constructor;
    return true;
 }

 void CremittanceDlg::calclateTotal()
 {
    m_deptRemittance.clear();
    m_accountRemittance.clear();
    m_allHq = true;
    m_remittanceAmount = 0;
    if (m_constructor) {
        return;
    }
    int rowCount = ui->m_table->rowCount();
    for (int row  = 0 ; row < rowCount; ++row) {
        QTableWidgetItem* idItem = ui->m_table->item(row, REMIT_DLG_TABLE_ID);
        QTableWidgetItem* receiptBookItem = ui->m_table->item(row, REMIT_DLG_TABLE_RECEIPT_BOOK_NO);
        QTableWidgetItem* receiptSlItem = ui->m_table->item(row, REMIT_DLG_TABLE_REEIPT_SL_NO);
        QTableWidgetItem* minItem = ui->m_table->item(row, REMIT_DLG_TABLE_MIN);
        QTableWidgetItem* piaksanItem = ui->m_table->item(row, REMIT_DLG_TABLE_PIAKSAN);
        QTableWidgetItem* amountItem = ui->m_table->item(row, REMIT_DLG_TABLE_AMOUNT );
        //QTableWidgetItem* idItem = ui->m_table->item(row, REMIT_DLG_TABLE_LOCAL_SHARE);
        //QTableWidgetItem* idItem = ui->m_table->item(row, REMIT_DLG_TABLE_HQ_SHARE);

        if (isEmptyOrNull(piaksanItem) || isEmptyOrNull(amountItem)) {
            continue;
        }
        QString accountName = piaksanItem->text().trimmed();
        int accountID = CaccountMap::Object()->getAccountID(accountName);
        int deptID = CaccountMap::Object()->getDeptForAccount(accountID);
        float hqPC = 0;
        float localPC = 100;
        std::map<int, std::pair<float, float> >::iterator fn = m_accountPct.find(accountID);
        if (fn != m_accountPct.end()) {
            hqPC = fn->second.first;
            localPC = fn->second.second;
        }

        double amount = amountItem->text().toDouble();
        m_remittanceAmount += amount;
        double local = (amount*localPC)/100;
        double hq = (amount*hqPC)/100;
        QString amountStr = CsdUtils::converAmountToString(amount,'f',2);

        if (localPC > 0) {
            m_allHq = false;
        }

        int id = 0;
        if (isEmptyOrNull(idItem) == false) {
            id = idItem->text().toInt();
        }
        QString min;
        if (isEmptyOrNull(minItem) == false) {
            min = minItem->text().trimmed();
        }
        QString receiptBook;
        if (isEmptyOrNull(receiptBookItem) == false) {
            receiptBook = receiptBookItem->text().trimmed();
        }
        QString receiptSl;
        if (isEmptyOrNull(receiptSlItem) == false) {
            receiptSl = receiptSlItem->text().trimmed();
        }

        if (receiptBook.isEmpty() || receiptSl.isEmpty()) {
            receiptBook = "";
            receiptSl = "";
        }
        int receiptBookNo = 0;
        int receiptSlNo = 0;
        if (receiptBook.isEmpty() == false && receiptSl.isEmpty() == false) {
            receiptBookNo = receiptBook.toInt();
            receiptSlNo = receiptSl.toInt();
        }
        if (localPC > 99) { // all local
            StransForRemittance & remits = m_deptRemittance[deptID];
            remits.m_localShare += local;
            remits.m_hqShare += hq;


            remits.m_allTrans.push_back(SremitTransDetail(id,
                                                          min,
                                                          amountStr,
                                                          receiptBook,
                                                          receiptSl,
                                                          accountID));
            if (receiptBookNo > 0 && receiptSlNo > 0) {
               std::set<int>& rno = remits.m_receipts[receiptBookNo];
               rno.insert(receiptSlNo);
            }

        } else {
            StransForRemittance& remits = m_accountRemittance[accountID];
            remits.m_localShare += local;
            remits.m_hqShare += hq;

            remits.m_allTrans.push_back(SremitTransDetail(id,
                                                          min,
                                                          amountStr,
                                                          receiptBook,
                                                          receiptSl,
                                                          accountID));

            if (receiptBookNo > 0 && receiptSlNo > 0) {
               std::set<int>& rno = remits.m_receipts[receiptBookNo];
               rno.insert(receiptSlNo);
            }

        }

    }

 }

 void CremittanceDlg::calculateTotalAndPopulateHtmlTable()
 {
    m_html = "";
    m_csvList.clear();
    m_deptRemittance.clear();
    m_accountRemittance.clear();
    m_allHq = true;
    if (m_constructor == false) {
        calclateTotal();
        populateHTMLTable();
    }

 }

 bool CremittanceDlg::isEmptyOrNull(QTableWidgetItem *item)
 {
    bool ret = false;
    if (item) {
         if (item->text().isEmpty()) {
            ret = true;
         }
    } else {
         ret = true;
    }
    return ret;
 }

 bool CremittanceDlg::checkInputValidity()
 {
    QString mode = ui->m_modeCombo->currentText();
    if (mode != "Cash") {
         QString ref = ui->m_referene->text();
         if (ref.isEmpty()) {
            QMessageBox::warning(this, "SaptuamData",
                                 "Reference field empty");
            return false;
         }
    }

    if (ui->m_toAddressBox->toPlainText().trimmed().isEmpty()) {
         QMessageBox::warning(this, "SaptuamData",
                              "To address field empty");
         return false;
    }

    if (ui->m_submitByBox->toPlainText().trimmed().isEmpty()) {
         QMessageBox::warning(this, "SaptuamData",
                              "Submitted by field empty");
         return false;
    }
    return true;
 }


void CremittanceDlg::resize()
{

    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    //int btnwidth = ui->m_pdfBtn->geometry().width();
    int btnheight = ui->m_pdfBtn->geometry().height();
    int x = XCORD;//ui->m_accountBox->geometry().x();
    int y = YCORD;//ui->m_accountBox->geometry().y();

    ui->m_remitCombolbl->setGeometry(x, y, DEFAULT_WIDTH*2, DEFAULT_HEIGHT);
    x += DEFAULT_WIDTH*2 + GAP;
    ui->m_remittanceNameCombo->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += DATE_SIZE.width() + GAP;
    ui->m_fromLbl->setGeometry(x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    x += DEFAULT_WIDTH + GAP;
    ui->m_periodFrom->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += DATE_SIZE.width() + GAP;
    ui->m_toLbl->setGeometry(x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    x += DEFAULT_WIDTH + GAP;
    ui->m_periodTo->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += DATE_SIZE.width() + GAP;
    ui->m_submitLbl->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += DATE_SIZE.width() + GAP;
    ui->m_submitDate->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);

    x = XCORD;
    y += DEFAULT_HEIGHT + GAP;
    int height = windowheight - y - YCORD - GAP;
    int width = DEPT_ACCOUNT_COMBO_SIZE.width()*0.6;
    ui->m_accountBox->setGeometry(x, y, width, height);


    int tableWidth = windowwidth - 2*XCORD - 2*width - 2*GAP;
    x += width + GAP;

    ui->m_table->setGeometry(x, y, tableWidth, height/2);
    ui->m_textBrowser->setGeometry(x , y + (height/2) + GAP  , tableWidth, (height/2));




   // ui->m_table->setGeometry(x, y, tableWidth, tableHeight);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_ID, 0);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_DATE, tableWidth*0.07);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_RECEIPT_BOOK_NO, tableWidth*0.10);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_REEIPT_SL_NO, tableWidth*0.10);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_MIN, tableWidth*0.20);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_PIAKSAN, tableWidth*0.20);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_AMOUNT, tableWidth*0.11);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_LOCAL_SHARE, tableWidth*0.11);
    ui->m_table->horizontalHeader()->resizeSection(REMIT_DLG_TABLE_HQ_SHARE, tableWidth*0.11);


    x += tableWidth + GAP;
    height = DEFAULT_WIDTH*1.5;
    ui->m_toAddressBox->setGeometry(x, y, width, height);
    y += height + GAP;
    ui->m_submitByBox->setGeometry(x, y, width, height);
    y += height + GAP;
    ui->m_modeLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    ui->m_modeCombo->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    ui->m_referene->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;
    ui->m_transDateLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;

    ui->m_transDate->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;

    ui->m_pdfBtn->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;

    ui->m_csvBtn->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += DEFAULT_HEIGHT + GAP;

    ui->m_cancelBtn->setGeometry(x, y, width, DEFAULT_HEIGHT);


}

void CremittanceDlg::disableEnable(bool enable)
{
    ui->m_accountBox->setEnabled(enable);
    //ui->m_remittanceNameCombo->setEnabled(enable);
   // ui->m_periodFrom->setEnabled(enable);
   // ui->m_periodTo->setEnabled(enable);
    ui->m_submitDate->setEnabled(enable);
    ui->m_toAddressBox->setEnabled(enable);
    ui->m_submitByBox->setEnabled(enable);
    ui->m_transDate->setEnabled(enable);
    ui->m_modeCombo->setEnabled(enable);
    ui->m_referene->setEnabled(enable);


}

void CremittanceDlg::setFieldBaseOnremittanceID()
{
    if (m_remittanceIndex > 0) {
         bool constructor = m_constructor;
         m_constructor = true;

         SremittanceData * data = CremittanceTable::Object()->getRemittanceData(m_remittanceIndex);
         if (data) {
            ui->m_periodFrom->setDate(data->m_fromDate);
            ui->m_periodTo->setDate(data->m_toDate);
            ui->m_toAddressBox->setPlainText(data->m_toAddress);
            ui->m_submitByBox->setPlainText(data->m_submitBy);
            ui->m_transDate->setDate(data->m_transactionDate);
            StransactionData* transData = CtransactionTable::Object()->getTransaction(data->m_transactionID);
            if (transData) {
               ui->m_submitDate->setDate(transData->m_date);
               ui->m_modeCombo->setCurrentText(CtransactionUtils::Object()->getTransactionModeStr((EtransactionMode)transData->m_mode));
               ui->m_referene->setText(transData->m_ref);
               delete transData;
            }

            delete data;
         }

         m_constructor = constructor;
    }
}

void CremittanceDlg::on_m_remittanceNameCombo_currentTextChanged(const QString &arg1)
{
    if (m_constructor == false) {
        m_remittanceIndex = 0;
    populateAccount();
    populateTable();
    }
}


void CremittanceDlg::on_m_periodFrom_dateChanged(const QDate &date)
{
    if (m_constructor == false) {
   m_remittanceIndex = 0;
   populateAccount();

    populateTable();
    }
}


void CremittanceDlg::on_m_periodTo_dateChanged(const QDate &date)
{
    if (m_constructor == false) {
    m_remittanceIndex = 0;
    populateAccount();

    populateTable();
    }
}


void CremittanceDlg::on_m_submitDate_dateChanged(const QDate &date)
{
   //populateTable();
}


void CremittanceDlg::on_m_modeCombo_currentTextChanged(const QString &arg1)
{
   //populateTable();
}


void CremittanceDlg::on_m_referene_editingFinished()
{
    //populateTable();
}


void CremittanceDlg::on_m_transDate_dateChanged(const QDate &date)
{
    //populateTable();
}

void CremittanceDlg::on_m_toAddressBox_textChanged()
{
    //populateTable();
}


void CremittanceDlg::on_m_submitByBox_textChanged()
{
    //populateTable();
}

void CremittanceDlg::accountCheckBoxClicked()
{
    populateTable();
}



void CremittanceDlg::resizeEvent(QResizeEvent *)
{
    resize();
}

void CremittanceDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CremittanceDlg::on_m_pdfBtn_clicked()
{
    if (checkInputValidity()) {
        calculateTotalAndPopulateHtmlTable();
         saveRemittance();

        CprintUtils print(m_html, false);
        print.printPDF();
        emit accept();
    }

}


void CremittanceDlg::on_m_csvBtn_clicked()
{
    if (checkInputValidity()) {
        calculateTotalAndPopulateHtmlTable();
        saveRemittance();

        CprintUtils print(m_csvList);
        print.printCSV();
        emit accept();
    }

}

void CremittanceDlg::deleteAllRow()
{
    m_selectRow = -1;
    int numRow = ui->m_table->rowCount();
    for (int i = numRow -1 ; i >= 0; --i) {
        ui->m_table->removeRow(i);
    }
    ui->m_textBrowser->setHtml("");
}

void CremittanceDlg::addRow()
{
    int numRow = ui->m_table->rowCount();
    if (numRow > 0) {
        ui->m_table->insertRow(numRow);
    }
}

void CremittanceDlg::deleteRow()
{
    if (m_selectRow > -1) {
        ui->m_table->removeRow(m_selectRow);
        calculateTotalAndPopulateHtmlTable();


    }
    m_selectRow = -1;
}


void CremittanceDlg::on_m_table_customContextMenuRequested(const QPoint &pos)
{
    m_selectRow = -1;
    if (m_remittanceIndex > 0) {
        return; // disable when view
    }
    int numRow = ui->m_table->rowCount();
    if (numRow > 0) {
        QMenu* menu = new QMenu(this);
        menu->addAction(m_addRowAction);

        QModelIndex index = ui->m_table->indexAt(pos);
        if (index.isValid()) {
            m_selectRow = index.row();
            menu->addAction(m_deleteRowAction);
        }
        menu->popup(ui->m_table->viewport()->mapToGlobal(pos));
    }

}


void CremittanceDlg::on_m_table_itemChanged(QTableWidgetItem *item)
{
    if (m_constructor) {
        return;
    }
    int column = item->column();
    if (column == REMIT_DLG_TABLE_AMOUNT) {
        calculateTotalAndPopulateHtmlTable();


    }
}

int CremittanceDlg::saveRemittance()
{

    if (m_remittanceIndex == 0) {
        //add the remittance to transaction
        QString name = ui->m_remittanceNameCombo->currentText().trimmed();
        StransactionData transData;
        transData.m_id = 0;
        int fromID = CcontactMap::Object()->getContactID(name);
        transData.m_fromId = fromID;
        transData.m_amount = m_remittanceAmount;
        transData.m_accountId = CaccountMap::Object()->getAccountID(gRemittanceAccountName);
        transData.m_date = ui->m_submitDate->date();
        transData.m_type = PAYMENT_TRANSACTION_TYPE;
        transData.m_mode = CtransactionUtils::Object()->getTransactionMode(ui->m_modeCombo->currentText().trimmed());
        transData.m_ref = ui->m_referene->text().trimmed();
        transData.m_remittanceID = 0;
        transData.m_parentID = 0;
        transData.m_closingID = 0;
        transData.m_bankID = 0;
        transData.m_status = true;
        transData.m_reeiptBook = "";
        transData.m_receiptSlNo = "";
        transData.m_particular = "Remittance " + name + " for  " +
                                 ui->m_periodFrom->date().toString("d MMM yyyy") + " - " +
                                 ui->m_periodTo->date().toString("d MMM yyyy");
        int remitTransID = transData.save();

        SremittanceNameData * nameData = CremittanceNameTable::Object()->getRemittanceNameData(name);
        //add remittance detail
        SremittanceData remitData;
        remitData.m_id = 0;
        remitData.m_transactionID = remitTransID;
        remitData.m_remitNameTableID = nameData ? nameData->m_id : 0;
        remitData.m_fromDate = ui->m_periodFrom->date();
        remitData.m_toDate = ui->m_periodTo->date();
        remitData.m_transactionDate = ui->m_transDate->date();
        remitData.m_toAddress = ui->m_toAddressBox->toPlainText();
        remitData.m_submitBy = ui->m_submitByBox->toPlainText();
        m_remittanceIndex = remitData.save();
        delete nameData;
        // update transaction with remmitance table id
        auto begin =     m_deptRemittance.begin();
        auto end = m_deptRemittance.end();
        while (begin != end) {
            StransForRemittance& transRemit = begin->second;
            for (auto alltransBegin = transRemit.m_allTrans.begin();
                 alltransBegin != transRemit.m_allTrans.end(); ++alltransBegin) {
               if (alltransBegin->m_transID > 0) {
                    CtransactionTable::Object()->updateValue(m_remittanceIndex, TRANSACTION_REMITTANCE_ID_IDX , alltransBegin->m_transID, TRANSACTION_ID_IDX);
               } else {
                    QString str = alltransBegin->toString();
                    SremittanceReconcileData reconcileData;
                    reconcileData.m_id = 0;
                    reconcileData.m_accontOrDeptTableID = begin->first;
                    reconcileData.m_remitanceTableID = m_remittanceIndex;
                    reconcileData.m_isAccount = false;
                    reconcileData.m_str = str;
                    reconcileData.save();
               }
            }
            ++begin;
        }

        begin = m_accountRemittance.begin();
        end = m_accountRemittance.end();
        std::map<int, std::pair<float, float> > accountIDPC;;
        while (begin != end) {

            float hqPC = 0;
            float localPC = 100;
            std::map<int, std::pair<float, float> >::iterator fn = m_accountPct.find(begin->first);
            if (fn != m_accountPct.end()) {
               hqPC = fn->second.first;
               localPC = fn->second.second;
            }
            accountIDPC.insert({begin->first, {hqPC, localPC}});
            StransForRemittance& transRemit = begin->second;
            for (auto alltransBegin = transRemit.m_allTrans.begin();
                alltransBegin != transRemit.m_allTrans.end(); ++alltransBegin) {
               if (alltransBegin->m_transID > 0) {
                    CtransactionTable::Object()->updateValue(m_remittanceIndex, TRANSACTION_REMITTANCE_ID_IDX , alltransBegin->m_transID, TRANSACTION_ID_IDX);
               } else {
                    QString str = alltransBegin->toString();
                    SremittanceReconcileData reconcileData;
                    reconcileData.m_id = 0;
                    reconcileData.m_accontOrDeptTableID = begin->first;
                    reconcileData.m_remitanceTableID = m_remittanceIndex;
                    reconcileData.m_isAccount = true;
                    reconcileData.m_str = str;
                    reconcileData.save();
               }
            }
            ++begin;
        }
        //upate the percentages
        for(auto accountPCT : accountIDPC) {
            SremittanceDetailData detailData;
            detailData.m_accuntTableID =   accountPCT.first;
            detailData.m_id = 0;
            detailData.m_remitanceTableID = m_remittanceIndex;
            detailData.m_hqShare = accountPCT.second.first;
            detailData.m_localShare = accountPCT.second.second;
            detailData.save();
        }

    }

    return m_remittanceIndex;
}





