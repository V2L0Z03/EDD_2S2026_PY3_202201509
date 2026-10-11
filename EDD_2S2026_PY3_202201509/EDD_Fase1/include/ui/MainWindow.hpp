#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QPixmap>
#include <QSplitter>

// Estructuras de modelos
#include "../structures/ArbolBST.hpp"
#include "../structures/ArbolAVL.hpp"
#include "../structures/MatrizDispersa.hpp"
#include "../structures/ListaCircularPromos.hpp"
#include "../structures/ListaDobleBeneficios.hpp"
#include "../structures/ListaCircularDobleSolicitudes.hpp"
#include "../services/CSVReader.hpp"
#include "../services/JSONReader.hpp"
#include "../services/GraphvizReporter.hpp"
#include "../models/SistemaCine.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& rol, QWidget* parent = nullptr);
    ~MainWindow() = default;

signals:
    void logout();

private slots:
    // ─ Admin Cartelera
    void onAdminCargarPeliculasFunciones();
    void onAdminCargarClientesReservas();
    void onAdminAgregarPelicula();
    void onAdminEliminarPelicula();
    void onAdminBuscarPelicula();

    // ─ Admin Funciones AVL (Fase 2 - Paso 2)
    void onAdminAgregarFuncion();
    void onAdminEditarFuncion();
    void onAdminEliminarFuncion();
    void onAdminBuscarFuncion();

    // ─ Admin Promociones
    void onAdminCrearPromocion();
    void onAdminAgregarBeneficio();
    void onPromoSeleccionada(int idx);

    // ─ Admin Solicitudes
    void onAdminAprobarSolicitud();
    void onAdminRechazarSolicitud();
    void onAdminEliminarSolicitud();

    // ─ Admin Sala
    void onAdminConfigurarSala();
    void refreshAdminSeatMap();
    void onAdminMovieSelectionChanged();
    void onAdminFuncionChanged();       // persistencia al cambiar función

    // ─ Admin Clientes (Fase 2 - Paso 3)
    void onAdminAgregarCliente();
    void onAdminEditarCliente();
    void onAdminEliminarCliente();
    void onAdminBuscarCliente();
    void onAdminVerReservasCliente();

    // ─ Admin Reportes
    void onGenerarTodosReportes();
    void onGenerarReporteBST();
    void onGenerarReporteAVL();
    void onGenerarReporteSolicitudes();
    void onGenerarReportePromociones();
    void onGenerarReporteMatriz();
    void onGenerarReporteClientes();

    // ── Cliente Cartelera
    void onClienteBuscarPelicula();
    void onClienteVerDetalle(int row, int col);

    // ── Cliente Asientos
    void onAsientoClicked();
    void refreshClienteSeatMap();
    void onClienteMovieSelectionChanged();
    void onClienteFuncionChanged();     // persistencia al cambiar función cliente

    // ── Cliente Mis Reservas (Fase 2 - Paso 3)
    void onClienteCancelarReservaHistorial();

    // ── Cliente Cancelación
    void onClienteConsultarAsiento();
    void onClienteCancelarReserva();

    // ── Cliente Solicitudes
    void onClienteEnviarSolicitud();
    void onClienteBuscarPorTelefono();

    // ── Logout
    void onLogout();

