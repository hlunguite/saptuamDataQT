#include "csettingdlg.h"
#include "ui_csettingdlg.h"
#include "csettingtable.h"
#include <QMessageBox>
#include <QFileDialog>
CsettingDlg::CsettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CsettingDlg)
{
    ui->setupUi(this);
    m_isNew = true;
    ui->m_org->setText("Evangelical Baptist Convention Church");
    ui->m_location->setText("Central Delhi");
    ui->m_saptuamMin->setText("EBCC Central Delhi");
    QDate date = QDate::currentDate();
    int year = date.year();
    int month = date.month();
    int startMonth = 4;
    int startYear = year;
    int endYear = year;
    int endMonth = 3;
    if (month < 4) {
        startYear = year -1;
    } else {
        endYear = year + 1;
    }
    ui->m_fyStart->setDate(QDate(startYear, startMonth, 1));
    ui->m_fyEnd->setDate(QDate(endYear, endMonth, 31));
    ui->m_fontSize->setValue(9);
    SsettingData* setting = CsettingTable::Object()->getSetting();
    if (setting) {
        ui->m_cwd->setText(setting->m_cwd);
        ui->m_org->setText(setting->m_orgination);
        ui->m_location->setText(setting->m_location);
        ui->m_saptuamMin->setText(setting->m_saptuamMin);
        ui->m_fontSize->setValue(setting->m_fontSize);
        ui->m_fyStart->setDate(setting->m_fyStart);
        ui->m_fyEnd->setDate(setting->m_fyEnd);
        //ui->m_password->setText(setting->m_password);
        delete setting;
        m_isNew = false;
        ui->m_passwordLbl->setText("Edit Password");

    } else {
        ui->m_passwordLbl->setText("New Password");
    }
}
/*    QString m_cwd;
    QString m_orgination;
    QString m_location;
    QString m_saptuamMin;
    int   m_fontSize;
    QDate   m_fyStart;
    QDate   m_fyEnd;
    QString m_password;*/
CsettingDlg::~CsettingDlg()
{
    delete ui;
}

void CsettingDlg::on_m_closeBtn_clicked()
{
    if (m_isNew) {
        if (ui->m_password->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "SaptuamData",
                                 "Password is not set");
            return;
        }

    }
    if (ui->m_password->text().trimmed().isEmpty() == false) {
        if (ui->m_password->text().trimmed() != ui->m_password_2->text().trimmed()) {
            QMessageBox::warning(this, "SaptuamData",
                                 "Password mismatch");
            return;
        }
    }

    if (ui->m_password->text().trimmed().isEmpty() == false) {
        if (ui->m_password->text().trimmed().size() < 8) {
            QMessageBox::warning(this, "SaptuamData",
                                 "Password should be atleast 8 character long");
            return;
        }
    }

    if (ui->m_cwd->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "SaptuamData",
                             "Directory cannot be empty");
        return;
    }




    SsettingData* setting = CsettingTable::Object()->getSetting();

    if (setting) {
        if (ui->m_cwd->text().trimmed() !=  setting->m_cwd) {
            setting->m_cwd = ui->m_cwd->text().trimmed();
            CsettingTable::Object()->updateValue(setting->m_cwd, SETTING_CWD_IDX, setting->m_idx, SETTING_ID_IDX);
        }
        if (ui->m_org->text().trimmed() !=  setting->m_orgination) {
            setting->m_orgination = ui->m_org->text().trimmed();
            CsettingTable::Object()->updateValue(setting->m_orgination, SETTING_ORGINATION_NAME_IDX, setting->m_idx, SETTING_ID_IDX);

        }
        if (ui->m_location->text().trimmed() != setting->m_location) {

            setting->m_location = ui->m_location->text().trimmed();
            CsettingTable::Object()->updateValue(setting->m_location, SETTING_LOCATION_IDX, setting->m_idx, SETTING_ID_IDX);

        }
        if (ui->m_saptuamMin->text().trimmed() != setting->m_saptuamMin) {

            setting->m_saptuamMin = ui->m_saptuamMin->text().trimmed();
            CsettingTable::Object()->updateValue(setting->m_saptuamMin, SETTING_SAPTUAM_MIN_IDX, setting->m_idx, SETTING_ID_IDX);

        }
        if (ui->m_fontSize->value() != setting->m_fontSize) {

            setting->m_fontSize =  ui->m_fontSize->value();
            CsettingTable::Object()->updateValue(setting->m_fontSize, SETTING_FONT_SIZE_IDX, setting->m_idx, SETTING_ID_IDX);

        }
        if (ui->m_fyStart->date() !=  setting->m_fyStart) {
            setting->m_fyStart = ui->m_fyStart->date();
            CsettingTable::Object()->updateValue(setting->m_fyStart, SETTING_FY_START_IDX, setting->m_idx, SETTING_ID_IDX);

        }
        if (ui->m_fyEnd->date() != setting->m_fyEnd) {
            setting->m_fyEnd = ui->m_fyEnd->date();
            CsettingTable::Object()->updateValue(setting->m_fyEnd, SETTING_FY_END_IDX, setting->m_idx, SETTING_ID_IDX);

        }

        if (ui->m_password->text().trimmed() != setting->m_password && ui->m_password->text().trimmed().isEmpty() == false) {
            setting->m_password = ui->m_password->text().trimmed();
            CsettingTable::Object()->updateValue(setting->m_password, SETTING_PASSWORD_IDX, setting->m_idx, SETTING_ID_IDX);

        }

    } else {
        setting = new SsettingData();
        setting->m_cwd = ui->m_cwd->text().trimmed();
        setting->m_orgination = ui->m_org->text().trimmed();
        setting->m_location = ui->m_location->text().trimmed();
        setting->m_saptuamMin = ui->m_saptuamMin->text().trimmed();
        setting->m_fontSize =  ui->m_fontSize->value();
        setting->m_fyStart = ui->m_fyStart->date();
        setting->m_fyEnd = ui->m_fyEnd->date();
        setting->m_password = ui->m_password->text().trimmed();
        setting->save();


    }


    delete setting;
    emit accept();
}


void CsettingDlg::on_m_bwroseDirBtn_clicked()
{
    QFileDialog fileDlg;
    QString str = fileDlg.getExistingDirectory(nullptr, "Select Folder");
    ui->m_cwd->setText(str);
}

