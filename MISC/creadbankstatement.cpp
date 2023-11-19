#include "creadbankstatement.h"
#include <QFileDialog>
#include "csdutils.h"
#include <QDebug>
#include "cimportbanktransactiontable.h"
CreadBankStatement::CreadBankStatement()
{

}

bool CreadBankStatement::selectFile()
{
    // QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString currentDirectory = CsdUtils::getCurrentWdir();
    m_fileName =  QFileDialog::getOpenFileName(nullptr,
                                                    "Open Bank Statement", currentDirectory,
                                                    "SaptuamData files (*.csv)");
    if (m_fileName.isEmpty()) {
        return false;
    }
    return true;
}

int CreadBankStatement::read()
{

    //qDebug()<<"file name is "<<fileName;
    int id = 0;
    QString dateStr;
    QVector<SbanktransDetail> allTrans;

    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //QString error = "Not able to open file " + fileName;
        //message(this,error);
        return id;
    }
    QString startDate;
    QString endDate;
    QTextStream in(&file);
    bool isTrans = false;
    QString line = in.readLine();
    while (!in.atEnd()) {
        line = in.readLine();
        //qDebug()<<"Processing:"<<line;
        if (isTrans == false){
             if(line.contains("Value Date") &&
                 line.contains("Description") &&
                 line.contains("Ref No./Cheque No.")){
                 isTrans = true;
             }
            if (line.contains("Start Date          :")) {
                QRegularExpression rx("(\\,)");
                QStringList query = line.split(rx);
                dateStr = query.at(1);
                //qDebug()<<"Start "<<query.at(1);

            } else if (line.contains("End Date            :")) {
                QRegularExpression rx("(\\,)");
                QStringList query = line.split(rx);
                dateStr += " - " + query.at(1);
                //qDebug()<<"End "<<query.at(1);

            }

         } else {
            QString description;
            QString refno;
            QString amtZatna;
            QString amtMuhna;
            QString date;
            QRegularExpression rx("(\\,)");
            QStringList query = line.split(rx);
            //qDebug()<<"line is "<<line;
            int size = query.size();
            if (size > 5) {
                description = query.at(2);
                date = query.at(1);
                //qDebug()<<"date" <<query.at(1);
                refno = query.at(3);
                int isPayment = 0;
                int isIncome = 0;
                int isOpenQuote = 0;
                for (int i = 5; i < size; ++i) {
                    QString value = query.at(i);
                    if (i == 5) {
                        isPayment = 1;
                    } else if (isPayment == 2 ) {
                        isIncome = 1;
                    }

                    if (isOpenQuote == 0 && value.isEmpty() == false && value.at(0) == '\"') {
                        isOpenQuote = 1;
                        value = value.right(value.size() -1);
                    }
                    if (isPayment == 1) {
                        // check empty
                        if (value.isEmpty()) {
                            isPayment = 2; // end of payment
                            isOpenQuote = 0;
                        } else if (isOpenQuote == 0) { // not quote
                            amtZatna = value;
                            isPayment = 2; // end of payment
                        } else {
                            bool isEndQuote = value.at(value.size()-1) == '\"';

                            // check within quote
                            if (isEndQuote) {
                                isOpenQuote = 0;
                                amtZatna += value.left(value.size()-2);
                                isPayment = 2;
                            } else {
                                amtZatna += value;
                            }

                        }

                    } else if (isIncome == 1) {
                        // check empty
                        if (value.isEmpty()) {
                            isIncome = 2; // end of income
                            isOpenQuote = 0;
                        } else if (isOpenQuote == 0) { // not quote
                            amtMuhna = value;
                            isIncome = 2; // end of payment
                        } else {
                            // check within quote
                            bool isEndQuote = value.at(value.size()-1) == '\"';

                            if (isEndQuote) {
                                isOpenQuote = 0;
                                amtMuhna += value.left(value.size()-2);
                                isIncome = 2;
                            } else {
                                amtMuhna += value;
                            }
                        }
                    }

                    if (isIncome == 2) {
                        break;
                    }
                }
            }
            amtMuhna = amtMuhna.trimmed();
            amtZatna = amtZatna.trimmed();
            if (amtMuhna.isEmpty() == false || amtZatna.isEmpty() == false) {
                SbanktransDetail  transDetail = processDescription(description);
                //qDebug()<<"From description:"<<transDetail.toString();

                SbanktransDetail  transDetailFromRef = processRef(refno, amtMuhna.isEmpty()? amtZatna : amtMuhna, date);
                //qDebug()<<"From refid:"<<transDetailFromRef.toString();

                if (transDetailFromRef.m_refID.isEmpty() == false) {
                    if (transDetail.m_refID.isEmpty() == false && transDetail.m_refID.contains("IMPS") ) {

                    } else {
                        transDetail.m_refID = transDetailFromRef.m_refID;
                    }


                }
                if (transDetailFromRef.m_min.isEmpty() == false) {
                    if (transDetail.m_min.isEmpty()) {
                        transDetail.m_min = transDetailFromRef.m_min;
                    } else {
                        //qDebug()<<"Fond tow name";
                        transDetail.m_min += transDetailFromRef.m_min;
                    }
                }

                transDetail.m_date = date;
                if (startDate.isEmpty()) {
                    startDate = date;
                }
                endDate = date;
                if (amtMuhna.isEmpty() == false) {
                    bool ok;
                    transDetail.m_amount = amtMuhna.toDouble(&ok);
                    if (ok == false) {
                        qDebug()<<"muhna is not double "<<amtMuhna;
                        Q_ASSERT(0);
                    }
                    transDetail.m_isIncome = true;
                } else if (amtZatna.isEmpty() == false) {
                    bool ok;
                    transDetail.m_amount = amtZatna.toDouble(&ok);
                    if (ok == false) {
                        qDebug()<<"zatna is not double "<<amtZatna;
                        Q_ASSERT(0);
                    }
                    transDetail.m_isIncome = false;
                }
                allTrans.push_back(transDetail);

                //qDebug()<<"Final is :"<<transDetail.toString()<<"\n";


            }
        }

    }
    file.close();
    if (dateStr.isEmpty()) {
        dateStr = startDate + " - " + endDate;
    }
    SimportBankTransactionData importData;
    importData.m_dateRange = dateStr;

    if (importData.save()) {
        for (SbanktransDetail& transData : allTrans) {
            transData.m_importID = importData.m_id;
            transData.save();
        }
    }
    qDebug()<<"import id is "<<importData.m_id;
    return importData.m_id;
}

