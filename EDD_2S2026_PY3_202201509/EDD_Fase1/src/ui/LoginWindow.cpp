#include "../../include/ui/LoginWindow.hpp"
#include "../../include/ui/MainWindow.hpp"
#include "../../include/models/SistemaCine.hpp"
#include "../../include/models/Cliente.hpp"
#include "../../include/structures/EstructuraClientes.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QFont>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QString>

// ─── Credenciales de administrador ──────────────────────────────────────────
static const QString ADMIN_USUARIO  = "AdminCine@gmail.com";
static const QString ADMIN_PASSWORD = "admin123Pass";

LoginWindow::LoginWindow(QWidget* parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle("EDD Cinema USAC — Acceso");
    setFixedSize(420, 580);
    setupUI();
}

void LoginWindow::setupUI() {
    // ── Fondo oscuro con gradiente
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #0D1117, stop:1 #161B22
            );
        }
        QLabel#lblTitle {
            color: #E2E8F0;
            font-size: 24pt;
            font-weight: bold;
            font-family: "Segoe UI";
        }
        QLabel#lblSubtitle {
            color: #8B949E;
            font-size: 10pt;
            font-family: "Segoe UI";
        }
        QLabel#lblField {
            color: #C9D1D9;
            font-size: 10pt;
            font-family: "Segoe UI";
            font-weight: 600;
        }
        QLabel#lblError {
            color: #EF4444;
            font-size: 9pt;
            font-family: "Segoe UI";
        }
        QLabel#lblHint {
            color: #8B949E;
            font-size: 8pt;
            font-family: "Segoe UI";
        }
        QLineEdit {
            background-color: #21262D;
            border: 1px solid #30363D;
            border-radius: 8px;
            color: #E2E8F0;
            font-size: 11pt;
            font-family: "Segoe UI";
            padding: 10px 14px;
            min-height: 20px;
        }
        QLineEdit:focus {
            border: 1px solid #7C3AED;
        }
        QPushButton#btnLogin {
            background: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #7C3AED, stop:1 #3B82F6
            );
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 12pt;
            font-weight: bold;
            font-family: "Segoe UI";
            padding: 12px;
            min-height: 24px;
        }
        QPushButton#btnLogin:hover {
            background: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #6D28D9, stop:1 #2563EB
            );
        }
        QPushButton#btnLogin:pressed {
            background: #5B21B6;
        }
        QPushButton#btnRegistrar {
            background: transparent;
            color: #7C3AED;
            border: 1px solid #7C3AED;
            border-radius: 8px;
            font-size: 11pt;
            font-weight: bold;
            font-family: "Segoe UI";
            padding: 10px;
            min-height: 20px;
        }
        QPushButton#btnRegistrar:hover {
            background: rgba(124, 58, 237, 0.15);
            border-color: #6D28D9;
        }
        QPushButton#btnRegistrar:pressed {
            background: rgba(124, 58, 237, 0.3);
        }
    )");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 36, 40, 36);
    mainLayout->setSpacing(0);

    // ── Logo / Título
    QLabel* lblIcon = new QLabel("🎬");
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setStyleSheet("font-size: 48pt; background: transparent;");
    mainLayout->addWidget(lblIcon);
    mainLayout->addSpacing(6);

    QLabel* lblTitle = new QLabel("EDD Cinema USAC");
    lblTitle->setObjectName("lblTitle");
    lblTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblTitle);

    QLabel* lblSubtitle = new QLabel("Sistema de Gestión — Hito 4");
    lblSubtitle->setObjectName("lblSubtitle");
    lblSubtitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblSubtitle);
    mainLayout->addSpacing(28);

    // ── Campo Usuario
    QLabel* lblU = new QLabel("Usuario / Correo");
    lblU->setObjectName("lblField");
    mainLayout->addWidget(lblU);
    mainLayout->addSpacing(4);

    inputUsuario = new QLineEdit();
    inputUsuario->setPlaceholderText("correo@ejemplo.com");
    inputUsuario->setObjectName("inputUsuario");
    mainLayout->addWidget(inputUsuario);
    mainLayout->addSpacing(16);

    // ── Campo Contraseña
    QLabel* lblP = new QLabel("Contraseña");
    lblP->setObjectName("lblField");
    mainLayout->addWidget(lblP);
    mainLayout->addSpacing(4);

    inputPassword = new QLineEdit();
    inputPassword->setPlaceholderText("••••••••");
    inputPassword->setEchoMode(QLineEdit::Password);
    inputPassword->setObjectName("inputPassword");
    connect(inputPassword, &QLineEdit::returnPressed, this, &LoginWindow::onLogin);
    mainLayout->addWidget(inputPassword);
    mainLayout->addSpacing(8);

    // ── Etiqueta de error
    lblError = new QLabel("");
    lblError->setObjectName("lblError");
    lblError->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblError);
    mainLayout->addSpacing(14);

    // ── Botón Login
    btnLogin = new QPushButton("Ingresar al Sistema");
    btnLogin->setObjectName("btnLogin");
    btnLogin->setCursor(Qt::PointingHandCursor);
    connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::onLogin);
    mainLayout->addWidget(btnLogin);
    mainLayout->addSpacing(10);

    // ── Separador
    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #30363D; background: #30363D;");
    mainLayout->addWidget(sep);
    mainLayout->addSpacing(10);

    // ── Botón Registrarse
    btnRegistrar = new QPushButton("✨  Crear Cuenta Nueva");
    btnRegistrar->setObjectName("btnRegistrar");
    btnRegistrar->setCursor(Qt::PointingHandCursor);
    connect(btnRegistrar, &QPushButton::clicked, this, &LoginWindow::onRegistrar);
    mainLayout->addWidget(btnRegistrar);
    mainLayout->addSpacing(12);

    // ── Hint de credenciales (admin)
    QLabel* lblHint = new QLabel("Admin: AdminCine@gmail.com  /  admin123Pass");
    lblHint->setObjectName("lblHint");
    lblHint->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblHint);

    mainLayout->addStretch();
}

