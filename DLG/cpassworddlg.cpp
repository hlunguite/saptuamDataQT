#include "cpassworddlg.h"
#include "ui_cpassworddlg.h"
#include "csettingtable.h"
#include <QMessageBox>

CpasswordDlg::CpasswordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CpasswordDlg)
{
    ui->setupUi(this);
    m_isPasswordCorrect = false;
}

CpasswordDlg::~CpasswordDlg()
{
    delete ui;
}

bool CpasswordDlg::isPasswordCorrect() const
{
    return m_isPasswordCorrect;
}

void CpasswordDlg::on_m_password_textChanged(const QString &arg1)
{
    m_isPasswordCorrect = false;
    SsettingData* setting = CsettingTable::Object()->getSetting();
    if (setting) {
        QString password = setting->m_password;
        delete setting;
        if (arg1.isEmpty()) {
            ui->m_msg->setText("Empty password");
        } else if (arg1.size() < 8) {
            ui->m_msg->setText("Password should be 8 character or more");
        } else {
            if (arg1 == password) {
                m_isPasswordCorrect = true;
                emit accept();
            }
        }
    } else {
        /*QMessageBox::warning(this, "SaptuamData",
                             "Password is not set");*/
        ui->m_msg->setText("Password not set");
    }
}