SbanktransDetail
CreadBankStatement::processDescription(QString description)
{
    QSet<QString>IGNORE{"NA", "", "trf", "MB  Transf", "Gift to relatives / Friends", "Donation", "Donation-", "UPI"};
    SbanktransDetail transDetail;
    //CASH DEPOSIT SELF
    //CASH DEPOSIT-CASH DEPOSIT SELF--
    QRegularExpression rx (".*CASH DEPOSIT SELF.*");
    QRegularExpressionMatch match = rx.match(description);

    if (match.hasMatch()) {
        transDetail.m_piakChan = "CASH DEPOSIT SELF";
        transDetail.m_type = BANK_CASH_DEPOSIT;
        transDetail.m_refID = "CASH DEPOSIT SELF";
        return transDetail;
    }
    //CAS DEP CDM-xxx--yyyy
    rx.setPattern("CSH DEP CDM-(.*)--.*");
    match = rx.match(description);
    if (match.hasMatch()) {

        transDetail.m_piakChan = "CASH DEPOSIT";
        transDetail.m_type = BANK_CASH_DEPOSIT;
        transDetail.m_refID = "CASH DEPOSIT";
        return transDetail;
    }
    //CHEQUE DEPOSIT---77777
    rx.setPattern("CHEQUE DEPOSIT---(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = "CHEQUE DEPOSIT";
        transDetail.m_type = CHEQUE;
        transDetail.m_refID = match.captured(1).trimmed();
        return transDetail;
    }
    //CHEQUE WDL-CHEQUE TRANSFER TO--
    rx.setPattern("CHEQUE WDL-CHEQUE TRANSFER TO--(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = "CHEQUE PAYMENT";
        transDetail.m_type = CHEQUE;
        transDetail.m_refID = match.captured(1).trimmed();
        return transDetail;
    }
    //CDM SERVICE CHARGES---aaaaa
    rx.setPattern("(CDM SERVICE CHARGES)---(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = match.captured(1).trimmed();
        transDetail.m_type = BANK_CHARGES;
        transDetail.m_refID = match.captured(2).trimmed();
        return transDetail;
    }
    //CREDIT INTEREST---
    rx.setPattern("CREDIT INTEREST.*");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = "BANK INTEREST";
        transDetail.m_type = BANK_INTEREST;
        transDetail.m_refID = "Bank Interest";
        return transDetail;
    }
    //CHEQUE BOOK ISSUE CHARGE
    rx.setPattern("CHEQUE BOOK ISSUE CHARGE.*");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = "CHEQUE BOOK ISSUE CHARGE";
        transDetail.m_refID = "CHEQUE BOOK ISSUE CHARGE";
        transDetail.m_type = BANK_CHARGES;
        return transDetail;
    }
    //BY CLEARING qqqq---9999
    rx.setPattern("BY CLEARING (.*)--(\\d+)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = "CHEQUE DEPOSIT";
        transDetail.m_refID = match.captured(2);
        transDetail.m_type = CHEQUE;
        return transDetail;
    }
    //TO CLEARING wwww--1234
    rx.setPattern("TO CLEARING-(.*)\\s+(.*)--(\\d+)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_min = match.captured(2).trimmed();
        transDetail.m_refID = match.captured(3).trimmed();
        transDetail.m_piakChan = "CHEQUE PAYMENT";
        transDetail.m_type = CHEQUE;
        return transDetail;
    }
    //TO TRANSFER-INB NEFT UTR NO: SBIN223101026491--min
    rx.setPattern("TO TRANSFER-(.*)UTR NO:(.*)--(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {

        transDetail.m_type = NOTASIGN;
        transDetail.m_piakChan = match.captured(3).trimmed();
        transDetail.m_refID =  match.captured(2).trimmed();
        return transDetail;
    }
    description = description.replace("*--", "");
    description = description.replace("- -", "");
    description = description.replace("--", "");
    description = description.replace("*", "#");
    description = description.replace("/URGENT/", "");

    transDetail.m_type = NOTASIGN;
    rx.setPattern("BY TRANSFER-NEFT(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        QString matchString = match.captured(1).trimmed();
        rx.setPattern("#(.*)#(.*)#(.*)#(.*)");
        match = rx.match(matchString);
        if (match.hasMatch()) {
            QString ifsc = match.captured(1);
            transDetail.m_refID = match.captured(2).trimmed();
            transDetail.m_min = match.captured(3).trimmed();
            QString piakziak = match.captured(4).trimmed();
            if (ifsc.contains(piakziak) == false) {
                transDetail.m_piakChan = piakziak;
            }
            return transDetail;
        } else {
            //BY TRANSFER-NEFT*BARB0TRDJHE*BARBR23283622507*G NIANG SUANMOI--
            rx.setPattern("#(.*)#(.*)#(.*)");
            match = rx.match(matchString);
            if (match.hasMatch()) {
                transDetail.m_refID = match.captured(2).trimmed();
                transDetail.m_min = match.captured(3).trimmed();
                return transDetail;
            }

        }
    }
    //BY TRANSFER-UPI/CR/328347987247/NIANG HA/SBIN/haunutonsi/UPI--
    rx.setPattern("BY TRANSFER-UPI/CR/(.*)/(.*)/(.*)/(.*)/(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
       transDetail.m_refID =  match.captured(1).trimmed();
       transDetail.m_min = match.captured(2).trimmed();
       QString piakchan = match.captured(5).trimmed();
        if (IGNORE.contains(piakchan)) {
            piakchan = "";
        }
        transDetail.m_piakChan = piakchan;
        QString phone = match.captured(4).trimmed();
        bool ok;
        phone.toLong(&ok);
        if (ok)
            transDetail.m_phone = phone;
        else {
            transDetail.m_min += " " + phone;
        }
        return transDetail;
    }
    //BY TRANSFER-INB MBS bldg fd mission byf box--
    rx.setPattern("BY TRANSFER-INB MBS(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = match.captured(1).trimmed();
        return transDetail;
    }
    match = rx.match(description);
    rx.setPattern("BY TRANSFER-INB IMPS/(.*)/(.*)/(.*)");
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(2).trimmed();
        return transDetail;
    }


    //BY TRANSFER-INB IMPS328315835340/7005582124/XX2261/Missionary--
    //BY TRANSFER-INB IMPS/P2A/UA0549031434/XXXXXXX505SBIN--
    rx.setPattern("BY TRANSFER-INB IMPS(.*)/(.*)/(.*)/(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
       transDetail.m_refID =  match.captured(1).trimmed();
       transDetail.m_phone = match.captured(2).trimmed();
       if (transDetail.m_refID.isEmpty() == false) {
            transDetail.m_refID = "IMPS" +  transDetail.m_refID;
       } else {
            transDetail.m_refID =  match.captured(3).trimmed();
            transDetail.m_phone = "";

       }
       QString piakchan = match.captured(4).trimmed();

       if (not IGNORE.contains(piakchan))  {
           transDetail.m_piakChan = piakchan;
       }
       return transDetail;
    }
    //BY TRANSFER-INB To EBCC Central Delhi--
    rx.setPattern("BY TRANSFER-INB (.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        QString ref = match.captured(1).trimmed();
        if (not IGNORE.contains(ref)) {
            transDetail.m_piakChan = ref;
        }
        return transDetail;
    }
    //BY TRANSFER-SBILT11102023074605834910-October tithe--
    rx.setPattern("BY TRANSFER-SBILT(\\d+)-(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_refID  = match.captured(1).trimmed();
        transDetail.m_piakChan = match.captured(2).trimmed();
        return transDetail;
    }

    rx.setPattern("BY TRANSFER-(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_piakChan = match.captured(1).trimmed();
        return transDetail;
    }


    //TO TRANSFER-INB Donation--
    rx.setPattern("TO TRANSFER-(\\S+)\\s+(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {
        transDetail.m_type = NOTASIGN;
        transDetail.m_piakChan = match.captured(2).trimmed();
        return transDetail;
    }
    rx.setPattern("TO TRANSFER-(.*)");
    match = rx.match(description);
    if (match.hasMatch()) {

        transDetail.m_piakChan = match.captured(1).trimmed();
        return transDetail;
    }


    rx.setPattern("TO TRANSFER.*");
    match = rx.match(description);
    if (match.hasMatch()) {

        transDetail.m_type = NOTASIGN;
        transDetail.m_piakChan = "Other Payment";
        return transDetail;
    }

    transDetail.m_piakChan = description;
    //qDebug()<<"NO MATCH "<<transDetail.toString();
    return transDetail;
}

