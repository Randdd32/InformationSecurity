#include "headers/views/mainwindow.h"
#include "headers/views/passworddialog.h"
#include "headers/utils/cryptoutils.h"

#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QDateTime>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyStyle();
    createMenus();
    setWindowTitle("Магма (ГОСТ Р 34.12-2018) - Гаммирование");
    setMinimumSize(550, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    _centerWidget = new QWidget(this);
    setCentralWidget(_centerWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(_centerWidget);
    mainLayout->setContentsMargins(30, 10, 30, 20);
    mainLayout->setSpacing(15);

    _studentInfoLabel = new QLabel("Студент: Родионов И.А.\nГруппа: ПИбд-41\nВариант: 26. Алгоритм Магма (режим гаммирования)");
    _studentInfoLabel->setObjectName("HeaderLabel");
    _studentInfoLabel->setAlignment(Qt::AlignCenter);

    _sourceFileLabel = new QLabel("Исходный файл: не выбран");
    _sourceFileLabel->setObjectName("FieldLabel");
    _sourceFileLabel->setStyleSheet("border: 1px dashed #555; padding: 10px; border-radius: 6px;");
    _sourceFileLabel->setWordWrap(true);

    _destFileLabel = new QLabel("Файл назначения: не выбран");
    _destFileLabel->setObjectName("FieldLabel");
    _destFileLabel->setStyleSheet("border: 1px dashed #555; padding: 10px; border-radius: 6px;");
    _destFileLabel->setWordWrap(true);

    _actionList = new QListWidget(this);
    _actionList->setIconSize(QSize(24, 24));
    _actionList->setSpacing(5);
    _actionList->setSelectionMode(QAbstractItemView::NoSelection);
    _actionList->setFocusPolicy(Qt::NoFocus);

    QListWidgetItem* folderItem = new QListWidgetItem(
        QIcon(":/images/folder.png"),
        "Выбрать исходный файл"
    );
    _actionList->addItem(folderItem);

    QListWidgetItem* saveItem = new QListWidgetItem(
        QIcon(":/images/save.png"),
        "Выбрать куда сохранить"
    );
    _actionList->addItem(saveItem);

    QListWidgetItem* encryptItem = new QListWidgetItem(
        QIcon(":/images/encrypt.png"),
        "Зашифровать"
        );
    _actionList->addItem(encryptItem);

    QListWidgetItem* decryptItem = new QListWidgetItem(
        QIcon(":/images/decrypt.png"),
        "Расшифровать"
        );
    _actionList->addItem(decryptItem);

    QListWidgetItem* logoutItem = new QListWidgetItem(
        QIcon(":/images/exit.png"),
        "Выйти из системы"
    );
    _actionList->addItem(logoutItem);

    connect(_actionList, &QListWidget::itemClicked, this, &MainWindow::onActionClicked);

    mainLayout->addWidget(_studentInfoLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(_sourceFileLabel);
    mainLayout->addWidget(_destFileLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(_actionList);
}

void MainWindow::applyStyle()
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

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction("Выбрать исходный файл", this, &MainWindow::selectSourceFile);
    fileMenu->addAction("Выбрать куда сохранять", this, &MainWindow::selectDestFile);

    QMenu* actionMenu = menuBar()->addMenu("Действия");
    actionMenu->addAction("Зашифровать", [this](){ processFile(true); });
    actionMenu->addAction("Расшифровать", [this](){ processFile(false); });

    QMenu* helpMenu = menuBar()->addMenu("Справка");
    helpMenu->addAction("О программе...", this, &MainWindow::showTheory);

    menuBar()->addAction("Выход", qApp, &QApplication::quit);
}

void MainWindow::onActionClicked(QListWidgetItem* item)
{
    QString text = item->text();
    if (text.contains("Выбрать исходный"))
        selectSourceFile();
    if (text.contains("Выбрать куда"))
        selectDestFile();
    if (text.contains("Зашифровать"))
        processFile(true);
    if (text.contains("Расшифровать"))
        processFile(false);
    if (text.contains("Теория"))
        showTheory();
    if (text.contains("Выйти"))
        qApp->quit();
}

void MainWindow::selectSourceFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Выбор файла", QDir::homePath());
    if (!path.isEmpty()) {
        QFileInfo info(path);
        if (info.size() < 1024) {
            QMessageBox::warning(this, "Ошибка", "Размер файла должен быть не менее 1 кБ!");
            return;
        }
        _sourcePath = path;

        if (_destPath.isEmpty()) {
            QFileInfo info(path);
            QString base = info.completeBaseName();
            QString ext  = info.suffix();

            _destPath = info.dir().filePath(base + "-2." + ext);
        }
        updateLabels();
    }
}

void MainWindow::selectDestFile()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить результат", _sourcePath);
    if (!path.isEmpty()) {
        _destPath = path;
        updateLabels();
    }
}

