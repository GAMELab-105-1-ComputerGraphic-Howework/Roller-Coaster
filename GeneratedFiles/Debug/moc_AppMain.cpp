/****************************************************************************
** Meta object code from reading C++ file 'AppMain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/AppMain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppMain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AppMain_t {
    QByteArrayData data[27];
    char stringdata[434];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AppMain_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AppMain_t qt_meta_stringdata_AppMain = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 13),
QT_MOC_LITERAL(2, 22, 0),
QT_MOC_LITERAL(3, 23, 13),
QT_MOC_LITERAL(4, 37, 7),
QT_MOC_LITERAL(5, 45, 16),
QT_MOC_LITERAL(6, 62, 4),
QT_MOC_LITERAL(7, 67, 16),
QT_MOC_LITERAL(8, 84, 14),
QT_MOC_LITERAL(9, 99, 16),
QT_MOC_LITERAL(10, 116, 15),
QT_MOC_LITERAL(11, 132, 19),
QT_MOC_LITERAL(12, 152, 21),
QT_MOC_LITERAL(13, 174, 18),
QT_MOC_LITERAL(14, 193, 15),
QT_MOC_LITERAL(15, 209, 17),
QT_MOC_LITERAL(16, 227, 18),
QT_MOC_LITERAL(17, 246, 17),
QT_MOC_LITERAL(18, 264, 18),
QT_MOC_LITERAL(19, 283, 18),
QT_MOC_LITERAL(20, 302, 3),
QT_MOC_LITERAL(21, 306, 15),
QT_MOC_LITERAL(22, 322, 18),
QT_MOC_LITERAL(23, 341, 22),
QT_MOC_LITERAL(24, 364, 22),
QT_MOC_LITERAL(25, 387, 22),
QT_MOC_LITERAL(26, 410, 22)
    },
    "AppMain\0LoadTrackPath\0\0SaveTrackPath\0"
    "ExitApp\0ChangeCameraType\0type\0"
    "ChangeCamToWorld\0ChangeCamToTop\0"
    "ChangeCamToTrain\0ChangeCurveType\0"
    "ChangeCurveToLinear\0ChangeCurveToCardinal\0"
    "ChangeCurveToCubic\0ChangeTrackType\0"
    "ChangeTrackToLine\0ChangeTrackToTrack\0"
    "ChangeTrackToRoad\0SwitchPlayAndPause\0"
    "ChangeSpeedOfTrain\0val\0AddControlPoint\0"
    "DeleteControlPoint\0RotateControlPointAddX\0"
    "RotateControlPointSubX\0RotateControlPointAddZ\0"
    "RotateControlPointSubZ\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppMain[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x08,
       3,    0,  130,    2, 0x08,
       4,    0,  131,    2, 0x08,
       5,    1,  132,    2, 0x08,
       7,    0,  135,    2, 0x08,
       8,    0,  136,    2, 0x08,
       9,    0,  137,    2, 0x08,
      10,    1,  138,    2, 0x08,
      11,    0,  141,    2, 0x08,
      12,    0,  142,    2, 0x08,
      13,    0,  143,    2, 0x08,
      14,    1,  144,    2, 0x08,
      15,    0,  147,    2, 0x08,
      16,    0,  148,    2, 0x08,
      17,    0,  149,    2, 0x08,
      18,    0,  150,    2, 0x08,
      19,    1,  151,    2, 0x08,
      21,    0,  154,    2, 0x08,
      22,    0,  155,    2, 0x08,
      23,    0,  156,    2, 0x08,
      24,    0,  157,    2, 0x08,
      25,    0,  158,    2, 0x08,
      26,    0,  159,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AppMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AppMain *_t = static_cast<AppMain *>(_o);
        switch (_id) {
        case 0: _t->LoadTrackPath(); break;
        case 1: _t->SaveTrackPath(); break;
        case 2: _t->ExitApp(); break;
        case 3: _t->ChangeCameraType((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->ChangeCamToWorld(); break;
        case 5: _t->ChangeCamToTop(); break;
        case 6: _t->ChangeCamToTrain(); break;
        case 7: _t->ChangeCurveType((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->ChangeCurveToLinear(); break;
        case 9: _t->ChangeCurveToCardinal(); break;
        case 10: _t->ChangeCurveToCubic(); break;
        case 11: _t->ChangeTrackType((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: _t->ChangeTrackToLine(); break;
        case 13: _t->ChangeTrackToTrack(); break;
        case 14: _t->ChangeTrackToRoad(); break;
        case 15: _t->SwitchPlayAndPause(); break;
        case 16: _t->ChangeSpeedOfTrain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->AddControlPoint(); break;
        case 18: _t->DeleteControlPoint(); break;
        case 19: _t->RotateControlPointAddX(); break;
        case 20: _t->RotateControlPointSubX(); break;
        case 21: _t->RotateControlPointAddZ(); break;
        case 22: _t->RotateControlPointSubZ(); break;
        default: ;
        }
    }
}

const QMetaObject AppMain::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_AppMain.data,
      qt_meta_data_AppMain,  qt_static_metacall, 0, 0}
};


const QMetaObject *AppMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppMain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppMain.stringdata))
        return static_cast<void*>(const_cast< AppMain*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int AppMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
