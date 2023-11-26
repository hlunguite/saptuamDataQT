#include "mainwindow.h"
//#include "qdesktopwidget.h"
#include "ui_mainwindow.h"
#include "csdutils.h"
#include "csaptuamdatadbmgr.h"
#include <QSettings>
#include "cnewtransactiondlg.h"
#include "cassignreceiptdlg.h"
#include "CsdDefine.h"
#include "creceiptsetupdlg.h"
#include "MISC/creadbankstatement.h"
#include "cremittancesetupdlg.h"
#include "cremittancedlg.h"
#include "cledgerreportdlg.h"
#include "csummaryreportdlg.h"
#include "csettingtable.h"
#include "csettingdlg.h"
#include "cpassworddlg.h"
#include "cimportmindlg.h"
#include "csmsdlg.h"
#include "cimportaccountdlg.h"
#include "cimporttransdlg.h"
#include <QFileDialog>
#include "cdepartmenttable.h"
#include "cpaymentaccounttable.h"
#include "caccounttable.h"
#include "ccontacttable.h"
#include "ctransactionutils.h"
#include "ctransactiontable.h"
#include "cremittancesetuptable.h"
#include "cremittancetable.h"
#include "cclosingbalancedlg.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    /*ui(new Ui::MainWindow),*/ m_saptuamDB(nullptr),
    m_mainTab(nullptr)
{
    //ui->setupUi(this);
    m_isError = false;
    setWindowTitle("Saptuam Data");
    setWindowIcon(QIcon(":icons/images/Saptuam Data.ico"));

    setMinimumSize(160, 160);

    QSize availableSize = qApp->primaryScreen()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    //add default locale
    QLocale locale(QLocale::English ,QLocale::India);
    QLocale::setDefault(locale);

    width *= 0.98; // 98% of the screen size
    height *= 0.95; // 99% of the screen size
    QSize newSize( width, height );
    CsdUtils::setMaxHeight(height);
    CsdUtils::setMaxWidth( width);
   // qDebug()<<"width "<<width<<" "<<height;
    setGeometry(QStyle::alignedRect( Qt::LeftToRight,
                Qt::AlignCenter,
                newSize,
                qApp->primaryScreen()->availableGeometry() ));
    createAction();
    createMenu();
    m_saptuamDB = new CsaptuamDataDBMgr(CsdUtils::getSaptuamMin());

    readSettings();
    if (m_isError == false) {
    setupTab();

        m_transactionDock = new QDockWidget("Transaction", this);
        m_transactionDock->hide();
        m_transactionDock->setFeatures(QDockWidget::DockWidgetClosable);
        m_transactionDock->setFeatures(QDockWidget::DockWidgetMovable);
        m_transactionDock->setFeatures(QDockWidget::DockWidgetFloatable);

        connect(m_mainTab, SIGNAL(signalFromMainTab(int)), this, SLOT(processSignals(int)));
    }
}

MainWindow::~MainWindow()
{
    //delete ui;
    if (m_saptuamDB) {
        delete m_saptuamDB;
    }
    delete m_mainTab;
    delete m_transactionDock;

}

bool MainWindow::getIsError()
{
    return m_isError;
}

void MainWindow::processSignals(int val)
{
    switch(val) {
    case NEW_TRANSACTION_SIGNAL: newTransaction(); break;
    case ASSIGN_RECEIPT_SIGNAK: assignReceipt();break;
    case IMPORT_BANK_STATEMENT: importBankStatement(); break;
    case REMITTANCE_SIGNAL : addRemittance(); break;
    case PROCESS_BANK_STATEMENT : processBankStatement(); break;
    case SUMMARY_SIGNAL: summaryReport(); break;
    case SEND_SMS_SIGNAL: sendSMS(); break;
    default: break;
    }
}

void MainWindow::newAccountSlot()
{
    m_mainTab->newAccount();
}

void MainWindow::newDeptSlot()
{
    m_mainTab->newDept();
}

void MainWindow::receiptSetup()
{
    QDialog* dlg = new CreceiptSetupDlg;
    addDlgToDock(dlg, "Setup Receipt");
    delete dlg;
}

void MainWindow::newTransaction()
{
    QDialog* dlg = new CnewTransactionDlg;
    addDlgToDock(dlg, "New Transaction");
    delete dlg;
}

void MainWindow::assignReceipt()
{
    QDialog* dlg = new CassignReceiptDlg;
    addDlgToDock(dlg, "Assign Receipt");
    delete dlg;
}

void MainWindow::importBankStatement()
{
    CreadBankStatement bs;
    if (bs.selectFile()) {

        int importID = bs.read();

        QDialog* dlg = new CnewTransactionDlg(true, importID);
        addDlgToDock(dlg, "Process Bank Transaction");
        delete dlg;
    }


}

