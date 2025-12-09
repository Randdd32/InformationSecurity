#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "headers/magma/magmacipher.h"

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

    void selectSourceFile();
    void selectDestFile();
    void processFile(bool encrypt);
    void showTheory();

private:
    void setupUI();
    void applyStyle();
    void createMenus();
    void updateLabels();

    MagmaCipher _cipher;

    QWidget* _centerWidget;
    QLabel* _studentInfoLabel;
    QLabel* _sourceFileLabel;
    QLabel* _destFileLabel;
    QListWidget* _actionList;

    QString _sourcePath;
    QString _destPath;
};

#endif // MAINWINDOW_H
