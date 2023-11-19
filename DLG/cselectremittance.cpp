#include "cselectremittance.h"
#include "ui_cselectremittance.h"
#include <QVBoxLayout>
#include "cremittancetable.h"

CselectRemittance::CselectRemittance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CselectRemittance)
{
    ui->setupUi(this);
    m_selectID = -1;
}

CselectRemittance::~CselectRemittance()
{
    delete ui;
}

int CselectRemittance::selectID() const
{
    return m_selectID;
}

void CselectRemittance::setAllID(const std::set<int> &newAllID)
{
    m_selectID = -1;
    QVBoxLayout *vbox = nullptr;
    if (ui->m_selectRemittanceBox->layout()) {
        vbox = (QVBoxLayout*)ui->m_selectRemittanceBox->layout();
        QLayoutItem *item;
        while ((item = vbox->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete ui->m_selectRemittanceBox->layout();
    }
    m_remitRadioBox.clear();
    m_allID.clear();
    vbox = new QVBoxLayout;
    QRadioButton* nonBox = new QRadioButton("None");
    nonBox->setCheckable(true);
    //qDebug()<<"id at "<<m_allID.size()<<" -1 "<<" text none";
    m_allID.push_back(-1);
    m_remitRadioBox.push_back(nonBox);
    vbox->addWidget(nonBox);
    for (auto id : newAllID) {
        QRadioButton* box = nullptr;
        if (id == 0) {
            box = new QRadioButton("Add new remittance");

        } else {
            SremittanceData* data = CremittanceTable::Object()->getRemittanceData(id);
            if (data){
                QString text ="Display remittance between ";
                text += data->m_fromDate.toString("d MMM yyyy") + " - ";
                text += data->m_toDate.toString("d MMM yyyy");


                box = new QRadioButton(text);
                delete data;
            }
        }
        //qDebug()<<"id at "<<m_allID.size()<<" "<<id<<" text "<<box->text();

        m_allID.push_back(id);
        m_remitRadioBox.push_back(box);
        vbox->addWidget(box);
        //std::vector<int> m_allID;
        //QVector<QRadioButton*>  m_remitRadioBox;
    }


    //m_allID = newAllID;
    vbox->addStretch(1);

    ui->m_selectRemittanceBox->setLayout(vbox);
}



void CselectRemittance::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CselectRemittance::on_m_okBtn_clicked()
{
    for (int i = 0; i < m_remitRadioBox.size(); ++i) {
        if (m_remitRadioBox.at(i)->isChecked()) {
            m_selectID = m_allID.at(i);
            //qDebug()<<" box click is at "<<i<<" text "<<m_remitRadioBox.at(i)->text()<<" select id "<<m_selectID;
            break;
        }
    }
    emit accept();
}

