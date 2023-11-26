#include "ccontactdlg.h"
#include "ui_ccontactdlg.h"
#include <QMessageBox>
#include "csdutils.h"
#include "DB/ccontacttable.h"
#include "DB/cpersontable.h"
#include "DB/cothercontacttable.h"
#include "ccontactmap.h"
#include "cimportmintable.h"
#include "MISC/CdlgDefine.h"
CcontactDLG::CcontactDLG(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CcontactDLG),
    m_parentSpouseCompleter(nullptr),
    m_contactEditID(0),
    m_personEditID(0),
    m_otherEditID(0),
    m_contactIDFullNameToUpdate(0),
    m_parentSpouseContactID(0),
    m_suggestedNameContactID(0),
    m_isPerson(true),
    m_forImport(false)

{
    ui->setupUi(this);
    ui->m_name->hide();

    TobjectList values;

    m_nameWithSpouseMap.clear();

    std::map<int, int > nameIDParentIDMap;
    std::map<int, QVector<QString> > idNameMap;
    std::map<int, QString> relationShipMap;
    QVector<SpersonData *>* persons = CpersonTable::Object()->getAllPresonData();
    for (auto person : *persons) {
        int id = person->m_id;
        QString title = person->m_title;
        QString firstName = person->m_firstName;
        QString lastName = person->m_lastName;
        QString relationshi = person->m_relationship;
        int parentID = person->m_parentOrSpousePersonId;
        //QString name = CsdUtils::createFullName(title, firstName, lastName);
        if (parentID > 0) {
            nameIDParentIDMap[id] = parentID;
            relationShipMap[id] = relationshi;
        }
        idNameMap[id].push_back(title);
        idNameMap[id].push_back(firstName);
        idNameMap[id].push_back(lastName);

        //nameIDParentIDMap[id] = std::make_pair(name, parentID);
    }
    delete persons;

    QString condition;// =  CcontactTable::Object()->getColName(CONTACT_PERSON_ID_IDX) + " > 0";
    if ( CcontactTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int contactID = values.at(i).at(CONTACT_ID_IDX).toInt();
            QString fullName = values.at(i).at(CONTACT_FULL_NAME_IDX).toString();
            QString fullNameWithoutTitle = values.at(i).at(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString();
            int id = values.at(i).at(CONTACT_PERSON_ID_IDX).toInt();
            //qDebug()<<"name "<<fullName<<" "<<fullNameWithoutTitle;
            //minlist.push_back(fullName);
            // m_personContactIdMap.insert(fullName, contactID);
            m_nameIDMap[fullName].insert(contactID);
            m_nameIDMap[fullNameWithoutTitle].insert(contactID);
            m_allFullName.insert(contactID, fullName);
            if (id > 0) {
                std::map<int, int >::iterator fn = nameIDParentIDMap.find(id);
                QString parentTitle;
                QString parentFirstName;
                QString parentLastName;
                QString relationship;
                if (fn != nameIDParentIDMap.end()) {
                    int parentID = fn->second;
                    QVector<QString>& n = idNameMap[parentID];
                    parentTitle = n.at(0);
                    parentFirstName = n.at(1);
                    parentLastName = n.at(2);
                    //parentName = idNameMap[parentID];
                    relationship = relationShipMap[id];
                }
                QString parentFullName;
                QString parentFullNameWithoutTitle;
                if (parentFirstName.isEmpty() == false) {
                    parentFullName = CsdUtils::createFullName(parentTitle, parentFirstName, parentLastName);
                    parentFullNameWithoutTitle = CsdUtils::createFullName("", parentFirstName, parentLastName);
                }
                QString name = CsdUtils::createFullNameWithSpouse(fullName, relationship, parentFullName);
                m_minList.push_back(name);
                m_nameWithSpouseMap.insert(name, contactID);
                //qDebug()<<name;
                m_displayName.insert(contactID, name);
                QString name1 = CsdUtils::createFullNameWithSpouse(fullName, "", parentFullName);
                if (name != name1) {
                    m_nameWithSpouseMap.insert(name1, contactID);
                    //qDebug()<<name1;
                }
                QString name2 = CsdUtils::createFullNameWithSpouse(fullNameWithoutTitle, relationship, parentFullNameWithoutTitle);
                if ((name != name2) && (name1 != name2)) {
                    m_nameWithSpouseMap.insert(name2, contactID);
                    //qDebug()<<name2;
                }

                QString name3 = CsdUtils::createFullNameWithSpouse(fullNameWithoutTitle, "", parentFullNameWithoutTitle);
                if ((name3 != name2)) {
                    m_nameWithSpouseMap.insert(name3, contactID);
                    //qDebug()<<name3;
                }
            } else {
                m_otherContactList.push_back(fullName);
                m_nameWithSpouseMap.insert(fullName, contactID);
                //qDebug()<<name;
                m_displayName.insert(contactID, fullName);
            }

        }
    }


    m_minList.sort();
    m_otherContactList.sort();
    if (m_minList.size() > 0) {
        m_parentSpouseCompleter = new QCompleter(m_minList, this);
        m_parentSpouseCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        m_parentSpouseCompleter->setWrapAround(true);
        m_parentSpouseCompleter->setFilterMode(Qt::MatchContains);
        ui->m_parentSpouse->setCompleter(m_parentSpouseCompleter);
        ui->m_name->setCompleter(m_parentSpouseCompleter);

    }
    ui->m_dataForPersoncheckBox->setChecked(true);
    on_m_dataForPersoncheckBox_clicked(true);
}

