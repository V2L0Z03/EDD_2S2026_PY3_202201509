
#include "../../include/ui/MainWindow.hpp"
#include "../../include/services/JSONReader.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QDir>
#include <QFile>
#include <QScrollBar>
#include <QSizePolicy>
#include <QFrame>
#include <QString>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QDateTime>
#include <QDebug>

// ─────────────────────────────────────────────────────────────
// HELPERS DE CREACIÓN DE WIDGETS
// ─────────────────────────────────────────────────────────────

static QPushButton* makeBtn(const QString& text, const QString& objName = "") {
    auto* b = new QPushButton(text);
    if (!objName.isEmpty()) b->setObjectName(objName);
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

static QGroupBox* makeGroup(const QString& title) {
    auto* g = new QGroupBox(title);
    return g;
}

static QScrollArea* makeScrollArea(QWidget* content) {
    auto* sa = new QScrollArea();
    sa->setWidget(content);
    sa->setWidgetResizable(true);
    sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    return sa;
}

// ─────────────────────────────────────────────────────────────
// CONSTRUCTOR
// ─────────────────────────────────────────────────────────────
MainWindow::MainWindow(const QString& rol, QWidget* parent)
    : QMainWindow(parent),
      rolActual(rol),
      cartelera(SistemaCine::getInstancia().cartelera),
      funciones(SistemaCine::getInstancia().funciones),
      promociones(SistemaCine::getInstancia().promociones),
      solicitudes(SistemaCine::getInstancia().solicitudes),
      clientes(SistemaCine::getInstancia().clientes),
      clienteActual(SistemaCine::getInstancia().clienteLogueado),
      contadorSolicitudes(SistemaCine::getInstancia().contadorSolicitudes),
      currPelicula(SistemaCine::getInstancia().currPelicula),
      currHorario(SistemaCine::getInstancia().currHorario),
      currSala(SistemaCine::getInstancia().currSala),
      currentAdminMovieNode(nullptr),
      currentClientMovieNode(nullptr),
      currentAdminHorario("14:00"),
      currentAdminSala("Sala 1"),
      currentClientHorario("14:00"),
      currentClientSala("Sala 1")
{
    // Crear directorio de persistencia de asientos si no existe
    QDir().mkpath("Asientos");

    setupGlobalStyle();
    setWindowTitle(QString("EDD Cinema USAC — Panel ") + (rol == "admin" ? "Administrador" : "Cliente"));
    resize(1200, 750);
    setMinimumSize(900, 600);

    setupUI(rol);
}

// ─────────────────────────────────────────────────────────────
// STYLESHEET GLOBAL
// ─────────────────────────────────────────────────────────────
void MainWindow::setupGlobalStyle() {
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #0D1117;
            color: #E2E8F0;
            font-family: "Segoe UI";
            font-size: 10pt;
        }
        QTabWidget::pane {
            border: 1px solid #30363D;
            background-color: #161B22;
            border-radius: 0px 6px 6px 6px;
        }
        QTabBar::tab {
            background-color: #21262D;
            color: #8B949E;
            padding: 9px 18px;
            border: 1px solid #30363D;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #7C3AED;
            color: #FFFFFF;
            font-weight: bold;
        }
        QTabBar::tab:hover:!selected {
            background-color: #2D333B;
            color: #E2E8F0;
        }
        QPushButton {
            background-color: #7C3AED;
            color: white;
            border: none;
            padding: 8px 18px;
            border-radius: 6px;
            font-weight: 600;
        }
        QPushButton:hover   { background-color: #6D28D9; }
        QPushButton:pressed { background-color: #5B21B6; }
        QPushButton[role="danger"] {
            background-color: #EF4444;
        }
        QPushButton[role="danger"]:hover { background-color: #DC2626; }
        QPushButton[role="success"] {
            background-color: #10B981;
        }
        QPushButton[role="success"]:hover { background-color: #059669; }
        QPushButton[role="warning"] {
            background-color: #F59E0B;
            color: #0D1117;
        }
        QPushButton[role="warning"]:hover { background-color: #D97706; }
        QPushButton[role="info"] {
            background-color: #3B82F6;
        }
        QPushButton[role="info"]:hover { background-color: #2563EB; }
        QTableWidget {
            background-color: #161B22;
            border: 1px solid #30363D;
            gridline-color: #21262D;
            color: #E2E8F0;
            selection-background-color: #4C1D95;
        }
        QTableWidget::item { padding: 6px; }
        QHeaderView::section {
            background-color: #21262D;
            color: #8B949E;
            padding: 7px;
            border: 1px solid #30363D;
            font-weight: 600;
        }
        QLineEdit, QTextEdit, QSpinBox, QComboBox {
            background-color: #21262D;
            border: 1px solid #30363D;
            color: #E2E8F0;
            padding: 7px 10px;
            border-radius: 5px;
        }
        QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QComboBox:focus {
            border-color: #7C3AED;
        }
        QComboBox::drop-down {
            border: none;
            background: #30363D;
            width: 22px;
            border-top-right-radius: 5px;
            border-bottom-right-radius: 5px;
        }
        QComboBox QAbstractItemView {
            background-color: #21262D;
            border: 1px solid #30363D;
            color: #E2E8F0;
            selection-background-color: #7C3AED;
        }
        QListWidget {
            background-color: #161B22;
            border: 1px solid #30363D;
            color: #E2E8F0;
        }
        QListWidget::item { padding: 6px 10px; }
        QListWidget::item:selected  { background-color: #4C1D95; }
        QListWidget::item:hover     { background-color: #21262D; }
        QScrollArea { border: none; background: #0D1117; }
        QGroupBox {
            border: 1px solid #30363D;
            border-radius: 6px;
            margin-top: 14px;
            padding-top: 10px;
            color: #E2E8F0;
        }
        QGroupBox::title {
            color: #7C3AED;
            subcontrol-origin: margin;
            padding: 2px 10px;
            font-weight: 700;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #30363D;
            border: none;
            width: 16px;
        }
        QLabel { color: #E2E8F0; }
        QLabel#lblSection {
            color: #7C3AED;
            font-size: 12pt;
            font-weight: 700;
        }
        QLabel#lblWarning {
            color: #F59E0B;
            font-size: 9pt;
        }
        QSplitter::handle { background: #30363D; }
        QMessageBox { background-color: #161B22; }
        QDialog { background-color: #161B22; }
    )");
}

// ─────────────────────────────────────────────────────────────
// SETUP GENERAL
// ─────────────────────────────────────────────────────────────
void MainWindow::setupUI(const QString& rol) {
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    setupAdminPanel();
    setupClientePanel();

    mainStack->addWidget(adminPanel);   // índice 0
    mainStack->addWidget(clientePanel); // índice 1

    if (rol == "admin") {
        mainStack->setCurrentIndex(0);
        refreshCarteleraAdmin();
        refreshFuncionesAdmin();
        refreshClientesAdmin();
        refreshAdminSeatMap();
        refreshSolicitudesAdmin();
        refreshPromocionesAdmin();
        refreshPromocionesCombo();
    } else {
        mainStack->setCurrentIndex(1);
        refreshCarteleraCliente();
        refreshMisReservasCliente();
        refreshPeliculasCombo();
        refreshClienteSeatMap();
        refreshPromocionesCliente();
    }
}

// ─────────────────────────────────────────────────────────────
// HELPER: llenar QTableWidget con recorrido inorden del BST
// ─────────────────────────────────────────────────────────────
void MainWindow::llenarTablaInorden(NodoBST* nodo, QTableWidget* tabla, int& filaTabla) {
    if (nodo == nullptr) return;

    llenarTablaInorden(nodo->izquierdo, tabla, filaTabla);

    QDate hoy = QDate::currentDate();
    QDate fechaEstreno = QDate::fromString(QString::fromStdString(nodo->pelicula.getFechaEstreno()), "yyyy-MM-dd");
    QDate fechaFin = QDate::fromString(QString::fromStdString(nodo->pelicula.getFechaFin()), "yyyy-MM-dd");
    
    bool esProximo = fechaEstreno.isValid() && fechaEstreno > hoy;
    bool prontoFin = fechaFin.isValid() && fechaFin <= hoy.addDays(7) && !esProximo;

    tabla->insertRow(filaTabla);

    QString strEstado = "En cartelera";
    if (esProximo) strEstado = "Próximo estreno";
    else if (prontoFin) strEstado = "Próximo a retirar";

    auto setItem = [&](int col, const QString& text) {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        if (esProximo) {
            item->setForeground(QColor("#60A5FA")); // Azul
        } else if (prontoFin) {
            item->setForeground(QColor("#FBBF24")); // Amarillo
        } else {
            item->setForeground(QColor("#34D399")); // Verde
        }
        tabla->setItem(filaTabla, col, item);
    };

    setItem(0, QString::fromStdString(nodo->pelicula.getCodigo()));
    setItem(1, QString::fromStdString(nodo->pelicula.getTitulo()));
    setItem(2, QString::fromStdString(nodo->pelicula.getGenero()));
    setItem(3, QString::number(nodo->pelicula.getDuracionMinutos()) + " min");
    setItem(4, QString::fromStdString(nodo->pelicula.getClasificacion()));
    setItem(5, QString::fromStdString(nodo->pelicula.getIdioma()));
    setItem(6, QString::fromStdString(nodo->pelicula.getFechaEstreno()));
    setItem(7, QString::fromStdString(nodo->pelicula.getFechaFin()));
    setItem(8, strEstado);

    filaTabla++;
    llenarTablaInorden(nodo->derecho, tabla, filaTabla);
}

// ─────────────────────────────────────────────────────────────
// REFRESH CARTELERA ADMIN
// ─────────────────────────────────────────────────────────────
void MainWindow::refreshCarteleraAdmin() {
    tblCarteleraAdmin->setRowCount(0);
    int f = 0;

    std::vector<NodoBST*> nodosOrdenados;
    if (cmbOrdenCartelera) {
        int idx = cmbOrdenCartelera->currentIndex();
        if (idx == 1) cartelera.recorridoPreorden(nodosOrdenados);
        else if (idx == 2) cartelera.recorridoPostorden(nodosOrdenados);
        else cartelera.recorridoInorden(nodosOrdenados);
    } else {
        cartelera.recorridoInorden(nodosOrdenados);
    }

    for (NodoBST* nodo : nodosOrdenados) {
        if (!nodo) continue;
        QDate hoy = QDate::currentDate();
        QDate fechaEstreno = QDate::fromString(QString::fromStdString(nodo->pelicula.getFechaEstreno()), "yyyy-MM-dd");
        QDate fechaFin = QDate::fromString(QString::fromStdString(nodo->pelicula.getFechaFin()), "yyyy-MM-dd");
        
        bool esProximo = fechaEstreno.isValid() && fechaEstreno > hoy;
        bool prontoFin = fechaFin.isValid() && fechaFin <= hoy.addDays(7) && !esProximo;

        tblCarteleraAdmin->insertRow(f);

        QString strEstado = "En cartelera";
        if (esProximo) strEstado = "Próximo estreno";
        else if (prontoFin) strEstado = "Próximo a retirar";

        auto setItem = [&](int col, const QString& text) {
            QTableWidgetItem* item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            if (esProximo) {
                item->setForeground(QColor("#60A5FA")); // Azul
            } else if (prontoFin) {
                item->setForeground(QColor("#FBBF24")); // Amarillo
            } else {
                item->setForeground(QColor("#34D399")); // Verde
            }
            tblCarteleraAdmin->setItem(f, col, item);
        };

        setItem(0, QString::fromStdString(nodo->pelicula.getCodigo()));
        setItem(1, QString::fromStdString(nodo->pelicula.getTitulo()));
        setItem(2, QString::fromStdString(nodo->pelicula.getGenero()));
        setItem(3, QString::number(nodo->pelicula.getDuracionMinutos()) + " min");
        setItem(4, QString::fromStdString(nodo->pelicula.getClasificacion()));
        setItem(5, QString::fromStdString(nodo->pelicula.getIdioma()));
        setItem(6, QString::fromStdString(nodo->pelicula.getFechaEstreno()));
        setItem(7, QString::fromStdString(nodo->pelicula.getFechaFin()));
        setItem(8, strEstado);

        f++;
    }
    tblCarteleraAdmin->resizeColumnsToContents();
    refreshPeliculasCombo();
}

// ─────────────────────────────────────────────────────────────
// PANEL ADMIN — CONSTRUCCIÓN
// ─────────────────────────────────────────────────────────────
void MainWindow::setupAdminPanel() {
    adminPanel = new QWidget();
    QVBoxLayout* vMain = new QVBoxLayout(adminPanel);
    vMain->setContentsMargins(0, 0, 0, 0);

    // ── Banner de usuario
    QWidget* banner = new QWidget();
    banner->setFixedHeight(48);
    banner->setStyleSheet("background-color:#161B22; border-bottom:1px solid #30363D;");
    QHBoxLayout* bannerLayout = new QHBoxLayout(banner);
    bannerLayout->setContentsMargins(16, 0, 16, 0);
    QLabel* lblBanner = new QLabel("🎬  EDD Cinema USAC  —  Panel Administrador");
    lblBanner->setStyleSheet("color:#7C3AED; font-size:13pt; font-weight:700;");
    bannerLayout->addWidget(lblBanner);
    bannerLayout->addStretch();
    
    QPushButton* btnLogoutAdmin = makeBtn("🚪 Regresar");
    btnLogoutAdmin->setProperty("role", "warning");
    btnLogoutAdmin->style()->unpolish(btnLogoutAdmin); btnLogoutAdmin->style()->polish(btnLogoutAdmin);
    connect(btnLogoutAdmin, &QPushButton::clicked, this, &MainWindow::onLogout);
    bannerLayout->addWidget(btnLogoutAdmin);
    
    vMain->addWidget(banner);

    // ── Tabs
    adminTabs = new QTabWidget();
    adminTabs->setTabPosition(QTabWidget::North);
    vMain->addWidget(adminTabs);

    // ─────────────────────────────
    // TAB 0: CARTELERA (BST)
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        // Barra de herramientas
        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnCargarPF = makeBtn("📂  Cargar Peliculas y Funciones", "btnCargarPF");
        auto* btnCargarCR = makeBtn("📂  Cargar Clientes y Reservas", "btnCargarCR");
        auto* btnAdd  = makeBtn("➕  Agregar Película", "btnAdd");
        auto* btnDel  = makeBtn("🗑  Eliminar", "btnDel");
        btnDel->setProperty("role", "danger");
        btnDel->style()->unpolish(btnDel); btnDel->style()->polish(btnDel);

        cmbOrdenCartelera = new QComboBox();
        cmbOrdenCartelera->addItems({"Inorden", "Preorden", "Postorden"});
        connect(cmbOrdenCartelera, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this]() { this->refreshCarteleraAdmin(); });

        inpBuscarAdmin = new QLineEdit();
        inpBuscarAdmin->setPlaceholderText("🔍  Buscar por código...");
        inpBuscarAdmin->setMaximumWidth(220);
        auto* btnBusc  = makeBtn("Buscar", "btnBusc");
        btnBusc->setProperty("role", "info");
        btnBusc->style()->unpolish(btnBusc); btnBusc->style()->polish(btnBusc);

        toolbar->addWidget(btnCargarPF);
        toolbar->addWidget(btnCargarCR);
        toolbar->addWidget(btnAdd);
        toolbar->addWidget(btnDel);
        toolbar->addWidget(cmbOrdenCartelera);
        toolbar->addStretch();
        toolbar->addWidget(inpBuscarAdmin);
        toolbar->addWidget(btnBusc);
        v->addLayout(toolbar);

        // Leyenda
        QLabel* lblLeyenda = new QLabel("⚠  Fondo naranja = película próxima a vencer (≤ 7 días)");
        lblLeyenda->setObjectName("lblWarning");
        v->addWidget(lblLeyenda);

        // Tabla
        tblCarteleraAdmin = new QTableWidget(0, 9);
        tblCarteleraAdmin->setHorizontalHeaderLabels({
            "Código","Título","Género","Duración","Clasif.","Idioma","Estreno","Retiro","Estado"
        });
        tblCarteleraAdmin->horizontalHeader()->setStretchLastSection(true);
        tblCarteleraAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblCarteleraAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblCarteleraAdmin->setAlternatingRowColors(true);
        tblCarteleraAdmin->setStyleSheet(
            "QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblCarteleraAdmin);

        connect(btnCargarPF, &QPushButton::clicked, this, &MainWindow::onAdminCargarPeliculasFunciones);
        connect(btnCargarCR, &QPushButton::clicked, this, &MainWindow::onAdminCargarClientesReservas);
        connect(btnAdd,  &QPushButton::clicked, this, &MainWindow::onAdminAgregarPelicula);
        connect(btnDel,  &QPushButton::clicked, this, &MainWindow::onAdminEliminarPelicula);
        connect(btnBusc, &QPushButton::clicked, this, &MainWindow::onAdminBuscarPelicula);
        connect(inpBuscarAdmin, &QLineEdit::returnPressed, this, &MainWindow::onAdminBuscarPelicula);

        adminTabs->addTab(tab, "🎬  Cartelera BST");
    }

    // ─────────────────────────────
    // TAB 1: FUNCIONES (AVL - Fase 2)
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnAddFunc = makeBtn("➕  Agregar Función");
        auto* btnEditFunc = makeBtn("✏️  Editar Función");
        auto* btnDelFunc = makeBtn("🗑  Eliminar Función");
        btnDelFunc->setProperty("role", "danger");
        btnDelFunc->style()->unpolish(btnDelFunc); btnDelFunc->style()->polish(btnDelFunc);

        cmbOrdenFunciones = new QComboBox();
        cmbOrdenFunciones->addItems({"Inorden", "Preorden", "Postorden"});
        connect(cmbOrdenFunciones, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this]() { this->refreshFuncionesAdmin(); });

        inpBuscarFuncion = new QLineEdit();
        inpBuscarFuncion->setPlaceholderText("🔍  Buscar por código (ej. F001)...");
        inpBuscarFuncion->setMaximumWidth(220);
        auto* btnBuscFunc = makeBtn("Buscar", "btnBuscFunc");
        btnBuscFunc->setProperty("role", "info");
        btnBuscFunc->style()->unpolish(btnBuscFunc); btnBuscFunc->style()->polish(btnBuscFunc);

        toolbar->addWidget(btnAddFunc);
        toolbar->addWidget(btnEditFunc);
        toolbar->addWidget(btnDelFunc);
        toolbar->addWidget(cmbOrdenFunciones);
        toolbar->addStretch();
        toolbar->addWidget(inpBuscarFuncion);
        toolbar->addWidget(btnBuscFunc);
        v->addLayout(toolbar);

        tblFuncionesAdmin = new QTableWidget(0, 8);
        tblFuncionesAdmin->setHorizontalHeaderLabels({
            "Código", "Película", "Fecha", "Hora", "Sala", "Precio", "Altura", "FE"
        });
        tblFuncionesAdmin->horizontalHeader()->setStretchLastSection(true);
        tblFuncionesAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblFuncionesAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblFuncionesAdmin->setAlternatingRowColors(true);
        tblFuncionesAdmin->setStyleSheet("QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblFuncionesAdmin);

        connect(btnAddFunc,  &QPushButton::clicked, this, &MainWindow::onAdminAgregarFuncion);
        connect(btnEditFunc, &QPushButton::clicked, this, &MainWindow::onAdminEditarFuncion);
        connect(btnDelFunc,  &QPushButton::clicked, this, &MainWindow::onAdminEliminarFuncion);
        connect(btnBuscFunc, &QPushButton::clicked, this, &MainWindow::onAdminBuscarFuncion);
        connect(inpBuscarFuncion, &QLineEdit::returnPressed, this, &MainWindow::onAdminBuscarFuncion);

        adminTabs->addTab(tab, "📅  Funciones (AVL)");
    }

    // ─────────────────────────────
    // TAB 2: CLIENTES (Fase 2 - Paso 3)
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnAddCli = makeBtn("➕  Agregar Cliente");
        auto* btnEditCli = makeBtn("✏️  Editar Cliente");
        auto* btnDelCli = makeBtn("🗑  Eliminar Cliente");
        btnDelCli->setProperty("role", "danger");
        btnDelCli->style()->unpolish(btnDelCli); btnDelCli->style()->polish(btnDelCli);
        auto* btnVerResCli = makeBtn("🎟️  Ver Reservas");
        btnVerResCli->setProperty("role", "info");
        btnVerResCli->style()->unpolish(btnVerResCli); btnVerResCli->style()->polish(btnVerResCli);

        inpBuscarClienteAdmin = new QLineEdit();
        inpBuscarClienteAdmin->setPlaceholderText("🔍  Buscar por ID o Correo...");
        inpBuscarClienteAdmin->setMaximumWidth(220);
        auto* btnBuscCli = makeBtn("Buscar");
        btnBuscCli->setProperty("role", "info");
        btnBuscCli->style()->unpolish(btnBuscCli); btnBuscCli->style()->polish(btnBuscCli);

        toolbar->addWidget(btnAddCli);
        toolbar->addWidget(btnEditCli);
        toolbar->addWidget(btnDelCli);
        toolbar->addWidget(btnVerResCli);
        toolbar->addStretch();
        toolbar->addWidget(inpBuscarClienteAdmin);
        toolbar->addWidget(btnBuscCli);
        v->addLayout(toolbar);

        tblClientesAdmin = new QTableWidget(0, 6);
        tblClientesAdmin->setHorizontalHeaderLabels({
            "ID", "Nombre", "Correo", "Password", "Teléfono", "Cant. Reservas"
        });
        tblClientesAdmin->horizontalHeader()->setStretchLastSection(true);
        tblClientesAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblClientesAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblClientesAdmin->setAlternatingRowColors(true);
        tblClientesAdmin->setStyleSheet("QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblClientesAdmin);

        connect(btnAddCli, &QPushButton::clicked, this, &MainWindow::onAdminAgregarCliente);
        connect(btnEditCli, &QPushButton::clicked, this, &MainWindow::onAdminEditarCliente);
        connect(btnDelCli, &QPushButton::clicked, this, &MainWindow::onAdminEliminarCliente);
        connect(btnVerResCli, &QPushButton::clicked, this, &MainWindow::onAdminVerReservasCliente);
        connect(btnBuscCli, &QPushButton::clicked, this, &MainWindow::onAdminBuscarCliente);
        connect(inpBuscarClienteAdmin, &QLineEdit::returnPressed, this, &MainWindow::onAdminBuscarCliente);

        adminTabs->addTab(tab, "👥  Gestión de Clientes");
    }

    // ─────────────────────────────
    // TAB 2: PROMOCIONES
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* hMain = new QHBoxLayout(tab);
        hMain->setContentsMargins(12, 12, 12, 12);
        hMain->setSpacing(12);

        // ── Panel izquierdo: Alta de Promociones
        QGroupBox* grpAlta = makeGroup("Alta de Promoción");
        QVBoxLayout* vAlta = new QVBoxLayout(grpAlta);

        auto addField = [&](QVBoxLayout* layout, const QString& label, QLineEdit*& field,
                            const QString& placeholder) {
            layout->addWidget(new QLabel(label));
            field = new QLineEdit();
            field->setPlaceholderText(placeholder);
            layout->addWidget(field);
        };

        addField(vAlta, "Código:",   inpPromoCodigo,  "Ej. PROMO01");
        addField(vAlta, "Nombre:",   inpPromoNombre,  "Ej. Martes de Locura");
        addField(vAlta, "Vigencia:", inpPromoVigencia,"Ej. 2026-12-31");
        addField(vAlta, "Días:",     inpPromoDias,    "Ej. Martes");

        auto* btnCrear = makeBtn("✅  Crear Promoción");
        btnCrear->setProperty("role","success");
        btnCrear->style()->unpolish(btnCrear); btnCrear->style()->polish(btnCrear);
        vAlta->addWidget(btnCrear);
        vAlta->addSpacing(12);

        vAlta->addWidget(new QLabel("Promociones registradas:"));
        lstPromociones = new QListWidget();
        lstPromociones->setFixedHeight(150);
        vAlta->addWidget(lstPromociones);
        vAlta->addStretch();

        connect(btnCrear, &QPushButton::clicked, this, &MainWindow::onAdminCrearPromocion);

        // ── Panel derecho: Beneficios
        QGroupBox* grpBen = makeGroup("Gestión de Beneficios");
        QVBoxLayout* vBen = new QVBoxLayout(grpBen);

        vBen->addWidget(new QLabel("Seleccionar Promoción:"));
        cmbPromoSelect = new QComboBox();
        vBen->addWidget(cmbPromoSelect);

        addField(vBen, "Tipo:", inpBenTipo, "Ej. Descuento / 2x1");
        addField(vBen, "Descripción:", inpBenDesc, "Descripción del beneficio");
        addField(vBen, "Valor:", inpBenValor, "Ej. 10% / NA");

        auto* btnAddBen = makeBtn("➕  Agregar Beneficio");
        vBen->addWidget(btnAddBen);
        vBen->addSpacing(8);

        vBen->addWidget(new QLabel("Beneficios de la promoción:"));
        lstBeneficios = new QListWidget();
        vBen->addWidget(lstBeneficios);
        vBen->addStretch();

        connect(btnAddBen,    &QPushButton::clicked,       this, &MainWindow::onAdminAgregarBeneficio);
        connect(cmbPromoSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onPromoSeleccionada);

        hMain->addWidget(grpAlta, 1);
        hMain->addWidget(grpBen,  1);

        adminTabs->addTab(tab, "🎁  Promociones");
    }

    // ─────────────────────────────
    // TAB 2: SOLICITUDES
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnApro = makeBtn("✅  Aprobar");
        auto* btnRech = makeBtn("❌  Rechazar");
        auto* btnElim = makeBtn("🗑  Eliminar");
        btnApro->setProperty("role","success"); btnApro->style()->unpolish(btnApro); btnApro->style()->polish(btnApro);
        btnRech->setProperty("role","danger");  btnRech->style()->unpolish(btnRech); btnRech->style()->polish(btnRech);
        btnElim->setProperty("role","warning"); btnElim->style()->unpolish(btnElim); btnElim->style()->polish(btnElim);
        toolbar->addWidget(btnApro);
        toolbar->addWidget(btnRech);
        toolbar->addWidget(btnElim);
        toolbar->addStretch();
        QLabel* lblInfo = new QLabel("Seleccione una solicitud y use los botones para gestionarla");
        lblInfo->setObjectName("lblWarning");
        toolbar->addWidget(lblInfo);
        v->addLayout(toolbar);

        tblSolicitudes = new QTableWidget(0, 6);
        tblSolicitudes->setHorizontalHeaderLabels({"ID","Cliente","Teléfono","Tipo","Descripción","Estado"});
        tblSolicitudes->horizontalHeader()->setStretchLastSection(true);
        tblSolicitudes->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblSolicitudes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblSolicitudes->setAlternatingRowColors(true);
        tblSolicitudes->setStyleSheet("QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblSolicitudes);

        connect(btnApro, &QPushButton::clicked, this, &MainWindow::onAdminAprobarSolicitud);
        connect(btnRech, &QPushButton::clicked, this, &MainWindow::onAdminRechazarSolicitud);
        connect(btnElim, &QPushButton::clicked, this, &MainWindow::onAdminEliminarSolicitud);

        adminTabs->addTab(tab, "📋  Solicitudes");
    }

    // ─────────────────────────────
    // TAB 3: SALA (MATRIZ DISPERSA)
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* hMain = new QHBoxLayout(tab);
        hMain->setContentsMargins(12, 12, 12, 12);
        hMain->setSpacing(12);

        // ── Panel izquierdo: configuración
        QGroupBox* grpConfig = makeGroup("Configuración de la Sala");
        grpConfig->setMaximumWidth(300);
        QFormLayout* form = new QFormLayout(grpConfig);

        cmbPeliculaMatriz = new QComboBox();
        cmbFuncionMatriz  = new QComboBox();
        cmbHorarioMatriz  = new QComboBox();
        cmbSalaMatriz = new QComboBox();

        form->addRow("Película:", cmbPeliculaMatriz);
        form->addRow("Función (AVL):", cmbFuncionMatriz);
        form->addRow("Horario:", cmbHorarioMatriz);
        form->addRow("Sala:", cmbSalaMatriz);

        connect(cmbPeliculaMatriz, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onAdminMovieSelectionChanged);
        connect(cmbFuncionMatriz, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int idx) {
                    if (idx < 0) return;
                    QString cod = cmbFuncionMatriz->itemData(idx).toString();
                    NodoAVL* nodo = funciones.buscar(cod.toStdString());
                    if (nodo) {
                        cmbHorarioMatriz->clear();
                        cmbHorarioMatriz->addItem(QString::fromStdString(nodo->funcion.getHora()));
                        cmbSalaMatriz->clear();
                        cmbSalaMatriz->addItem(QString::fromStdString(nodo->funcion.getSala()));
                        // Seleccionar película automáticamente
                        int peliIdx = cmbPeliculaMatriz->findData(QString::fromStdString(nodo->funcion.getCodigoPelicula()));
                        if (peliIdx >= 0) cmbPeliculaMatriz->setCurrentIndex(peliIdx);
                    }
                });
        connect(cmbHorarioMatriz, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onAdminFuncionChanged);
        connect(cmbSalaMatriz, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onAdminFuncionChanged);

        spnFilas    = new QSpinBox(); spnFilas->setRange(1, 20);    spnFilas->setValue(5);
        spnColumnas = new QSpinBox(); spnColumnas->setRange(1, 20); spnColumnas->setValue(5);
        form->addRow("Filas:", spnFilas);
        form->addRow("Columnas:", spnColumnas);

        auto* btnConf = makeBtn("⚙  Configurar Sala");
        btnConf->setProperty("role","info"); btnConf->style()->unpolish(btnConf); btnConf->style()->polish(btnConf);
        form->addRow(btnConf);

        auto* btnRefreshMapa = makeBtn("🔄  Actualizar Mapa");
        form->addRow(btnRefreshMapa);

        lblSalaInfo = new QLabel(QString("Sala actual: 5 x 5 = 25 asientos"));
        lblSalaInfo->setStyleSheet("color:#10B981; font-size:11pt; font-weight:600;");
        form->addRow(lblSalaInfo);

        QLabel* leyendaAdmin = new QLabel("🟢 Verde = Libre  |  🔴 Rojo = Reservado");
        leyendaAdmin->setObjectName("lblWarning");
        form->addRow(leyendaAdmin);

        QLabel* nota = new QLabel(
            "⚠  Al reconfigurar la sala se perderán\n"
            "   todas las reservas actuales.");
        nota->setObjectName("lblWarning");
        form->addRow(nota);

        connect(btnConf,       &QPushButton::clicked, this, &MainWindow::onAdminConfigurarSala);
        connect(btnRefreshMapa,&QPushButton::clicked, this, &MainWindow::refreshAdminSeatMap);

        // ── Panel derecho: mapa visual de asientos (solo lectura para el admin)
        QGroupBox* grpMapa = makeGroup("Mapa de Asientos en Tiempo Real");
        QVBoxLayout* vMapa = new QVBoxLayout(grpMapa);

        widgetAdminSeatMap = new QWidget();
        gridAdminSeatMap   = new QGridLayout(widgetAdminSeatMap);
        gridAdminSeatMap->setSpacing(4);

        QScrollArea* scrollAdmin = new QScrollArea();
        scrollAdmin->setWidget(widgetAdminSeatMap);
        scrollAdmin->setWidgetResizable(true);
        vMapa->addWidget(scrollAdmin);

        hMain->addWidget(grpConfig);
        hMain->addWidget(grpMapa, 1);

        adminTabs->addTab(tab, "🏛  Sala");
    }

    // ─────────────────────────────
    // TAB 4: REPORTES GRAPHVIZ
    // ─────────────────────────────
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnTodos = makeBtn("🔄  Regenerar Todos");
        btnTodos->setProperty("role","info"); btnTodos->style()->unpolish(btnTodos); btnTodos->style()->polish(btnTodos);
        auto* btnBST   = makeBtn("Árbol BST");
        auto* btnAVL   = makeBtn("Árbol AVL");
        auto* btnSol   = makeBtn("Solicitudes");
        auto* btnPro   = makeBtn("Promociones");
        auto* btnMat   = makeBtn("Matriz");
        auto* btnCli   = makeBtn("Clientes");
        toolbar->addWidget(btnTodos);
        toolbar->addWidget(new QLabel(" | "));
        toolbar->addWidget(btnBST);
        toolbar->addWidget(btnAVL);
        toolbar->addWidget(btnSol);
        toolbar->addWidget(btnPro);
        toolbar->addWidget(btnMat);
        toolbar->addWidget(btnCli);
        toolbar->addStretch();
        v->addLayout(toolbar);

        reportesTabs = new QTabWidget();

        auto makeReportTab = [&](QLabel*& imgLabel, const QString& titulo) {
            QWidget* rt = new QWidget();
            QVBoxLayout* rv = new QVBoxLayout(rt);
            rv->setContentsMargins(4, 4, 4, 4);
            imgLabel = new QLabel("⏳  Presione 'Regenerar Todos' para generar el reporte.");
            imgLabel->setAlignment(Qt::AlignCenter);
            imgLabel->setStyleSheet("color:#8B949E; font-size:11pt;");
            imgLabel->setMinimumSize(600, 400);
            QScrollArea* sa = makeScrollArea(imgLabel);
            rv->addWidget(sa);
            reportesTabs->addTab(rt, titulo);
        };

        makeReportTab(imgBST,          "🌳  Árbol BST (Cartelera)");
        makeReportTab(imgAVL,          "⚖️  Árbol AVL (Funciones)");
        makeReportTab(imgSolicitudes,   "🔗  Solicitudes");
        makeReportTab(imgPromociones,   "🎁  Promociones");
        makeReportTab(imgMatriz,        "🗺  Matriz (Asientos)");
        makeReportTab(imgClientes,      "👥  Clientes y Reservas");

        v->addWidget(reportesTabs);

        connect(btnTodos, &QPushButton::clicked, this, &MainWindow::onGenerarTodosReportes);
        connect(btnBST,   &QPushButton::clicked, this, &MainWindow::onGenerarReporteBST);
        connect(btnAVL,   &QPushButton::clicked, this, &MainWindow::onGenerarReporteAVL);
        connect(btnSol,   &QPushButton::clicked, this, &MainWindow::onGenerarReporteSolicitudes);
        connect(btnPro,   &QPushButton::clicked, this, &MainWindow::onGenerarReportePromociones);
        connect(btnMat,   &QPushButton::clicked, this, &MainWindow::onGenerarReporteMatriz);
        connect(btnCli,   &QPushButton::clicked, this, &MainWindow::onGenerarReporteClientes);

        adminTabs->addTab(tab, "📊  Reportes");
    }
}

// ─────────────────────────────────────────────────────────────
// PANEL CLIENTE — CONSTRUCCIÓN
// ─────────────────────────────────────────────────────────────
void MainWindow::setupClientePanel() {
    clientePanel = new QWidget();
    QVBoxLayout* vMain = new QVBoxLayout(clientePanel);
    vMain->setContentsMargins(0, 0, 0, 0);

    // Banner
    QWidget* banner = new QWidget();
    banner->setFixedHeight(48);
    banner->setStyleSheet("background-color:#161B22; border-bottom:1px solid #30363D;");
    QHBoxLayout* bl = new QHBoxLayout(banner);
    bl->setContentsMargins(16, 0, 16, 0);
    QLabel* lbl = new QLabel("🎟  EDD Cinema USAC  —  Taquilla / Cliente");
    lbl->setStyleSheet("color:#3B82F6; font-size:13pt; font-weight:700;");
    bl->addWidget(lbl);
    bl->addStretch();

    QPushButton* btnLogoutCliente = makeBtn("🚪 Regresar");
    btnLogoutCliente->setProperty("role", "warning");
    btnLogoutCliente->style()->unpolish(btnLogoutCliente); btnLogoutCliente->style()->polish(btnLogoutCliente);
    connect(btnLogoutCliente, &QPushButton::clicked, this, &MainWindow::onLogout);
    bl->addWidget(btnLogoutCliente);

    vMain->addWidget(banner);

    clienteTabs = new QTabWidget();
    vMain->addWidget(clienteTabs);

    // ── TAB 0: CARTELERA CLIENTE
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12,12,12,12); v->setSpacing(10);

        QHBoxLayout* toolbar = new QHBoxLayout();
        inpBuscarCliente = new QLineEdit();
        inpBuscarCliente->setPlaceholderText("🔍  Buscar por código...");
        inpBuscarCliente->setMaximumWidth(260);
        auto* btnB = makeBtn("Buscar", "");
        btnB->setProperty("role","info"); btnB->style()->unpolish(btnB); btnB->style()->polish(btnB);
        auto* btnAll = makeBtn("Ver Todo");
        toolbar->addWidget(inpBuscarCliente);
        toolbar->addWidget(btnB);
        toolbar->addWidget(btnAll);
        toolbar->addStretch();
        QLabel* hint = new QLabel("Doble clic en una fila para ver detalles");
        hint->setObjectName("lblWarning");
        toolbar->addWidget(hint);
        v->addLayout(toolbar);

        tblCarteleraCliente = new QTableWidget(0, 9);
        tblCarteleraCliente->setHorizontalHeaderLabels(
            {"Código","Título","Género","Duración","Clasif.","Idioma","Estreno","Retiro","Estado"});
        tblCarteleraCliente->horizontalHeader()->setStretchLastSection(true);
        tblCarteleraCliente->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblCarteleraCliente->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblCarteleraCliente->setAlternatingRowColors(true);
        tblCarteleraCliente->setStyleSheet("QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblCarteleraCliente);

        connect(btnB,   &QPushButton::clicked, this, &MainWindow::onClienteBuscarPelicula);
        connect(btnAll, &QPushButton::clicked, this, &MainWindow::refreshCarteleraCliente);
        connect(inpBuscarCliente, &QLineEdit::returnPressed, this, &MainWindow::onClienteBuscarPelicula);
        connect(tblCarteleraCliente, &QTableWidget::cellDoubleClicked, this, &MainWindow::onClienteVerDetalle);

        clienteTabs->addTab(tab, "🎬  Cartelera");
    }

    // ── TAB 1: ASIENTOS
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12,12,12,12); v->setSpacing(8);

        QHBoxLayout* hTop = new QHBoxLayout();
        hTop->addWidget(new QLabel("Película:"));
        cmbClientePelicula = new QComboBox();
        hTop->addWidget(cmbClientePelicula);

        hTop->addWidget(new QLabel("Función AVL:"));
        cmbClienteFuncion = new QComboBox();
        hTop->addWidget(cmbClienteFuncion);

        hTop->addWidget(new QLabel("Horario:"));
        cmbClienteHorario = new QComboBox();
        hTop->addWidget(cmbClienteHorario);

        hTop->addWidget(new QLabel("Sala:"));
        cmbClienteSala = new QComboBox();
        hTop->addWidget(cmbClienteSala);

        auto* btnVerSala = makeBtn("Ver Asientos");
        btnVerSala->setProperty("role","info"); btnVerSala->style()->unpolish(btnVerSala); btnVerSala->style()->polish(btnVerSala);
        hTop->addWidget(btnVerSala);
        hTop->addStretch();
        v->addLayout(hTop);

        lblInfoMatrizCliente = new QLabel("Seleccione una película, horario y sala, luego presione 'Ver Asientos'");
        lblInfoMatrizCliente->setStyleSheet("color:#60A5FA; font-weight:bold; font-size:11pt;");
        v->addWidget(lblInfoMatrizCliente);

        connect(btnVerSala, &QPushButton::clicked, this, &MainWindow::onClienteMovieSelectionChanged);
        connect(cmbClienteFuncion, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int idx) {
                    if (idx < 0) return;
                    QString cod = cmbClienteFuncion->itemData(idx).toString();
                    NodoAVL* nodo = funciones.buscar(cod.toStdString());
                    if (nodo) {
                        cmbClienteHorario->clear();
                        cmbClienteHorario->addItem(QString::fromStdString(nodo->funcion.getHora()));
                        cmbClienteSala->clear();
                        cmbClienteSala->addItem(QString::fromStdString(nodo->funcion.getSala()));
                        // Seleccionar película automáticamente
                        int peliIdx = cmbClientePelicula->findData(QString::fromStdString(nodo->funcion.getCodigoPelicula()));
                        if (peliIdx >= 0) cmbClientePelicula->setCurrentIndex(peliIdx);
                    }
                });
        connect(cmbClienteHorario, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onClienteFuncionChanged);
        connect(cmbClienteSala, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onClienteFuncionChanged);

        QLabel* legend = new QLabel("🟢 Verde = Libre  |  🔴 Rojo = Ocupado  |  Clic para reservar");
        legend->setObjectName("lblWarning");
        v->addWidget(legend);

        widgetAsientos = new QWidget();
        gridAsientos   = new QGridLayout(widgetAsientos);
        gridAsientos->setSpacing(6);

        scrollAsientos = new QScrollArea();
        scrollAsientos->setWidget(widgetAsientos);
        scrollAsientos->setWidgetResizable(true);
        v->addWidget(scrollAsientos);

        clienteTabs->addTab(tab, "💺  Asientos");
    }

    // ── TAB 2: MIS RESERVAS (Fase 2 - Paso 3)
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->setContentsMargins(12, 12, 12, 12);
        v->setSpacing(10);

        lblClienteHeader = new QLabel("Mis Reservas");
        lblClienteHeader->setStyleSheet("color:#3B82F6; font-size:12pt; font-weight:bold;");
        v->addWidget(lblClienteHeader);

        QHBoxLayout* toolbar = new QHBoxLayout();
        auto* btnCancelHist = makeBtn("🗑  Cancelar Reserva Seleccionada");
        btnCancelHist->setProperty("role", "danger");
        btnCancelHist->style()->unpolish(btnCancelHist); btnCancelHist->style()->polish(btnCancelHist);
        auto* btnRefHist = makeBtn("🔄  Actualizar");
        btnRefHist->setProperty("role", "info");
        btnRefHist->style()->unpolish(btnRefHist); btnRefHist->style()->polish(btnRefHist);

        toolbar->addWidget(btnCancelHist);
        toolbar->addWidget(btnRefHist);
        toolbar->addStretch();
        v->addLayout(toolbar);

        tblMisReservasCliente = new QTableWidget(0, 6);
        tblMisReservasCliente->setHorizontalHeaderLabels({
            "Código Reserva", "Código Función", "Fila", "Columna", "Fecha Reserva", "Estado"
        });
        tblMisReservasCliente->horizontalHeader()->setStretchLastSection(true);
        tblMisReservasCliente->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblMisReservasCliente->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblMisReservasCliente->setAlternatingRowColors(true);
        tblMisReservasCliente->setStyleSheet("QTableWidget { alternate-background-color: #1C2128; }");
        v->addWidget(tblMisReservasCliente);

        connect(btnCancelHist, &QPushButton::clicked, this, &MainWindow::onClienteCancelarReservaHistorial);
        connect(btnRefHist, &QPushButton::clicked, this, &MainWindow::refreshMisReservasCliente);

        clienteTabs->addTab(tab, "🎟️  Mis Reservas");
    }

    // ── TAB 2: CANCELACIONES
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* hMain = new QHBoxLayout(tab);
        hMain->setContentsMargins(12,12,12,12);

        QGroupBox* grp = makeGroup("Cancelar Reserva");
        grp->setMaximumWidth(380);
        QFormLayout* form = new QFormLayout(grp);

        spnCancelFila = new QSpinBox(); spnCancelFila->setRange(1,20); spnCancelFila->setValue(1);
        spnCancelCol  = new QSpinBox(); spnCancelCol->setRange(1,20);  spnCancelCol->setValue(1);
        form->addRow("Fila:", spnCancelFila);
        form->addRow("Columna:", spnCancelCol);

        auto* btnConsultar = makeBtn("🔍  Consultar Asiento");
        btnConsultar->setProperty("role","info"); btnConsultar->style()->unpolish(btnConsultar); btnConsultar->style()->polish(btnConsultar);
        form->addRow(btnConsultar);

        lblInfoAsiento = new QLabel("—");
        lblInfoAsiento->setStyleSheet("color:#E2E8F0; font-size:11pt; padding:8px;");
        form->addRow("Estado:", lblInfoAsiento);

        auto* btnCancel = makeBtn("❌  Liberar Asiento");
        btnCancel->setProperty("role","danger"); btnCancel->style()->unpolish(btnCancel); btnCancel->style()->polish(btnCancel);
        form->addRow(btnCancel);

        connect(btnConsultar, &QPushButton::clicked, this, &MainWindow::onClienteConsultarAsiento);
        connect(btnCancel,    &QPushButton::clicked, this, &MainWindow::onClienteCancelarReserva);

        hMain->addWidget(grp);
        hMain->addStretch();
        clienteTabs->addTab(tab, "🔓  Cancelaciones");
    }

    // ── TAB 3: SOLICITUDES + PROMOCIONES
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* hMain = new QHBoxLayout(tab);
        hMain->setContentsMargins(12,12,12,12); hMain->setSpacing(12);

        // Panel izquierdo: enviar solicitud
        QGroupBox* grpSol = makeGroup("Registrar Solicitud Especial");
        QVBoxLayout* vSol = new QVBoxLayout(grpSol);

        auto addF = [&](QVBoxLayout* layout, const QString& label, QLineEdit*& field, const QString& ph) {
            layout->addWidget(new QLabel(label));
            field = new QLineEdit(); field->setPlaceholderText(ph);
            layout->addWidget(field);
        };
        addF(vSol, "Nombre:", inpCliNombre, "Tu nombre completo");
        addF(vSol, "Teléfono:", inpCliTelefono, "Ej. 5555-1234");
        addF(vSol, "Tipo:", inpCliTipo, "Cumpleaños / Accesibilidad / Queja");
        vSol->addWidget(new QLabel("Descripción:"));
        inpCliDescripcion = new QTextEdit();
        inpCliDescripcion->setPlaceholderText("Detalle de tu solicitud...");
        inpCliDescripcion->setMaximumHeight(80);
        vSol->addWidget(inpCliDescripcion);
        auto* btnEnviar = makeBtn("📤  Enviar Solicitud");
        btnEnviar->setProperty("role","success"); btnEnviar->style()->unpolish(btnEnviar); btnEnviar->style()->polish(btnEnviar);
        vSol->addWidget(btnEnviar);

        vSol->addSpacing(10);
        vSol->addWidget(new QLabel("Buscar mis solicitudes por teléfono:"));
        QHBoxLayout* hBusc = new QHBoxLayout();
        inpBuscarTelefono = new QLineEdit(); inpBuscarTelefono->setPlaceholderText("Teléfono...");
        auto* btnBuscTel = makeBtn("🔍 Buscar");
        btnBuscTel->setProperty("role","info"); btnBuscTel->style()->unpolish(btnBuscTel); btnBuscTel->style()->polish(btnBuscTel);
        hBusc->addWidget(inpBuscarTelefono); hBusc->addWidget(btnBuscTel);
        vSol->addLayout(hBusc);
        lstSolicitudesCliente = new QListWidget();
        lstSolicitudesCliente->setMaximumHeight(120);
        vSol->addWidget(lstSolicitudesCliente);
        vSol->addStretch();

        // Panel derecho: Promociones disponibles
        QGroupBox* grpPro = makeGroup("Promociones Disponibles");
        QVBoxLayout* vPro = new QVBoxLayout(grpPro);
        lstPromocionesCliente = new QListWidget();
        vPro->addWidget(lstPromocionesCliente);

        connect(btnEnviar,  &QPushButton::clicked, this, &MainWindow::onClienteEnviarSolicitud);
        connect(btnBuscTel, &QPushButton::clicked, this, &MainWindow::onClienteBuscarPorTelefono);

        hMain->addWidget(grpSol, 3);
        hMain->addWidget(grpPro, 2);

        clienteTabs->addTab(tab, "📝  Solicitudes & Promos");
    }
}

