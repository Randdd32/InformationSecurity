#include "headers/views/adduserdialog.h"
#include <QPushButton>
#include <QMessageBox>

AddUserDialog::AddUserDialog(QSharedPointer<AdminUserService> adminService, QWidget *parent)
    : QDialog(parent), _adminService(adminService)
{
    setupUI();
    setWindowTitle("Новый пользователь");
    setMinimumSize(500, 290);
    setModal(true);
}

void AddUserDialog::setupUI()
{
    _titleLabel = new QLabel("Создание пользователя");
    _titleLabel->setObjectName("HeaderLabel");
    _titleLabel->setAlignment(Qt::AlignCenter);

    _nameLabel = new QLabel("Имя пользователя");
    _nameLabel->setObjectName("FieldLabel");

    _nameEdit = new QLineEdit();
    _nameEdit->setPlaceholderText("Введите уникальное имя");
    _nameEdit->setFixedHeight(40);

    _createButton = new QPushButton("Создать");
    _createButton->setCursor(Qt::PointingHandCursor);
    _createButton->setMinimumHeight(50);

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
    _mainLayout->addWidget(_nameLabel);
    _mainLayout->addWidget(_nameEdit);

    _mainLayout->addStretch();
    _mainLayout->addWidget(_createButton);
    _mainLayout->addSpacing(5);
    _mainLayout->addWidget(_cancelButton, 0, Qt::AlignLeft);

    connect(_createButton, &QPushButton::clicked, this, &AddUserDialog::onCreateClicked);
    connect(_nameEdit, &QLineEdit::returnPressed, this, &AddUserDialog::onCreateClicked);

    connect(_cancelButton, &QPushButton::clicked, this, &AddUserDialog::reject);
}

void AddUserDialog::onCreateClicked()
{
    QString username = _nameEdit->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя не может быть пустым.");
        return;
    }

    QString error;
    bool success = _adminService->createUser(username, error);

    if (success) {
        QMessageBox::information(this, "Успех", "Пользователь успешно создан.");
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", error);
    }
}