CcontactDLG::~CcontactDLG()
{
    if (m_parentSpouseCompleter) {
        delete m_parentSpouseCompleter;
    }
    delete ui;
}

void CcontactDLG::on_m_dataForPersoncheckBox_clicked(bool checked)
{
    if (m_parentSpouseCompleter) {
        delete m_parentSpouseCompleter;
    }
    if (checked) {
        m_isPerson = true;
        ui->m_titleComboBox->show();
        ui->m_titleLbl->show();
        ui->m_firstNameLbl->setText("First Name");
        ui->m_firstNameLbl->show();
        ui->m_firstNameLineEdit->show();
        ui->m_lastName->show();
        ui->m_lastNameLbl->show();
        ui->m_parentSpouse->show();
        ui->m_parentSpuseLbl->show();
        ui->m_genderCombo->show();
        ui->m_genderLbl->show();
        ui->m_relationShipCombo->show();
        ui->m_relationShipLbl->show();
        ui->m_parentgroupBox->hide();
        m_minList.sort();
        if (m_minList.size() > 0) {
            m_parentSpouseCompleter = new QCompleter(m_minList, this);
            m_parentSpouseCompleter->setCaseSensitivity(Qt::CaseInsensitive);
            m_parentSpouseCompleter->setWrapAround(true);
            m_parentSpouseCompleter->setFilterMode(Qt::MatchContains);
            ui->m_parentSpouse->setCompleter(m_parentSpouseCompleter);
            ui->m_name->setCompleter(m_parentSpouseCompleter);
        }

    } else {
        m_isPerson = false;
        ui->m_titleComboBox->hide();
        ui->m_titleLbl->hide();
        ui->m_firstNameLbl->setText("Name");
        ui->m_firstNameLbl->show();
        ui->m_firstNameLineEdit->show();
        ui->m_lastName->hide();
        ui->m_lastNameLbl->hide();
        ui->m_parentSpouse->hide();
        ui->m_parentSpuseLbl->hide();
        ui->m_genderCombo->hide();
        ui->m_genderLbl->hide();
        ui->m_relationShipCombo->hide();
        ui->m_relationShipLbl->hide();
        ui->m_parentgroupBox->hide();
        m_parentSpouseCompleter = new QCompleter(m_otherContactList, this);
        m_parentSpouseCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        m_parentSpouseCompleter->setWrapAround(true);
        m_parentSpouseCompleter->setFilterMode(Qt::MatchContains);
        ui->m_parentSpouse->setCompleter(m_parentSpouseCompleter);
        ui->m_name->setCompleter(m_parentSpouseCompleter);
    }
}

void CcontactDLG::on_m_cancelButton_clicked()
{
    emit  reject();

}

