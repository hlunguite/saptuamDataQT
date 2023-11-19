#include "ccontacttab.h"
#include "caddresstab.h"
CcontactTab::CcontactTab(QWidget *parent) : QWidget(parent)
{
    m_toolBar = new QToolBar("Directory Toolbar");

    m_addContact = new QAction("New Contact", this);
    m_addContact->setToolTip("New Contact");
    m_toolBar->addAction(m_addContact);
    m_toolBar->addSeparator();
    m_editAction = new QAction("Edit Contact", this);
    m_editAction->setToolTip("Edit Selected Contact");
    m_toolBar->addAction(m_editAction);
    m_toolBar->addSeparator();

    m_searchLabel = new QLabel("Search");
    m_toolBar->addWidget(m_searchLabel);

    m_searchLine = new QLineEdit();
    m_searchLine->setFixedWidth(200);
    m_searchLine->setMinimumWidth(120);
    m_toolBar->addWidget(m_searchLine);

    m_searchIn = new QLabel(" in ");
    m_toolBar->addWidget(m_searchIn);

    m_searchCombo = new QComboBox();
    m_searchCombo->setInsertPolicy(QComboBox::InsertAtTop);
    m_searchCombo->setDuplicatesEnabled(false);
    m_searchCombo->addItem("");
    QStringList list;
    list <<"Min"<<"Phone No";
    m_searchCombo->addItems(list);
    m_toolBar->addWidget(m_searchCombo);



    m_vboxLayout = new QVBoxLayout();
    m_vboxLayout->addWidget(m_toolBar);
    m_addressTab = new CaddressTab;
    m_vboxLayout->addWidget(m_addressTab);
    setLayout(m_vboxLayout);

    connect(m_addContact, SIGNAL(triggered()), m_addressTab, SLOT(addContact()));
    connect(m_editAction, SIGNAL(triggered()), m_addressTab, SLOT(editSelectMember()));
    connect(m_searchLine, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));
    connect(m_searchCombo, SIGNAL(currentTextChanged(QString)), SLOT(search(QString)));

}

CcontactTab::~CcontactTab()
{
    delete m_searchLine;
    delete m_searchCombo;
    delete m_searchIn;
    delete m_searchLabel;
    delete m_addContact;
    delete m_editAction;
    delete m_toolBar;
    delete m_vboxLayout;
    delete m_addressTab;
}

void CcontactTab::search(QString txt)
{
 Q_UNUSED(txt)
    search();
}

void CcontactTab::refreshTableAddress()
{
    m_addressTab->refreshTable();
}


void CcontactTab::search()
{
    QString searchtext = m_searchLine->text();
    QString searchin = m_searchCombo->currentText();
    m_addressTab->search(searchtext, searchin);
}


