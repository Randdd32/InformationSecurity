#include "headers/views/mainwindow.h"
#include "headers/md-5/md5hasher.h"

#include <QFile>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyStyle();
    createMenus();
    setWindowTitle("Файл в MD5");
    setMinimumSize(500, 510);
}

MainWindow::~MainWindow()
{}

void MainWindow::setupUI()
{
    _centerWidget = new QWidget(this);
    setCentralWidget(_centerWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(_centerWidget);
    mainLayout->setContentsMargins(30, 10, 30, 20);
    mainLayout->setSpacing(15);

    _studentInfoLabel = new QLabel("Студент: Родионов И.А.\nГруппа: ПИбд-41\nВариант: 26 (хэш-функция MD5)");
    _studentInfoLabel->setObjectName("HeaderLabel");
    _studentInfoLabel->setAlignment(Qt::AlignCenter);

    _fileInfoLabel = new QLabel("Файл не выбран");
    _fileInfoLabel->setObjectName("FieldLabel");
    _fileInfoLabel->setWordWrap(true);
    _fileInfoLabel->setAlignment(Qt::AlignCenter);
    _fileInfoLabel->setStyleSheet("border: 1px dashed black; padding: 10px; border-radius: 6px;");

    _resultLabel = new QLabel("MD5: -");
    _resultLabel->setObjectName("HeaderLabel");
    _resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _resultLabel->setWordWrap(true);

    _actionList = new QListWidget(this);
    _actionList->setIconSize(QSize(24, 24));
    _actionList->setSpacing(5);
    _actionList->setSelectionMode(QAbstractItemView::NoSelection);
    _actionList->setFocusPolicy(Qt::NoFocus);

    QListWidgetItem* fileItem = new QListWidgetItem(
        QIcon(":/images/file.png"),
        "Выбрать файл..."
    );
    _actionList->addItem(fileItem);

    QListWidgetItem* calcItem = new QListWidgetItem(
        QIcon(":/images/fingerprint.png"),
        "Рассчитать хэш MD5"
    );
    _actionList->addItem(calcItem);

    QListWidgetItem* reportItem = new QListWidgetItem(
        QIcon(":/images/save.png"),
        "Сохранить отчет"
    );
    _actionList->addItem(reportItem);

    QListWidgetItem* logoutItem = new QListWidgetItem(
        QIcon(":/images/exit.png"),
        "Выйти из системы"
    );
    _actionList->addItem(logoutItem);

    connect(_actionList, &QListWidget::itemClicked, this, &MainWindow::onActionClicked);

    mainLayout->addWidget(_studentInfoLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(_fileInfoLabel);
    mainLayout->addWidget(_resultLabel);
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
    if (menuBar()) {
        delete menuBar();
    }

    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    _openAction = new QAction("Выбрать файл...", this);
    _openAction->setShortcut(QKeySequence::Open);
    connect(_openAction, &QAction::triggered, this, &MainWindow::selectFile);

    _saveAction = new QAction("Сохранить отчет", this);
    _saveAction->setShortcut(QKeySequence::Save);
    connect(_saveAction, &QAction::triggered, this, &MainWindow::saveResult);

    _exitAction = new QAction("Выход", this);
    _exitAction->setShortcut(QKeySequence::Quit);
    connect(_exitAction, &QAction::triggered, qApp, &QApplication::quit);

    _aboutAction = new QAction("О программе...", this);
    connect(_aboutAction, &QAction::triggered, this, &MainWindow::showAboutInfo);

    QMenu* fileMenu = menuBar->addMenu("Файл");
    fileMenu->addAction(_openAction);
    fileMenu->addAction(_saveAction);

    QMenu* helpMenu = menuBar->addMenu("Справка");
    helpMenu->addAction(_aboutAction);

    menuBar->addAction(_exitAction);
}

void MainWindow::onActionClicked(QListWidgetItem* item)
{
    QString text = item->text();

    if (text == "Выбрать файл...") {
        selectFile();
    }
    if (text == "Рассчитать хэш MD5") {
        calculateHash();
    }
    if (text == "Сохранить отчет") {
        saveResult();
    }
    if (text == "Выйти из системы") {
        qApp->quit();
    }
}

void MainWindow::selectFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл", QDir::homePath(), "Все файлы (*.*)");

    if (!path.isEmpty()) {
        QFileInfo fileInfo(path);

        if (fileInfo.size() < 1024) {
            QMessageBox::warning(this, "Ошибка", "Размер файла должен быть не менее 1 кБ!");
            _currentFilePath.clear();
            _fileInfoLabel->setText("Файл не выбран (ошибка размера)");
        } else {
            _currentFilePath = path;
            _currentHash.clear();
            _resultLabel->setText("MD5: (можно рассчитать)");
            updateFileLabel();
        }
    }
}

void MainWindow::updateFileLabel()
{
    if (_currentFilePath.isEmpty()) {
        return;
    }
    QFileInfo info(_currentFilePath);
    _fileInfoLabel->setText(QString("Выбран: %1\nРазмер: %2 байт").arg(info.fileName()).arg(info.size()));
}

bool MainWindow::calculateHash()
{
    if (_currentFilePath.isEmpty()) {
        QMessageBox::information(this, "Внимание", "Сначала выберите файл.");
        return false;
    }

    QFile file(_currentFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    _currentHash = MD5Hasher::hash(fileData);

    _resultLabel->setText("MD5:\n" + _currentHash);
    return true;
}

void MainWindow::saveResult()
{
    if (_currentHash.isEmpty()) {
        if (_currentFilePath.isEmpty()) {
            QMessageBox::information(this, "Внимание", "Файл не выбран.");
            return;
        }

        if (!calculateHash()) {
            return;
        }
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Сохранить отчет", "report.txt", "Text files (*.txt)");
    if (savePath.isEmpty()) {
        return;
    }

    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ОТЧЕТ О ХЭШИРОВАНИИ" << "\n";
        out << "Дата: " << QDateTime::currentDateTime().toString() << "\n";
        out << "Файл: " << _currentFilePath << "\n";
        out << "Размер: " << QFileInfo(_currentFilePath).size() << " байт\n";
        out << "Алгоритм: MD5" << "\n";
        out << "Хэш-сумма: " << _currentHash << "\n";
        file.close();
        QMessageBox::information(this, "Успех", "Отчет успешно сохранен.");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл отчета.");
    }
}

void MainWindow::showAboutInfo()
{
    QMessageBox::about(this, "Алгоритм MD5",
                       "<h3>Алгоритм MD5 (Message Digest 5)</h3>"
                       "<p><b>Описание:</b> 128-битный алгоритм хеширования, разработанный профессором Рональдом Л. Ривестом.</p>"
                       "<p><b>Принцип работы:</b></p>"
                       "<ul>"
                       "<li>Входной поток разбивается на блоки по 512 бит.</li>"
                       "<li>Производится выравнивание потока (добавление битов).</li>"
                       "<li>Инициализируются 4 буфера (A, B, C, D).</li>"
                       "<li>В цикле выполняются 4 раунда преобразований над блоками.</li>"
                       "<li>Результат — 128-битное число (обычно в виде 32 hex-цифр).</li>"
                       "</ul>"
                       );
}