void CcontactDLG::on_m_okButton_clicked()
{
    m_title = "";
    m_firstName = "";
    m_lastName = "";
    m_relationShip = "";
    m_parentSpouse = "";
    m_gender = "";
    m_phone = "";
    m_suggestedName = "";
    m_contactIDFullNameToUpdate = 0;
    m_contactParentFullNameToUpdate = 0;
    m_suggestedName = ui->m_name->text().trimmed().simplified();
    bool checkName = false;
    if (m_forImport == false || m_suggestedName.isEmpty()) {
        checkName = true;
        m_firstName = CsdUtils::ucfirst(ui->m_firstNameLineEdit->text().trimmed().simplified());
        m_lastName = CsdUtils::ucfirst(ui->m_lastName->text().trimmed().simplified());

        if (m_firstName.isEmpty()) {
        //check empty first name
            message("Empty Name");
            return;
        }
    }

    m_phone = ui->m_phone->text().trimmed();
    if (ui->m_dataForPersoncheckBox->isChecked()) {
        m_parentSpouse = CsdUtils::ucfirst(ui->m_parentSpouse->text().trimmed().simplified());
        if (checkName) {
            m_title = ui->m_titleComboBox->currentText().trimmed();
            m_relationShip = ui->m_relationShipCombo->currentText();
            m_gender = ui->m_genderCombo->currentText();
            bool isFemaleTitle = false;
            bool isMaletitle = false;
            if (m_title == "Miss" || m_title == "Mrs" || m_title == "Ms") {
            isFemaleTitle = true;
        }
        if (m_title == "Mr" || m_title == "Upa" || m_title == "Pastor" || m_title == "Rev") {
            isMaletitle = true;
        }
        if (m_gender.isEmpty()) {
            if (isFemaleTitle) {
                m_gender = "Female";
            } else if (isMaletitle) {
                m_gender = "Male";
            } else {
                message("Gender cannot be empty");
                return;
            }
        }
            //check if title and gender match
            bool isMale = false;
            bool isFemale = false;
            if (m_gender.compare("Male") == 0) {
            isMale = true;
            if (isFemaleTitle) {
                QString error = "Gender " + m_gender + " cannot be " + m_title;
                message(error);
                return;
            }
            } else if (m_gender.compare("Female") == 0) {
            isFemale = true;
            if (isMaletitle) {
                QString error = "Gender " + m_gender + " cannot be " + m_title;
                message(error);
                return;
            }

        }
            //qDebug()<<"Gender "<<m_gender;
            //check if relationship and gender match
            if (m_relationShip == "s/o" && isMale == false) {
            message("Gender should be Male");
            return;
            } else if ((m_relationShip == "w/o" || m_relationShip == "d/o") && (isFemale == false)) {
            message("Gender should be Female");
            return;
        }
        }

        if (checkName) {
        //check parent full name
            bool isSpouseAdded = false;
            QString parentFullName;
            if (m_parentSpouse.isEmpty()) {
                m_parentTitle = ui->m_parentTitleCombo->currentText();
                m_parentFirstName = CsdUtils::ucfirst(ui->m_parentFirstName->text().trimmed().simplified());
                m_parentLastName = CsdUtils::ucfirst(ui->m_parentLastName->text().trimmed().simplified());
                if (m_parentFirstName.isEmpty() == false) {
                    parentFullName = CsdUtils::createFullName(m_parentTitle, m_parentFirstName, m_parentLastName);
                    /*int id = findContactID(parentSpouse);
                    if (id ) {
                        m_parentSpouse = getDisplayName(id);
                        parentFullName = getFullName(id);
                    }*/

                    isSpouseAdded = true;
                }
            } else {
                int id = findContactID(m_parentSpouse);
                parentFullName = getFullName(id);
            }

            m_fullName = CsdUtils::createFullName(m_title, m_firstName, m_lastName);
            QString nameWithSpouse = CsdUtils::createFullNameWithSpouse(m_fullName, m_relationShip, parentFullName);
            int id = findContactID(m_fullName);
            if (m_contactEditID > 0) {
                if(id > 0 && id != m_contactEditID) { // same name already exist


                    if (getDisplayName(m_contactEditID) == getDisplayName(id)) {
                        message("Name already exist");
                        return;
                    }
                    m_contactIDFullNameToUpdate = id;

                }
            } else { // new contact
                if (id > 0) {// same name already exist.
                    QString nameWithSpouse = CsdUtils::createFullNameWithSpouse(m_fullName, m_relationShip, parentFullName);
                    QString foundName = getDisplayName(id);
                    if (nameWithSpouse == foundName ) {
                        message("Name already exist");
                        return;
                    }
                    m_contactIDFullNameToUpdate = id;
                }

            }
            if (isSpouseAdded) {
                // check if other spouse name already exist

                int id = findContactID(parentFullName);
                if (id > 0) {
                    m_contactParentFullNameToUpdate = id;
                }

            }
        }


    } else {
        if (checkName) {
           QString nameWithSpouse = CsdUtils::createFullName("", m_firstName, m_lastName);
           m_firstName = nameWithSpouse;
           m_lastName = "";
            m_fullName = nameWithSpouse;
            if (m_forImport == false) {
            //check if name already exist
                if (nameWithSpouse.isEmpty() == false) {
                    TobjectList values;
                    QString condition = CcontactTable::Object()->getSearchString(nameWithSpouse, CONTACT_FULL_NAME_WITH_SPOUSE_IDX);
                    if ( CcontactTable::Object()->getTableValues(condition, values)) {
                        int id = values.at(0).at(CONTACT_ID_IDX).toInt();
                        if (m_contactEditID > 0) {
                            if (id != m_contactEditID) {
                                message("Name already exist");
                                return;
                            }
                        } else {
                            message("Name alrady exist");
                            return;
                        }
                    }
                }
            } else {
                if (m_suggestedName.isEmpty() == false) {
                    m_fullName = m_suggestedName;
                    m_firstName = m_suggestedName;
                }
            }
        }
    }

    if (m_parentSpouse.isEmpty() == false) {
        if (m_nameWithSpouseMap.find(m_parentSpouse) == m_nameWithSpouseMap.end()) {
            if (m_parentFirstName.isEmpty()) {
                message("Parent first name empty");
                return;
            }
        }
    }

    emit accept();
}
bool CcontactDLG::isPerson() const
{
    return m_isPerson;
}