// ─────────────────────────────────────────────────────────────
// SLOT: Iniciar sesión
// ─────────────────────────────────────────────────────────────
void LoginWindow::onLogin() {
    QString usuario  = inputUsuario->text().trimmed();
    QString password = inputPassword->text();

    lblError->setText("");

    // ── Acceso administrador (credenciales actualizadas)
    if (usuario == ADMIN_USUARIO && password == ADMIN_PASSWORD) {
        rolIngresado = "admin";
        SistemaCine::getInstancia().clienteLogueado = nullptr;
        emit loginExitoso("admin");
        MainWindow* mainWin = new MainWindow("admin");
        connect(mainWin, &MainWindow::logout, this, [this, mainWin]() {
            this->show();
            this->inputPassword->clear();
            mainWin->deleteLater();
        });
        mainWin->show();
        this->hide();
    } else {
        // ── Acceso como cliente (busca en EstructuraClientes → ArbolB)
        EstructuraClientes& listaClientes = SistemaCine::getInstancia().clientes;
        NodoCliente* nc = listaClientes.buscarPorCredenciales(
            usuario.toStdString(), password.toStdString());

        if (nc != nullptr) {
            rolIngresado = "cliente";
            SistemaCine::getInstancia().clienteLogueado = &(nc->cliente);
            emit loginExitoso("cliente");
            MainWindow* mainWin = new MainWindow("cliente");
            connect(mainWin, &MainWindow::logout, this, [this, mainWin]() {
                this->show();
                this->inputPassword->clear();
                mainWin->deleteLater();
            });
            mainWin->show();
            this->hide();
        } else {
            lblError->setText("⚠  Usuario o contraseña incorrectos");
            inputPassword->clear();
            inputPassword->setFocus();
        }
    }
}

