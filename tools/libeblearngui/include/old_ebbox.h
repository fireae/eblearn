/********************************************************************************
** Form generated from reading ui file 'ebbox.ui'
**
** Created: Thu May 29 15:12:44 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_EBBOX_H
#define UI_EBBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

class Ui_Ebbox
{
public:	
	QWidget* subcontainer;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayoutcont;
    QPushButton *pushButton_expand;
    QPushButton *pushButton_print;
    QPushButton *pushButton_properties;

    void setupUi(QWidget *Ebbox, const char* title = "title")
    {
    if (Ebbox->objectName().isEmpty())
        Ebbox->setObjectName(QString::fromUtf8(title));
    
	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(3);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    pushButton_expand = new QPushButton();
    pushButton_expand->setObjectName(QString::fromUtf8("pushButton_expand"));
    pushButton_expand->setEnabled(true);
    pushButton_expand->setMinimumSize(QSize(20, 20));
    pushButton_expand->setMaximumSize(QSize(25, 25));
    pushButton_expand->setCheckable(false);

    hboxLayout->addWidget(pushButton_expand);

    pushButton_print = new QPushButton();
    pushButton_print->setObjectName(QString::fromUtf8("pushButton_print"));
    pushButton_print->setMinimumSize(QSize(30, 20));
    pushButton_print->setMaximumSize(QSize(40, 25));

    hboxLayout->addWidget(pushButton_print);

    pushButton_properties = new QPushButton();
    pushButton_properties->setObjectName(QString::fromUtf8("pushButton_properties"));
    pushButton_properties->setMinimumSize(QSize(60, 20));
    pushButton_properties->setMaximumSize(QSize(16777215, 25));

    hboxLayout->addWidget(pushButton_properties);
    
    subcontainer = new QWidget();
    vboxLayoutcont = new QVBoxLayout();
    subcontainer->setLayout(vboxLayoutcont);
    subcontainer->setSizePolicy(policy);
    
    vboxLayout = new QVBoxLayout();
    vboxLayout->addLayout(hboxLayout);
    vboxLayout->addWidget(subcontainer);
    Ebbox->setLayout(vboxLayout);
    

	Ebbox->setSizePolicy(policy);
	
    retranslateUi(Ebbox, title);
    
    QMetaObject::connectSlotsByName(Ebbox);
    } // setupUi

    void retranslateUi(QWidget *Ebbox, const char* title)
    {
    Ebbox->setWindowTitle(QApplication::translate("Ebbox", "Form", 0, QApplication::UnicodeUTF8));
    pushButton_expand->setText(QString());
    pushButton_print->setText(QApplication::translate("Ebbox", "Print", 0, QApplication::UnicodeUTF8));
    pushButton_properties->setText(QApplication::translate("Ebbox", title, 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Ebbox);
    } // retranslateUi

    
};
    
namespace Ui {
    class Ebbox: public Ui_Ebbox {};
} // namespace Ui


#endif // UI_EBBOX_H
