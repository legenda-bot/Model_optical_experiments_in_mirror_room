/****************************************************************************
** Meta object code from reading C++ file 'mirrorroom.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mirrorroom.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mirrorroom.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
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
struct qt_meta_tag_ZN10MirrorRoomE_t {};
} // unnamed namespace

template <> constexpr inline auto MirrorRoom::qt_create_metaobjectdata<qt_meta_tag_ZN10MirrorRoomE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MirrorRoom",
        "wallSelected",
        "",
        "wallIndex",
        "setRoomCreationMode",
        "MirrorRoom::RoomCreationMode",
        "mode",
        "setNumberOfWalls",
        "count",
        "startRayExperiment",
        "startPoint",
        "angle",
        "clearRoom",
        "saveExperiment",
        "filename",
        "loadExperiment",
        "getWall",
        "Wall*",
        "index",
        "getWalls",
        "setSelectingStartPoint",
        "selecting",
        "setSelectingAngle",
        "setCurrentAngle",
        "getRayStartPoint",
        "getCurrentAngle"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'wallSelected'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'setRoomCreationMode'
        QtMocHelpers::SlotData<void(MirrorRoom::RoomCreationMode)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'setNumberOfWalls'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'startRayExperiment'
        QtMocHelpers::SlotData<void(const QPointF &, double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QPointF, 10 }, { QMetaType::Double, 11 },
        }}),
        // Slot 'startRayExperiment'
        QtMocHelpers::SlotData<void(double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 11 },
        }}),
        // Slot 'clearRoom'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'saveExperiment'
        QtMocHelpers::SlotData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'loadExperiment'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'getWall'
        QtMocHelpers::SlotData<Wall *(int) const>(16, 2, QMC::AccessPublic, 0x80000000 | 17, {{
            { QMetaType::Int, 18 },
        }}),
        // Slot 'getWalls'
        QtMocHelpers::SlotData<void() const>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setSelectingStartPoint'
        QtMocHelpers::SlotData<void(bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setSelectingAngle'
        QtMocHelpers::SlotData<void(bool)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setCurrentAngle'
        QtMocHelpers::SlotData<void(double)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 11 },
        }}),
        // Slot 'getRayStartPoint'
        QtMocHelpers::SlotData<QPointF() const>(24, 2, QMC::AccessPublic, QMetaType::QPointF),
        // Slot 'getCurrentAngle'
        QtMocHelpers::SlotData<double() const>(25, 2, QMC::AccessPublic, QMetaType::Double),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MirrorRoom, qt_meta_tag_ZN10MirrorRoomE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MirrorRoom::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MirrorRoomE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MirrorRoomE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MirrorRoomE_t>.metaTypes,
    nullptr
} };

void MirrorRoom::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MirrorRoom *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->wallSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->setRoomCreationMode((*reinterpret_cast< std::add_pointer_t<MirrorRoom::RoomCreationMode>>(_a[1]))); break;
        case 2: _t->setNumberOfWalls((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->startRayExperiment((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 4: _t->startRayExperiment((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->clearRoom(); break;
        case 6: _t->saveExperiment((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->loadExperiment((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: { Wall* _r = _t->getWall((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< Wall**>(_a[0]) = std::move(_r); }  break;
        case 9: _t->getWalls(); break;
        case 10: _t->setSelectingStartPoint((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->setSelectingAngle((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->setCurrentAngle((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 13: { QPointF _r = _t->getRayStartPoint();
            if (_a[0]) *reinterpret_cast< QPointF*>(_a[0]) = std::move(_r); }  break;
        case 14: { double _r = _t->getCurrentAngle();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MirrorRoom::*)(int )>(_a, &MirrorRoom::wallSelected, 0))
            return;
    }
}

const QMetaObject *MirrorRoom::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MirrorRoom::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MirrorRoomE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MirrorRoom::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MirrorRoom::wallSelected(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
