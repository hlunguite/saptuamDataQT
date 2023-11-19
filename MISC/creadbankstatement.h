#ifndef CREADBANKSTATEMENT_H
#define CREADBANKSTATEMENT_H
#include <QString>
#include "cbanktransactiontable.h"

class CreadBankStatement
{
public:

    CreadBankStatement();
    bool selectFile();
    int read();
private:
    SbanktransDetail processDescription(QString description);
    SbanktransDetail processRef(QString refStr, QString amount, QString date);
    QString m_fileName;

};

#endif // CREADBANKSTATEMENT_H