// ─────────────────────────────────────────────────────────────
// SLOT: Abrir formulario de registro
// ─────────────────────────────────────────────────────────────
void LoginWindow::onRegistrar() {
    // ── Creamos el diálogo de registro
    QDialog dlg(this);
    dlg.setWindowTitle("🎬  EDD Cinema USAC — Registro de Cliente");
    dlg.setFixedSize(440, 520);
    dlg.setStyleSheet(R"(
        QDialog {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #0D1117, stop:1 #161B22
            );
        }
        QLabel {
            color: #C9D1D9;
            font-family: "Segoe UI";
            font-size: 10pt;
        }
        QLabel#lblTitle {
            color: #E2E8F0;
            font-size: 16pt;
            font-weight: bold;
        }
        QLabel#lblError {
            color: #EF4444;
            font-size: 9pt;
        }
        QLabel#lblSuccess {
            color: #10B981;
            font-size: 9pt;
            font-weight: bold;
        }
        QLineEdit {
            background-color: #21262D;
            border: 1px solid #30363D;
            border-radius: 8px;
            color: #E2E8F0;
            font-size: 10pt;
            font-family: "Segoe UI";
            padding: 9px 12px;
        }
        QLineEdit:focus { border: 1px solid #7C3AED; }
        QPushButton#btnOk {
            background: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #10B981, stop:1 #059669
            );
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 11pt;
            font-weight: bold;
            font-family: "Segoe UI";
            padding: 11px;
        }
        QPushButton#btnOk:hover { background: #059669; }
        QPushButton#btnCancel {
            background: transparent;
            color: #8B949E;
            border: 1px solid #30363D;
            border-radius: 8px;
            font-size: 10pt;
            font-family: "Segoe UI";
            padding: 9px;
        }
        QPushButton#btnCancel:hover { background: #21262D; color: #E2E8F0; }
    )");

    QVBoxLayout* vMain = new QVBoxLayout(&dlg);
    vMain->setContentsMargins(36, 28, 36, 28);
    vMain->setSpacing(0);

    // Título
    QLabel* lblTitReg = new QLabel("✨  Crear Cuenta Nueva");
    lblTitReg->setObjectName("lblTitle");
    lblTitReg->setAlignment(Qt::AlignCenter);
    vMain->addWidget(lblTitReg);
    vMain->addSpacing(4);

    QLabel* lblSub = new QLabel("Completa los campos para registrarte como cliente");
    lblSub->setAlignment(Qt::AlignCenter);
    lblSub->setStyleSheet("color: #8B949E; font-size: 9pt;");
    vMain->addWidget(lblSub);
    vMain->addSpacing(20);

    // Campos del formulario
    auto makeField = [&](const QString& placeholder, bool esPassword = false) -> QLineEdit* {
        QLineEdit* le = new QLineEdit();
        le->setPlaceholderText(placeholder);
        if (esPassword) le->setEchoMode(QLineEdit::Password);
        return le;
    };

    // ── Autogenerar ID: usamos el tamaño actual + 1 con prefijo "UC"
    EstructuraClientes& listaClientes = SistemaCine::getInstancia().clientes;
    QString idGenerado = QString("UC%1").arg(listaClientes.getTamanio() + 1, 3, 10, QChar('0'));

    QLabel* lblIdDisplay = new QLabel(QString("ID asignado automáticamente:  <b>%1</b>").arg(idGenerado));
    lblIdDisplay->setStyleSheet("color: #60A5FA; font-size: 9pt; background: #21262D;"
                                " border: 1px solid #30363D; border-radius: 6px; padding: 8px 12px;");
    vMain->addWidget(lblIdDisplay);
    vMain->addSpacing(12);

    // Campos
    QFormLayout* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setSpacing(10);

    QLineEdit* inpNombre   = makeField("Ej. Juan Pérez García");
    QLineEdit* inpCorreo   = makeField("Ej. juan@correo.com");
    QLineEdit* inpTelefono = makeField("Ej. 5555-1234");
    QLineEdit* inpPass     = makeField("Mínimo 6 caracteres", true);
    QLineEdit* inpPass2    = makeField("Repite tu contraseña", true);

    QLabel* lblNom = new QLabel("Nombre completo:");
    QLabel* lblCor = new QLabel("Correo electrónico:");
    QLabel* lblTel = new QLabel("Teléfono:");
    QLabel* lblPw  = new QLabel("Contraseña:");
    QLabel* lblPw2 = new QLabel("Confirmar contraseña:");

    form->addRow(lblNom, inpNombre);
    form->addRow(lblCor, inpCorreo);
    form->addRow(lblTel, inpTelefono);
    form->addRow(lblPw,  inpPass);
    form->addRow(lblPw2, inpPass2);
    vMain->addLayout(form);
    vMain->addSpacing(10);

    // Etiqueta de error del formulario
    QLabel* lblErrReg = new QLabel("");
    lblErrReg->setObjectName("lblError");
    lblErrReg->setAlignment(Qt::AlignCenter);
    lblErrReg->setWordWrap(true);
    vMain->addWidget(lblErrReg);
    vMain->addSpacing(14);

    // Botones
    QHBoxLayout* hBtns = new QHBoxLayout();
    QPushButton* btnCancel = new QPushButton("Cancelar");
    btnCancel->setObjectName("btnCancel");
    btnCancel->setCursor(Qt::PointingHandCursor);
    QPushButton* btnOk = new QPushButton("✅  Registrarme");
    btnOk->setObjectName("btnOk");
    btnOk->setCursor(Qt::PointingHandCursor);
    hBtns->addWidget(btnCancel);
    hBtns->addWidget(btnOk);
    vMain->addLayout(hBtns);

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    // ── Lógica del botón Registrar
    connect(btnOk, &QPushButton::clicked, &dlg, [&]() {
        lblErrReg->setText("");
        lblErrReg->setObjectName("lblError");

        QString nombre   = inpNombre->text().trimmed();
        QString correo   = inpCorreo->text().trimmed().toLower();
        QString telefono = inpTelefono->text().trimmed();
        QString pass     = inpPass->text();
        QString pass2    = inpPass2->text();

        // ── 1. Validar campos vacíos
        if (nombre.isEmpty() || correo.isEmpty() || telefono.isEmpty()
                || pass.isEmpty() || pass2.isEmpty()) {
            lblErrReg->setText("⚠  Todos los campos son obligatorios.");
            return;
        }

        // ── 2. Validar formato de correo básico
        if (!correo.contains('@') || !correo.contains('.')) {
            lblErrReg->setText("⚠  El correo electrónico no es válido.");
            return;
        }

        // ── 3. Validar contraseñas
        if (pass.length() < 6) {
            lblErrReg->setText("⚠  La contraseña debe tener al menos 6 caracteres.");
            return;
        }
        if (pass != pass2) {
            lblErrReg->setText("⚠  Las contraseñas no coinciden.");
            return;
        }

        // ── 4. BUSCAR EN EL ÁRBOL B si el correo ya existe
        //      (usamos el método buscarPorCorreo() de EstructuraClientes que delega al ArbolB)
        EstructuraClientes& clts = SistemaCine::getInstancia().clientes;
        NodoCliente* existente = clts.buscarPorCorreo(correo.toStdString());
        if (existente != nullptr) {
            lblErrReg->setText("⚠  El correo ya está en uso. Usa otro correo o inicia sesión.");
            return;
        }

        // ── 5. Recalcular ID (tamaño podría haber cambiado si otros se registraron)
        QString idFinal = QString("UC%1").arg(clts.getTamanio() + 1, 3, 10, QChar('0'));

        // ── 6. Crear el objeto Cliente e insertar en EstructuraClientes → ArbolB
        Cliente nuevoCliente(
            idFinal.toStdString(),
            nombre.toStdString(),
            correo.toStdString(),
            pass.toStdString(),
            telefono.toStdString()
        );

        bool insertado = clts.insertar(nuevoCliente);
        if (!insertado) {
            // ID duplicado (muy raro, pero lo manejamos)
            lblErrReg->setText("⚠  Error interno al generar ID. Intenta de nuevo.");
            return;
        }

        // ── 7. Éxito: mostrar mensaje y cerrar el formulario
        QMessageBox::information(
            &dlg,
            "✅  ¡Registro Exitoso!",
            QString(
                "¡Bienvenido/a, <b>%1</b>!<br><br>"
                "Tu cuenta ha sido creada exitosamente.<br>"
                "<b>ID asignado:</b>  %2<br>"
                "<b>Correo:</b>  %3<br><br>"
                "Ya puedes iniciar sesión con tu correo y contraseña."
            ).arg(nombre).arg(idFinal).arg(correo)
        );

        dlg.accept();

        // Pre-llenar el campo de usuario en el login para facilitar el acceso
        inputUsuario->setText(correo);
        inputPassword->clear();
        inputPassword->setFocus();
    });

    dlg.exec();
}