// ─────────────────────────────────────────────────────────────
// MÉTODOS REFRESH Y UTILIDADES
// ─────────────────────────────────────────────────────────────
void MainWindow::refreshCarteleraCliente() {
    tblCarteleraCliente->setRowCount(0);
    int f = 0;
    llenarTablaInorden(cartelera.getRaiz(), tblCarteleraCliente, f);
    tblCarteleraCliente->resizeColumnsToContents();
}

void MainWindow::refreshSolicitudesAdmin() {
    tblSolicitudes->setRowCount(0);
    if (solicitudes.estaVacia()) return;

    NodoSolicitud* actual = solicitudes.getCabeza();
    int row = 0;
    do {
        tblSolicitudes->insertRow(row);
        tblSolicitudes->setItem(row, 0, new QTableWidgetItem(QString::number(actual->solicitud.getId())));
        tblSolicitudes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(actual->solicitud.getCliente())));
        tblSolicitudes->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(actual->solicitud.getTelefono())));
        tblSolicitudes->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(actual->solicitud.getTipo())));
        tblSolicitudes->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(actual->solicitud.getDescripcion())));
        
        QTableWidgetItem* stItem = new QTableWidgetItem(QString::fromStdString(actual->solicitud.getEstado()));
        if (actual->solicitud.getEstado() == "Pendiente") {
            stItem->setForeground(QColor("#F59E0B"));
        } else if (actual->solicitud.getEstado() == "Aprobada") {
            stItem->setForeground(QColor("#10B981"));
        } else if (actual->solicitud.getEstado() == "Atendida") {
            stItem->setForeground(QColor("#3B82F6"));
        } else {
            stItem->setForeground(QColor("#EF4444"));
        }
        tblSolicitudes->setItem(row, 5, stItem);
        
        row++;
        actual = actual->siguiente;
    } while (actual != solicitudes.getCabeza());
}

