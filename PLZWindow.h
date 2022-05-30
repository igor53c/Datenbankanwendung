#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QKeyEvent>

#include "PostleitzahlenDAO.h"

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

    void tableView_selectionChanged();

private:
    Ui::PLZWindow *ui;

    QLabel* statusLabel;

    void init();

    void showTable();

    QSqlTableModel* setTableViewModel();

    bool eventFilter(QObject* sender, QEvent* event) override;
};

