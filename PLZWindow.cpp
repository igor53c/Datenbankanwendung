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

    QPalette palette = ui->tableView->palette();

    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Highlight, QColor(0, 112, 255));

    ui->tableView->setPalette(palette);

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

void PLZWindow::showPLZDialog(const qint64 key)
{
    PLZDialog plzDlg(key, this);

    connect(&plzDlg, &PLZDialog::dataModified, this, &PLZWindow::modifyTableView);

    plzDlg.exec();
}

QSqlTableModel *PLZWindow::setTableViewModel()
{
    delete ui->tableView->model();

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
            else if(keyEvent->key() == Qt::Key_Return)
            {
                on_tableView_doubleClicked(ui->tableView->currentIndex());
            }
            else if(keyEvent->key() == Qt::Key_Delete)
            {
                deleteEntry(ui->tableView->currentIndex());
            }
        }
    }

    return QObject::eventFilter(sender, event);
}

void PLZWindow::refreshTableView(const qint64 key)
{
    statusLabel->setText("Einträge werden aktualisiert...");

    QApplication::processEvents();

    setTableViewModel();

    findItemInTableView("PRIMARYKEY", QVariant(key));
}

void PLZWindow::findItemInTableView(const QString &columnName, const QVariant &value)
{
    int row;

    bool found = false;

    QSqlTableModel* model = static_cast<QSqlTableModel*>(ui->tableView->model());

    QSqlQuery query = model->query();

    int colIndex = model->record().indexOf(columnName);

    if(colIndex < 0)
        return;

    query.first();

    row = query.at();

    do
    {
        if(query.value(colIndex).toString().contains(value.toString(), Qt::CaseInsensitive));
        {
            found = true;

            break;
        }

        row++;
    }
    while(query.next());

    if(found)
    {
        ui->tableView->selectRow(row);
    }
    else
    {
        ui->tableView->selectRow(0);
    }
}

void PLZWindow::updateTableView(const qint64 key)
{
    Postleitzahl* plz = PostleitzahlenDAO::readPostleitzahl(key);

    if(plz == nullptr)
        return;

    QModelIndex currentIndex = ui->tableView->currentIndex();

    QSqlTableModel* model = static_cast<QSqlTableModel*>(ui->tableView->model());

    QModelIndex index  = model->index(currentIndex.row(), model->record().indexOf("PLZ"));

    model->setData(index, QVariant(plz->getPLZ()), Qt::EditRole);

    emit model->dataChanged(index, index);

    index  = model->index(currentIndex.row(), model->record().indexOf("ORT"));

    model->setData(index, QVariant(plz->getOrt()), Qt::EditRole);

    emit model->dataChanged(index, index);

    delete plz;
}

void PLZWindow::deleteEntry(const QModelIndex &index)
{
    QSqlTableModel* model = static_cast<QSqlTableModel*>(ui->tableView->model());

    qint64 key = model->record(index.row()).value("PRIMARYKEY").toLongLong();

    QString PLZ = model->record(index.row()).value("PLZ").toString();

    QString Ort = model->record(index.row()).value("ORT").toString();

    int msgValue = QMessageBox::question(this, this->windowTitle(), "Daten löschen:\n" + PLZ + " - " + Ort,
                                         QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

    if(msgValue == QMessageBox::Cancel)
        return;

    if(PostleitzahlenDAO::deletePostleitzahl(key))
    {
        statusLabel->setText(("Einträge werden aktulisiert..."));

        QApplication::processEvents();

        setTableViewModel();

        int row = (index.row() - 1 < 0) ? 0 : index.row() - 1;

        if(ui->tableView->model()->rowCount() >= row)
            ui->tableView->selectRow(row);
    }
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

void PLZWindow::modifyTableView(const qint64 key, const PLZDialog::EditMode editMode)
{
    switch (editMode) {
    case PLZDialog::EditMode::NEW:
        refreshTableView(key);
        break;
    case PLZDialog::EditMode::UPDATE:
        updateTableView(key);
        break;
    }
}

void PLZWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QSqlTableModel* model = static_cast<QSqlTableModel*>(ui->tableView->model());

    showPLZDialog(model->record(index.row()).value("PRIMARYKEY").toLongLong());
}

void PLZWindow::on_actionNdern_triggered()
{
    QSqlTableModel* model = static_cast<QSqlTableModel*>(ui->tableView->model());

    QModelIndex index = ui->tableView->currentIndex();

    showPLZDialog(model->record(index.row()).value("PRIMARYKEY").toLongLong());
}

void PLZWindow::on_actionNeu_triggered()
{
    showPLZDialog(0);
}

void PLZWindow::on_actionLschen_triggered()
{
    deleteEntry(ui->tableView->currentIndex());
}