void MainWindow::refreshPromocionesCombo() {
    cmbPromoSelect->clear();
    if (promociones.estaVacia()) return;
    
    NodoPromocion* actual = promociones.getCabeza();
    do {
        cmbPromoSelect->addItem(QString::fromStdString(actual->promocion.getCodigo() + " - " + actual->promocion.getNombre()), 
                                QString::fromStdString(actual->promocion.getCodigo()));
        actual = actual->siguiente;
    } while (actual != promociones.getCabeza());
}

void MainWindow::refreshPromocionesAdmin() {
    lstPromociones->clear();
    if (promociones.estaVacia()) return;

    NodoPromocion* actual = promociones.getCabeza();
    do {
        QString text = QString::fromStdString(
            actual->promocion.getCodigo() + " | " + actual->promocion.getNombre() + 
            " (Valido: " + actual->promocion.getVigencia() + ")"
        );
        lstPromociones->addItem(text);
        actual = actual->siguiente;
    } while (actual != promociones.getCabeza());
}

void MainWindow::refreshPromocionesCliente() {
    lstPromocionesCliente->clear();
    if (promociones.estaVacia()) {
        lstPromocionesCliente->addItem("No hay promociones activas en este momento.");
        return;
    }

    NodoPromocion* actual = promociones.getCabeza();
    do {
        QString text = QString::fromStdString(
            "🎁 " + actual->promocion.getNombre() + "\n" +
            "   Aplica: " + actual->promocion.getDiasAplicables() + "\n" +
            "   Vigencia: " + actual->promocion.getVigencia() + "\n"
        );
        
        NodoBeneficio* ben = actual->promocion.getBeneficios().getCabeza();
        while (ben != nullptr) {
            text += QString::fromStdString("   - " + ben->beneficio.getTipo() + ": " + ben->beneficio.getDescripcion() + "\n");
            ben = ben->siguiente;
        }
        
        QListWidgetItem* item = new QListWidgetItem(text);
        lstPromocionesCliente->addItem(item);
        actual = actual->siguiente;
    } while (actual != promociones.getCabeza());
}

