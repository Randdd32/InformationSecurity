#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>

#include "headers/services/adminuserservice.h"

class AddUserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddUserDialog(QSharedPointer<AdminUserService> adminService, QWidget *parent = nullptr);

private slots:
    void onCreateClicked();

private:
    void setupUI();

    QSharedPointer<AdminUserService> _adminService;

    QLabel* _titleLabel;
    QLabel* _nameLabel;
    QLineEdit* _nameEdit;
    QPushButton* _createButton;
    QPushButton* _cancelButton;
    QVBoxLayout* _mainLayout;
};

#endif // ADDUSERDIALOG_H
