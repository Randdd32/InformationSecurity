#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>

class PasswordDialog : public QDialog {
    Q_OBJECT
public:
    explicit PasswordDialog(QWidget *parent = nullptr);

    QByteArray getKey() const;

private slots:
    void toggleMode();

    void browseKeyFile();

    void confirm();

private:
    QRadioButton *_rbText;
    QRadioButton *_rbFile;
    QLineEdit *_passEdit;
    QPushButton *_browseBtn;
    QLabel *_fileLabel;

    QString _selectedKeyPath;
    QByteArray _generatedKey;

    void setupUI();
};

#endif // PASSWORDDIALOG_H
