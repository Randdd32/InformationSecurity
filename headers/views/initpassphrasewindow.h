#ifndef INITPASSPHRASEWINDOW_H
#define INITPASSPHRASEWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>

#include "headers/services/authservice.h"

class InitPassphraseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InitPassphraseWindow(QSharedPointer<AuthService> authService, const QString& encryptedFile, QWidget *parent = nullptr);
    ~InitPassphraseWindow();

private slots:
    void onEnterClicked();

private:
    void setupUI();
    void applyStyle();

    QSharedPointer<AuthService> _authService;
    bool _isFirstRun;

    QLabel* _titleLabel;
    QLabel* _promptLabel;
    QLineEdit* _passphraseEdit;
    QPushButton* _enterButton;
    QPushButton* _cancelButton;
    QVBoxLayout* _mainLayout;

    QString getStatusMessage(InitDBStatus status);
};

#endif // INITPASSPHRASEWINDOW_H
