#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include "headers/services/authservice.h"

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QSharedPointer<AuthService> authService, QWidget *parent = nullptr);

signals:
    void loginSuccess(const UserAccount& user);

private slots:
    void onLoginClicked();
    void checkUserStatus();

private:
    void setupUI();
    void updateMode(bool isFirstLogin, const UserAccount& userConf = UserAccount());

    void handleAuthStatus(AuthStatus status, const UserAccount& user);

    QSharedPointer<AuthService> _authService;
    bool _isFirstLoginMode = false;

    QLabel* _titleLabel;
    QLabel* _userLabel;
    QLineEdit* _usernameEdit;
    QLabel* _passLabel;
    QLineEdit* _passwordEdit;
    QLabel* _confirmLabel;
    QLineEdit* _confirmEdit;
    QLabel* _requirementsLabel;
    QPushButton* _actionButton;
    QVBoxLayout* _mainLayout;
};

#endif // LOGINWINDOW_H
