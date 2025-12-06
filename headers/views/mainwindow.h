#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onActionClicked(QListWidgetItem* item);

    void selectFile();
    bool calculateHash();
    void saveResult();
    void showAboutInfo();

private:
    void setupUI();
    void applyStyle();
    void createMenus();

    void updateFileLabel();

    QWidget* _centerWidget;
    QLabel* _studentInfoLabel;
    QLabel* _fileInfoLabel;
    QLabel* _resultLabel;
    QListWidget* _actionList;

    QAction* _openAction;
    QAction* _saveAction;
    QAction* _exitAction;
    QAction* _aboutAction;

    QString _currentFilePath;
    QString _currentHash;
};

#endif // MAINWINDOW_H
