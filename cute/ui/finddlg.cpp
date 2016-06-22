/****************************************************************************
** Form implementation generated from reading ui file 'ui/finddlg.ui'
**
** Created: Fre Nov 8 19:14:27 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "finddlg.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a FindDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FindDialog::FindDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "FindDialog" );
    resize( 401, 213 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "Find" ) );
    FindDialogLayout = new QVBoxLayout( this, 11, 6, "FindDialogLayout"); 

    Layout7 = new QVBoxLayout( 0, 0, 6, "Layout7"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "String to find:" ) );
    Layout7->addWidget( TextLabel1 );

    stringToFind = new QLineEdit( this, "stringToFind" );
    stringToFind->setCursor( QCursor( 0 ) );
    stringToFind->setFocusPolicy( QLineEdit::StrongFocus );
    Layout7->addWidget( stringToFind );
    FindDialogLayout->addLayout( Layout7 );

    Layout9 = new QHBoxLayout( 0, 0, 6, "Layout9"); 

    Layout2 = new QVBoxLayout( 0, 0, 6, "Layout2"); 

    regularExpression = new QCheckBox( this, "regularExpression" );
    regularExpression->setText( trUtf8( "Regular expression" ) );
    Layout2->addWidget( regularExpression );

    caseSensitve = new QCheckBox( this, "caseSensitve" );
    caseSensitve->setText( trUtf8( "Case sensitive" ) );
    Layout2->addWidget( caseSensitve );
    Layout9->addLayout( Layout2 );
    QSpacerItem* spacer = new QSpacerItem( 41, 41, QSizePolicy::Fixed, QSizePolicy::Minimum );
    Layout9->addItem( spacer );

    Layout3 = new QVBoxLayout( 0, 0, 6, "Layout3"); 

    wholeWord = new QCheckBox( this, "wholeWord" );
    wholeWord->setText( trUtf8( "Whole word" ) );
    Layout3->addWidget( wholeWord );

    wrapAround = new QCheckBox( this, "wrapAround" );
    wrapAround->setText( trUtf8( "Wrap around" ) );
    wrapAround->setAccel( 8388608 );
    wrapAround->setChecked( TRUE );
    Layout3->addWidget( wrapAround );
    Layout9->addLayout( Layout3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout9->addItem( spacer_2 );
    FindDialogLayout->addLayout( Layout9 );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, ButtonGroup1->sizePolicy().hasHeightForWidth() ) );
    ButtonGroup1->setTitle( trUtf8( "Search direction:" ) );
    ButtonGroup1->setColumnLayout(0, Qt::Vertical );
    ButtonGroup1->layout()->setSpacing( 6 );
    ButtonGroup1->layout()->setMargin( 11 );
    ButtonGroup1Layout = new QHBoxLayout( ButtonGroup1->layout() );
    ButtonGroup1Layout->setAlignment( Qt::AlignTop );

    forward = new QRadioButton( ButtonGroup1, "forward" );
    forward->setText( trUtf8( "Forward" ) );
    forward->setChecked( TRUE );
    ButtonGroup1Layout->addWidget( forward );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ButtonGroup1Layout->addItem( spacer_3 );

    backward = new QRadioButton( ButtonGroup1, "backward" );
    backward->setText( trUtf8( "Backward" ) );
    ButtonGroup1Layout->addWidget( backward );
    FindDialogLayout->addWidget( ButtonGroup1 );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6"); 

    searchButton = new QPushButton( this, "searchButton" );
    searchButton->setCursor( QCursor( 0 ) );
    searchButton->setText( trUtf8( "Search" ) );
    searchButton->setAccel( 0 );
    Layout6->addWidget( searchButton );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout6->addItem( spacer_4 );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setText( trUtf8( "Cancel" ) );
    cancelButton->setAccel( 0 );
    Layout6->addWidget( cancelButton );
    FindDialogLayout->addLayout( Layout6 );

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( searchButton, SIGNAL( clicked() ), this, SLOT( find() ) );
    connect( stringToFind, SIGNAL( returnPressed() ), this, SLOT( find() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FindDialog::~FindDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void FindDialog::find()
{
    qWarning( "FindDialog::find(): Not implemented yet!" );
}

