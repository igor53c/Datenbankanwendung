#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QProgressBar>
#include <QTranslator>
#include <QLibraryInfo>
#include <QPushButton>

#include "DAOLib.h"
#include "PostleitzahlenDAO.h"
#include "PLZWindow.h"

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

    void on_actionPostleitzahlen_triggered();

    void onImport_Cancel();

private:
    Ui::MainWindow *ui;

    PLZWindow* plzWindow;

    QLabel* statusLabel;

    QProgressBar* progressBar;

    QTranslator* qtTranslator;

    QPushButton* cancelButton;

    bool stopImport;

    bool ignoreClose;

    void init();
    bool openDatabase();
    void enableDatabase(bool bEnable);
    void importPostleitzahlen();
    void importPLZIntoDatabase(const QString &filename);
    int getRecordCount(const QString&);
    qint64 getFileSize(const QString&);

    void closeEvent(QCloseEvent* event) override;
};