void CcontactDLG::setIsPerson(bool isPerson)
{
    on_m_dataForPersoncheckBox_clicked(isPerson);
    m_isPerson = isPerson;
}

QString CcontactDLG::fullName() const
{
    return m_fullName;
}

void CcontactDLG::setFullName(const QString &fullName)
{
    m_fullName = fullName;
}
/*
QString CcontactDLG::nameWithSpouse() const
{
    return m_nameWithSpouse;
}

void CcontactDLG::setNameWithSpouse(const QString &nameWithSpouse)
{
    m_nameWithSpouse = nameWithSpouse;
}*/


void CcontactDLG::populatePersonForEdit()
{
    ui->m_dataForPersoncheckBox->setChecked(true);

    setIsPerson(true);

    SpersonData* person =  CpersonTable::Object()->getPersonData(m_personEditID);
    if (person) {
        m_title = person->m_title;
        m_firstName = person->m_firstName;
        m_lastName = person->m_lastName;
        m_relationShip = person->m_relationship;
        m_parentSpouse = person->getParentFullName();
        m_parentSpouseContactID = findContactID(m_parentSpouse);
        if (m_parentSpouseContactID  == 0) {
            m_parentSpouse = "";
        }

        m_gender = person->m_gender;
        m_phone = person->m_phone;
        //qDebug()<<"edit for "<<m_firstName<<" phone "<<m_phone;
        delete person;
    }
}

void CcontactDLG::populateOtherForEdit()
{
    ui->m_dataForPersoncheckBox->setChecked(false);

    setIsPerson(false);
    TobjectList values;

    QString condition = CotherContactTable::Object()->getSearchString(QString::number(m_otherEditID), OTHER_CONTACT_ID_IDX);
    if ( CotherContactTable::Object()->getTableValues(condition, values)) {

        m_firstName = values.at(0).at(OTHER_NAME_IDX).toString();
        m_phone = values.at(0).at(OTHER_PHONE_IDX).toString();
    }


}

