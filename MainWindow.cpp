#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

void MainWindow::init()
{
    statusLabel = new QLabel(this);

    statusLabel->setIndent(5);

    statusBar()->addWidget(statusLabel, 1);

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

    statusText = statusLabel->text();

    ui->actionPLZImportiern->setEnabled(false);

    statusLabel->setText("PLZ werden importieren...");

    QApplication::processEvents();

    QFile file(filename);

    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);

        while(!in.atEnd())
        {
            line = in.readLine();

            recordCounter++;

            if(recordCounter % 1000 == 0)
            {
                statusLabel->setText(QString::number(recordCounter));
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

    ui->actionPLZImportiern->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DAOLib::closeConnection();

    event->accept();
}

void MainWindow::on_actionPLZImportiern_triggered()
{
    importPostleitzahlen();
}

