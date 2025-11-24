#ifndef USERLISTWINDOW_H
#define USERLISTWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

#include "headers/services/adminuserservice.h"

class UserListWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UserListWindow(QSharedPointer<AdminUserService> adminService, QWidget *parent = nullptr);

private slots:
    void onSearchTextChanged(const QString &text);
    void onClearSearchClicked();
    void onAddUserClicked();
    void onTableItemChanged(QTableWidgetItem *item);

private:
    void setupUI();
    void loadUsers();
    void setupTableColumns();

    QSharedPointer<AdminUserService> _adminService;
    bool _isLoadingData;

    enum Columns {
        ID = 0,
        Username,
        IsBlocked,
        Restrictions,
        MinLength,
        Expiration,
        ColumnCount
    };

    QLineEdit* _searchEdit;
    QPushButton* _searchButton;
    QPushButton* _addUserButton;
    QTableWidget* _usersTable;
};

#endif // USERLISTWINDOW_H