void CcontactDLG::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int x = XCORD;
    int y = YCORD;

    int btnWidth  = BUTTON_SIZE.width();
    int btnHeight = BUTTON_SIZE.height();

    ui->m_dataForPersoncheckBox->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    int width = windowwidth - (2*XCORD + GAP + DATE_SIZE.width());
    x +=  GAP + DATE_SIZE.width();
    ui->m_name->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x = XCORD;
    y += GAP +  DEFAULT_HEIGHT;
    width = windowwidth - 2*XCORD;
    int height = DEFAULT_HEIGHT*2 + (6*GAP);
    ui->m_nameBox->setGeometry(x, y, width, height);
    int y1 = y;
    x = GAP;
    y = 2*GAP;

    //width = DEFAULT_WIDTH*4.2;
    int titleWidth = (DEFAULT_WIDTH*2);
    width = (width - (titleWidth + 4*GAP))/2;
    ui->m_titleLbl->setGeometry(x, y, titleWidth , DEFAULT_HEIGHT);
    x += GAP + titleWidth;
    ui->m_firstNameLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP + width;
    ui->m_lastNameLbl->setGeometry(x, y,  width, DEFAULT_HEIGHT);
    x = GAP;
    y += GAP + DEFAULT_HEIGHT;
    ui->m_titleComboBox->setGeometry(x, y, titleWidth, DEFAULT_HEIGHT);
    x += GAP + titleWidth;
    ui->m_firstNameLineEdit->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP + width;
    ui->m_lastName->setGeometry(x, y,  width, DEFAULT_HEIGHT);


    y = y1 + height + GAP;
    x = XCORD;
    ui->m_relationShipLbl->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP + DATE_SIZE.width();
    ui->m_genderLbl->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP + DATE_SIZE.width();
    ui->m_parentSpuseLbl->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width(), DEFAULT_HEIGHT);
    y += GAP + DEFAULT_HEIGHT;
    x = XCORD;
    ui->m_relationShipCombo->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP + DATE_SIZE.width();
    ui->m_genderCombo->setGeometry(x, y, DATE_SIZE.width(), DEFAULT_HEIGHT);
    x += GAP + DATE_SIZE.width();
    width = windowwidth - (2*XCORD + GAP + DATE_SIZE.width() +GAP + DATE_SIZE.width() );
    ui->m_parentSpouse->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += GAP + DEFAULT_HEIGHT;
    x = XCORD;
    ui->m_phoneLbl->setGeometry(x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    width = windowwidth - ((2*XCORD) + GAP +  DEFAULT_WIDTH);
    x += DEFAULT_WIDTH + GAP;
    ui->m_phone->setGeometry(x, y, width, DEFAULT_HEIGHT);
    y += GAP + DEFAULT_HEIGHT;
    y1 = y;

    x = XCORD;
    width = windowwidth - 2*XCORD;
    ui->m_parentgroupBox->setGeometry(x, y, width, height);
    width = (width - (titleWidth + 4*GAP))/2;
    x = GAP;
    y = 2*GAP;
    ui->m_parentTitleLbl->setGeometry(x, y, titleWidth , DEFAULT_HEIGHT);
    x += GAP + titleWidth;
    ui->m_parentFirstNameLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP + width;
    ui->m_parentLastNameLbl->setGeometry(x, y,  width, DEFAULT_HEIGHT);
    x = GAP;
    y += GAP + DEFAULT_HEIGHT;
    ui->m_parentTitleCombo->setGeometry(x, y, titleWidth, DEFAULT_HEIGHT);
    x += GAP + titleWidth;
    ui->m_parentFirstName->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP + width;
    ui->m_parentLastName->setGeometry(x, y,  width, DEFAULT_HEIGHT);

    y = y1 + height + DEFAULT_HEIGHT;

    x = windowwidth/2 - BUTTON_SIZE.width() -GAP/2;
    ui->m_okButton->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += BUTTON_SIZE.width() + GAP;
    ui->m_cancelButton->setGeometry(x, y,  BUTTON_SIZE.width(), BUTTON_SIZE.height());

}
QString CcontactDLG::phone() const
{
    return m_phone;
}

void CcontactDLG::setPhone(const QString &phone)
{
    m_phone = phone;
}

QString CcontactDLG::gender() const
{
    return m_gender;
}

void CcontactDLG::setGender(const QString &gender)
{
    m_gender = gender;
}

QString CcontactDLG::parentSpouse() const
{
    return m_parentSpouse;
}

void CcontactDLG::setParentSpouse(const QString &parentSpouse)
{
    m_parentSpouse = parentSpouse;
}



QString CcontactDLG::relationShip() const
{
    return m_relationShip;
}

void CcontactDLG::setRelationShip(const QString &relationShip)
{
    m_relationShip = relationShip;
}

