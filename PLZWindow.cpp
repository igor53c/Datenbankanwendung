#include "PLZWindow.h"
#include "ui_PLZWindow.h"

PLZWindow::PLZWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PLZWindow)
{
    ui->setupUi(this);

    init();
}

void PLZWindow::init()
{
    statusLabel = new QLabel(this);

    statusLabel->setAlignment(Qt::AlignCenter);

    statusBar()->addWidget(statusLabel, 1);

    ui->tableView->installEventFilter(this);

    showTable();
}

PLZWindow::~PLZWindow()
{
    delete ui->tableView->model();

    delete ui;
}

void PLZWindow::showTable()
{
    QSqlTableModel* model = setTableViewModel();

    QFont font = ui->tableView->horizontalHeader()->font();

    font.setPixelSize(15);

    ui->tableView->horizontalHeader()->setFont(font);

    ui->tableView->horizontalHeader()->setFixedHeight(35);

    ui->tableView->horizontalHeader()->setStyleSheet("color: blue;");

    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignmentFlag::AlignLeft |
                                                           Qt::AlignmentFlag::AlignVCenter);

    // ui->tableView->hideColumn(0);

    ui->tableView->hideColumn(model->record().indexOf("PRIMARYKEY"));

    ui->tableView->hideColumn(model->record().indexOf("TIMESTAMP"));

    ui->tableView->setColumnWidth(model->record().indexOf("PLZ"), 100);

    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    ui->tableView->setEnabled(model->rowCount() > 0);

    ui->actionNdern->setEnabled(ui->tableView->isEnabled());

    ui->actionLschen->setEnabled(ui->tableView->isEnabled());

    if(ui->tableView->isEnabled())
        ui->tableView->selectRow(0);
}

QSqlTableModel *PLZWindow::setTableViewModel()
{
    QSqlTableModel* model = PostleitzahlenDAO::readPLZIntoTableModel();

    model->setHeaderData(model->record().indexOf("PLZ"), Qt::Horizontal, "Postleitzahl");

    model->setHeaderData(model->record().indexOf("ORT"), Qt::Horizontal, "Ort");

    model->sort(model->record().indexOf("PLZ"), Qt::AscendingOrder);

    ui->tableView->setModel(model);

    while(model->canFetchMore())
        model->fetchMore();

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &PLZWindow::tableView_selectionChanged);

    return model;
}

bool PLZWindow::eventFilter(QObject *sender, QEvent *event)
{
    if(sender == ui->tableView)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if(keyEvent->key() == Qt::Key_Home)
            {
                ui->tableView->scrollToTop();

                ui->tableView->selectRow(0);
            }
            else if(keyEvent->key() == Qt::Key_End)
            {
                ui->tableView->scrollToBottom();

                ui->tableView->selectRow(ui->tableView->model()->rowCount() - 1);
            }
        }
    }

    return QObject::eventFilter(sender, event);
}

void PLZWindow::on_actionSchliEen_triggered()
{
    close();
}

void PLZWindow::tableView_selectionChanged()
{
    int currentRow = ui->tableView->selectionModel()->currentIndex().row() + 1;

    int rowCount = ui->tableView->model()->rowCount();

    QString msg = QString("Datensatz %L1 von %L2").arg(currentRow).arg(rowCount);

    statusLabel->setText(msg);
}

