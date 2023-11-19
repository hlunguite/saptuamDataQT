#include "cremittancesetupdlg.h"
#include "ui_cremittancesetupdlg.h"
#include <QRadioButton>
#include <QDebug>
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

    int x = ui->m_accountTable->geometry().x();
    int y = ui->m_accountTable->geometry().y();
    int btnWidth = ui->m_okBtn->geometry().width();
    int btnheight = ui->m_okBtn->geometry().height();
    //qDebug()<<"btnwidth "<<btnWidth<<" height "<<btnheight;
    int width = windowwidth - 2*x;
    int height = windowheight  - y - (2*btnheight);

    ui->m_accountTable->setGeometry(x, y, width, height);
    ui->m_accountTable->horizontalHeader()->resizeSection(0, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(1, width*.055);
    ui->m_accountTable->horizontalHeader()->resizeSection(2, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(3, width*0.055);
    ui->m_accountTable->horizontalHeader()->resizeSection(4, width*.28);
    ui->m_accountTable->horizontalHeader()->resizeSection(5, width*0.055);


    y = windowheight - (btnheight*1.5);
    x = windowwidth/2 - btnWidth;
    //ui->gridLayout->setGeometry(QRect(x, y, 2*btnWidth, btnheight));
    ui->m_cancelBtn->setGeometry(x, y, btnWidth, btnheight);
    x += btnWidth + 1;
    ui->m_okBtn->setGeometry(x, y, btnWidth, btnheight);

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

}

void CremittanceSetupDlg::populateRemittance(QString remitName)
{

}

void CremittanceSetupDlg::on_m_remitNameLineEdit_editingFinished()
{

    populateTable();
}


void CremittanceSetupDlg::on_m_remitNameList_currentTextChanged(const QString &arg1)
{
   populateTable();
}