QString CcontactDLG::lastName() const
{
    return m_lastName;
}

void CcontactDLG::setLastName(const QString &lastName)
{
    m_lastName = lastName;
}

QString CcontactDLG::firstName() const
{
    return m_firstName;
}

void CcontactDLG::setFirstName(const QString &firstName)
{
    m_firstName = firstName;
}

QString CcontactDLG::title() const
{
    return m_title;
}

void CcontactDLG::setTitle(const QString &title)
{
    m_title = title;
}

int CcontactDLG::contactEditID() const
{
    return m_contactEditID;
}

bool CcontactDLG::setContactEditID(int contactEditID)
{
    m_contactEditID = contactEditID;
    ScontactData* data  =  CcontactTable::Object()->getContactFromID(m_contactEditID);
    m_title.clear();
    m_firstName.clear();
    m_lastName.clear();
    m_relationShip.clear();
    m_parentSpouse.clear();
    m_gender.clear();
    m_phone.clear();
    m_parentFirstName.clear();
    m_parentLastName.clear();
    m_parentTitle.clear();
    if (data) {
        m_personEditID = data->m_personTableID;
        m_otherEditID = data->m_otherContactTableId;
        delete data;
    } else {
        return false;
    }
    if (m_personEditID > 0) {
        populatePersonForEdit();
    } else if (m_otherEditID > 0) {
        populateOtherForEdit();
    }


    ui->m_titleComboBox->setCurrentText(m_title);
    ui->m_firstNameLineEdit->setText(m_firstName);
    ui->m_lastName->setText(m_lastName);
    ui->m_relationShipCombo->setCurrentText(m_relationShip);
    ui->m_parentSpouse->setText(m_parentSpouse);
    ui->m_genderCombo->setCurrentText(m_gender);
    ui->m_phone->setText(m_phone);
    ui->m_dataForPersoncheckBox->setEnabled(false);
    ui->m_parentTitleCombo->setCurrentText("");
    ui->m_parentFirstName->clear();
    ui->m_parentLastName->clear();


    return true;
}


void CcontactDLG::on_m_parentSpouse_editingFinished()
{
    m_parentSpouse = ui->m_parentSpouse->text().trimmed().simplified();
    m_parentSpouseContactID =0;
    m_parentSpouseContactID = findContactID(m_parentSpouse);
    if (m_parentSpouseContactID == 0) {
        ui->m_parentSpouse->clear();
        ui->m_parentgroupBox->show();
        m_parentSpouse.clear();
    } else {
        ui->m_parentgroupBox->hide();
    }

}
QString CcontactDLG::parentLastName() const
{
    return m_parentLastName;
}

void CcontactDLG::setParentLastName(const QString &parentLastName)
{
    m_parentLastName = parentLastName;
}

QString CcontactDLG::parentFirstName() const
{
    return m_parentFirstName;
}

void CcontactDLG::setParentFirstName(const QString &parentFirstName)
{
    m_parentFirstName = parentFirstName;
}

QString CcontactDLG::parentTitle() const
{
    return m_parentTitle;
}

void CcontactDLG::setParentTitle(const QString &parentTitle)
{
    m_parentTitle = parentTitle;
}

int CcontactDLG::otherEditID() const
{
    return m_otherEditID;
}

void CcontactDLG::setOtherEditID(int otherEditID)
{
    m_otherEditID = otherEditID;
}

int CcontactDLG::personEditID() const
{
    return m_personEditID;
}

void CcontactDLG::setPersonEditID(int personEditID)
{
    m_personEditID = personEditID;
}


void CcontactDLG::on_m_relationShipCombo_currentTextChanged(const QString &arg1)
{
    if (arg1.compare("s/o") == 0) {
        ui->m_genderCombo->setCurrentText("Male");
        ui->m_genderLbl->hide();
        ui->m_genderCombo->hide();
    } else if (arg1.compare("w/o") == 0) {
        ui->m_genderCombo->setCurrentText("Female");
        ui->m_genderLbl->hide();
        ui->m_genderCombo->hide();
    } else if (arg1.compare("d/o") == 0) {
        ui->m_genderCombo->setCurrentText("Female");
        ui->m_genderLbl->hide();
        ui->m_genderCombo->hide();
    } else {
        ui->m_genderLbl->show();
        ui->m_genderCombo->show();
    }
}


