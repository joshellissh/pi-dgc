/********************************************************************************
** Form generated from reading UI file 'hwdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HWDIALOG_H
#define UI_HWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTimeEdit>

QT_BEGIN_NAMESPACE

class Ui_HWDialog
{
public:
    QPlainTextEdit *plainTextEdit;
    QPushButton *closeButton;
    QPushButton *saveButton;
    QGroupBox *groupBox;
    QLabel *label_4;
    QLineEdit *ppmCurrent;
    QSpinBox *ppmSpinBox;
    QLabel *label_5;
    QGroupBox *groupBox_2;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *currentDateTime;
    QDateEdit *dateEdit;
    QTimeEdit *timeEdit;

    void setupUi(QDialog *HWDialog)
    {
        if (HWDialog->objectName().isEmpty())
            HWDialog->setObjectName(QString::fromUtf8("HWDialog"));
        HWDialog->resize(983, 446);
        plainTextEdit = new QPlainTextEdit(HWDialog);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 10, 541, 421));
        QFont font;
        font.setPointSize(11);
        plainTextEdit->setFont(font);
        closeButton = new QPushButton(HWDialog);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setGeometry(QRect(880, 390, 89, 41));
        saveButton = new QPushButton(HWDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setGeometry(QRect(780, 390, 89, 41));
        groupBox = new QGroupBox(HWDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(560, 6, 411, 91));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 46, 67, 17));
        ppmCurrent = new QLineEdit(groupBox);
        ppmCurrent->setObjectName(QString::fromUtf8("ppmCurrent"));
        ppmCurrent->setGeometry(QRect(80, 31, 113, 51));
        ppmCurrent->setReadOnly(true);
        ppmSpinBox = new QSpinBox(groupBox);
        ppmSpinBox->setObjectName(QString::fromUtf8("ppmSpinBox"));
        ppmSpinBox->setGeometry(QRect(260, 30, 141, 51));
        ppmSpinBox->setAutoFillBackground(true);
        ppmSpinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        ppmSpinBox->setAccelerated(true);
        ppmSpinBox->setMaximum(1000000);
        ppmSpinBox->setSingleStep(10);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(210, 47, 41, 17));
        groupBox_2 = new QGroupBox(HWDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(560, 110, 411, 131));
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 40, 67, 17));
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 90, 41, 17));
        currentDateTime = new QLineEdit(groupBox_2);
        currentDateTime->setObjectName(QString::fromUtf8("currentDateTime"));
        currentDateTime->setGeometry(QRect(80, 30, 321, 41));
        currentDateTime->setReadOnly(true);
        dateEdit = new QDateEdit(groupBox_2);
        dateEdit->setObjectName(QString::fromUtf8("dateEdit"));
        dateEdit->setGeometry(QRect(80, 80, 151, 41));
        dateEdit->setCalendarPopup(true);
        timeEdit = new QTimeEdit(groupBox_2);
        timeEdit->setObjectName(QString::fromUtf8("timeEdit"));
        timeEdit->setGeometry(QRect(240, 80, 161, 41));
        timeEdit->setAccelerated(true);
        timeEdit->setProperty("showGroupSeparator", QVariant(false));
        timeEdit->setCalendarPopup(true);

        retranslateUi(HWDialog);
        QObject::connect(saveButton, SIGNAL(pressed()), HWDialog, SLOT(accept()));
        QObject::connect(closeButton, SIGNAL(pressed()), HWDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(HWDialog);
    } // setupUi

    void retranslateUi(QDialog *HWDialog)
    {
        HWDialog->setWindowTitle(QCoreApplication::translate("HWDialog", "Hardware Interface", nullptr));
        closeButton->setText(QCoreApplication::translate("HWDialog", "Close", nullptr));
        saveButton->setText(QCoreApplication::translate("HWDialog", "Save", nullptr));
        groupBox->setTitle(QCoreApplication::translate("HWDialog", "Pulses Per Mile", nullptr));
        label_4->setText(QCoreApplication::translate("HWDialog", "Current:", nullptr));
        label_5->setText(QCoreApplication::translate("HWDialog", "New:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("HWDialog", "Time && Date", nullptr));
        label_8->setText(QCoreApplication::translate("HWDialog", "Current:", nullptr));
        label_9->setText(QCoreApplication::translate("HWDialog", "New:", nullptr));
        dateEdit->setDisplayFormat(QCoreApplication::translate("HWDialog", "MM/dd/yyyy", nullptr));
        timeEdit->setDisplayFormat(QCoreApplication::translate("HWDialog", "hh:mm:ss AP", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HWDialog: public Ui_HWDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HWDIALOG_H