void MainWindow::onAdminMovieSelectionChanged() {
    if (!cmbPeliculaMatriz) return;
    // Persistir matriz anterior antes de cambiar
    if (currentAdminMovieNode) {
        persistirMatrizActual(currentAdminMovieNode, currentAdminHorario, currentAdminSala);
    }
    QString codigo = cmbPeliculaMatriz->currentData().toString();
    currentAdminMovieNode = cartelera.buscar(codigo.toStdString());
    currentAdminHorario = cmbHorarioMatriz ? cmbHorarioMatriz->currentText() : "14:00";
    currentAdminSala    = cmbSalaMatriz ? cmbSalaMatriz->currentText() : "Sala 1";
    if (currentAdminMovieNode) {
        cargarMatrizFuncion(currentAdminMovieNode, currentAdminHorario, currentAdminSala);
    }
    refreshAdminSeatMap();
}

void MainWindow::onAdminFuncionChanged() {
    if (!cmbHorarioMatriz || !cmbSalaMatriz) return;
    // Persistir antes de cambiar horario/sala
    if (currentAdminMovieNode) {
        persistirMatrizActual(currentAdminMovieNode, currentAdminHorario, currentAdminSala);
    }
    currentAdminHorario = cmbHorarioMatriz->currentText();
    currentAdminSala    = cmbSalaMatriz->currentText();
    if (currentAdminMovieNode) {
        cargarMatrizFuncion(currentAdminMovieNode, currentAdminHorario, currentAdminSala);
    }
    refreshAdminSeatMap();
}