void CcontactDLG::on_m_titleComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1.compare("Mr") == 0 || arg1.compare("Rev") == 0 || arg1.compare("Pastor") == 0 || arg1.compare("Upa") == 0) {
        ui->m_genderCombo->setCurrentText("Male");
        ui->m_genderLbl->hide();
        ui->m_genderCombo->hide();

    } else if (arg1.compare("Miss") == 0 || arg1.compare("Ms") == 0 || arg1.compare("Mrs") == 0) {
        ui->m_genderLbl->hide();
        ui->m_genderCombo->hide();
        ui->m_genderCombo->setCurrentText("Female");

    } else {
        ui->m_genderLbl->show();
        ui->m_genderCombo->show();
    }
}

QString CcontactDLG::suggestedName() const
{
    return m_suggestedName;
}

bool CcontactDLG::forImport() const
{
    return m_forImport;
}

void CcontactDLG::setForImport(SimportMinData* data)
{
    m_forImport = data ? true: false;
    if (m_forImport) {
        ui->m_name->show();
        if (data->m_isPerson) {
            ui->m_dataForPersoncheckBox->setChecked(true);
            on_m_dataForPersoncheckBox_clicked(true);
            setIsPerson(true);

            m_title = data->m_title;
            ui->m_titleComboBox->setCurrentText(m_title);
            m_firstName = data->m_firstName;
            ui->m_firstNameLineEdit->setText(m_firstName);
            m_lastName = data->m_lastName;
            ui->m_lastName->setText(m_lastName);
            m_relationShip = data->m_relationship;
            ui->m_relationShipCombo->setCurrentText(m_relationShip);
            //ui->m_parentSpouse->setText(m_parentSpouse);
            //ui->m_genderCombo->setCurrentText(m_gender);
            m_phone = data->m_phone;
            ui->m_phone->setText(m_phone);
            ui->m_dataForPersoncheckBox->setEnabled(true);
            //ui->m_parentTitleCombo->setCurrentText("");
            m_parentTitle= "";
            m_parentFirstName = data->m_parentFirstName;
            ui->m_parentFirstName->setText(m_parentFirstName);
            m_parentLastName = data->m_parentLastName;
            ui->m_parentLastName->setText(m_parentLastName);
            ui->m_parentgroupBox->show();
            if (data->m_isPerson) {
                ui->m_dataForPersoncheckBox->setChecked(true);
            } else {
                ui->m_dataForPersoncheckBox->setChecked(false);
            }

            int nameID = findContactID(m_title, m_firstName, m_lastName, m_relationShip, m_parentTitle, m_parentFirstName, m_parentLastName);
            int parentID = findContactID(m_parentTitle, m_parentFirstName, m_parentLastName, "", "", "", "");



            m_suggestedName = "";
            if (nameID > 0) {
                m_suggestedName = getDisplayName(nameID);
                m_suggestedNameContactID = nameID;
            }
            //m_suggestedName = CsdUtils::createFullName(m_title, m_firstName, m_lastName);

            if (m_suggestedName.isEmpty() == false){
                ui->m_name->setText(m_suggestedName);
            }
            m_parentSpouse = "";
            if (parentID) {
                m_parentSpouse = getDisplayName(parentID);
            }

            if (m_parentSpouse.isEmpty() == false){
                ui->m_parentSpouse->setText(m_parentSpouse);
                m_parentSpouseContactID = parentID;
            }


        } else {
            ui->m_dataForPersoncheckBox->setChecked(false);
            on_m_dataForPersoncheckBox_clicked(false);
            setIsPerson(false);
            m_suggestedName = "";
            int id = findContactID("", data->m_firstName, data->m_lastName, "", "", "", "");
            if (id > 0) {
                m_suggestedName = getDisplayName(id);
                m_suggestedNameContactID = id;
            }
            ui->m_name->show();
            m_firstName = data->m_firstName;
            m_lastName = data->m_lastName;
            QString name = CsdUtils::createFullName("", m_firstName, m_lastName);
            m_firstName = name;
            m_lastName = "";
            m_phone = data->m_phone;
            ui->m_firstNameLineEdit->setText(m_firstName);
            ui->m_lastName->setText(m_lastName);
            ui->m_phone->setText(m_phone);
            if (m_suggestedName.isEmpty() == false){
                ui->m_name->setText(m_suggestedName);
            }


        }



    } else {
        ui->m_name->hide();

    }
}


