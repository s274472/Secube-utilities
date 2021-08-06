/****************************************************************************
** Meta object code from reading C++ file 'utilities.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.1.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../secube_utilities/utilities.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'utilities.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.1.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Utilities_t {
    const uint offsetsAndSize[32];
    char stringdata0[329];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Utilities_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Utilities_t qt_meta_stringdata_Utilities = {
    {
QT_MOC_LITERAL(0, 9), // "Utilities"
QT_MOC_LITERAL(10, 23), // "on_browseButton_clicked"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 27), // "on_deviceListButton_clicked"
QT_MOC_LITERAL(63, 25), // "on_group_line_textChanged"
QT_MOC_LITERAL(89, 4), // "arg1"
QT_MOC_LITERAL(94, 24), // "on_user_line_textChanged"
QT_MOC_LITERAL(119, 23), // "on_key_line_textChanged"
QT_MOC_LITERAL(143, 25), // "on_browseButton_2_clicked"
QT_MOC_LITERAL(169, 25), // "on_browseButton_3_clicked"
QT_MOC_LITERAL(195, 25), // "on_lineEdit_7_textChanged"
QT_MOC_LITERAL(221, 25), // "on_lineEdit_8_textChanged"
QT_MOC_LITERAL(247, 25), // "on_lineEdit_9_textChanged"
QT_MOC_LITERAL(273, 23), // "on_comboBox_2_activated"
QT_MOC_LITERAL(297, 5), // "index"
QT_MOC_LITERAL(303, 25) // "on_browseButton_4_clicked"

    },
    "Utilities\0on_browseButton_clicked\0\0"
    "on_deviceListButton_clicked\0"
    "on_group_line_textChanged\0arg1\0"
    "on_user_line_textChanged\0"
    "on_key_line_textChanged\0"
    "on_browseButton_2_clicked\0"
    "on_browseButton_3_clicked\0"
    "on_lineEdit_7_textChanged\0"
    "on_lineEdit_8_textChanged\0"
    "on_lineEdit_9_textChanged\0"
    "on_comboBox_2_activated\0index\0"
    "on_browseButton_4_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Utilities[] = {

 // content:
       9,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x08,    0 /* Private */,
       3,    0,   87,    2, 0x08,    1 /* Private */,
       4,    1,   88,    2, 0x08,    2 /* Private */,
       6,    1,   91,    2, 0x08,    4 /* Private */,
       7,    1,   94,    2, 0x08,    6 /* Private */,
       8,    0,   97,    2, 0x08,    8 /* Private */,
       9,    0,   98,    2, 0x08,    9 /* Private */,
      10,    1,   99,    2, 0x08,   10 /* Private */,
      11,    1,  102,    2, 0x08,   12 /* Private */,
      12,    1,  105,    2, 0x08,   14 /* Private */,
      13,    1,  108,    2, 0x08,   16 /* Private */,
      15,    0,  111,    2, 0x08,   18 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,

       0        // eod
};

void Utilities::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Utilities *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_browseButton_clicked(); break;
        case 1: _t->on_deviceListButton_clicked(); break;
        case 2: _t->on_group_line_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->on_user_line_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->on_key_line_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->on_browseButton_2_clicked(); break;
        case 6: _t->on_browseButton_3_clicked(); break;
        case 7: _t->on_lineEdit_7_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->on_lineEdit_8_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->on_lineEdit_9_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_comboBox_2_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_browseButton_4_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject Utilities::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_Utilities.offsetsAndSize,
    qt_meta_data_Utilities,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Utilities_t

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *Utilities::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Utilities::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Utilities.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Utilities::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