SbanktransDetail CreadBankStatement::processRef(QString referenceid,
                                                QString amount,
                                                QString date)
{

    //qDebug()<<"ref id "<<referenceid;
    SbanktransDetail transDetail;
    //UA0549031434MOAIAALNX0               TRANSFER FROM 31412708482                         Mr. THANGZALUN  GUITE /
    QRegularExpression rx ("(\\w+)(\\s+)TRANSFER FROM (\\d+)(\\s+)(.*)/");
    QRegularExpressionMatch match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(1).trimmed();
        transDetail.m_min = match.captured(5).trimmed();
        return transDetail;
    }


    rx.setPattern("^TRANSFER FROM (\\d+)\\s+(.*)/(\\d+)$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_min = match.captured(2).trimmed();
        transDetail.m_refID =  match.captured(3).trimmed();

        return transDetail;
    }

    //TRANSFER FROM 20152041807                         Mrs. ELIAKIM  ./
    rx.setPattern("^TRANSFER FROM (\\d+)\\s+(.*)/");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_min = match.captured(2).trimmed();
        //transDetail.m_refID =  match.captured(1).trimmed();

        return transDetail;
    }


    //MAA001408810077          MAA001408810077          TRANSFER FROM 4597936162097 /
    rx.setPattern("^(\\w+)\\s+(\\w+)\\s+TRANSFER FROM (\\d+)/$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(1).trimmed();

        return transDetail;
    }
    rx.setPattern("^TRANSFER FROM\\s+(\\d+)\\s+(.*)/\\s*(\\d+)$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_min = match.captured(2).trimmed();
        transDetail.m_refID = match.captured(3).trimmed();

        return transDetail;
    }
    rx.setPattern("^TRANSFER FROM\\s+(\\d+)\\s+(.*)/$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_min = match.captured(2).trimmed();
        transDetail.m_refID = match.captured(1).trimmed() + "_" + amount + "_" + date;

        return transDetail;
    }
    rx.setPattern("^TRANSFER FROM\\s+(\\d+)/$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_type = NOTASIGN;
        transDetail.m_refID = match.captured(1).trimmed();

        return transDetail;
    }


    rx.setPattern("^TRANSFER TO\\s+(\\d+)\\s+(.*)/(\\d+)$");
    match = rx.match(referenceid);
    if (match.hasMatch() ) {
        transDetail.m_min  = match.captured(2).trimmed();
        transDetail.m_refID = match.captured(3).trimmed();

        return transDetail;
    }

    //NEFT INB: CNACKRDSV3                              TRANSFER TO 3199302044302 / min
    rx.setPattern("(\\w+)(\\s+)TRANSFER TO (\\d+)(\\s+)(.*)/\\s+(\\w+.*)");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(1).trimmed();
        transDetail.m_min = match.captured(6).trimmed();

        return transDetail;
    }

    //CTZ0029627               TRANSFER TO 41778569056                           GRACE BIBLE COLLEGE EN /
    rx.setPattern("(\\w+)(\\s+)TRANSFER TO (\\d+)(\\s+)(.*)/");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(1).trimmed();
        transDetail.m_min = match.captured(5).trimmed();

        return transDetail;
    }

    rx.setPattern("^/(\\d+)$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID  = match.captured(1).trimmed();

        return transDetail;
    }
    rx.setPattern("^/$");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_type = NOTASIGN;

        return transDetail;
    }

    rx.setPattern("TRANSFER TO (\\d+)/");
    match = rx.match(referenceid);
    if (match.hasMatch()) {
        transDetail.m_refID = match.captured(1).trimmed();
        return transDetail;
    }
    //TRANSFER FROM 4897733162090 /
    //qDebug()<<"NOT FOUND "<<referenceid;
    transDetail.m_type = NOTASIGN;
    return transDetail;
}

