#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>

#include "DAOLib.h"
#include "PostleitzahlenDAO.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionBEenden_triggered();

    void on_actionPLZImportiern_triggered();

private:
    Ui::MainWindow *ui;

    QLabel* statusLabel;

    void init();
    bool openDatabase();
    void enableDatabase(bool bEnable);
    void importPostleitzahlen();
    void importPLZIntoDatabase(const QString &filename);

    void closeEvent(QCloseEvent* event) override;
};