void MainWindow::onClienteMovieSelectionChanged() {
    if (!cmbClientePelicula) return;
    // Persistir antes de cambiar
    if (currentClientMovieNode) {
        persistirMatrizActual(currentClientMovieNode, currentClientHorario, currentClientSala);
    }
    QString codigo = cmbClientePelicula->currentData().toString();
    currentClientMovieNode = cartelera.buscar(codigo.toStdString());
    currentClientHorario = cmbClienteHorario ? cmbClienteHorario->currentText() : "14:00";
    currentClientSala    = cmbClienteSala ? cmbClienteSala->currentText() : "Sala 1";
    if (currentClientMovieNode) {
        cargarMatrizFuncion(currentClientMovieNode, currentClientHorario, currentClientSala);
        if (lblInfoMatrizCliente) {
            lblInfoMatrizCliente->setText(
                QString("Asientos para: %1 | %2 | %3")
                .arg(QString::fromStdString(currentClientMovieNode->pelicula.getTitulo()))
                .arg(currentClientHorario)
                .arg(currentClientSala));
        }
    } else {
        if (lblInfoMatrizCliente) {
            lblInfoMatrizCliente->setText("Película no encontrada.");
        }
    }
    refreshClienteSeatMap();
}

void MainWindow::onClienteFuncionChanged() {
    if (!cmbClienteHorario || !cmbClienteSala) return;
    if (currentClientMovieNode) {
        persistirMatrizActual(currentClientMovieNode, currentClientHorario, currentClientSala);
    }
    currentClientHorario = cmbClienteHorario->currentText();
    currentClientSala    = cmbClienteSala->currentText();
    if (currentClientMovieNode) {
        cargarMatrizFuncion(currentClientMovieNode, currentClientHorario, currentClientSala);
        if (lblInfoMatrizCliente) {
            lblInfoMatrizCliente->setText(
                QString("Asientos para: %1 | %2 | %3")
                .arg(QString::fromStdString(currentClientMovieNode->pelicula.getTitulo()))
                .arg(currentClientHorario)
                .arg(currentClientSala));
        }
        refreshClienteSeatMap();
    }
}

void MainWindow::refreshClienteSeatMap() {
    if (!gridAsientos) return;
    // Limpiar grid anterior
    QLayoutItem* item;
    while ((item = gridAsientos->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    if (!currentClientMovieNode) return;

    int fMax = currentClientMovieNode->salaCine.getMaxFilas();
    int cMax = currentClientMovieNode->salaCine.getMaxColumnas();

    for (int f = 1; f <= fMax; ++f) {
        for (int c = 1; c <= cMax; ++c) {
            QPushButton* btn = new QPushButton(QString("%1,%2").arg(f).arg(c));
            btn->setFixedSize(50, 50);
            
            bool ocupado = currentClientMovieNode->salaCine.estaOcupado(f, c);
            
            if (ocupado) {
                btn->setStyleSheet("background-color: #E74C3C; color: white; border-radius: 5px; font-weight: bold;"); // Rojo
                btn->setToolTip("Ocupado por: " + QString::fromStdString(currentClientMovieNode->salaCine.getClienteAsiento(f, c)));
            } else {
                btn->setStyleSheet("background-color: #2ECC71; color: white; border-radius: 5px; font-weight: bold;"); // Verde
                btn->setToolTip("Libre");
            }
            
            // Pasar la fila y columna como propiedades del botón
            btn->setProperty("fila", f);
            btn->setProperty("columna", c);
            btn->setProperty("ocupado", ocupado);
            
            connect(btn, &QPushButton::clicked, this, &MainWindow::onAsientoClicked);
            
            gridAsientos->addWidget(btn, f - 1, c - 1);
        }
    }
}

void MainWindow::refreshAdminSeatMap() {
    if (!gridAdminSeatMap) return;
    // Limpiar grid anterior
    QLayoutItem* item;
    while ((item = gridAdminSeatMap->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    if (!currentAdminMovieNode) return;

    int fMax = currentAdminMovieNode->salaCine.getMaxFilas();
    int cMax = currentAdminMovieNode->salaCine.getMaxColumnas();

    if (fMax == 0 || cMax == 0) return;

    for (int f = 1; f <= fMax; ++f) {
        for (int c = 1; c <= cMax; ++c) {
            bool ocupado = currentAdminMovieNode->salaCine.estaOcupado(f, c);
            QString texto = ocupado ? QString("F%1,C%2\n%3").arg(f).arg(c).arg(QString::fromStdString(currentAdminMovieNode->salaCine.getClienteAsiento(f, c)))
                                    : QString("F%1,C%2").arg(f).arg(c);

            QPushButton* btn = new QPushButton(texto);
            btn->setFixedSize(70, 70); // Un poco más grande para que quepa el nombre
            
            if (ocupado) {
                btn->setStyleSheet("background-color: #E74C3C; color: white; border-radius: 5px; font-weight: bold; font-size: 8pt;"); // Rojo
            } else {
                btn->setStyleSheet("background-color: #2ECC71; color: white; border-radius: 5px; font-weight: bold; font-size: 8pt;"); // Verde
            }
            
            // Botón de solo lectura
            btn->setCursor(Qt::ArrowCursor);
            btn->setFocusPolicy(Qt::NoFocus);
            
            gridAdminSeatMap->addWidget(btn, f - 1, c - 1);
        }
    }
}


// ─────────────────────────────────────────────────────────────
// SLOTS - PANEL ADMIN
// ─────────────────────────────────────────────────────────────
void MainWindow::onAdminCargarPeliculasFunciones() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo JSON de Películas y Funciones",
        "",
        "JSON Files (*.json);;All Files (*)");
    if (!fileName.isEmpty()) {
        if (JSONReader::cargarPeliculasDesdeJSON(fileName.toStdString(), cartelera, funciones)) {
            QMessageBox::information(this, "Éxito",
                QString("Películas y Funciones cargadas exitosamente.\n"
                        "Películas en BST: %1\n"
                        "Funciones en AVL: %2")
                .arg(cartelera.getTamanio())
                .arg(funciones.getTamanio()));
            refreshCarteleraAdmin();
            refreshFuncionesAdmin();
            refreshPeliculasCombo();
            refreshFuncionesCombo();
        } else {
            QMessageBox::critical(this, "Error",
                "No se pudo cargar el archivo JSON.\n"
                "Verifique que el archivo tenga la estructura correcta para Películas y Funciones.");
        }
    }
}

void MainWindow::onAdminCargarClientesReservas() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo JSON de Clientes y Reservas",
        "",
        "JSON Files (*.json);;All Files (*)");
    if (!fileName.isEmpty()) {
        if (JSONReader::cargarClientesDesdeJSON(fileName.toStdString(), clientes, funciones, cartelera)) {
            QMessageBox::information(this, "Éxito",
                QString("Clientes y Reservas cargadas exitosamente.\n"
                        "Clientes cargados: %1")
                .arg(clientes.getTamanio()));
            refreshClientesAdmin();
        } else {
            QMessageBox::critical(this, "Error",
                "No se pudo cargar el archivo JSON.\n"
                "Verifique que el archivo tenga la estructura correcta para Clientes y Reservas.");
        }
    }
}

void MainWindow::onAdminAgregarPelicula() {
    QDialog dialog(this);
    dialog.setWindowTitle("Agregar Película");
    QFormLayout form(&dialog);

    QLineEdit cod, tit, gen, dur, cla, idi, fecE, fecF;
    form.addRow("Código:", &cod);
    form.addRow("Título:", &tit);
    form.addRow("Género:", &gen);
    form.addRow("Duración (min):", &dur);
    form.addRow("Clasificación:", &cla);
    form.addRow("Idioma:", &idi);
    form.addRow("Fecha Estreno (AAAA-MM-DD):", &fecE);
    form.addRow("Fecha Fin (AAAA-MM-DD):", &fecF);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (cod.text().isEmpty() || tit.text().isEmpty()) {
            QMessageBox::warning(this, "Error", "El código y título son obligatorios.");
            return;
        }
        Pelicula p(cod.text().toStdString(), tit.text().toStdString(),
                   gen.text().toStdString(), dur.text().toInt(),
                   cla.text().toStdString(), idi.text().toStdString(),
                   fecE.text().toStdString(), fecF.text().toStdString());
        cartelera.insertar(p);
        refreshCarteleraAdmin();
        QMessageBox::information(this, "Éxito", "Película agregada con éxito.");
    }
}

void MainWindow::onAdminEliminarPelicula() {
    auto items = tblCarteleraAdmin->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "Atención", "Seleccione una película para eliminar.");
        return;
    }
    
    QString codigo = items.first()->text();
    auto reply = QMessageBox::question(this, "Confirmar", "¿Eliminar la película " + codigo + "?");
    
    if (reply == QMessageBox::Yes) {
        if (cartelera.eliminar(codigo.toStdString())) {
            refreshCarteleraAdmin();
            QMessageBox::information(this, "Éxito", "Película eliminada.");
        }
    }
}

void MainWindow::onAdminBuscarPelicula() {
    QString codigo = inpBuscarAdmin->text().trimmed();
    if (codigo.isEmpty()) {
        refreshCarteleraAdmin();
        return;
    }
    
    NodoBST* nodo = cartelera.buscar(codigo.toStdString());
    tblCarteleraAdmin->setRowCount(0);
    
    if (nodo != nullptr) {
        int f = 0;
        // Solo inserto este
        QDate hoy = QDate::currentDate();
        QDate fechaFin = QDate::fromString(QString::fromStdString(nodo->pelicula.getFechaFin()), "yyyy-MM-dd");
        bool pronto = fechaFin.isValid() && fechaFin <= hoy.addDays(7);

        tblCarteleraAdmin->insertRow(f);

        auto setItem = [&](int col, const QString& text) {
            QTableWidgetItem* item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            if (pronto) {
                item->setBackground(QColor(120, 30, 0));
                item->setForeground(QColor("#FBBF24"));
            }
            tblCarteleraAdmin->setItem(f, col, item);
        };

        setItem(0, QString::fromStdString(nodo->pelicula.getCodigo()));
        setItem(1, QString::fromStdString(nodo->pelicula.getTitulo()));
        setItem(2, QString::fromStdString(nodo->pelicula.getGenero()));
        setItem(3, QString::number(nodo->pelicula.getDuracionMinutos()) + " min");
        setItem(4, QString::fromStdString(nodo->pelicula.getClasificacion()));
        setItem(5, QString::fromStdString(nodo->pelicula.getIdioma()));
        setItem(6, QString::fromStdString(nodo->pelicula.getFechaEstreno()));
        setItem(7, QString::fromStdString(nodo->pelicula.getFechaFin()) + (pronto ? " ⚠" : ""));
    } else {
        QMessageBox::information(this, "Búsqueda", "No se encontró película con ese código.");
    }
}

// ─────────────────────────────────────────────────────────────
// SLOTS & REFRESH - FUNCIONES AVL (Fase 2 - Paso 2)
// ─────────────────────────────────────────────────────────────
void MainWindow::refreshFuncionesAdmin() {
    if (!tblFuncionesAdmin) return;
    tblFuncionesAdmin->setRowCount(0);
    int fila = 0;
    
    std::vector<NodoAVL*> nodosOrdenados;
    if (cmbOrdenFunciones) {
        int idx = cmbOrdenFunciones->currentIndex();
        if (idx == 1) funciones.obtenerPreorden(nodosOrdenados);
        else if (idx == 2) funciones.obtenerPostorden(nodosOrdenados);
        else funciones.obtenerInorden(nodosOrdenados);
    } else {
        funciones.obtenerInorden(nodosOrdenados);
    }
    
    for (NodoAVL* nodo : nodosOrdenados) {
        if (!nodo) continue;
        tblFuncionesAdmin->insertRow(fila);

        int altIzq = nodo->izquierdo ? nodo->izquierdo->altura : 0;
        int altDer = nodo->derecho ? nodo->derecho->altura : 0;
        int fe = altIzq - altDer;

        tblFuncionesAdmin->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(nodo->funcion.getCodigo())));
        tblFuncionesAdmin->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(nodo->funcion.getCodigoPelicula())));
        tblFuncionesAdmin->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(nodo->funcion.getFecha())));
        tblFuncionesAdmin->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(nodo->funcion.getHora())));
        tblFuncionesAdmin->setItem(fila, 4, new QTableWidgetItem(QString::fromStdString(nodo->funcion.getSala())));
        tblFuncionesAdmin->setItem(fila, 5, new QTableWidgetItem(QString::number(nodo->funcion.getPrecio(), 'f', 2)));
        tblFuncionesAdmin->setItem(fila, 6, new QTableWidgetItem(QString::number(nodo->altura)));
        tblFuncionesAdmin->setItem(fila, 7, new QTableWidgetItem(QString::number(fe)));
        fila++;
    }
}

void MainWindow::refreshFuncionesCombo() {
    if (cmbFuncionMatriz) cmbFuncionMatriz->clear();
    if (cmbClienteFuncion) cmbClienteFuncion->clear();

    funciones.recorridoInorden([&](NodoAVL* nodo) {
        if (!nodo) return;
        QString text = QString("[%1] Peli:%2 - %3 %4 - %5 ($%6)")
            .arg(QString::fromStdString(nodo->funcion.getCodigo()))
            .arg(QString::fromStdString(nodo->funcion.getCodigoPelicula()))
            .arg(QString::fromStdString(nodo->funcion.getFecha()))
            .arg(QString::fromStdString(nodo->funcion.getHora()))
            .arg(QString::fromStdString(nodo->funcion.getSala()))
            .arg(nodo->funcion.getPrecio(), 0, 'f', 2);
        QString data = QString::fromStdString(nodo->funcion.getCodigo());

        if (cmbFuncionMatriz) cmbFuncionMatriz->addItem(text, data);
        if (cmbClienteFuncion) cmbClienteFuncion->addItem(text, data);
    });
    
    // Forzar actualización inicial si hay datos
    if (cmbFuncionMatriz && cmbFuncionMatriz->count() > 0) {
        cmbFuncionMatriz->setCurrentIndex(0);
        emit cmbFuncionMatriz->currentIndexChanged(0);
    }
    if (cmbClienteFuncion && cmbClienteFuncion->count() > 0) {
        cmbClienteFuncion->setCurrentIndex(0);
        emit cmbClienteFuncion->currentIndexChanged(0);
    }
}

