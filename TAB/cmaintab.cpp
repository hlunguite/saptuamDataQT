#include "cmaintab.h"
#include "caccounttab.h"
#include "ccontacttab.h"
#include "ctransactiontab.h"
#include "cpaymentaccounttab.h"
CmainTab::CmainTab(QTabWidget * parent):
    QTabWidget(parent)
{
    m_accountTab = new CaccountTab();
    addTab(m_accountTab, tr("Account"));
    m_transactionTab = new CtransactionTab();
    addTab(m_transactionTab, tr("Transaction"));
    m_directoryTab = new CcontactTab();
    addTab(m_directoryTab, tr("Contact"));
    connect(m_transactionTab, SIGNAL(signalFromTransactionTab(int)), this, SLOT(processSignal(int)));
    m_paymentAccountTab = new CpaymentAccountTab();
    addTab(m_paymentAccountTab, tr("Payment Account"));
}

CmainTab::~CmainTab()
{
    delete m_accountTab;
    delete m_directoryTab;
    delete m_transactionTab;
    delete m_paymentAccountTab;
}

void CmainTab::setTabWidth(int width)
{

        m_accountTab->setAccountTabWidth(width);

}

void CmainTab::refreshTabs()
{
    if (m_transactionTab) {
        m_transactionTab->refreshTable();
    }
    updateDirectory();
}

void CmainTab::newAccount()
{
    m_accountTab->createNewAccount();
}

void CmainTab::newDept()
{
    m_accountTab->createNewDept();
}

void CmainTab::updateDirectory()
{

    m_directoryTab->refreshTableAddress();
}

void CmainTab::updateAccountTab()
{
    m_accountTab->refreshAccounts();
}

void CmainTab::processSignal(int val)
{
    emit signalFromMainTab(val);
}


