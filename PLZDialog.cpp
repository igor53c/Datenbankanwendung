#include "PLZDialog.h"
#include "ui_PLZDialog.h"

PLZDialog::PLZDialog(qint64 key, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PLZDialog)
{
    ui->setupUi(this);

    dlgKey = key;

    init();
}

PLZDialog::~PLZDialog()
{
    delete ui;
}

void PLZDialog::init()
{
    ui->textPLZ->setInputMask("99999");

    if(dlgKey > 0)
        readEntry(dlgKey);

    isModified = false;

    ui->btnSpeichern->setEnabled(false);


}

void PLZDialog::readEntry(qint64 key)
{
    Postleitzahl* plz = PostleitzahlenDAO::readPostleitzahl(key);

    if(plz == nullptr)
        return;

    ui->textPLZ->setText(plz->getPLZ());
    ui->textOrt->setText(plz->getOrt());

    timestamp = plz->getTimestamp();

    delete plz;
}

bool PLZDialog::querySave()
{
    bool retValue = false;

    if(!isModified)
        return true;

    int msgValue = QMessageBox::warning(this, this->windowTitle(), "Gespeichert",
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                        QMessageBox::Cancel);

    if(msgValue == QMessageBox::Discard)
        retValue = true;
    else if(msgValue == QMessageBox::Cancel)
        retValue = false;
    else
        retValue = saveEntry();

    return retValue;
}

bool PLZDialog::saveEntry()
{
    bool retValue = false;

    if(!entryIsValid())
        return retValue;

    if(dlgKey > 0)
    {
        retValue = updateEntry(dlgKey);

        if(retValue)
        {
            emit dataModified(dlgKey, EditMode::UPDATE);
        }
    }
    else
    {
        retValue = insertEntry();

        if(retValue)
        {
            emit dataModified(dlgKey, EditMode::NEW);
        }
    }

    isModified = !retValue;

    return retValue;
}

bool PLZDialog::entryIsValid()
{
    bool retValue = true;

    if(ui->textPLZ->text().length() == 0)
    {
        QMessageBox::critical(this, this->windowTitle(), "Eingabe fehlt");

        ui->textPLZ->setFocus();

        retValue = false;
    }
    else if(ui->textOrt->text().length() == 0)
    {
        QMessageBox::critical(this, this->windowTitle(), "Eingabe fehlt");

        ui->textOrt->setFocus();

        retValue = false;
    }
    else if(PostleitzahlenDAO::postleitzahlExists(ui->textPLZ->text(), ui->textOrt->text()))
    {
        QMessageBox::critical(this, this->windowTitle(), "Schon existiert");

        ui->textPLZ->setFocus();

        retValue = false;
    }

    return retValue;
}

bool PLZDialog::updateEntry(qint64 key)
{
    Postleitzahl* plz = PostleitzahlenDAO::readPostleitzahl(key);

    if(plz == nullptr)
        return false;

    QString currentTimestamp = plz->getTimestamp();

    delete plz;

    if(timestamp != currentTimestamp)
    {
        QMessageBox::warning(this, this->windowTitle(), "geändert");

        return true;
    }

    return PostleitzahlenDAO::updatePostleitzahl(key, ui->textPLZ->text(), ui->textOrt->text());
}

bool PLZDialog::insertEntry()
{
    bool retValue = false;

    if(PostleitzahlenDAO::insertPostleitzahl(ui->textPLZ->text(), ui->textOrt->text()))
    {
        dlgKey = DAOLib::getLastIdentity();

        retValue = true;
    }

    return retValue;
}

void PLZDialog::closeEvent(QCloseEvent *event)
{
    if(querySave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void PLZDialog::reject()
{
    // return;

    close();
}

void PLZDialog::on_textPLZ_textChanged(const QString &)
{
    ui->btnSpeichern->setEnabled(true);

    isModified = true;
}

void PLZDialog::on_textOrt_textChanged(const QString &)
{
    ui->btnSpeichern->setEnabled(true);

    isModified = true;
}

void PLZDialog::on_textPLZ_returnPressed()
{
    this->focusNextChild();
}

void PLZDialog::on_textOrt_returnPressed()
{
    this->focusNextChild();
}

void PLZDialog::on_btnAbbrechen_clicked()
{
    close();
}

void PLZDialog::on_btnSpeichern_clicked()
{
    if(isModified)
    {
        if(!saveEntry())
            return;
    }

    close();
}