void MainWindow::onAdminAgregarFuncion() {
    QDialog dlg(this);
    dlg.setWindowTitle("Agregar Nueva Función (AVL)");
    QFormLayout form(&dlg);

    QLineEdit txtCodigo, txtCodPeli, txtFecha, txtHora, txtSala, txtPrecio;
    txtCodigo.setPlaceholderText("Ej. F001");
    txtCodPeli.setPlaceholderText("Ej. P001");
    txtFecha.setText(QDate::currentDate().toString("yyyy-MM-dd"));
    txtHora.setText("18:00");
    txtSala.setText("Sala 1");
    txtPrecio.setText("45.00");

    form.addRow("Código de Función:", &txtCodigo);
    form.addRow("Código Película:", &txtCodPeli);
    form.addRow("Fecha (YYYY-MM-DD):", &txtFecha);
    form.addRow("Hora (HH:MM):", &txtHora);
    form.addRow("Sala:", &txtSala);
    form.addRow("Precio ($):", &txtPrecio);

    QDialogButtonBox bbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&bbox);
    connect(&bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        std::string cod = txtCodigo.text().trimmed().toStdString();
        std::string codPeli = txtCodPeli.text().trimmed().toStdString();
        std::string fec = txtFecha.text().trimmed().toStdString();
        std::string hor = txtHora.text().trimmed().toStdString();
        std::string sal = txtSala.text().trimmed().toStdString();
        double prec = txtPrecio.text().toDouble();

        if (cod.empty() || codPeli.empty()) {
            QMessageBox::warning(this, "Atención", "Código de función y película son requeridos.");
            return;
        }

        Funcion f(cod, codPeli, fec, hor, sal, prec);
        if (funciones.insertar(f)) {
            QMessageBox::information(this, "Éxito", "Función agregada e insertada en el Árbol AVL.");
            refreshFuncionesAdmin();
            refreshFuncionesCombo();
        } else {
            QMessageBox::warning(this, "Error", "No se pudo insertar la función (código ya existe o inválido).");
        }
    }
}

void MainWindow::onAdminEditarFuncion() {
    int row = tblFuncionesAdmin->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Atención", "Seleccione una función de la tabla para editar.");
        return;
    }
    QString cod = tblFuncionesAdmin->item(row, 0)->text();
    NodoAVL* nodo = funciones.buscar(cod.toStdString());
    if (!nodo) return;

    QDialog dlg(this);
    dlg.setWindowTitle("Editar Función " + cod);
    QFormLayout form(&dlg);

    QLineEdit txtCodPeli, txtFecha, txtHora, txtSala, txtPrecio;
    txtCodPeli.setText(QString::fromStdString(nodo->funcion.getCodigoPelicula()));
    txtFecha.setText(QString::fromStdString(nodo->funcion.getFecha()));
    txtHora.setText(QString::fromStdString(nodo->funcion.getHora()));
    txtSala.setText(QString::fromStdString(nodo->funcion.getSala()));
    txtPrecio.setText(QString::number(nodo->funcion.getPrecio(), 'f', 2));

    form.addRow("Código Película:", &txtCodPeli);
    form.addRow("Fecha:", &txtFecha);
    form.addRow("Hora:", &txtHora);
    form.addRow("Sala:", &txtSala);
    form.addRow("Precio ($):", &txtPrecio);

    QDialogButtonBox bbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&bbox);
    connect(&bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        Funcion f(cod.toStdString(),
                  txtCodPeli.text().trimmed().toStdString(),
                  txtFecha.text().trimmed().toStdString(),
                  txtHora.text().trimmed().toStdString(),
                  txtSala.text().trimmed().toStdString(),
                  txtPrecio.text().toDouble());
        if (funciones.editar(f)) {
            QMessageBox::information(this, "Éxito", "Función actualizada en el Árbol AVL.");
            refreshFuncionesAdmin();
            refreshFuncionesCombo();
        }
    }
}

void MainWindow::onAdminEliminarFuncion() {
    int row = tblFuncionesAdmin->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Atención", "Seleccione una función de la tabla para eliminar.");
        return;
    }
    QString cod = tblFuncionesAdmin->item(row, 0)->text();
    NodoAVL* nodo = funciones.buscar(cod.toStdString());
    if (!nodo) return;

    QString peli = QString::fromStdString(nodo->funcion.getCodigoPelicula());
    QString hor  = QString::fromStdString(nodo->funcion.getHora());
    QString sal  = QString::fromStdString(nodo->funcion.getSala());

    auto reply = QMessageBox::question(this, "Confirmar eliminación",
        QString("¿Desea eliminar la función %1?\nTambién se eliminará su archivo de asientos persistido.").arg(cod),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (funciones.eliminar(cod.toStdString())) {
            // Eliminar archivo de asientos asociado
            QString jsonPath = rutaJsonFuncion(peli, hor, sal);
            if (QFile::exists(jsonPath)) {
                QFile::remove(jsonPath);
            }
            QString altPath = QString("Asientos/%1_funcion.json").arg(cod);
            if (QFile::exists(altPath)) {
                QFile::remove(altPath);
            }

            QMessageBox::information(this, "Éxito", "Función eliminada del Árbol AVL y archivo de asientos removido.");
            refreshFuncionesAdmin();
            refreshFuncionesCombo();
        }
    }
}

void MainWindow::onAdminBuscarFuncion() {
    QString qcod = inpBuscarFuncion->text().trimmed();
    if (qcod.isEmpty()) {
        QMessageBox::information(this, "Buscar", "Ingrese un código de función a buscar.");
        return;
    }
    NodoAVL* nodo = funciones.buscar(qcod.toStdString());
    if (nodo) {
        int altIzq = nodo->izquierdo ? nodo->izquierdo->altura : 0;
        int altDer = nodo->derecho ? nodo->derecho->altura : 0;
        int fe = altIzq - altDer;

        QMessageBox::information(this, "Función Encontrada",
            QString("Código: %1\nPelícula: %2\nFecha: %3\nHora: %4\nSala: %5\nPrecio: $%6\nAltura: %7 | FE: %8")
            .arg(QString::fromStdString(nodo->funcion.getCodigo()))
            .arg(QString::fromStdString(nodo->funcion.getCodigoPelicula()))
            .arg(QString::fromStdString(nodo->funcion.getFecha()))
            .arg(QString::fromStdString(nodo->funcion.getHora()))
            .arg(QString::fromStdString(nodo->funcion.getSala()))
            .arg(nodo->funcion.getPrecio(), 0, 'f', 2)
            .arg(nodo->altura)
            .arg(fe));
    } else {
        QMessageBox::warning(this, "No encontrada", "La función con código '" + qcod + "' no existe en el Árbol AVL.");
    }
}

void MainWindow::onAdminCrearPromocion() {
    QString cod = inpPromoCodigo->text().trimmed();
    QString nom = inpPromoNombre->text().trimmed();
    QString vig = inpPromoVigencia->text().trimmed();
    QString dia = inpPromoDias->text().trimmed();
    
    if (cod.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Error", "El código y nombre son obligatorios.");
        return;
    }
    
    promociones.agregarPromocion(Promocion(cod.toStdString(), nom.toStdString(), vig.toStdString(), dia.toStdString()));
    refreshPromocionesAdmin();
    refreshPromocionesCombo();
    
    inpPromoCodigo->clear();
    inpPromoNombre->clear();
    inpPromoVigencia->clear();
    inpPromoDias->clear();
}

void MainWindow::onAdminAgregarBeneficio() {
    if (cmbPromoSelect->currentIndex() < 0) {
        QMessageBox::warning(this, "Atención", "Debe seleccionar una promoción.");
        return;
    }
    
    QString cod = cmbPromoSelect->currentData().toString();
    QString tipo = inpBenTipo->text().trimmed();
    QString desc = inpBenDesc->text().trimmed();
    QString valor = inpBenValor->text().trimmed();
    
    if (tipo.isEmpty()) {
        QMessageBox::warning(this, "Error", "El tipo de beneficio es obligatorio.");
        return;
    }
    
    NodoPromocion* p = promociones.buscarPromocion(cod.toStdString());
    if (p) {
        p->promocion.agregarBeneficio(Beneficio(tipo.toStdString(), desc.toStdString(), valor.toStdString()));
        onPromoSeleccionada(cmbPromoSelect->currentIndex()); // Refrescar lista de beneficios
        
        inpBenTipo->clear();
        inpBenDesc->clear();
        inpBenValor->clear();
    }
}

void MainWindow::onPromoSeleccionada(int /*idx*/) {
    lstBeneficios->clear();
    if (cmbPromoSelect->currentIndex() < 0) return;
    
    QString cod = cmbPromoSelect->currentData().toString();
    NodoPromocion* p = promociones.buscarPromocion(cod.toStdString());
    
    if (p) {
        NodoBeneficio* actual = p->promocion.getBeneficios().getCabeza();
        while (actual != nullptr) {
            QString text = QString::fromStdString(
                actual->beneficio.getTipo() + " - " + actual->beneficio.getDescripcion() + " (" + actual->beneficio.getValor() + ")"
            );
            lstBeneficios->addItem(text);
            actual = actual->siguiente;
        }
    }
}

void MainWindow::onAdminAprobarSolicitud() {
    auto items = tblSolicitudes->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "Atención", "Seleccione una solicitud de la lista.");
        return;
    }
    int id = tblSolicitudes->item(items.first()->row(), 0)->text().toInt();
    // Permitir elegir entre Aprobada o Atendida
    QStringList opciones = {"Aprobada", "Atendida"};
    bool ok;
    QString seleccion = QInputDialog::getItem(this, "Aprobar Solicitud",
        "Nuevo estado:", opciones, 0, false, &ok);
    if (!ok) return;
    solicitudes.cambiarEstado(id, seleccion.toStdString());
    refreshSolicitudesAdmin();
}

void MainWindow::onAdminRechazarSolicitud() {
    auto items = tblSolicitudes->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "Atención", "Seleccione una solicitud de la lista.");
        return;
    }
    int id = tblSolicitudes->item(items.first()->row(), 0)->text().toInt();
    solicitudes.cambiarEstado(id, "Rechazada");
    refreshSolicitudesAdmin();
}

void MainWindow::onAdminEliminarSolicitud() {
    auto items = tblSolicitudes->selectedItems();
    if (items.isEmpty()) return;
    
    int id = tblSolicitudes->item(items.first()->row(), 0)->text().toInt();
    solicitudes.eliminarSolicitud(id);
    refreshSolicitudesAdmin();
}

void MainWindow::onAdminConfigurarSala() {
    int f = spnFilas->value();
    int c = spnColumnas->value();
    QString p = cmbPeliculaMatriz->currentText();
    QString h = cmbHorarioMatriz->currentText();
    QString s = cmbSalaMatriz->currentText();
    
    if (!currentAdminMovieNode) {
        QMessageBox::warning(this, "Atención", "Debe seleccionar una película primero.");
        return;
    }

    auto reply = QMessageBox::question(this, "Confirmar", 
        "¿Reconfigurar sala a " + QString::number(f) + "x" + QString::number(c) + " para " + p + "?\nSe perderán todas las reservas actuales.");
        
    if (reply == QMessageBox::Yes) {
        currentAdminMovieNode->salaCine.inicializarDimensiones(f, c);
        lblSalaInfo->setText(QString("Sala de %1: %2 x %3 = %4 asientos").arg(p).arg(f).arg(c).arg(f*c));
        refreshAdminSeatMap();
        QMessageBox::information(this, "Éxito", "Sala reconfigurada.");
    }
}

// ─────────────────────────────────────────────────────────────
// REPORTES GRAPHVIZ
// ─────────────────────────────────────────────────────────────
void MainWindow::cargarImagenReporte(QLabel* lbl, const QString& rutaPng) {
    QString absolutePath = rutaPng;
    if (!QFile::exists(absolutePath)) {
        absolutePath = "../" + rutaPng; // Fallback si se ejecuta desde bin/
    }
    QPixmap pixmap(absolutePath);
    if (!pixmap.isNull()) {
        lbl->setPixmap(pixmap);
        lbl->setMinimumSize(pixmap.size()); // Permitir scroll
    } else {
        lbl->setText("❌ Error al cargar la imagen generada. (" + absolutePath + ")");
    }
}

void MainWindow::onGenerarTodosReportes() {
    onGenerarReporteBST();
    onGenerarReporteAVL();
    onGenerarReporteSolicitudes();
    onGenerarReportePromociones();
    onGenerarReporteMatriz();
    onGenerarReporteClientes();
    QMessageBox::information(this, "Reportes", "Reportes generados en la carpeta reports_output.");
}

void MainWindow::onGenerarReporteBST() {
    GraphvizReporter::generarReporteBST(cartelera, "reporte_bst");
    cargarImagenReporte(imgBST, "reports_output/reporte_bst.png");
}

