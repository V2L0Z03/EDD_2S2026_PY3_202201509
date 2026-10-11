#ifndef LOGIN_WINDOW_HPP
#define LOGIN_WINDOW_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    QString getRol() const { return rolIngresado; }

signals:
    void loginExitoso(const QString& rol);

private slots:
    void onLogin();
    void onRegistrar();

private:
    void setupUI();

    QLineEdit*   inputUsuario;
    QLineEdit*   inputPassword;
    QPushButton* btnLogin;
    QPushButton* btnRegistrar;
    QLabel*      lblError;
    QString      rolIngresado;
};

#endif // LOGIN_WINDOW_HPP
