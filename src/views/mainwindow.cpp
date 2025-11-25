#include "headers/views/mainwindow.h"
#include "headers/views/loginwindow.h"
#include "headers/views/changepassworddialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QSharedPointer<AuthService> authService,
                       QSharedPointer<AdminUserService> adminService,
                       QWidget *parent)
    : QMainWindow(parent),
    _authService(authService),
    _adminService(adminService),
    _userListWindow(nullptr)
{
    setupUI();
    setWindowTitle("Программа  разграничения полномочий пользователей");
    setMinimumSize(400, 200);

    connect(this, &MainWindow::requestLogout, qApp, &QApplication::quit);
    showLoginScreen();
}

void MainWindow::setupUI()
{
    _centerWidget = new QWidget(this);
    setCentralWidget(_centerWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(_centerWidget);
    mainLayout->setContentsMargins(30, 10, 30, 20);
    mainLayout->setSpacing(10);

    _statusLabel = new QLabel("Режим работы программы:");
    _statusLabel->setObjectName("HeaderLabel");

    _userLabel = new QLabel("Имя пользователя:");
    _userLabel->setObjectName("FieldLabel");

    _changePassButton = new QPushButton("Изменить пароль");
    _changePassButton->setMinimumHeight(50);
    _changePassButton->setCursor(Qt::PointingHandCursor);
    connect(_changePassButton, &QPushButton::clicked, this, &MainWindow::openChangePasswordDialog);

    _userListButton = new QPushButton("Управление пользователями");
    _userListButton->setMinimumHeight(50);
    _userListButton->setMaximumWidth(200);
    _userListButton->setCursor(Qt::PointingHandCursor);
    connect(_userListButton, &QPushButton::clicked, this, &MainWindow::openUserListWindow);

    _logoutButton = new QPushButton("Выйти из системы");
    _logoutButton->setObjectName("CancelButton");
    _logoutButton->setMinimumHeight(40);
    _logoutButton->setMaximumWidth(250);
    _logoutButton->setCursor(Qt::PointingHandCursor);
    connect(_logoutButton, &QPushButton::clicked, this, &MainWindow::logout);

    _buttonGrid = new QGridLayout();
    _buttonGrid->setHorizontalSpacing(30);
    _buttonGrid->setVerticalSpacing(20);

    mainLayout->addWidget(_statusLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(_userLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(_buttonGrid);
    mainLayout->addStretch();
    mainLayout->addWidget(_logoutButton, 0, Qt::AlignCenter);

    _centerWidget->setVisible(false);
}

void MainWindow::createMenus()
{
    if (menuBar()) {
        delete menuBar();
    }

    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->setStyleSheet("QMenuBar { font-size: 16px; } "
                             "QMenu { font-size: 16px; }");
    setMenuBar(menuBar);

    QMenu* actionMenu = menuBar->addMenu("Действия");
    _changePassAction = actionMenu->addAction("Изменить пароль");
    connect(_changePassAction, &QAction::triggered, this, &MainWindow::openChangePasswordDialog);
    if (_currentUser.isAdmin()) {
        _userListAction = actionMenu->addAction("Управление пользователями");
        connect(_userListAction, &QAction::triggered, this, &MainWindow::openUserListWindow);
    }

    QMenu* helpMenu = menuBar->addMenu("Справка");
    _aboutAction = helpMenu->addAction("О программе...");
    connect(_aboutAction, &QAction::triggered, this, &MainWindow::openAboutDialog);

    _exitAction = menuBar->addAction("Выход");
    connect(_exitAction, &QAction::triggered, this, &MainWindow::logout);
}

void MainWindow::updateInterfaceForUser(const UserAccount& user)
{
    _currentUser = user;

    createMenus();

    QString mode = user.isAdmin() ? "АДМИНИСТРАТОР" : "ОБЫЧНЫЙ ПОЛЬЗОВАТЕЛЬ";
    _statusLabel->setText(QString("Режим работы: %1").arg(mode));
    _userLabel->setText(QString("Имя пользователя: %1").arg(user.username()));

    QLayoutItem *item;
    while ((item = _buttonGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (user.isAdmin()) {
        _buttonGrid->addWidget(_changePassButton, 0, 0, 1, 1);
        _buttonGrid->addWidget(_userListButton, 0, 1, 1, 1);
        _changePassButton->setMaximumWidth(200);
    } else {
        _buttonGrid->addWidget(_changePassButton, 0, 0, 1, 2);
        _changePassButton->setMinimumWidth(0);
    }

    _centerWidget->setVisible(true);
}

void MainWindow::showLoginScreen()
{
    _centerWidget->setVisible(false);
    menuBar()->setVisible(false);

    LoginWindow* loginWindow = new LoginWindow(_authService);
    connect(loginWindow, &LoginWindow::loginSuccess, this, &MainWindow::onLoginSuccess);

    loginWindow->show();
}

void MainWindow::onLoginSuccess(const UserAccount& user)
{
    updateInterfaceForUser(user);

    this->show();

    LoginWindow* senderLogin = qobject_cast<LoginWindow*>(sender());
    if (senderLogin) {
        senderLogin->close();
        senderLogin->deleteLater();
    }
}

void MainWindow::openAboutDialog()
{
    QMessageBox::about(this, "О программе",
                       "<h3>Программа  разграничения полномочий пользователей на основе парольной аутентификации c использованием встроенных криптопровайдеров</h3>"
                       "<p>Версия: 1.0</p>"
                       "<p><b>Автор:</b> Родионов Илья - студент ПИбд-41</p>"
                       "<p><b>Индивидуальное задание (26 вариант):</b></p>"
                       "<p><b>1.</b> Ограничения на выбираемые  пароли - «Наличие латинских букв, символов кириллицы и цифр»</p>"
                       "<p><b>2.</b> Используемый режим шифрования алгоритма DES для шифрования файла - CBC</p>"
                       "<p><b>3.</b> Добавление к ключу случайного значения - нет</p>"
                       "<p><b>4.</b> Используемый алгоритм хеширования пароля - MD5</p>"
                       );
}

void MainWindow::openChangePasswordDialog()
{
    ChangePasswordDialog dialog(_currentUser.username(), _authService, this);
    dialog.exec();
}

void MainWindow::openUserListWindow()
{
    if (_userListWindow) {
        _userListWindow->show();
        _userListWindow->activateWindow();
        return;
    }

    _userListWindow = new UserListWindow(_adminService, nullptr);
    _userListWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(_userListWindow, &QObject::destroyed, this, [this]() {
        _userListWindow = nullptr;
    });
    _userListWindow->show();
}

void MainWindow::logout()
{
    if (QMessageBox::question(this, "Выход", "Вы действительно хотите завершить работу с программой?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        _currentUser = UserAccount();

        if (_userListWindow) {
            _userListWindow->close();
        }

        emit requestLogout();
    }
}