void MainWindow::onGenerarReporteAVL() {
    GraphvizReporter::generarReporteAVL(funciones, "reporte_avl");
    cargarImagenReporte(imgAVL, "reports_output/reporte_avl.png");
}

void MainWindow::refreshReporteAVL() {
    onGenerarReporteAVL();
}

void MainWindow::onGenerarReporteSolicitudes() {
    GraphvizReporter::generarReporteSolicitudes(solicitudes, "reporte_solicitudes");
    cargarImagenReporte(imgSolicitudes, "reports_output/reporte_solicitudes.png");
}

void MainWindow::onGenerarReportePromociones() {
    GraphvizReporter::generarReportePromociones(promociones, "reporte_promociones");
    cargarImagenReporte(imgPromociones, "reports_output/reporte_promociones.png");
}

void MainWindow::onGenerarReporteMatriz() {
    if (!currentAdminMovieNode) {
        QMessageBox::warning(this, "Atención", "Debe seleccionar una película en la pestaña de Sala primero.");
        return;
    }
    GraphvizReporter::generarReporteMatriz(currentAdminMovieNode->salaCine, "reporte_matriz");
    cargarImagenReporte(imgMatriz, "reports_output/reporte_matriz.png");
}

void MainWindow::onGenerarReporteClientes() {
    refreshReporteClientes();
}

void MainWindow::refreshReporteClientes() {
    if (!imgClientes) return;
    if (GraphvizReporter::generarReporteArbolB(clientes, "reporte_clientes")) {
        cargarImagenReporte(imgClientes, "reports_output/reporte_clientes.png");
    }
}

// ─────────────────────────────────────────────────────────────
// SLOTS - PANEL CLIENTE
// ─────────────────────────────────────────────────────────────
void MainWindow::onClienteBuscarPelicula() {
    QString codigo = inpBuscarCliente->text().trimmed();
    if (codigo.isEmpty()) {
        refreshCarteleraCliente();
        return;
    }
    
    NodoBST* nodo = cartelera.buscar(codigo.toStdString());
    tblCarteleraCliente->setRowCount(0);
    
    if (nodo != nullptr) {
        int f = 0;
        tblCarteleraCliente->insertRow(f);
        tblCarteleraCliente->setItem(f, 0, new QTableWidgetItem(QString::fromStdString(nodo->pelicula.getCodigo())));
        tblCarteleraCliente->setItem(f, 1, new QTableWidgetItem(QString::fromStdString(nodo->pelicula.getTitulo())));
        tblCarteleraCliente->setItem(f, 2, new QTableWidgetItem(QString::fromStdString(nodo->pelicula.getGenero())));
        tblCarteleraCliente->setItem(f, 3, new QTableWidgetItem(QString::number(nodo->pelicula.getDuracionMinutos()) + " min"));
        tblCarteleraCliente->setItem(f, 4, new QTableWidgetItem(QString::fromStdString(nodo->pelicula.getClasificacion())));
        tblCarteleraCliente->setItem(f, 5, new QTableWidgetItem(QString::fromStdString(nodo->pelicula.getIdioma())));
    } else {
        QMessageBox::information(this, "Búsqueda", "No se encontró la película.");
    }
}

void MainWindow::onClienteVerDetalle(int row, int /*col*/) {
    QString codigo = tblCarteleraCliente->item(row, 0)->text();
    NodoBST* nodo = cartelera.buscar(codigo.toStdString());
    if (nodo) {
        QString info = QString("Título: %1\nGénero: %2\nClasificación: %3\nDuración: %4 min\n\nEstreno: %5\nRetiro: %6")
            .arg(QString::fromStdString(nodo->pelicula.getTitulo()))
            .arg(QString::fromStdString(nodo->pelicula.getGenero()))
            .arg(QString::fromStdString(nodo->pelicula.getClasificacion()))
            .arg(nodo->pelicula.getDuracionMinutos())
            .arg(QString::fromStdString(nodo->pelicula.getFechaEstreno()))
            .arg(QString::fromStdString(nodo->pelicula.getFechaFin()));
            
        QMessageBox::information(this, "Detalle Película", info);
    }
}

void MainWindow::onAsientoClicked() {
    if (!currentClientMovieNode) return;
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    int fila = btn->property("fila").toInt();
    int columna = btn->property("columna").toInt();
    bool ocupado = btn->property("ocupado").toBool();
    
    if (ocupado) {
        QString cli = QString::fromStdString(currentClientMovieNode->salaCine.getClienteAsiento(fila, columna));
        QMessageBox::warning(this, "Asiento Ocupado", "El asiento " + QString::number(fila) + "," + QString::number(columna) + " ya está reservado por: " + cli);
        return;
    }
    
    bool ok;
    QString nombre = QInputDialog::getText(this, "Reservar Asiento",
                                           "Asiento " + QString::number(fila) + "," + QString::number(columna) + "\nIngrese su nombre:", 
                                           QLineEdit::Normal, "", &ok);
    if (ok && !nombre.isEmpty()) {
        // Generar código de reserva único: PELID-F-C-TIMESTAMP
        QString codReserva = QString("%1-F%2-C%3-%4")
            .arg(QString::fromStdString(currentClientMovieNode->pelicula.getCodigo()))
            .arg(fila).arg(columna)
            .arg(QDateTime::currentMSecsSinceEpoch());
        if (currentClientMovieNode->salaCine.reservarAsiento(
                fila, columna, nombre.toStdString(), codReserva.toStdString())) {
            // Persistir inmediatamente después de reservar
            persistirMatrizActual(currentClientMovieNode, currentClientHorario, currentClientSala);
            QMessageBox::information(this, "Éxito",
                QString("Asiento reservado.\nCódigo de reserva: %1").arg(codReserva));
            refreshClienteSeatMap();
        }
    }
}

void MainWindow::onClienteConsultarAsiento() {
    if (!currentClientMovieNode) return;
    int f = spnCancelFila->value();
    int c = spnCancelCol->value();
    
    if (currentClientMovieNode->salaCine.estaOcupado(f, c)) {
        QString cli = QString::fromStdString(currentClientMovieNode->salaCine.getClienteAsiento(f, c));
        lblInfoAsiento->setText("Ocupado por: " + cli);
        lblInfoAsiento->setStyleSheet("color:#E74C3C; font-size:11pt; font-weight:bold; padding:8px;");
    } else {
        lblInfoAsiento->setText("Libre");
        lblInfoAsiento->setStyleSheet("color:#2ECC71; font-size:11pt; font-weight:bold; padding:8px;");
    }
}

void MainWindow::onClienteCancelarReserva() {
    if (!currentClientMovieNode) {
        QMessageBox::warning(this, "Atención", "Debe seleccionar una película primero.");
        return;
    }
    int f = spnCancelFila->value();
    int c = spnCancelCol->value();
    
    if (!currentClientMovieNode->salaCine.estaOcupado(f, c)) {
        QMessageBox::warning(this, "Atención", "Este asiento no está reservado.");
        return;
    }
    
    auto reply = QMessageBox::question(this, "Confirmar", "¿Desea cancelar la reserva del asiento " + QString::number(f) + "," + QString::number(c) + "?");
    
    if (reply == QMessageBox::Yes) {
        if (currentClientMovieNode->salaCine.cancelarReserva(f, c)) {
            // Persistir tras cancelar
            persistirMatrizActual(currentClientMovieNode, currentClientHorario, currentClientSala);
            QMessageBox::information(this, "Éxito", "Reserva cancelada.");
            lblInfoAsiento->setText("—");
            lblInfoAsiento->setStyleSheet("color:#E2E8F0; font-size:11pt; padding:8px;");
            refreshClienteSeatMap();
        }
    }
}

void MainWindow::onClienteEnviarSolicitud() {
    QString cli = inpCliNombre->text().trimmed();
    QString tel = inpCliTelefono->text().trimmed();
    QString tip = inpCliTipo->text().trimmed();
    QString des = inpCliDescripcion->toPlainText().trimmed();
    
    if (cli.isEmpty() || tel.isEmpty()) {
        QMessageBox::warning(this, "Atención", "Nombre y teléfono son obligatorios.");
        return;
    }
    
    Solicitud s(contadorSolicitudes++, cli.toStdString(), tel.toStdString(), tip.toStdString(), des.toStdString());
    solicitudes.agregarSolicitud(s);
    
    QMessageBox::information(this, "Enviado", "Solicitud registrada con el ID " + QString::number(s.getId()));
    
    inpCliNombre->clear();
    inpCliTelefono->clear();
    inpCliTipo->clear();
    inpCliDescripcion->clear();
    
    refreshSolicitudesAdmin();
}

void MainWindow::onClienteBuscarPorTelefono() {
    QString tel = inpBuscarTelefono->text().trimmed();
    lstSolicitudesCliente->clear();
    
    if (tel.isEmpty()) return;
    
    // Buscar todas las solicitudes de este teléfono
    bool found = false;
    if (!solicitudes.estaVacia()) {
        NodoSolicitud* actual = solicitudes.getCabeza();
        do {
            if (QString::fromStdString(actual->solicitud.getTelefono()) == tel) {
                QString text = QString("ID: %1 | Tipo: %2 | Estado: %3")
                    .arg(actual->solicitud.getId())
                    .arg(QString::fromStdString(actual->solicitud.getTipo()))
                    .arg(QString::fromStdString(actual->solicitud.getEstado()));
                lstSolicitudesCliente->addItem(text);
                found = true;
            }
            actual = actual->siguiente;
        } while (actual != solicitudes.getCabeza());
    }
    
    if (!found) {
        lstSolicitudesCliente->addItem("No se encontraron solicitudes para este teléfono.");
    }
}

void MainWindow::llenarComboInorden(NodoBST* nodo) {
    if (nodo == nullptr) return;
    llenarComboInorden(nodo->izquierdo);
    QString tit = QString::fromStdString(nodo->pelicula.getTitulo());
    QString cod = QString::fromStdString(nodo->pelicula.getCodigo());
    if (cmbPeliculaMatriz) cmbPeliculaMatriz->addItem(tit, cod);
    if (cmbClientePelicula) cmbClientePelicula->addItem(tit, cod);
    llenarComboInorden(nodo->derecho);
}

void MainWindow::refreshPeliculasCombo() {
    if (cmbPeliculaMatriz) cmbPeliculaMatriz->clear();
    if (cmbClientePelicula) cmbClientePelicula->clear();
    llenarComboInorden(cartelera.getRaiz());
    if (cmbPeliculaMatriz && cmbPeliculaMatriz->count() == 0) {
        cmbPeliculaMatriz->addItem("Sin películas", "");
        if (cmbClientePelicula) cmbClientePelicula->addItem("Sin películas", "");
    }
    // Set default movies
    currentAdminMovieNode = nullptr;
    currentClientMovieNode = nullptr;
    onAdminMovieSelectionChanged();
    onClienteMovieSelectionChanged();
}

void MainWindow::onLogout() {
    // Persistir matrices activas antes de salir
    if (currentAdminMovieNode) {
        persistirMatrizActual(currentAdminMovieNode, currentAdminHorario, currentAdminSala);
    }
    if (currentClientMovieNode && currentClientMovieNode != currentAdminMovieNode) {
        persistirMatrizActual(currentClientMovieNode, currentClientHorario, currentClientSala);
    }
    emit logout();
}

// ─────────────────────────────────────────────────────────────
// HELPERS DE PERSISTENCIA JSON (Fase 2)
// ─────────────────────────────────────────────────────────────
QString MainWindow::rutaJsonFuncion(const QString& codigoPelicula,
                                     const QString& horario,
                                     const QString& sala) const {
    // Sanitizar: reemplazar espacios y ':' por '_'
    QString h = horario; h.replace(':', '-').replace(' ', '_');
    QString s = sala;    s.replace(' ', '_');
    return QString("Asientos/%1_%2_%3_funcion.json")
           .arg(codigoPelicula).arg(h).arg(s);
}

void MainWindow::persistirMatrizActual(NodoBST* nodo,
                                        const QString& horario,
                                        const QString& sala) {
    if (!nodo) return;
    QDir().mkpath("Asientos");
    QString ruta = rutaJsonFuncion(
        QString::fromStdString(nodo->pelicula.getCodigo()), horario, sala);
    nodo->salaCine.serializarJSON(ruta.toStdString());
}

void MainWindow::cargarMatrizFuncion(NodoBST* nodo,
                                      const QString& horario,
                                      const QString& sala) {
    if (!nodo) return;
    QString ruta = rutaJsonFuncion(
        QString::fromStdString(nodo->pelicula.getCodigo()), horario, sala);
    
    // 1. Vaciar matriz y reiniciar dimensiones para que sea única de la función
    nodo->salaCine.vaciar();
    nodo->salaCine.inicializarDimensiones(0, 0);

    // 2. Intentar cargar configuraciones guardadas localmente
    nodo->salaCine.deserializarJSON(ruta.toStdString());

    // 3. Buscar el codigo de esta función en AVL
    std::string codigoPeli = nodo->pelicula.getCodigo();
    std::string h = horario.toStdString();
    std::string s = sala.toStdString();
    std::string codFunc = "";

    SistemaCine::getInstancia().funciones.recorridoInorden([&](NodoAVL* n) {
        if (n->funcion.getCodigoPelicula() == codigoPeli &&
            n->funcion.getHora() == h &&
            n->funcion.getSala() == s) {
            codFunc = n->funcion.getCodigo();
        }
    });

    // 4. Leer las reservas REALES de la Tabla Hash para ESTA función
    if (!codFunc.empty()) {
        auto& tablaH = SistemaCine::getInstancia().reservas.getTabla();
        for (int i = 0; i < (int)tablaH.size(); ++i) {
            NodoReserva* nr = tablaH[i].getCabeza();
            while (nr != nullptr) {
                if (nr->reserva.getCodigoFuncion() == codFunc) {
                    nodo->salaCine.reservarAsiento(
                        nr->reserva.getFila(), 
                        nr->reserva.getColumna(), 
                        nr->reserva.getIdCliente(), 
                        nr->reserva.getCodigoReserva()
                    );
                }
                nr = nr->siguiente;
            }
        }
    }
    
    // 5. Dimensiones default si no hubo persistencias previas ni reservas
    if (nodo->salaCine.getMaxFilas() == 0) {
        nodo->salaCine.inicializarDimensiones(5, 5);
    }
}

