#include "headers/views/changepassworddialog.h"
#include <QPushButton>
#include <QMessageBox>

ChangePasswordDialog::ChangePasswordDialog(const QString& username,
                                           QSharedPointer<AuthService> authService,
                                           QWidget *parent)
    : QDialog(parent), _username(username), _authService(authService)
{
    setupUI();
    setWindowTitle("Смена пароля");
    setMinimumSize(500, 375);
    setModal(true);
}

void ChangePasswordDialog::setupUI()
{
    _titleLabel = new QLabel("Изменение пароля");
    _titleLabel->setObjectName("HeaderLabel");
    _titleLabel->setAlignment(Qt::AlignCenter);

    _oldPassLabel = new QLabel("Старый пароль");
    _oldPassLabel->setObjectName("FieldLabel");

    _oldPassEdit = new QLineEdit();
    _oldPassEdit->setEchoMode(QLineEdit::Password);
    _oldPassEdit->setMinimumHeight(40);

    _newPassLabel = new QLabel("Новый пароль");
    _newPassLabel->setObjectName("FieldLabel");

    _newPassEdit = new QLineEdit();
    _newPassEdit->setEchoMode(QLineEdit::Password);
    _newPassEdit->setMinimumHeight(40);

    _infoLabel = new QLabel("Введите новый пароль");
    _infoLabel->setObjectName("InfoLabel");
    _infoLabel->setWordWrap(true);

    try {
        UserAccount userConf = _authService->getUser(_username);
        QString reqText = QString("Пароль должен быть длиной не менее %1 символов.").arg(userConf.minPasswordLength());
        if (userConf.passwordRestrictions()) {
            reqText += "\nТребуется наличие: латинских букв (A-z), кириллицы (А-я) и цифр (0-9).";
        }
        _infoLabel->setText(reqText);
    } catch (...) {
        _infoLabel->setText("Введите новый пароль");
    }

    _confirmLabel = new QLabel("Подтверждение нового пароля");
    _confirmLabel->setObjectName("FieldLabel");

    _confirmEdit = new QLineEdit();
    _confirmEdit->setEchoMode(QLineEdit::Password);
    _confirmEdit->setMinimumHeight(40);

    _changeButton = new QPushButton("Изменить пароль");
    _changeButton->setCursor(Qt::PointingHandCursor);
    _changeButton->setMinimumHeight(50);

    _cancelButton = new QPushButton("Отмена");
    _cancelButton->setObjectName("CancelButton");
    _cancelButton->setCursor(Qt::PointingHandCursor);
    _cancelButton->setMinimumHeight(30);
    _cancelButton->setMinimumWidth(150);

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->setContentsMargins(30, 10, 30, 20);
    _mainLayout->setSpacing(10);

    _mainLayout->addWidget(_titleLabel);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_oldPassLabel);
    _mainLayout->addWidget(_oldPassEdit);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_newPassLabel);
    _mainLayout->addWidget(_newPassEdit);
    _mainLayout->addWidget(_infoLabel);
    _mainLayout->addSpacing(10);
    _mainLayout->addWidget(_confirmLabel);
    _mainLayout->addWidget(_confirmEdit);
    _mainLayout->addStretch();
    _mainLayout->addWidget(_changeButton);
    _mainLayout->addSpacing(5);
    _mainLayout->addWidget(_cancelButton, 0, Qt::AlignLeft);

    connect(_changeButton, &QPushButton::clicked, this, &ChangePasswordDialog::onChangeClicked);
    connect(_cancelButton, &QPushButton::clicked, this, &ChangePasswordDialog::reject);
}

void ChangePasswordDialog::onChangeClicked()
{
    QString oldPass = _oldPassEdit->text();
    QString newPass = _newPassEdit->text();
    QString confirm = _confirmEdit->text();

    if (newPass != confirm) {
        QMessageBox::warning(this, "Ошибка", "Новый пароль и подтверждение не совпадают");
        return;
    }

    if (oldPass == newPass) {
        QMessageBox::warning(this, "Внимание", "Новый пароль совпадает со старым");
        return;
    }

    QString errorMessage;
    bool success = _authService->changePassword(_username, oldPass, newPass, errorMessage);

    if (success) {
        QMessageBox::information(this, "Успех", "Пароль успешно изменен.");
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", errorMessage);
    }
}
