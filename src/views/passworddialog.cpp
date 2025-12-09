#include "headers/views/passworddialog.h"
#include "headers/utils/cryptoutils.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>

PasswordDialog::PasswordDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Ввод ключа шифрования");
    setModal(true);
    setupUI();
}

QByteArray PasswordDialog::getKey() const {
    return _generatedKey;
}

void PasswordDialog::toggleMode() {
    bool isFile = _rbFile->isChecked();
    _passEdit->setEnabled(!isFile);
    _browseBtn->setEnabled(isFile);
}

void PasswordDialog::browseKeyFile() {
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл-ключ");
    if (!path.isEmpty()) {
        _selectedKeyPath = path;
        _fileLabel->setText("Выбран: " + QFileInfo(path).fileName());
    }
}

void PasswordDialog::confirm() {
    if (_rbText->isChecked()) {
        if (_passEdit->text().isEmpty())
            return;
        _generatedKey = CryptoUtils::keyFromPassword(_passEdit->text());
    } else {
        if (_selectedKeyPath.isEmpty())
            return;
        bool ok;
        _generatedKey = CryptoUtils::keyFromFile(_selectedKeyPath, ok);
        if (!ok) {
            QMessageBox::critical(this, "Ошибка", "Не удалось сгенерировать ключ на основе выбранного файла.");
            return;
        }
    }
    accept();
}

void PasswordDialog::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 10, 30, 20);
    layout->setSpacing(10);

    _rbText = new QRadioButton("Ввод пароля вручную");
    _rbFile = new QRadioButton("Загрузить пароль из файла");
    _rbText->setChecked(true);

    _passEdit = new QLineEdit();
    _passEdit->setEchoMode(QLineEdit::Password);
    _passEdit->setPlaceholderText("Введите пароль...");

    _browseBtn = new QPushButton("Обзор...");
    _browseBtn->setEnabled(false);
    _fileLabel = new QLabel("Файл не выбран");

    QPushButton *okBtn = new QPushButton("Принять");

    layout->addWidget(_rbText);
    layout->addWidget(_passEdit);
    layout->addSpacing(10);
    layout->addWidget(_rbFile);
    layout->addWidget(_browseBtn);
    layout->addWidget(_fileLabel);
    layout->addStretch();
    layout->addWidget(okBtn);

    connect(_rbText, &QRadioButton::toggled, this, &PasswordDialog::toggleMode);
    connect(_rbFile, &QRadioButton::toggled, this, &PasswordDialog::toggleMode);
    connect(_browseBtn, &QPushButton::clicked, this, &PasswordDialog::browseKeyFile);
    connect(okBtn, &QPushButton::clicked, this, &PasswordDialog::confirm);
}
