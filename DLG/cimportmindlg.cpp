#include "cimportmindlg.h"
#include "ui_cimportmindlg.h"
#include "csdutils.h"
#include "DB/ccontacttable.h"
#include "DB/cpersontable.h"
#include "DB/cothercontacttable.h"
#include "ccontactmap.h"
#include <QFileDialog>
#include <QRegularExpression>
#include "ccontactdlg.h"
CimportMinDlg::CimportMinDlg(QWidget *parent) :
     CdlgBase(parent),
    ui(new Ui::CimportMinDlg)
{
    ui->setupUi(this);
    
    m_importMin = CimportMinTable::Object()->getImportedMin();
    m_index = 0;
    ui->m_progressBar->hide();
    populateTable();
}

CimportMinDlg::~CimportMinDlg()
{
    delete ui;
    if (m_importMin) {
        for (auto min : *m_importMin) {
            delete min;
        }
        delete m_importMin;
    }
}

void CimportMinDlg::on_m_minFileBtn_clicked()
{
    // QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString currentDirectory = CsdUtils::getCurrentWdir();
    QString fileName =  QFileDialog::getOpenFileName(nullptr,
                                              "Open min list", currentDirectory,
                                              "SaptuamData files (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //QString error = "Not able to open file " + fileName;
        //message(this,error);
        return ;
    }

    QTextStream in(&file);

    QString line ;
    QVector<QString> lines;
    while (!in.atEnd()) {
         line = in.readLine();
        if (line.contains("First Name")) {
            continue;
         }
        lines.push_back(line);
    }
    ui->m_progressBar->show();

    file.close();
    int count = lines.size();
    int i = 0;
    for (QString line : lines) {
        ++i;
        int value = ((float)i/(float)count)*100;
        ui->m_progressBar->setValue(value);
        QRegularExpression rx("(\\,)");
        QStringList query = line.split(rx);
        if (query.size() < 11) {
            continue;
        }
        QString id = query.at(0).trimmed().simplified();
        QString title = query.at(1).trimmed().simplified();
        QString firstName = query.at(2).trimmed().simplified();
        QString lastName = query.at(3).trimmed().simplified();
        QString relation = query.at(4).trimmed().simplified();
        QString parentFistName = query.at(5).trimmed().simplified();
        QString parentLastName = query.at(6).trimmed().simplified();
        QString phone = query.at(7).trimmed().simplified();
        QString bank = query.at(8).trimmed().simplified();
        QString isPersonStr = query.at(9).trimmed().simplified();
        QString remark = query.at(10).trimmed().simplified();
        if (firstName.isEmpty()) {
            continue;
        }
        bool isPerson = (isPersonStr.isEmpty() == false) ? ( isPersonStr.contains("yes", Qt::CaseInsensitive)  ) : false;


        SimportMinData data;
        data.m_idx = 0;
        bool ok;
        data.m_importIdx = id.toInt(&ok);
        data.m_title = title;
        data.m_firstName = firstName;
        data.m_lastName = lastName;
        data.m_relationship = relation;
        data.m_parentFirstName = parentFistName;
        data.m_parentLastName = parentLastName;
        data.m_phone = phone;
        data.m_bankName = bank;
        data.m_contactTableIdx = 0;
        data.m_isPerson = isPerson;
        data.m_transRemark = remark;
        data.save();

    }
    //file.close();

    ui->m_progressBar->hide();

    populateTable();

}



void CimportMinDlg::on_m_closeBtn_clicked()
{
    emit reject();
}

void CimportMinDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = ui->m_table->geometry().x();
    int y = ui->m_table->geometry().y();
    int buttonheight = ui->m_nextBtn->geometry().height();
    int buttonwidth = ui->m_nextBtn->geometry().width();

    int width = windowwidth - (2*x);
    int height = windowheight - y - (2*buttonheight);
    ui->m_table->setGeometry(x, y, width, height);


     ui->m_table->horizontalHeader()->resizeSection(0,width*.05);
    ui->m_table->horizontalHeader()->resizeSection(1,width*.05);
     ui->m_table->horizontalHeader()->resizeSection(2,width*.15);
    ui->m_table->horizontalHeader()->resizeSection(3,width*.10);
     ui->m_table->horizontalHeader()->resizeSection(4,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(5,width*.10);
     ui->m_table->horizontalHeader()->resizeSection(6,width*.15);
    ui->m_table->horizontalHeader()->resizeSection(7,width*.10);
     ui->m_table->horizontalHeader()->resizeSection(8,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(9,width*.10);
     ui->m_table->hideColumn(10);
    //ui->m_table->hideColumn(9);


    y = y + height + (buttonheight*0.5);
    x = windowwidth - x - buttonwidth;
    ui->m_closeBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x = x - buttonwidth - 3;
    ui->m_nextBtn->setGeometry(x, y, buttonwidth, buttonheight);

}

void CimportMinDlg::populateTable()
{
    m_index = 0;
    if (m_importMin) {
        for (auto min : *m_importMin) {
            delete min;
        }
        delete m_importMin;
    }
    m_importMin = CimportMinTable::Object()->getImportedMin();

    ui->m_table->clearContents();
    ui->m_table->setRowCount(0);
    if (m_importMin == nullptr) {
        return;
    }
    ui->m_progressBar->show();

    int size = m_importMin->size();
    for (int idx = 0; idx < size; ++idx) {
        int value = ((float)(idx + 1)/(float)size)*100;
        ui->m_progressBar->setValue(value);

        SimportMinData* data = m_importMin->at(idx);
        QVector<QTableWidgetItem*> cells(11);
        cells[0] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_importIdx)));
        cells[1] = new QTableWidgetItem(tr("%1").arg(data->m_title));
        cells[2] = new QTableWidgetItem(tr("%1").arg(data->m_firstName));
        cells[3] = new QTableWidgetItem(tr("%1").arg(data->m_lastName));
        cells[4] = new QTableWidgetItem(tr("%1").arg(data->m_relationship));
        cells[5] = new QTableWidgetItem(tr("%1").arg(data->m_phone));
        cells[6] = new QTableWidgetItem(tr("%1").arg(data->m_parentFirstName));
        cells[7] = new QTableWidgetItem(tr("%1").arg(data->m_parentLastName));
        cells[8] = new QTableWidgetItem(tr("%1").arg(data->m_bankName));
        cells[9] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_contactTableIdx)));
        cells[10] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_idx)));

        int row = ui->m_table->rowCount();
        ui->m_table->insertRow(row);
        for (int i = 0; i < 11; ++i) {
             ui->m_table->setItem(row, i, cells[i]);
        }

    }
    ui->m_progressBar->hide();

}

void CimportMinDlg::resizeEvent(QResizeEvent *event)
{
    resize();
}


void CimportMinDlg::on_m_nextBtn_clicked()
{
    populateTable();
    if (m_importMin ==nullptr) {
        message("No data to process");
        return;
    }
    /*{
        QVector<SimportMinData *>* mins = CimportMinTable::Object()->getImportedMin(false);
        for (auto min: *mins) {
             if (min->m_firstName == "") {
                 qDebug()<<min->m_firstName<<" "<<min->m_parentFirstName;
                 min->m_contactTableIdx = 0;
                 min->save();
                 delete min;
             }
        }
        delete mins;

    }*/

    bool tocontinue = true;
    m_index = 0;
    int size = m_importMin->size();
    ui->m_progressBar->show();
    do {

        if (m_index >= m_importMin->size()) {
             message("No data to process");
             return;
        }
        int value = ((float)m_index/(float)size)*100;
        ui->m_progressBar->setValue(value);

        QTableWidgetItem* firstNameItem = ui->m_table->item(m_index, 2);

        if (firstNameItem) {
             ui->m_table->scrollToItem(firstNameItem);
        }
        CcontactDLG dlg;
        dlg.setForImport(m_importMin->at(m_index));
        dlg.setWindowTitle("Import Name");
        if (dlg.exec()) {
             int id = CcontactMap::Object()->addEditContact(&dlg);
             SimportMinData* data = m_importMin->at(m_index);
             if (data) {
                 if (data->m_bankName.isEmpty() == false) {
                     ScontactData* contactData = CcontactTable::Object()->getContactFromID(id);
                     if (contactData) {
                         contactData->m_bankName = data->m_bankName;
                         contactData->save();
                         delete contactData;
                     }
                 }

                 data->m_contactTableIdx = id;
                 data->save();
                 QTableWidgetItem* contactTableID = ui->m_table->item(m_index, 9);
                 if (contactTableID) {
                     contactTableID->setText(QString::number(id));
                     emit updateContactTable();
                 }

             }
             ++m_index;

        } else {
             tocontinue = false;
             break;
        }
    }while(tocontinue);

    ui->m_progressBar->hide();
}

