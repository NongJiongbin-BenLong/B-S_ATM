/********************************************************************************
** Form generated from reading UI file 'newopendialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWOPENDIALOG_H
#define UI_NEWOPENDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewopenDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *UseridEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *PasswordEdit;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewopenDialog)
    {
        if (NewopenDialog->objectName().isEmpty())
            NewopenDialog->setObjectName(QString::fromUtf8("NewopenDialog"));
        NewopenDialog->resize(384, 241);
        verticalLayout = new QVBoxLayout(NewopenDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(NewopenDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        UseridEdit = new QLineEdit(NewopenDialog);
        UseridEdit->setObjectName(QString::fromUtf8("UseridEdit"));

        horizontalLayout->addWidget(UseridEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(NewopenDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        PasswordEdit = new QLineEdit(NewopenDialog);
        PasswordEdit->setObjectName(QString::fromUtf8("PasswordEdit"));

        horizontalLayout_2->addWidget(PasswordEdit);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(NewopenDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewopenDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewopenDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewopenDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewopenDialog);
    } // setupUi

    void retranslateUi(QDialog *NewopenDialog)
    {
        NewopenDialog->setWindowTitle(QApplication::translate("NewopenDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("NewopenDialog", "\350\264\246\345\217\267\357\274\232", nullptr));
        label_2->setText(QApplication::translate("NewopenDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewopenDialog: public Ui_NewopenDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWOPENDIALOG_H
