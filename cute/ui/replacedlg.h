/****************************************************************************
** Form interface generated from reading ui file 'ui/replacedlg.ui'
**
** Created: Fre Nov 8 19:14:27 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

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

class ReplaceDialog : public QDialog
{ 
    Q_OBJECT

public:
    ReplaceDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ReplaceDialog();

    QLabel* TextLabel1;
    QLineEdit* stringToFind;
    QLabel* TextLabel1_2;
    QLineEdit* replaceWith;
    QCheckBox* regularExpression;
    QCheckBox* caseSensitve;
    QCheckBox* wholeWord;
    QCheckBox* wrapAround;
    QButtonGroup* ButtonGroup1;
    QRadioButton* forward;
    QRadioButton* backward;
    QPushButton* replaceButton;
    QPushButton* replaceInSelectionButton;
    QPushButton* replaceAllButton;
    QPushButton* cancelButton;


public slots:
    virtual void replaceInSelection();
    virtual void replace();
    virtual void replaceAll();

protected:
    QVBoxLayout* ReplaceDialogLayout;
    QVBoxLayout* Layout7;
    QVBoxLayout* Layout7_2;
    QHBoxLayout* Layout9;
    QVBoxLayout* Layout2;
    QVBoxLayout* Layout3;
    QHBoxLayout* ButtonGroup1Layout;
    QHBoxLayout* Layout17;
};

#endif // REPLACEDIALOG_H
