#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qtTranslator = new QTranslator(this);

    if(qtTranslator->load("qt_de", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(qtTranslator);

    ui->setupUi(this);

    init();
}

void MainWindow::init()
{
    plzWindow = nullptr;

    ignoreClose = false;

    statusLabel = new QLabel(this);

    statusLabel->setIndent(5);

    statusBar()->addWidget(statusLabel, 1);

    progressBar = new QProgressBar(this);

    progressBar->setFixedWidth(200);

    QString styleSheet = "QProgressBar { text-align: center; }";

    progressBar->setStyleSheet(styleSheet);

    progressBar->setVisible(false);

    statusBar()->addPermanentWidget(progressBar);

    enableDatabase(openDatabase());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionBEenden_triggered()
{
    close();
}

bool MainWindow::openDatabase()
{
    QString driver = "QODBC";

    QString driverName = "DRIVER={SQL Server}";

    QString server = "localhost\\SQLEXPRESS";

    QString databaseName = "alfatraining";

    bool retValue = DAOLib::connectToDatabase(driver, driverName, server, databaseName);

    return retValue;
}

void MainWindow::enableDatabase(bool bEnable)
{
    ui->menuStammdaten->setEnabled(bEnable);
    ui->menuExtras->setEnabled(bEnable);

    if(bEnable)
        statusLabel->setText("Datenbank: " + DAOLib::getDatabaseName());
    else
        statusLabel->setText("Datenbank: (keine)");
}

void MainWindow::importPostleitzahlen()
{
    QString currentPath = QDir::homePath();

    QString defaultFilter = "Textdokumente (*.txt)";

    QString filename = QFileDialog::getOpenFileName(this, "Textdokument öffnen", currentPath,
                                          "Alle Dateien (*.*);;" + defaultFilter, &defaultFilter);

    if(filename.isEmpty())
        return;

    importPLZIntoDatabase(filename);
}

void MainWindow::importPLZIntoDatabase(const QString &filename)
{
    QString line;
    QString statusText;
    QString sOrt;

    int insertCounter = 0;
    int recordCounter = 0;

    int msgValue;

    qint64 progressValue = 0;
    qint64 fileSize = 0;

    statusText = statusLabel->text();

    stopImport = false;

    ignoreClose = true;

    ui->menubar->setEnabled(false);

    if(PostleitzahlenDAO::getRowCount() > 0)
    {
        msgValue = QMessageBox::question(this, "PLZ importieren", "Gelöscht",
                                         QMessageBox::Yes |QMessageBox::No | QMessageBox::Cancel,
                                         QMessageBox::Cancel);

        if(msgValue == QMessageBox::Cancel)
            return;

        if(msgValue == QMessageBox::Yes)
        {
            statusLabel->setText("Datensätze werden gelöscht...");

            QApplication::processEvents();

            PostleitzahlenDAO::deleteTable();
        }
    }

    ui->actionPLZImportiern->setEnabled(false);

    QApplication::processEvents();

    statusLabel->setText("PLZ werden importiert. Abbrechen mit der ESC-Taste...");

    QApplication::processEvents();

//    progressBar->setMinimum(0);

//    progressBar->setMaximum(getRecordCount(filename));

//    progressBar->setValue(0);

//    progressBar->setVisible(true);

    progressBar->setMinimum(0);

    progressBar->setMaximum(100);

    progressBar->setValue(0);

    progressBar->setVisible(true);

    fileSize = getFileSize(filename);

    QFile file(filename);

    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);

        while(!in.atEnd())
        {
            if(stopImport)
            {
                msgValue = QMessageBox::question(this, "PLZ importieren", "Abgebrochen",
                                                 QMessageBox::Yes |QMessageBox::No, QMessageBox::No);

                if(msgValue == QMessageBox::Yes)
                    break;

                stopImport = false;
            }

            line = in.readLine();

            recordCounter++;

//            if(recordCounter % 100 == 0)
//            {
//                progressBar->setValue(recordCounter);

//                QApplication::processEvents();
//            }

            progressValue += line.length() + 2;

            if(recordCounter % 100 == 0)
            {
                progressBar->setValue(static_cast<int>(progressValue * 100 / fileSize));

                QApplication::processEvents();
            }

            QStringList splitList = line.split(";", Qt::KeepEmptyParts);

            if(splitList.size() >= 2)
            {
                sOrt = splitList[1];

                for(int i = 2; i < splitList.size(); i++)
                    sOrt += "," + splitList[i];
            }

            if(PostleitzahlenDAO::postleitzahlExists(splitList[0], sOrt))
                continue;

            if(PostleitzahlenDAO::insertPostleitzahl(splitList[0], sOrt))
                insertCounter++;
        }

        file.close();
    }

    statusLabel->setText(statusText);

    progressBar->setVisible(false);

    ui->actionPLZImportiern->setEnabled(true);

    ignoreClose = false;

    ui->menubar->setEnabled(true);

    QMessageBox::information(this, "PLZ importieren",
                          QString::number(recordCounter) + ", " + QString::number(insertCounter));
}

int MainWindow::getRecordCount(const QString &filename)
{
    int retValue = 0;

    QFile file(filename);

    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);

        while(in.atEnd())
        {
            in.readLine();

            retValue++;
        }

        file.close();
    }

    return retValue;
}

qint64 MainWindow::getFileSize(const QString &filename)
{
    qint64 retValue = 0;

    QFile file(filename);

    if(file.open(QFile::ReadOnly))
    {
        retValue = file.size();

        file.close();
    }

    return retValue;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(ignoreClose)
    {
        event->ignore();
    }
    else
    {
        DAOLib::closeConnection();

        event->accept();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
        stopImport = true;
}

void MainWindow::on_actionPLZImportiern_triggered()
{
    importPostleitzahlen();
}


void MainWindow::on_actionPostleitzahlen_triggered()
{
    delete plzWindow;

    plzWindow = new PLZWindow(this);

    plzWindow->show();
}

