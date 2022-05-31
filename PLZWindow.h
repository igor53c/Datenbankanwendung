#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QKeyEvent>

#include "PostleitzahlenDAO.h"
#include "PLZDialog.h"

namespace Ui {
class PLZWindow;
}

class PLZWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PLZWindow(QWidget *parent = nullptr);
    ~PLZWindow();

private slots:
    void on_actionSchliEen_triggered();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_actionNdern_triggered();

    void on_actionNeu_triggered();

    void tableView_selectionChanged();

    void modifyTableView(const qint64 key, const PLZDialog::EditMode);

    void on_actionLschen_triggered();

private:
    Ui::PLZWindow *ui;

    QLabel* statusLabel;

    void init();

    void showTable();

    void showPLZDialog(const qint64 key);

    QSqlTableModel* setTableViewModel();

    bool eventFilter(QObject* sender, QEvent* event) override;

    void refreshTableView(const qint64 key);

    void findItemInTableView(const QString& columnName, const QVariant& value);

    void updateTableView(const qint64 key);

    void deleteEntry(const QModelIndex& index);
};
