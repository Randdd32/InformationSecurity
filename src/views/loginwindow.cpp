#include "headers/views/loginwindow.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>

LoginWindow::LoginWindow(QSharedPointer<AuthService> authService, QWidget *parent)
    : QWidget(parent), _authService(authService)
{
    setupUI();
    setWindowTitle("Вход в систему");
    setMinimumSize(500, 275);
}

void LoginWindow::setupUI()
{
    _titleLabel = new QLabel("Авторизация");
    _titleLabel->setObjectName("HeaderLabel");
    _titleLabel->setAlignment(Qt::AlignCenter);

    _userLabel = new QLabel("Имя пользователя");
    _userLabel->setObjectName("FieldLabel");

    _usernameEdit = new QLineEdit();
    _usernameEdit->setPlaceholderText("Введите имя пользователя");
    _usernameEdit->setMinimumHeight(40);

    _passLabel = new QLabel("Пароль");
    _passLabel->setObjectName("FieldLabel");

    _passwordEdit = new QLineEdit();
    _passwordEdit->setEchoMode(QLineEdit::Password);
    _passwordEdit->setMinimumHeight(40);

    _requirementsLabel = new QLabel("");
    _requirementsLabel->setObjectName("InfoLabel");
    _requirementsLabel->setWordWrap(true);
    _requirementsLabel->setVisible(false);

    _confirmLabel = new QLabel("Подтверждение пароля");
    _confirmLabel->setObjectName("FieldLabel");
    _confirmLabel->setVisible(false);

    _confirmEdit = new QLineEdit();
    _confirmEdit->setEchoMode(QLineEdit::Password);
    _confirmEdit->setMinimumHeight(40);
    _confirmEdit->setVisible(false);

    _actionButton = new QPushButton("Войти в систему");
    _actionButton->setCursor(Qt::PointingHandCursor);
    _actionButton->setMinimumHeight(50);

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->setContentsMargins(30, 10, 30, 20);
    _mainLayout->setSpacing(10);

    _mainLayout->addWidget(_titleLabel);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_userLabel);
    _mainLayout->addWidget(_usernameEdit);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_passLabel);
    _mainLayout->addWidget(_passwordEdit);
    _mainLayout->addWidget(_requirementsLabel);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_confirmLabel);
    _mainLayout->addWidget(_confirmEdit);
    _mainLayout->addSpacing(10);
    _mainLayout->addStretch();
    _mainLayout->addWidget(_actionButton);

    connect(_actionButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(_usernameEdit, &QLineEdit::editingFinished, this, &LoginWindow::checkUserStatus);
}

void LoginWindow::checkUserStatus()
{
    QString username = _usernameEdit->text().trimmed();
    if (username.isEmpty()) {
        return;
    }

    UserAccount userConf;
    bool isFirst = _authService->checkIsFirstLogin(username, userConf);

    updateMode(isFirst, userConf);
}

void LoginWindow::updateMode(bool isFirstLogin, const UserAccount& userConf)
{
    _isFirstLoginMode = isFirstLogin;

    if (_isFirstLoginMode) {
        _titleLabel->setText("Смена пароля");
        _actionButton->setText("Установить и войти");
        _confirmLabel->setVisible(true);
        _confirmEdit->setVisible(true);
        _confirmEdit->clear();
        _passwordEdit->clear();

        QString reqText = QString("Пароль должен быть длиной не менее %1 символов.").arg(userConf.minPasswordLength());
        if (userConf.passwordRestrictions()) {
            reqText += "\nТребуется наличие: латинских букв (A-z), кириллицы (А-я) и цифр (0-9).";
        }

        _requirementsLabel->setText(reqText);
        _requirementsLabel->setVisible(true);
    } else {
        _titleLabel->setText("Авторизация");
        _actionButton->setText("Войти в систему");
        _confirmLabel->setVisible(false);
        _confirmEdit->setVisible(false);
        _requirementsLabel->setVisible(false);
    }

    layout()->invalidate();
    adjustSize();
}

void LoginWindow::onLoginClicked()
{
    QString username = _usernameEdit->text();
    QString password = _passwordEdit->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя пользователя");
        return;
    }

    UserAccount user;
    AuthStatus status;

    if (_isFirstLoginMode) {
        QString confirm = _confirmEdit->text();

        if (password != confirm) {
            QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
            return;
        }

        QString error;
        bool changeSuccess = _authService->changePassword(username, "", password, error);
        if (!changeSuccess) {
            QMessageBox::warning(this, "Ошибка смены пароля", error);
            return;
        }

        QMessageBox::information(this, "Успех", "Пароль успешно установлен. Выполняется вход...");

        status = _authService->authenticate(username, password, user);
        handleAuthStatus(status, user);
    } else {
        status = _authService->authenticate(username, password, user);
        handleAuthStatus(status, user);
    }
}

void LoginWindow::handleAuthStatus(AuthStatus status, const UserAccount& user)
{
    switch (status) {
        case AuthStatus::Success:
            emit loginSuccess(user);
            this->close();
            break;
        case AuthStatus::IncorrectPassword:
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверный пароль. Попытка %1 из 3")
                                     .arg(_authService->loginAttemptCount()));

            if (_authService->loginAttemptCount() >= 3) {
                QMessageBox::critical(this, "Блокировка", "Превышено количество попыток. Программа будет закрыта.");
                QApplication::quit();
            }
            break;
        case AuthStatus::UserBlocked:
            QMessageBox::warning(this, "Доступ запрещен", "Учетная запись заблокирована администратором.");
            break;
        case AuthStatus::UserNotFound:
            QMessageBox::warning(this, "Ошибка", "Пользователь не найден. Проверьте имя.");
            break;
        case AuthStatus::DatabaseError:
            QMessageBox::warning(this, "Ошибка", "Ошибка доступа к базе данных.");
            break;
        case AuthStatus::SystemError:
        default:
            QMessageBox::critical(this, "Ошибка", "Внутренняя ошибка системы.");
            break;
    }
}
