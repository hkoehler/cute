/****************************************************************************
** Form interface generated from reading ui file 'ui/finddlg.ui'
**
** Created: Fre Nov 8 19:14:27 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class FindDialog : public QDialog
{ 
    Q_OBJECT

public:
    FindDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FindDialog();

    QLabel* TextLabel1;
    QLineEdit* stringToFind;
    QCheckBox* regularExpression;
    QCheckBox* caseSensitve;
    QCheckBox* wholeWord;
    QCheckBox* wrapAround;
    QButtonGroup* ButtonGroup1;
    QRadioButton* forward;
    QRadioButton* backward;
    QPushButton* searchButton;
    QPushButton* cancelButton;


public slots:
    virtual void find();

protected:
    QVBoxLayout* FindDialogLayout;
    QVBoxLayout* Layout7;
    QHBoxLayout* Layout9;
    QVBoxLayout* Layout2;
    QVBoxLayout* Layout3;
    QHBoxLayout* ButtonGroup1Layout;
    QHBoxLayout* Layout6;
};

#endif // FINDDIALOG_H