void MainWindow::processBankStatement()
{
    QDialog* dlg = new CnewTransactionDlg(true, 0);
    addDlgToDock(dlg, "Process Bank Transaction");
    delete dlg;
}

void MainWindow::addRemittance()
{
    QDialog* dlg = new CremittanceDlg();
    addDlgToDock(dlg, "Remittance");
    delete dlg;
}
/*
void MainWindow::remittancePCSetup()
{
    CremittancePercentSetup dlg;
    dlg.setWindowTitle("Percentage Setup");
    dlg.exec();
}
*/
void MainWindow::remittanceSetup()
{
    QDialog* dlg = new CremittanceSetupDlg();
    addDlgToDock(dlg, "Remittance Setup");
    delete dlg;
}

void MainWindow::ledgerReport()
{
    QDialog* dlg = new CledgerReportDlg();
    addDlgToDock(dlg, "Ledger");
    delete dlg;
}

void MainWindow::summaryReport()
{
    QDialog* dlg = new CsummaryReportDlg();
    addDlgToDock(dlg, "Summary Report");
    delete dlg;
}

void MainWindow::sendSMS()
{
    CsmsDlg* dlg = new CsmsDlg();
    addDlgToDock(dlg, "Send SMS");
    delete dlg;
}

void MainWindow::setting()
{
    CsettingDlg dlg;
    dlg.setWindowTitle("Setting");
    dlg.exec();
    readSettings();

}

void MainWindow::minImport()
{
    CimportMinDlg* dlg  = new CimportMinDlg();
    connect(dlg, SIGNAL(updateContactTable()), m_mainTab, SLOT(updateDirectory()));
    addDlgToDock(dlg, "Import Min");
    delete dlg;
}

void MainWindow::accountImport()
{
    CimportAccountDlg* dlg = new CimportAccountDlg();
    connect(dlg, SIGNAL(updateAccountTab()), m_mainTab, SLOT(updateAccountTab()));
    addDlgToDock(dlg, "Import Account");
    delete dlg;
}

void MainWindow::transImport()
{
    CimportTransDlg* dlg = new CimportTransDlg();
    connect(dlg, SIGNAL(updateTransaction()), m_mainTab, SLOT(refreshTabs()));
    addDlgToDock(dlg, "Import Trans");
    delete dlg;
}

void MainWindow::saveAll()
{
    QString csv = CsdUtils::makedir("csv");
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    "Save SaptuamData", csv,
                                                    "SaptuamData files (*.dat)");
    if (fileName.isEmpty()) {
        return ;
    }
    QFile outFile(fileName);
    if(outFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&outFile);
        //payment account
        CpaymentAccountTable::Object()->serialize(out);
        //department
        CdepartmentTable::Object()->serialize(out);
        //account
        CaccountUtils::Object()->serialize(out);
        CaccountTable::Object()->serialize(out);
        //min
        CcontactTable::Object()->serialize(out);
        //transaction
        CtransactionUtils::Object()->serialize(out);
        CtransactionTable::Object()->serialize(out);
        //remittance setup
        CremittanceNameTable::Object()->serialize(out);
        CremittancePercentTable::Object()->serialize(out);
        CremittanceSetupTable::Object()->serialize(out);
        //remittancetable
        CremittanceTable::Object()->serialize(out);
        CremittanceDetailTable::Object()->serialize(out);
        CremittanceReconcileTable::Object()->serialize(out);
        outFile.close();
    }


   /* QFile inFile(fileName);
    if(inFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&inFile);
        //payment account
        CpaymentAccountTable::Object()->desserialize(in);
        //department
        CdepartmentTable::Object()->desserialize(in);
        //account
        CaccountUtils::Object()->desserialize(in);
        CaccountTable::Object()->desserialize(in);
        //min
        CcontactTable::Object()->desserialize(in);
        //transaction
        CtransactionUtils::Object()->desserialize(in);
        CtransactionTable::Object()->desserialize(in);
        //remittance setup
        CremittanceNameTable::Object()->desserialize(in);
        CremittancePercentTable::Object()->desserialize(in);
        CremittanceSetupTable::Object()->desserialize(in);
        //remittancetable
        CremittanceTable::Object()->desserialize(in);
        CremittanceDetailTable::Object()->desserialize(in);
        CremittanceReconcileTable::Object()->desserialize(in);
        inFile.close();
    }*/


}

void MainWindow::closing()
{
    CclosingBalanceDlg * dlg = new CclosingBalanceDlg();
    //connect(dlg, SIGNAL(updateTransaction()), m_mainTab, SLOT(refreshTabs()));
    addDlgToDock(dlg, "Closing Balance");
    delete dlg;
}


