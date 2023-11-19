#include "cclosingbalancedlg.h"
#include "ui_cclosingbalancedlg.h"
#include "MISC/cclosingcalculator.h"

CclosingBalanceDlg::CclosingBalanceDlg(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CclosingBalanceDlg)
{
    ui->setupUi(this);
    QDate date = QDate::currentDate();
    ui->m_closingDate->setDate(date);
}

CclosingBalanceDlg::~CclosingBalanceDlg()
{
    delete ui;
}


void CclosingBalanceDlg::on_m_saveCloseBtn_clicked()
{
    emit accept();
}


void CclosingBalanceDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}

void CclosingBalanceDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = ui->m_table->geometry().x();
    int y = ui->m_table->geometry().y();
    int buttonheight = ui->m_saveCloseBtn->geometry().height();
    int buttonwidth = ui->m_saveCloseBtn->geometry().width();

    int width = windowwidth - (2*x);
    int height = windowheight - y - (2*buttonheight);
    ui->m_table->setGeometry(x, y, width, height);
    ui->m_table->horizontalHeader()->resizeSection(0,width*0.5);
    ui->m_table->horizontalHeader()->resizeSection(1,width*0.5);
    x = windowwidth/2 - buttonwidth;
    y = y + height + (0.5*buttonheight);
    ui->m_saveCloseBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x += buttonwidth;
    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);

}

void CclosingBalanceDlg::populateTable(QDate date)
{
    CclosingCalculator calc(date);
}


void CclosingBalanceDlg::on_m_closingDate_dateChanged(const QDate &date)
{
    populateTable(date);
}

