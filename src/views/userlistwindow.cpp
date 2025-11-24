#include "headers/views/userlistwindow.h"
#include "headers/views/adduserdialog.h"
#include "headers/models/useraccount.h"
#include "headers/utils/buttonhoverwatcher.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QScrollBar>

UserListWindow::UserListWindow(QSharedPointer<AdminUserService> adminService, QWidget *parent)
    : QWidget(parent), _adminService(adminService), _isLoadingData(false)
{
    setupUI();
    loadUsers();
    setWindowTitle("Управление пользователями");
    resize(900, 600);
}

void UserListWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 10, 30, 20);
    mainLayout->setSpacing(15);

    _searchEdit = new QLineEdit();
    _searchButton = new QPushButton(this);
    ButtonHoverWatcher *watcher = new ButtonHoverWatcher(this);

    _searchButton->installEventFilter(watcher);
    _searchButton->setCursor(Qt::PointingHandCursor);
    _searchButton->setStyleSheet("background: transparent; border: none;");
    _searchButton->setIcon(QIcon(":/images/clear.png"));
    _searchButton->setFixedSize(22, 22);
    _searchButton->setToolTip("Очистить поиск");

    QMargins margins = _searchEdit->textMargins();
    _searchEdit->setTextMargins(margins.left(), margins.top(), _searchButton->width(), margins.bottom());
    _searchEdit->setMinimumHeight(40);
    _searchEdit->setMaxLength(100);
    _searchEdit->setPlaceholderText("Поиск пользователей по имени...");

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addStretch();
    searchLayout->addWidget(_searchButton);
    searchLayout->setSpacing(0);
    searchLayout->setContentsMargins(0, 2, 5, 2);
    _searchEdit->setLayout(searchLayout);

    connect(_searchEdit, &QLineEdit::textChanged, this, &UserListWindow::onSearchTextChanged);
    connect(_searchButton, &QPushButton::clicked, this, &UserListWindow::onClearSearchClicked);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    _addUserButton = new QPushButton("Добавить пользователя");
    _addUserButton->setCursor(Qt::PointingHandCursor);
    _addUserButton->setMinimumHeight(40);
    _addUserButton->setMinimumWidth(200);
    connect(_addUserButton, &QPushButton::clicked, this, &UserListWindow::onAddUserClicked);

    btnLayout->addWidget(_addUserButton);

    _usersTable = new QTableWidget();
    _usersTable->setColumnCount(Columns::ColumnCount);
    setupTableColumns();

    _usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _usersTable->setAlternatingRowColors(true);
    _usersTable->verticalHeader()->setVisible(false);
    _usersTable->horizontalHeader()->setStretchLastSection(true);
    _usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _usersTable->horizontalHeader()->setSectionResizeMode(Columns::Username, QHeaderView::Stretch);

    connect(_usersTable, &QTableWidget::itemChanged, this, &UserListWindow::onTableItemChanged);

    mainLayout->addWidget(_searchEdit);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(_usersTable);
}

void UserListWindow::setupTableColumns()
{
    QStringList headers;
    headers << "ID" << "Имя пользователя" << "Блокировка"
            << "Ограничения" << "Мин. длина" << "Срок (мес.)";
    _usersTable->setHorizontalHeaderLabels(headers);
    _usersTable->setColumnHidden(Columns::ID, true);
}

void UserListWindow::loadUsers()
{
    _isLoadingData = true;
    _usersTable->blockSignals(true);
    _usersTable->clearContents();

    QString error;
    QString searchText = _searchEdit->text();

    QList<UserAccount> users = _adminService->getAllUsers(searchText, error);

    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Ошибка загрузки", error);
    }

    _usersTable->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        const UserAccount& user = users[i];

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(user.id()));
        idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
        _usersTable->setItem(i, Columns::ID, idItem);

        QTableWidgetItem* nameItem = new QTableWidgetItem(user.username());
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        _usersTable->setItem(i, Columns::Username, nameItem);

        QTableWidgetItem* blockItem = new QTableWidgetItem();
        blockItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        blockItem->setCheckState(user.isBlocked() ? Qt::Checked : Qt::Unchecked);
        _usersTable->setItem(i, Columns::IsBlocked, blockItem);

        QTableWidgetItem* restrItem = new QTableWidgetItem();
        restrItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        restrItem->setCheckState(user.passwordRestrictions() ? Qt::Checked : Qt::Unchecked);
        _usersTable->setItem(i, Columns::Restrictions, restrItem);

        QTableWidgetItem* lenItem = new QTableWidgetItem(QString::number(user.minPasswordLength()));
        _usersTable->setItem(i, Columns::MinLength, lenItem);

        QTableWidgetItem* expItem = new QTableWidgetItem(QString::number(user.passwordExpirationMonths()));
        _usersTable->setItem(i, Columns::Expiration, expItem);
    }

    _usersTable->blockSignals(false);
    _isLoadingData = false;
}

void UserListWindow::onSearchTextChanged(const QString &text)
{
    loadUsers();
}

void UserListWindow::onClearSearchClicked()
{
    _searchEdit->clear();
}

void UserListWindow::onAddUserClicked()
{
    AddUserDialog dialog(_adminService, this);
    if (dialog.exec() == QDialog::Accepted) {
        _searchEdit->clear();
        loadUsers();
        _usersTable->scrollToTop();
    }
}

void UserListWindow::onTableItemChanged(QTableWidgetItem *item)
{
    if (_isLoadingData) {
        return;
    }

    int row = item->row();
    int col = item->column();

    QString username = _usersTable->item(row, Columns::Username)->text();
    QString error;
    bool success = true;

    switch (col) {
        case Columns::IsBlocked: {
            bool blocked = (item->checkState() == Qt::Checked);
            success = _adminService->changeUserBlockStatus(username, blocked, error);
            break;
        }
        case Columns::Restrictions: {
            bool enabled = (item->checkState() == Qt::Checked);
            success = _adminService->setPasswordRestrictions(username, enabled, error);
            break;
        }
        case Columns::MinLength: {
            bool ok;
            int val = item->text().toInt(&ok);
            if (!ok || val < 0) {
                error = "Длина должна быть целым неотрицательным числом";
                success = false;
            } else {
                success = _adminService->setPasswordMinLength(username, val, error);
            }
            break;
        }
        case Columns::Expiration: {
            bool ok;
            int val = item->text().toInt(&ok);
            if (!ok || val < 0) {
                error = "Срок действия должен быть целым неотрицательным числом";
                success = false;
            } else {
                success = _adminService->setPasswordExpiration(username, val, error);
            }
            break;
        }
        default:
            return;
    }

    if (!success) {
        QMessageBox::warning(this, "Ошибка обновления", error);
        loadUsers();
    }
}