void MainWindow::createMenu()
{
    //qDebug()<<"Create menu";
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_minImport);
    m_fileMenu->addAction(m_accountImport);
    m_fileMenu->addAction(m_transImport);
    m_actionMenu = menuBar()->addMenu(tr("Action"));
    m_receiptMenu = m_actionMenu->addMenu(tr("Receipt"));
    m_receiptMenu->addAction(m_receiptSetup);
    m_receiptMenu->addAction(m_assignReceiptToTransaction);

    m_transactionMenu = m_actionMenu->addMenu("Transaction");
    m_transactionMenu->addAction(m_newTransaction);
    m_transactionMenu->addAction(m_processBankImport);
    m_transactionMenu->addAction(m_newBankImport);

   // m_actionMenu->addAction(m_remittance);
    m_remittanceMenu = m_actionMenu->addMenu("Remittance");
    m_remittanceMenu->addAction(m_remittance);
    //m_remittanceMenu->addAction(m_remitPercentSetup);
    m_remittanceMenu->addAction(m_remittanceSetup);

    m_actionMenu->addAction(m_newAccount);
    m_actionMenu->addAction(m_newDept);
    m_actionMenu->addAction(m_saveAll);

    m_reportMenu = menuBar()->addMenu(tr("Report"));
    m_reportMenu->addAction(m_ledgerReport);
    //m_reportMenu->addAction(m_statementReport);
    m_reportMenu->addAction(m_summaryReport);
    m_reportMenu->addAction(m_closingReport);
    m_reportMenu->addAction(m_sendSms);


    m_settingsMenu = menuBar()->addMenu(tr("Settings"));
    //m_settingsMenu->addAction(m_fySetting);
    m_settingsMenu->addAction(m_passwordSetting);

    m_help = menuBar()->addMenu(tr("Help"));

}

void MainWindow::enableMenu(bool flag)
{
    //qDebug()<<"Enable menu "<<flag;
    m_fileMenu->setEnabled(flag);
    m_actionMenu->setEnabled(flag);
    m_reportMenu->setEnabled(flag);
    m_settingsMenu->setEnabled(flag);
    m_help->setEnabled(flag);
}

void MainWindow::setupTab()
{
    m_mainTab = new CmainTab();
    QWidget * widget = centralWidget();
    if(widget)widget->deleteLater();
    int windowWidth = this->width();

     m_mainTab->setTabWidth(windowWidth);
     setCentralWidget(m_mainTab);

}

void MainWindow::addDlgToDock(QDialog* dlg, QString title)
{
    int height = geometry().height();
    int minheight = (height*.7);

    m_transactionDock->setWidget(dlg);
    m_transactionDock->show();
    m_transactionDock->setMinimumHeight(minheight);
    //qDebug()<<"dock min height "<<minheight;

    m_transactionDock->setWindowTitle(title);
    addDockWidget(Qt::BottomDockWidgetArea, m_transactionDock);

    if(dlg->exec()){
        m_mainTab->refreshTabs();
     }
    m_transactionDock->hide();
}



void MainWindow::createAction()
{
    m_newAccount = new QAction("New Account", this);
    m_newDept = new QAction("New Department",this);
    m_newTransaction = new QAction("New Transaction", this);
    m_newBankImport = new QAction("Import Bank Statement", this);
    m_processBankImport = new QAction("Process Bank Statement", this);
    m_saveAll = new QAction("Save All", this);
    m_receiptSetup = new QAction("Receipt Setup", this);
    m_assignReceiptToTransaction = new QAction("Assign Receipt to Transaction", this);
    m_sendSms = new QAction("Send SMS", this);
   // m_remitPercentSetup = new QAction("Remittance Percentage Setup", this);
   m_remittanceSetup = new QAction("Remittance Setup", this);

    m_ledgerReport = new QAction("Ledger", this);
    //m_statementReport = new QAction("Statement Report", this);
    m_summaryReport = new QAction("Summary Report", this);
    m_closingReport = new QAction("Closing Balance", this);

    m_fySetting = new QAction("Financial Year", this);
    m_passwordSetting = new QAction("Edit Setting", this);

    m_remittance = new QAction("Add Remittance", this);

    m_minImport = new QAction("Import Min", this);
    m_accountImport = new QAction("Import Account", this);
    m_transImport = new QAction("Import Trans" , this);


    connect(m_newAccount, SIGNAL(triggered()), this, SLOT(newAccountSlot()));
    connect(m_newDept, SIGNAL(triggered()), this, SLOT(newDeptSlot()));
    connect(m_receiptSetup, SIGNAL(triggered()), this, SLOT(receiptSetup()));
    connect(m_assignReceiptToTransaction, SIGNAL(triggered()), this, SLOT(assignReceipt()));
    connect (m_newBankImport, SIGNAL(triggered()), this, SLOT(importBankStatement()));
    connect(m_processBankImport, SIGNAL(triggered()), this, SLOT(processBankStatement()));
    connect(m_newTransaction, SIGNAL(triggered()), this, SLOT(newTransaction()));
    //connect(m_remitPercentSetup, SIGNAL(triggered()), this, SLOT(remittancePCSetup()));
    connect(m_remittanceSetup, SIGNAL(triggered()), this, SLOT(remittanceSetup()));
    connect(m_remittance, SIGNAL(triggered()), this, SLOT(addRemittance()));
    connect(m_ledgerReport, SIGNAL(triggered()), this, SLOT(ledgerReport()));
    connect(m_summaryReport, SIGNAL(triggered()), this, SLOT(summaryReport()));
    connect(m_passwordSetting, SIGNAL(triggered()), this, SLOT(setting()));
    connect(m_minImport, SIGNAL(triggered()), this, SLOT(minImport()));
    connect(m_accountImport, SIGNAL(triggered()), this, SLOT(accountImport()) );
    connect(m_transImport, SIGNAL(triggered()), this, SLOT(transImport()));
    connect(m_sendSms, SIGNAL(triggered()), this, SLOT(sendSMS()));
    connect(m_saveAll, SIGNAL(triggered()), this, SLOT(saveAll()));
    connect(m_closingReport, SIGNAL(triggered()), this, SLOT(closing()));
}


