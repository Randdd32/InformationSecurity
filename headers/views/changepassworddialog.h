#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>

#include "headers/services/authservice.h"

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChangePasswordDialog(const QString& username,
                                  QSharedPointer<AuthService> authService,
                                  QWidget *parent = nullptr);

private slots:
    void onChangeClicked();

private:
    void setupUI();

    QString _username;
    QSharedPointer<AuthService> _authService;

    QLabel* _titleLabel;
    QLabel* _oldPassLabel;
    QLineEdit* _oldPassEdit;
    QLabel* _newPassLabel;
    QLineEdit* _newPassEdit;
    QLabel* _infoLabel;
    QLabel* _confirmLabel;
    QLineEdit* _confirmEdit;
    QPushButton* _changeButton;
    QPushButton* _cancelButton;
    QVBoxLayout* _mainLayout;
};

#endif // CHANGEPASSWORDDIALOG_H
