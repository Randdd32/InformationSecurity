#include "headers/views/initpassphrasewindow.h"

#include <QFile>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>

InitPassphraseWindow::InitPassphraseWindow(QSharedPointer<AuthService> authService, const QString& encryptedFile, QWidget *parent)
    : QDialog(parent),
    _authService(authService),
    _isFirstRun(!QFile::exists(encryptedFile))
{
    setupUI();
    applyStyle();
    setWindowTitle(_isFirstRun ? "Инициализация БД" : "Вход: парольная фраза");
}

InitPassphraseWindow::~InitPassphraseWindow() {}

void InitPassphraseWindow::setupUI()
{
    _titleLabel = new QLabel("Загрузка базы данных");
    _titleLabel->setObjectName("TitleLabel");
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold; margin-bottom: 20px;");

    _promptLabel = new QLabel("Введите парольную фразу для базы данных:");
    _promptLabel->setObjectName("PromptLabel");
    _promptLabel->setStyleSheet("font-size: 14pt;");

    _passphraseEdit = new QLineEdit();
    _passphraseEdit->setObjectName("PassphraseEdit");
    _passphraseEdit->setEchoMode(QLineEdit::Password);
    _passphraseEdit->setPlaceholderText("Парольная фраза");

    _enterButton = new QPushButton("Подтвердить");
    _enterButton->setObjectName("EnterButton");
    _enterButton->setMinimumHeight(50);

    _cancelButton = new QPushButton("Отмена ввода");
    _cancelButton->setObjectName("CancelButton");
    _cancelButton->setMinimumHeight(30);
    _cancelButton->setMinimumWidth(150);

    if (_isFirstRun) {
        _titleLabel->setText("Инициализация базы данных");
        _promptLabel->setText("База данных не найдена. Выберите парольную фразу для инициализации новой:");
        _enterButton->setText("Инициализировать");
    }

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->addWidget(_titleLabel);
    _mainLayout->addWidget(_promptLabel);
    _mainLayout->addWidget(_passphraseEdit);
    _mainLayout->addSpacing(15);
    _mainLayout->addWidget(_enterButton);
    _mainLayout->addSpacing(5);
    _mainLayout->addWidget(_cancelButton, 0, Qt::AlignLeft);
    _mainLayout->setContentsMargins(30, 30, 30, 30);

    connect(_enterButton, &QPushButton::clicked, this, &InitPassphraseWindow::onEnterClicked);
    connect(_passphraseEdit, &QLineEdit::returnPressed, this, &InitPassphraseWindow::onEnterClicked);

    connect(_cancelButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(
            this,
            "Отмена",
            "Отказ от ввода парольной фразы. Приложение будет закрыто.",
            QMessageBox::Ok
        );
        QApplication::quit();
    });

    setLayout(_mainLayout);
}

void InitPassphraseWindow::applyStyle()
{
    QFile file(":/styles.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning() << "Could not load styles.qss";
    }
}

void InitPassphraseWindow::onEnterClicked()
{
    QString passphrase = _passphraseEdit->text();

    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Парольная фраза не может быть пустой.", QMessageBox::Ok);
        return;
    }

    InitDBStatus status = _authService->initDB(passphrase);

    if (status == InitDBStatus::Success || status == InitDBStatus::FirstRunSuccess) {
        QMessageBox::information(this, "Успех",
                                 getStatusMessage(status),
                                 QMessageBox::Ok);
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              getStatusMessage(status),
                              QMessageBox::Close);

        if (status == InitDBStatus::PassphraseIncorrect || status == InitDBStatus::DecryptionException || status == InitDBStatus::DBFileCorrupted) {
            QApplication::quit();
        }
    }
}

QString InitPassphraseWindow::getStatusMessage(InitDBStatus status)
{
    switch (status) {
    case InitDBStatus::Success:
        return "База данных успешно загружена.";
    case InitDBStatus::FirstRunSuccess:
        return "База данных успешно инициализирована. Создан пользователь ADMIN с пустым паролем.";
    case InitDBStatus::DBFileCorrupted:
        return "Файл базы данных поврежден (отсутствует учетная запись администратора или произошла ошибка при получении IV). Приложение будет закрыто.";
    case InitDBStatus::PassphraseIncorrect:
        return "Парольная фраза неверна. Приложение будет закрыто.";
    case InitDBStatus::DecryptionException:
        return "Произошла ошибка при расшифровке файла БД. Повреждены данные или парольная фраза неверна. Приложение будет закрыто.";
    case InitDBStatus::FileOpenError:
        return "Произошла ошибка доступа к файлам БД (зашифрованному или временному). Проверьте права доступа.";
    case InitDBStatus::SystemError:
        return "Произошла ошибка в работе системы. Пожалуйста, попробуйте снова.";
    default:
        return "Ошибка инициализации. Пожалуйста, попробуйте снова.";
    }
}