void MainWindow::closeEvent(QCloseEvent * event)
{
    if(okToContinue()){
        //writeSettings();
        event->accept();
    }else {
        event->ignore();
    }
}

bool MainWindow::okToContinue()
{
    return true;
}

void MainWindow::readSettings()
{
    m_isError = false;
    bool good = true;
    SsettingData* setting = CsettingTable::Object()->getSetting();
    if (setting) {
        good = false;
        CpasswordDlg dlg;
        dlg.setWindowTitle("Enter password");
        if (dlg.exec()) {
            good = dlg.isPasswordCorrect();
        }
    } else {
        CsettingDlg dlg;
        dlg.setWindowTitle("Setting");
        if (dlg.exec()) {
            good = true;
            setting = CsettingTable::Object()->getSetting();
        } else {
            good = false;
        }
    }

    if (good == false) {
        m_isError = true;
        if (setting){
            delete setting;
        }
        return;
    }
    //QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString currentDirectory =  setting->m_cwd;
    QString orgination = setting->m_orgination;
    QString location = setting->m_location;
    int fontsize1 = setting->m_fontSize;
    if(fontsize1 == 0) fontsize1 = 9;
    QString saptuamMin = setting->m_saptuamMin;
    QDate sdate = setting->m_fyStart;
    QDate edate = setting->m_fyEnd;
    if(sdate.isValid() && edate.isValid()){
        CsdUtils::setFyStartDate(sdate);
        CsdUtils::setFyEndDate(edate);
    }
    delete setting;

    CsdUtils::setFontSize(fontsize1);

    if (currentDirectory.isEmpty() || saptuamMin.isEmpty()) {
        currentDirectory = currentDirectory.isEmpty() ?  "." : currentDirectory;

        orgination = orgination.isEmpty() ? "EVANGELICAL BAPTIST CONVENTION CHURCH" : orgination;
        location = location.isEmpty() ?  "DELHI" : location;
        saptuamMin = saptuamMin.isEmpty() ? "CENTRAL DELHI" : saptuamMin;

    }else if(orgination.isEmpty()){
        currentDirectory = currentDirectory.isEmpty() ?  "." : currentDirectory;
        orgination = "EVANGELICAL BAPTIST CONVENTION CHURCH";
        location = location.isEmpty()?  "DELHI" : location;
        saptuamMin = saptuamMin.isEmpty() ? "CENTRAL DELHI" : saptuamMin;
    }

    CsdUtils::setCurrentWdir(currentDirectory);
    CsdUtils::setOrgination(orgination);
    CsdUtils::setLocation(location);
    CsdUtils::setSaptuamMin(saptuamMin);
    QString shortName = CsdUtils::abbravate(orgination) + " "  + CsdUtils::abbravate(saptuamMin);
    CsdUtils::setSaptuamShortName(shortName);



    enableMenu(true);


}

void MainWindow::writeSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("currentDirectory", CsdUtils::getCurrentWdir());
    settings.setValue("orgination", CsdUtils::getOrgination());
    settings.setValue("location", CsdUtils::ucfirst(CsdUtils::getLocaton()) );
    settings.setValue("fontSize", CsdUtils::getFontSize());
    settings.setValue("saptuammin", CsdUtils::ucfirst(CsdUtils::getSaptuamMin()));
    QDate start = CsdUtils::getFyStartDate() ;
    QDate endDate = CsdUtils::getFyEndDate() ;
    settings.setValue("fstart",start.toString("yyyy-MM-dd"));
    settings.setValue("fend",endDate.toString("yyyy-MM-dd"));

}