void CcontactDLG::on_m_name_editingFinished()
{
    if (forImport()) {
        m_suggestedName = ui->m_name->text().trimmed().simplified();
        m_suggestedNameContactID = findContactID(m_suggestedName);
        if (m_suggestedNameContactID == 0) {

            ui->m_name->clear();
            m_suggestedName.clear();
            ui->m_nameBox->show();
            ui->m_parentgroupBox->show();

            ui->m_genderCombo->show();
            ui->m_genderLbl->show();
            ui->m_relationShipCombo->show();
            ui->m_relationShipLbl->show();
            ui->m_parentSpouse->show();
            ui->m_parentSpuseLbl->show();
            ui->m_phone->show();
            ui->m_phoneLbl->show();
        } else {
            ui->m_nameBox->hide();
            ui->m_parentgroupBox->hide();

            ui->m_genderCombo->hide();
            ui->m_genderLbl->hide();
            ui->m_relationShipCombo->hide();
            ui->m_relationShipLbl->hide();
            ui->m_parentSpouse->hide();
            ui->m_parentSpuseLbl->hide();
            ui->m_phone->hide();
            ui->m_phoneLbl->hide();
        }
    }
}

int CcontactDLG::findContactID(QString name)
{
    int id = 0;
    if (name.isEmpty() == false) {
        auto fn1 = m_nameWithSpouseMap.find(name);
        if (fn1 != m_nameWithSpouseMap.end()) {
            id = fn1.value();
        } else {
            //QMap<QString, QSet<int> >
            auto fn = m_nameIDMap.find(name);
            if (fn != m_nameIDMap.end()) {
                if (fn.value().size() == 1) {
                    id = *(fn.value().begin());
                }
            }

        }
    }
    return id;
}

int CcontactDLG::findContactID(QString title, QString firstName, QString lastName, QString relationship, QString parentTitle, QString parentFirstName, QString parentLastName)
{
    QString fullName = CsdUtils::createFullName(title, firstName, lastName);
    QString fullNameWithoutTitle =  CsdUtils::createFullName("", firstName, lastName);

    int id = 0;
    if (parentFirstName.isEmpty()) {
        id = findContactID(fullName);
        if (id > 0) {
            return id;
        }
        id = findContactID(fullNameWithoutTitle);
        if (id > 0) {
            return id;
        }

    }

    QString parentFullName;
    QString parentFullNameWithoutTitle;
    if (parentFirstName.isEmpty() == false) {
        parentFullName = CsdUtils::createFullName(parentTitle, parentFirstName, parentLastName);
        parentFullNameWithoutTitle = CsdUtils::createFullName("", parentFirstName, parentLastName);
    }

    QString name = CsdUtils::createFullNameWithSpouse(fullName, relationship, parentFullName);
    id = findContactID(name);
    if (id > 0) {
        return id;
    }

    name = CsdUtils::createFullNameWithSpouse(fullName, "", parentFullName);
    id = findContactID(name);
    if (id > 0) {
        return id;
    }

    name = CsdUtils::createFullNameWithSpouse(fullNameWithoutTitle, relationship, parentFullNameWithoutTitle);
    id = findContactID(name);
    if (id > 0) {
        return id;
    }


    name = CsdUtils::createFullNameWithSpouse(fullNameWithoutTitle, "", parentFullNameWithoutTitle);
    id = findContactID(name);
    if (id > 0) {
        return id;
    }

    return id;
}

QString CcontactDLG::getDisplayName(int id)
{
    QString name;
    if (id > 0) {
        auto fn = m_displayName.find(id);
        if (fn != m_displayName.end()) {
            name = fn.value();
        }
    }
    return name;
}

QString CcontactDLG::getFullName(int id)
{
    QString name;
    if (id > 0) {
        auto fn = m_allFullName.find(id);
        if (fn != m_allFullName.end()) {
            name = fn.value();
        }
    }
    return name;

}
int CcontactDLG::contactParentFullNameToUpdate() const
{
    return m_contactParentFullNameToUpdate;
}
int CcontactDLG::parentSpouseContactID() const
{
    return m_parentSpouseContactID;
}