// ─────────────────────────────────────────────────────────────
// GESTIÓN DE CLIENTES Y RESERVAS (FASE 2 - PASO 3)
// ─────────────────────────────────────────────────────────────

void MainWindow::refreshClientesAdmin() {
    if (!tblClientesAdmin) return;
    tblClientesAdmin->setRowCount(0);

    QString busqueda = inpBuscarClienteAdmin ? inpBuscarClienteAdmin->text().trimmed().toLower() : "";

    NodoCliente* aux = clientes.getCabeza();
    int r = 0;
    while (aux != nullptr) {
        QString id     = QString::fromStdString(aux->cliente.getId());
        QString nombre = QString::fromStdString(aux->cliente.getNombre());
        QString correo = QString::fromStdString(aux->cliente.getCorreo());
        QString pass   = QString::fromStdString(aux->cliente.getPassword());
        QString tel    = QString::fromStdString(aux->cliente.getTelefono());
        int numRes     = aux->cliente.getReservas().getTamanio();

        if (!busqueda.isEmpty()) {
            if (!id.toLower().contains(busqueda) && !correo.toLower().contains(busqueda) && !nombre.toLower().contains(busqueda)) {
                aux = aux->siguiente;
                continue;
            }
        }

        tblClientesAdmin->insertRow(r);
        tblClientesAdmin->setItem(r, 0, new QTableWidgetItem(id));
        tblClientesAdmin->setItem(r, 1, new QTableWidgetItem(nombre));
        tblClientesAdmin->setItem(r, 2, new QTableWidgetItem(correo));
        tblClientesAdmin->setItem(r, 3, new QTableWidgetItem(pass));
        tblClientesAdmin->setItem(r, 4, new QTableWidgetItem(tel));
        tblClientesAdmin->setItem(r, 5, new QTableWidgetItem(QString::number(numRes)));
        r++;

        aux = aux->siguiente;
    }
}

void MainWindow::onAdminAgregarCliente() {
    QDialog dlg(this);
    dlg.setWindowTitle("Agregar Cliente");
    QFormLayout form(&dlg);

    QLineEdit txtId(&dlg);
    QLineEdit txtNombre(&dlg);
    QLineEdit txtCorreo(&dlg);
    QLineEdit txtPass(&dlg);
    QLineEdit txtTel(&dlg);

    form.addRow("ID / CTI:", &txtId);
    form.addRow("Nombre:", &txtNombre);
    form.addRow("Correo:", &txtCorreo);
    form.addRow("Contraseña:", &txtPass);
    form.addRow("Teléfono:", &txtTel);

    QDialogButtonBox bbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&bbox);
    connect(&bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        std::string id = txtId.text().trimmed().toStdString();
        std::string nom = txtNombre.text().trimmed().toStdString();
        std::string correo = txtCorreo.text().trimmed().toStdString();
        std::string pass = txtPass.text().toStdString();
        std::string tel = txtTel.text().trimmed().toStdString();

        if (id.empty() || correo.empty()) {
            QMessageBox::warning(this, "Error", "El ID y el Correo son requeridos.");
            return;
        }

        Cliente nuevo(id, nom, correo, pass, tel);
        if (clientes.insertar(nuevo)) {
            QMessageBox::information(this, "Éxito", "Cliente agregado correctamente.");
            refreshClientesAdmin();
        } else {
            QMessageBox::warning(this, "Error", "Ya existe un cliente con ese ID o Correo.");
        }
    }
}

void MainWindow::onAdminEditarCliente() {
    int r = tblClientesAdmin->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Atención", "Seleccione un cliente de la tabla para editar.");
        return;
    }

    std::string id = tblClientesAdmin->item(r, 0)->text().toStdString();
    NodoCliente* nc = clientes.buscarPorId(id);
    if (!nc) return;

    QDialog dlg(this);
    dlg.setWindowTitle("Editar Cliente");
    QFormLayout form(&dlg);

    QLineEdit txtNombre(QString::fromStdString(nc->cliente.getNombre()), &dlg);
    QLineEdit txtCorreo(QString::fromStdString(nc->cliente.getCorreo()), &dlg);
    QLineEdit txtPass(QString::fromStdString(nc->cliente.getPassword()), &dlg);
    QLineEdit txtTel(QString::fromStdString(nc->cliente.getTelefono()), &dlg);

    form.addRow("ID:", new QLabel(QString::fromStdString(id)));
    form.addRow("Nombre:", &txtNombre);
    form.addRow("Correo:", &txtCorreo);
    form.addRow("Contraseña:", &txtPass);
    form.addRow("Teléfono:", &txtTel);

    QDialogButtonBox bbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&bbox);
    connect(&bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        nc->cliente.setNombre(txtNombre.text().trimmed().toStdString());
        nc->cliente.setCorreo(txtCorreo.text().trimmed().toStdString());
        nc->cliente.setPassword(txtPass.text().toStdString());
        nc->cliente.setTelefono(txtTel.text().trimmed().toStdString());

        QMessageBox::information(this, "Éxito", "Cliente actualizado correctamente.");
        refreshClientesAdmin();
    }
}

void MainWindow::onAdminEliminarCliente() {
    int r = tblClientesAdmin->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Atención", "Seleccione un cliente para eliminar.");
        return;
    }

    QString id = tblClientesAdmin->item(r, 0)->text();
    auto res = QMessageBox::question(this, "Confirmar eliminación",
                                     "¿Desea eliminar al cliente con ID " + id + "?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        if (clientes.eliminar(id.toStdString())) {
            QMessageBox::information(this, "Éxito", "Cliente eliminado.");
            refreshClientesAdmin();
        }
    }
}

void MainWindow::onAdminBuscarCliente() {
    refreshClientesAdmin();
}

void MainWindow::onAdminVerReservasCliente() {
    if (!tblClientesAdmin) return;
    int r = tblClientesAdmin->currentRow();
    qDebug() << "[VerReservas] 1. Iniciando Ver Reservas. Fila seleccionada:" << r;
    if (r < 0 || tblClientesAdmin->item(r, 0) == nullptr) {
        QMessageBox::warning(this, "Atención", "Seleccione un cliente para ver sus reservas.");
        return;
    }

    std::string id = tblClientesAdmin->item(r, 0)->text().toStdString();
    qDebug() << "[VerReservas] 2. ID obtenido de tabla:" << QString::fromStdString(id);

    NodoCliente* nc = clientes.buscarPorId(id);
    qDebug() << "[VerReservas] 3. Puntero por ID:" << (void*)nc;
    if (!nc) {
        nc = clientes.buscarPorCorreo(id);
        qDebug() << "[VerReservas] 4. Puntero por Correo:" << (void*)nc;
    }
    if (!nc) {
        QMessageBox::warning(this, "Atención", "No se encontró el cliente seleccionado.");
        return;
    }

    qDebug() << "[VerReservas] 5. Cliente encontrado:" << QString::fromStdString(nc->cliente.getNombre())
             << "| Reservas en lista:" << nc->cliente.getReservas().getTamanio()
             << "| Cabeza:" << (void*)nc->cliente.getReservas().getCabeza();

    QDialog dlg(this);
    dlg.setWindowTitle("Reservas del Cliente: " + QString::fromStdString(nc->cliente.getNombre()));
    dlg.resize(650, 350);
    QVBoxLayout layout(&dlg);

    QTableWidget* tblRes = new QTableWidget(0, 6, &dlg);
    tblRes->setHorizontalHeaderLabels({"Código Reserva", "Código Función", "Estado Función", "Fila", "Columna", "Fecha Reserva"});
    tblRes->horizontalHeader()->setStretchLastSection(true);
    tblRes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblRes->setEditTriggers(QAbstractItemView::NoEditTriggers);

    NodoReserva* nr = nc->cliente.getReservas().getCabeza();
    int row = 0;
    while (nr != nullptr) {
        qDebug() << "[VerReservas] 6. Procesando reserva" << row << "| Nodo:" << (void*)nr
                 << "| Cod:" << QString::fromStdString(nr->reserva.getCodigoReserva());
        std::string codFunc = nr->reserva.getCodigoFuncion();
        NodoAVL* nodoF = !codFunc.empty() ? funciones.buscar(codFunc) : nullptr;
        QString estadoFunc = (nodoF != nullptr) ? "Válida" : "No Encontrada";

        tblRes->insertRow(row);
        tblRes->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(nr->reserva.getCodigoReserva())));
        tblRes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(codFunc)));
        tblRes->setItem(row, 2, new QTableWidgetItem(estadoFunc));
        tblRes->setItem(row, 3, new QTableWidgetItem(QString::number(nr->reserva.getFila())));
        tblRes->setItem(row, 4, new QTableWidgetItem(QString::number(nr->reserva.getColumna())));
        tblRes->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(nr->reserva.getFechaReserva())));
        row++;
        nr = nr->siguiente;
    }
    qDebug() << "[VerReservas] 7. Total filas procesadas:" << row;
    layout.addWidget(tblRes);

    QDialogButtonBox* bbox = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    layout.addWidget(bbox);
    connect(bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);

    dlg.exec();
}

void MainWindow::refreshMisReservasCliente() {
    if (!tblMisReservasCliente) return;
    tblMisReservasCliente->setRowCount(0);

    if (clienteActual == nullptr) {
        if (lblClienteHeader) lblClienteHeader->setText("Sin cliente autenticado");
        return;
    }

    if (lblClienteHeader) {
        lblClienteHeader->setText("🎟️ Mis Reservas — " + QString::fromStdString(clienteActual->getNombre()) + " (" + QString::fromStdString(clienteActual->getCorreo()) + ")");
    }

    NodoReserva* nr = clienteActual->getReservas().getCabeza();
    int r = 0;
    while (nr != nullptr) {
        tblMisReservasCliente->insertRow(r);
        tblMisReservasCliente->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(nr->reserva.getCodigoReserva())));
        tblMisReservasCliente->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(nr->reserva.getCodigoFuncion())));
        tblMisReservasCliente->setItem(r, 2, new QTableWidgetItem(QString::number(nr->reserva.getFila())));
        tblMisReservasCliente->setItem(r, 3, new QTableWidgetItem(QString::number(nr->reserva.getColumna())));
        tblMisReservasCliente->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(nr->reserva.getFechaReserva())));
        
        // Validación de Estado (VÁLIDA vs VENCIDA)
        NodoAVL* nodoF = funciones.buscar(nr->reserva.getCodigoFuncion());
        QTableWidgetItem* itemEstado = new QTableWidgetItem("DESCONOCIDO");
        if (nodoF) {
            QDate fechaFunc = QDate::fromString(QString::fromStdString(nodoF->funcion.getFecha()), "yyyy-MM-dd");
            if (fechaFunc.isValid() && fechaFunc >= QDate::currentDate()) {
                itemEstado->setText("VÁLIDA");
                itemEstado->setForeground(QColor("#10B981")); // Verde
            } else {
                itemEstado->setText("VENCIDA");
                itemEstado->setForeground(QColor("#EF4444")); // Rojo
            }
        }
        tblMisReservasCliente->setItem(r, 5, itemEstado);
        
        r++;
        nr = nr->siguiente;
    }
}

void MainWindow::onClienteCancelarReservaHistorial() {
    if (!tblMisReservasCliente || !clienteActual) return;
    int r = tblMisReservasCliente->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Atención", "Seleccione una reserva de la lista para cancelar.");
        return;
    }

    std::string codRes = tblMisReservasCliente->item(r, 0)->text().toStdString();
    NodoReserva* nr = clienteActual->getReservas().buscar(codRes);
    if (!nr) return;

    std::string codFunc = nr->reserva.getCodigoFuncion();
    int fila = nr->reserva.getFila();
    int col = nr->reserva.getColumna();

    auto resp = QMessageBox::question(this, "Cancelar Reserva",
                                      "¿Está seguro de cancelar la reserva " + QString::fromStdString(codRes) + "?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (resp == QMessageBox::Yes) {
        clienteActual->getReservas().eliminar(codRes);
        SistemaCine::getInstancia().reservas.eliminar(codRes);

        NodoAVL* nodoF = funciones.buscar(codFunc);
        if (nodoF != nullptr) {
            NodoBST* nodoP = cartelera.buscar(nodoF->funcion.getCodigoPelicula());
            if (nodoP != nullptr) {
                nodoP->salaCine.cancelarReserva(fila, col);
                persistirMatrizActual(nodoP, QString::fromStdString(nodoF->funcion.getHora()), QString::fromStdString(nodoF->funcion.getSala()));
            }
        }

        QMessageBox::information(this, "Éxito", "Reserva cancelada correctamente.");
        refreshMisReservasCliente();
        refreshClienteSeatMap();
    }
}
