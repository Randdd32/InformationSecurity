#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>

#include "headers/services/authservice.h"
#include "headers/services/adminuserservice.h"
#include "headers/views/userlistwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QSharedPointer<AuthService> authService,
                        QSharedPointer<AdminUserService> adminService,
                        QWidget *parent = nullptr);

    void showLoginScreen();

signals:
    void requestLogout();

private slots:
    void onLoginSuccess(const UserAccount& user);
    void openAboutDialog();
    void openChangePasswordDialog();
    void openUserListWindow();
    void logout();

private:
    void setupUI();
    void createMenus();
    void updateInterfaceForUser(const UserAccount& user);

    UserAccount _currentUser;
    QSharedPointer<AuthService> _authService;
    QSharedPointer<AdminUserService> _adminService;

    QLabel* _statusLabel;
    QLabel* _userLabel;
    QPushButton* _changePassButton;
    QPushButton* _userListButton;
    QPushButton* _logoutButton;
    QGridLayout* _buttonGrid;
    QAction* _changePassAction;
    QAction* _userListAction;
    QAction* _aboutAction;
    QAction* _exitAction;
    QWidget* _centerWidget;
    UserListWindow* _userListWindow;
};
#endif // MAINWINDOW_H
