/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/ui/MainWindow.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "logout",
        "",
        "onAdminCargarPeliculasFunciones",
        "onAdminCargarClientesReservas",
        "onAdminAgregarPelicula",
        "onAdminEliminarPelicula",
        "onAdminBuscarPelicula",
        "onAdminAgregarFuncion",
        "onAdminEditarFuncion",
        "onAdminEliminarFuncion",
        "onAdminBuscarFuncion",
        "onAdminCrearPromocion",
        "onAdminAgregarBeneficio",
        "onPromoSeleccionada",
        "idx",
        "onAdminAprobarSolicitud",
        "onAdminRechazarSolicitud",
        "onAdminEliminarSolicitud",
        "onAdminConfigurarSala",
        "refreshAdminSeatMap",
        "onAdminMovieSelectionChanged",
        "onAdminFuncionChanged",
        "onAdminAgregarCliente",
        "onAdminEditarCliente",
        "onAdminEliminarCliente",
        "onAdminBuscarCliente",
        "onAdminVerReservasCliente",
        "onGenerarTodosReportes",
        "onGenerarReporteBST",
        "onGenerarReporteAVL",
        "onGenerarReporteSolicitudes",
        "onGenerarReportePromociones",
        "onGenerarReporteMatriz",
        "onGenerarReporteClientes",
        "onClienteBuscarPelicula",
        "onClienteVerDetalle",
        "row",
        "col",
        "onAsientoClicked",
        "refreshClienteSeatMap",
        "onClienteMovieSelectionChanged",
        "onClienteFuncionChanged",
        "onClienteCancelarReservaHistorial",
        "onClienteConsultarAsiento",
        "onClienteCancelarReserva",
        "onClienteEnviarSolicitud",
        "onClienteBuscarPorTelefono",
        "onLogout"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'logout'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onAdminCargarPeliculasFunciones'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminCargarClientesReservas'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminAgregarPelicula'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEliminarPelicula'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminBuscarPelicula'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminAgregarFuncion'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEditarFuncion'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEliminarFuncion'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminBuscarFuncion'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminCrearPromocion'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminAgregarBeneficio'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPromoSeleccionada'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onAdminAprobarSolicitud'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminRechazarSolicitud'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEliminarSolicitud'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminConfigurarSala'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshAdminSeatMap'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminMovieSelectionChanged'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminFuncionChanged'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminAgregarCliente'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEditarCliente'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminEliminarCliente'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminBuscarCliente'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAdminVerReservasCliente'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarTodosReportes'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReporteBST'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReporteAVL'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReporteSolicitudes'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReportePromociones'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReporteMatriz'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGenerarReporteClientes'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteBuscarPelicula'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteVerDetalle'
        QtMocHelpers::SlotData<void(int, int)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 37 }, { QMetaType::Int, 38 },
        }}),
        // Slot 'onAsientoClicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshClienteSeatMap'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteMovieSelectionChanged'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteFuncionChanged'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteCancelarReservaHistorial'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteConsultarAsiento'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteCancelarReserva'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteEnviarSolicitud'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClienteBuscarPorTelefono'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLogout'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->logout(); break;
        case 1: _t->onAdminCargarPeliculasFunciones(); break;
        case 2: _t->onAdminCargarClientesReservas(); break;
        case 3: _t->onAdminAgregarPelicula(); break;
        case 4: _t->onAdminEliminarPelicula(); break;
        case 5: _t->onAdminBuscarPelicula(); break;
        case 6: _t->onAdminAgregarFuncion(); break;
        case 7: _t->onAdminEditarFuncion(); break;
        case 8: _t->onAdminEliminarFuncion(); break;
        case 9: _t->onAdminBuscarFuncion(); break;
        case 10: _t->onAdminCrearPromocion(); break;
        case 11: _t->onAdminAgregarBeneficio(); break;
        case 12: _t->onPromoSeleccionada((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onAdminAprobarSolicitud(); break;
        case 14: _t->onAdminRechazarSolicitud(); break;
        case 15: _t->onAdminEliminarSolicitud(); break;
        case 16: _t->onAdminConfigurarSala(); break;
        case 17: _t->refreshAdminSeatMap(); break;
        case 18: _t->onAdminMovieSelectionChanged(); break;
        case 19: _t->onAdminFuncionChanged(); break;
        case 20: _t->onAdminAgregarCliente(); break;
        case 21: _t->onAdminEditarCliente(); break;
        case 22: _t->onAdminEliminarCliente(); break;
        case 23: _t->onAdminBuscarCliente(); break;
        case 24: _t->onAdminVerReservasCliente(); break;
        case 25: _t->onGenerarTodosReportes(); break;
        case 26: _t->onGenerarReporteBST(); break;
        case 27: _t->onGenerarReporteAVL(); break;
        case 28: _t->onGenerarReporteSolicitudes(); break;
        case 29: _t->onGenerarReportePromociones(); break;
        case 30: _t->onGenerarReporteMatriz(); break;
        case 31: _t->onGenerarReporteClientes(); break;
        case 32: _t->onClienteBuscarPelicula(); break;
        case 33: _t->onClienteVerDetalle((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 34: _t->onAsientoClicked(); break;
        case 35: _t->refreshClienteSeatMap(); break;
        case 36: _t->onClienteMovieSelectionChanged(); break;
        case 37: _t->onClienteFuncionChanged(); break;
        case 38: _t->onClienteCancelarReservaHistorial(); break;
        case 39: _t->onClienteConsultarAsiento(); break;
        case 40: _t->onClienteCancelarReserva(); break;
        case 41: _t->onClienteEnviarSolicitud(); break;
        case 42: _t->onClienteBuscarPorTelefono(); break;
        case 43: _t->onLogout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::logout, 0))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 44)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 44;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 44)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 44;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::logout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
