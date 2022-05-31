#pragma once

#include <QDialog>
#include <QCloseEvent>

#include "Postleitzahl.h"
#include "PostleitzahlenDAO.h"

namespace Ui {
class PLZDialog;
}

class PLZDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PLZDialog(qint64 key, QWidget *parent = nullptr);
    ~PLZDialog();

    enum EditMode
    {
        NEW,
        UPDATE
    };

signals:
    void dataModified(const qint64 key, const EditMode editMode);

private slots:
    void on_textPLZ_textChanged(const QString &arg1);

    void on_textOrt_textChanged(const QString &arg1);

    void on_textPLZ_returnPressed();

    void on_textOrt_returnPressed();

    void on_btnAbbrechen_clicked();

    void on_btnSpeichern_clicked();

private:
    Ui::PLZDialog *ui;

    qint64 dlgKey;

    bool isModified;

    QString timestamp;

    void init();

    void readEntry(qint64 key);

    bool querySave();

    bool saveEntry();

    bool entryIsValid();

    bool updateEntry(qint64 key);

    bool insertEntry();

    void closeEvent(QCloseEvent* event) override;

    void reject() override;
};

