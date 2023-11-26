#include "cremittancesetupdlg.h"
#include "ui_cremittancesetupdlg.h"
#include <QRadioButton>
#include <QDebug>
#include "MISC/CdlgDefine.h"
CremittanceSetupDlg::CremittanceSetupDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CremittanceSetupDlg)
{
    ui->setupUi(this);
    resize();
}

CremittanceSetupDlg::~CremittanceSetupDlg()
{
    delete ui;
}


void CremittanceSetupDlg::resizeEvent(QResizeEvent *)
{
    resize();
}

void CremittanceSetupDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = XCORD;//ui->m_accountTable->geometry().x();
    int y = YCORD;ui->m_accountTable->geometry().y();
    int btnWidth = BUTTON_SIZE.width();
    int btnheight = BUTTON_SIZE.height();

    ui->m_remitNameLbl->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP +  DATE_SIZE.width();
    ui->m_orLbl->setGeometry(x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    x += GAP + DEFAULT_WIDTH;
    int width = 2*DATE_SIZE.width();
    ui->m_selectremitnalelbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x = XCORD;
    y = YCORD + GAP + DEFAULT_HEIGHT;
    ui->m_remitNameLineEdit->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP +  DATE_SIZE.width() + GAP + DEFAULT_WIDTH;
    ui->m_remitNameList->setGeometry(x, y,  DATE_SIZE.width(), DEFAULT_HEIGHT);
    x = XCORD;
    y += DEFAULT_HEIGHT + GAP;
    ui->m_selectAccountLbl->setGeometry(x, y, DATE_SIZE.width()*2, DEFAULT_HEIGHT);

    y += GAP + DEFAULT_HEIGHT;
    width = windowwidth - 2*XCORD;
    int height = windowheight  - y - GAP - btnheight - YCORD;

    ui->m_accountTable->setGeometry(x, y, width, height);
    ui->m_accountTable->horizontalHeader()->resizeSection(0, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(1, width*.055);
    ui->m_accountTable->horizontalHeader()->resizeSection(2, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(3, width*0.055);
    ui->m_accountTable->horizontalHeader()->resizeSection(4, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(5, width*0.055);


    y += height + GAP ;
    x = windowwidth/2 - btnWidth - GAP/2;
    ui->m_okBtn->setGeometry(x, y, btnWidth, btnheight);
    x += GAP + btnWidth;
    //ui->gridLayout->setGeometry(QRect(x, y, 2*btnWidth, btnheight));
    ui->m_cancelBtn->setGeometry(x, y, btnWidth, btnheight);


}

void CremittanceSetupDlg::populateTable()
{
    ui->m_accountTable->clearContents();
    ui->m_accountTable->setRowCount(0);
    QString remitName = ui->m_remitNameList->currentText();
    QString newName = ui->m_remitNameLineEdit->text().simplified();
    if (newName.isEmpty() == false || remitName.isEmpty()) {
        populateAllAccount();
    } else if (remitName.isEmpty() == false){
        populateRemittance(remitName);
    }

}

void CremittanceSetupDlg::populateAllAccount()
{
    //TODO
    //TODO also dlg for payment account
}

void CremittanceSetupDlg::populateRemittance(QString remitName)
{
    //TODO
}

void CremittanceSetupDlg::on_m_remitNameLineEdit_editingFinished()
{

    populateTable();
}


void CremittanceSetupDlg::on_m_remitNameList_currentTextChanged(const QString &arg1)
{
   populateTable();
}


void CremittanceSetupDlg::on_m_cancelBtn_clicked()
{
   emit reject();
}

