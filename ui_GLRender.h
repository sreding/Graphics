/********************************************************************************
** Form generated from reading UI file 'GLRender.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLRENDER_H
#define UI_GLRENDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>
#include "CCanvas.h"

QT_BEGIN_NAMESPACE

class Ui_GLRender
{
public:
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionExit;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    CCanvas *canvas;
    QMenuBar *menubar;
    QMenu *menuFile;

    void setupUi(QMainWindow *GLRender)
    {
        if (GLRender->objectName().isEmpty())
            GLRender->setObjectName(QStringLiteral("GLRender"));
        GLRender->resize(800, 600);
        actionLoad = new QAction(GLRender);
        actionLoad->setObjectName(QStringLiteral("actionLoad"));
        actionSave = new QAction(GLRender);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionExit = new QAction(GLRender);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralwidget = new QWidget(GLRender);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        canvas = new CCanvas(centralwidget);
        canvas->setObjectName(QStringLiteral("canvas"));

        gridLayout->addWidget(canvas, 0, 0, 1, 1);

        GLRender->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GLRender);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        GLRender->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionExit);

        retranslateUi(GLRender);
        QObject::connect(actionExit, SIGNAL(triggered()), GLRender, SLOT(close()));

        QMetaObject::connectSlotsByName(GLRender);
    } // setupUi

    void retranslateUi(QMainWindow *GLRender)
    {
        GLRender->setWindowTitle(QApplication::translate("GLRender", "MainWindow", Q_NULLPTR));
        actionLoad->setText(QApplication::translate("GLRender", "Load", Q_NULLPTR));
        actionSave->setText(QApplication::translate("GLRender", "Save", Q_NULLPTR));
        actionExit->setText(QApplication::translate("GLRender", "Exit", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("GLRender", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GLRender: public Ui_GLRender {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLRENDER_H