void MainWindow::updateLabels()
{
    if (_sourcePath.isEmpty()) {
        _sourceFileLabel->setText("Исходный файл: не выбран");
    }
    else {
       _sourceFileLabel->setText(QString("Исходный: %1\n(%2 байт)").arg(QFileInfo(_sourcePath).fileName()).arg(QFileInfo(_sourcePath).size()));
    }


    if (_destPath.isEmpty()) {
        _destFileLabel->setText("Файл назначения: не выбран");
    } else {
        _destFileLabel->setText("Назначение: " + QFileInfo(_destPath).fileName());
    }
}

void MainWindow::processFile(bool encrypt)
{
    if (_sourcePath.isEmpty() || _destPath.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Выберите исходный файл и файл назначения.");
        return;
    }

    PasswordDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    QByteArray key = dlg.getKey();
    if (key.size() != 32) {
        QMessageBox::critical(this, "Ошибка", "Ошибка генерации ключа.");
        return;
    }

    _cipher.setKey(key);

    QFile srcFile(_sourcePath);
    QFile dstFile(_destPath);

    if (!srcFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось прочитать исходный файл.");
        return;
    }
    if (!dstFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл назначения.");
        return;
    }

    QByteArray inputData = srcFile.readAll();
    srcFile.close();

    QByteArray outputData;

    try {
        if (encrypt) {
            QByteArray iv = CryptoUtils::generateIV();

            QByteArray fullIV = iv;
            fullIV.resize(8, 0);

            QByteArray encryptedBody = _cipher.processCTR(inputData, fullIV);

            dstFile.write(iv);
            dstFile.write(encryptedBody);

            QMessageBox::information(this, "Успех", "Файл успешно зашифрован!");
        }
        else {
            if (inputData.size() < 4) {
                throw std::runtime_error("Файл слишком мал или поврежден.");
            }

            QByteArray iv = inputData.left(4);
            QByteArray encryptedBody = inputData.mid(4);

            QByteArray fullIV = iv;
            fullIV.resize(8, 0);

            outputData = _cipher.processCTR(encryptedBody, fullIV);

            dstFile.write(outputData);
            QMessageBox::information(this, "Успех", "Файл успешно расшифрован!");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Произошла ошибка: %1").arg(e.what()));
    }

    dstFile.close();
}

void MainWindow::showTheory()
{
    QString html = R"(
    <h3>ГОСТ Р 34.12-2018 "Магма" (режим CTR)</h3>
    <p><b>Магма</b> — блочный шифр с размером блока 64 бита и длиной ключа 256 бит. Является немного измененной версией советского ГОСТ 28147-89.</p>
    <p><b>Режим гаммирования (Counter, CTR):</b></p>
    <ul>
        <li>Позволяет шифровать данные произвольной длины (нет необходимости в Padding).</li>
        <li>Использует <i>синхропосылку (IV)</i> для генерации уникальной гаммы.</li>
        <li>Шифрование: <code>C = P XOR Gamma(Key, IV)</code></li>
        <li>Расшифрование: <code>P = C XOR Gamma(Key, IV)</code></li>
    </ul>
    <p>В данной программе IV (4 байта) генерируется случайно и сохраняется в начало файла при шифровании.</p>
    )";
    QMessageBox::about(this, "Теория", html);
}