private:
    void setupUI(const QString& rol);
    void setupAdminPanel();
    void setupClientePanel();
    void setupGlobalStyle();

    // Refresh de datos → widgets
    void refreshCarteleraAdmin();
    void refreshFuncionesAdmin();
    void refreshClientesAdmin();
    void refreshCarteleraCliente();
    void refreshMisReservasCliente();
    void refreshSolicitudesAdmin();
    void refreshPromocionesAdmin();
    void refreshPromocionesCombo();
    void refreshPeliculasCombo();
    void refreshFuncionesCombo();
    void llenarComboInorden(NodoBST* nodo);
    void refreshAsientosGrid();
    void refreshPromocionesCliente();
    void refreshHorariosCombo(QComboBox* cmbHorario, QComboBox* cmbSala);

    // Refresh de reportes Graphviz
    void cargarImagenReporte(QLabel* lbl, const QString& rutaPng);
    void refreshReporteBST();
    void refreshReporteAVL();
    void refreshReporteSolicitudes();
    void refreshReportePromociones();
    void refreshReporteMatriz();
    void refreshReporteClientes();

    // Helper BST/AVL inorden → tabla
    void llenarTablaInorden(NodoBST* nodo, QTableWidget* tabla, int& filaTabla);

    // ── Persistencia funciones
    // Construye la ruta del JSON para una función dada
    QString rutaJsonFuncion(const QString& codigoPelicula,
                            const QString& horario,
                            const QString& sala) const;
    // Persiste la matriz activa y la desvincula
    void persistirMatrizActual(NodoBST* nodo, const QString& horario, const QString& sala);
    // Carga la matriz para la función seleccionada
    void cargarMatrizFuncion(NodoBST* nodo, const QString& horario, const QString& sala);

    // ── Estado Global → referencia al Singleton
    QString rolActual;

    // Referencia directa a las estructuras del Singleton
    ArbolBST&                       cartelera;
    ArbolAVL&                       funciones;
    ListaCircularPromos&             promociones;
    ListaCircularDobleSolicitudes&   solicitudes;
    EstructuraClientes&             clientes;
    Cliente*&                       clienteActual;
    int&                             contadorSolicitudes;
    QString&                         currPelicula;
    QString&                         currHorario;
    QString&                         currSala;

    NodoBST*                         currentAdminMovieNode;
    NodoBST*                         currentClientMovieNode;

    // Horario y sala activos (para persistencia)
    QString currentAdminHorario;
    QString currentAdminSala;
    QString currentClientHorario;
    QString currentClientSala;

    // ── Layout principal
    QStackedWidget* mainStack = nullptr;

    // PANEL ADMIN
    QWidget*    adminPanel = nullptr;
    QTabWidget* adminTabs = nullptr;

    // Tab 0 – Cartelera BST
    QTableWidget* tblCarteleraAdmin = nullptr;
    QLineEdit*    inpBuscarAdmin = nullptr;
    QComboBox*    cmbOrdenCartelera = nullptr;

    // Tab 1 – Funciones AVL (Fase 2 - Paso 2)
    QTableWidget* tblFuncionesAdmin = nullptr;
    QLineEdit*    inpBuscarFuncion = nullptr;
    QComboBox*    cmbOrdenFunciones = nullptr;

    // Tab 2 – Clientes (Fase 2 - Paso 3)
    QTableWidget* tblClientesAdmin = nullptr;
    QLineEdit*    inpBuscarClienteAdmin = nullptr;

    // Tab 3 – Promociones
    QComboBox*   cmbPromoSelect = nullptr;
    QListWidget* lstPromociones = nullptr;
    QListWidget* lstBeneficios = nullptr;
    QLineEdit*   inpPromoCodigo = nullptr;
    QLineEdit*   inpPromoNombre = nullptr;
    QLineEdit*   inpPromoVigencia = nullptr;
    QLineEdit*   inpPromoDias = nullptr;
    QLineEdit*   inpBenTipo = nullptr;
    QLineEdit*   inpBenDesc = nullptr;
    QLineEdit*   inpBenValor = nullptr;

    // Tab 4 – Solicitudes
    QTableWidget* tblSolicitudes = nullptr;

    // Tab 5 – Sala
    QComboBox* cmbPeliculaMatriz = nullptr;
    QComboBox* cmbFuncionMatriz = nullptr;
    QComboBox* cmbHorarioMatriz = nullptr;
    QComboBox* cmbSalaMatriz = nullptr;
    QSpinBox*  spnFilas = nullptr;
    QSpinBox*  spnColumnas = nullptr;
    QLabel*    lblSalaInfo = nullptr;
    // Mapa visual de asientos para el Admin
    QWidget*     widgetAdminSeatMap = nullptr;
    QGridLayout* gridAdminSeatMap = nullptr;

    // Tab 6 – Reportes
    QTabWidget* reportesTabs = nullptr;
    QLabel*     imgBST = nullptr;
    QLabel*     imgAVL = nullptr;
    QLabel*     imgSolicitudes = nullptr;
    QLabel*     imgPromociones = nullptr;
    QLabel*     imgMatriz = nullptr;
    QLabel*     imgClientes = nullptr;

    // ════════════════════════════════
    // PANEL CLIENTE

    QWidget*    clientePanel = nullptr;
    QTabWidget* clienteTabs = nullptr;

    // Tab 0 – Cartelera cliente
    QLineEdit*    inpBuscarCliente = nullptr;
    QTableWidget* tblCarteleraCliente = nullptr;

    // Tab 1 – Asientos
    QComboBox*   cmbClientePelicula = nullptr;
    QComboBox*   cmbClienteFuncion = nullptr;
    QComboBox*   cmbClienteHorario = nullptr;
    QComboBox*   cmbClienteSala = nullptr;
    QLabel*      lblInfoMatrizCliente = nullptr;
    QScrollArea* scrollAsientos = nullptr;
    QWidget*     widgetAsientos = nullptr;
    QGridLayout* gridAsientos = nullptr;

    // Tab 2 – Mis Reservas (Fase 2 - Paso 3)
    QLabel*       lblClienteHeader = nullptr;
    QTableWidget* tblMisReservasCliente = nullptr;

    // Tab 3 – Cancelaciones
    QSpinBox* spnCancelFila = nullptr;
    QSpinBox* spnCancelCol = nullptr;
    QLabel*   lblInfoAsiento = nullptr;

    // Tab 4 – Solicitudes cliente
    QLineEdit* inpCliNombre = nullptr;
    QLineEdit* inpCliTelefono = nullptr;
    QLineEdit* inpCliTipo = nullptr;
    QTextEdit* inpCliDescripcion = nullptr;
    QLineEdit* inpBuscarTelefono = nullptr;
    QListWidget* lstSolicitudesCliente = nullptr;
    QListWidget* lstPromocionesCliente = nullptr;
};

#endif // MAINWINDOW_HPP