/****************************************************************************
** Form implementation generated from reading ui file 'ui/replacedlg.ui'
**
** Created: Fre Nov 8 19:14:29 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "replacedlg.h"

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
 *  Constructs a ReplaceDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ReplaceDialog::ReplaceDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ReplaceDialog" );
    resize( 515, 264 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "Replace" ) );
    ReplaceDialogLayout = new QVBoxLayout( this, 11, 6, "ReplaceDialogLayout"); 

    Layout7 = new QVBoxLayout( 0, 0, 6, "Layout7"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "String to find:" ) );
    Layout7->addWidget( TextLabel1 );

    stringToFind = new QLineEdit( this, "stringToFind" );
    stringToFind->setCursor( QCursor( 0 ) );
    stringToFind->setFocusPolicy( QLineEdit::StrongFocus );
    Layout7->addWidget( stringToFind );
    ReplaceDialogLayout->addLayout( Layout7 );

    Layout7_2 = new QVBoxLayout( 0, 0, 6, "Layout7_2"); 

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel1_2->sizePolicy().hasHeightForWidth() ) );
    TextLabel1_2->setText( trUtf8( "Replace with:" ) );
    Layout7_2->addWidget( TextLabel1_2 );

    replaceWith = new QLineEdit( this, "replaceWith" );
    replaceWith->setCursor( QCursor( 0 ) );
    replaceWith->setFocusPolicy( QLineEdit::StrongFocus );
    Layout7_2->addWidget( replaceWith );
    ReplaceDialogLayout->addLayout( Layout7_2 );

    Layout9 = new QHBoxLayout( 0, 0, 6, "Layout9"); 

    Layout2 = new QVBoxLayout( 0, 0, 6, "Layout2"); 

    regularExpression = new QCheckBox( this, "regularExpression" );
    regularExpression->setText( trUtf8( "Regular expression" ) );
    Layout2->addWidget( regularExpression );

    caseSensitve = new QCheckBox( this, "caseSensitve" );
    caseSensitve->setText( trUtf8( "Case sensitive" ) );
    Layout2->addWidget( caseSensitve );
    Layout9->addLayout( Layout2 );
    QSpacerItem* spacer = new QSpacerItem( 162, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
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
    ReplaceDialogLayout->addLayout( Layout9 );

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
    QSpacerItem* spacer_3 = new QSpacerItem( 319, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    ButtonGroup1Layout->addItem( spacer_3 );

    backward = new QRadioButton( ButtonGroup1, "backward" );
    backward->setText( trUtf8( "Backward" ) );
    ButtonGroup1Layout->addWidget( backward );
    ReplaceDialogLayout->addWidget( ButtonGroup1 );

    Layout17 = new QHBoxLayout( 0, 0, 28, "Layout17"); 

    replaceButton = new QPushButton( this, "replaceButton" );
    replaceButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, replaceButton->sizePolicy().hasHeightForWidth() ) );
    replaceButton->setCursor( QCursor( 0 ) );
    replaceButton->setText( trUtf8( "Replace" ) );
    replaceButton->setAccel( 0 );
    Layout17->addWidget( replaceButton );

    replaceInSelectionButton = new QPushButton( this, "replaceInSelectionButton" );
    replaceInSelectionButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, replaceInSelectionButton->sizePolicy().hasHeightForWidth() ) );
    replaceInSelectionButton->setText( trUtf8( "Replace in Selection" ) );
    Layout17->addWidget( replaceInSelectionButton );

    replaceAllButton = new QPushButton( this, "replaceAllButton" );
    replaceAllButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, replaceAllButton->sizePolicy().hasHeightForWidth() ) );
    replaceAllButton->setText( trUtf8( "Replace All" ) );
    Layout17->addWidget( replaceAllButton );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, cancelButton->sizePolicy().hasHeightForWidth() ) );
    cancelButton->setText( trUtf8( "Cancel" ) );
    cancelButton->setAccel( 0 );
    Layout17->addWidget( cancelButton );
    ReplaceDialogLayout->addLayout( Layout17 );

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( replaceButton, SIGNAL( clicked() ), this, SLOT( replace() ) );
    connect( replaceInSelectionButton, SIGNAL( clicked() ), this, SLOT( replaceInSelection() ) );
    connect( replaceAllButton, SIGNAL( clicked() ), this, SLOT( replaceAll() ) );

    // tab order
    setTabOrder( stringToFind, replaceWith );
    setTabOrder( replaceWith, regularExpression );
    setTabOrder( regularExpression, caseSensitve );
    setTabOrder( caseSensitve, wholeWord );
    setTabOrder( wholeWord, wrapAround );
    setTabOrder( wrapAround, forward );
    setTabOrder( forward, backward );
    setTabOrder( backward, replaceButton );
    setTabOrder( replaceButton, replaceInSelectionButton );
    setTabOrder( replaceInSelectionButton, replaceAllButton );
    setTabOrder( replaceAllButton, cancelButton );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ReplaceDialog::~ReplaceDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void ReplaceDialog::replaceInSelection()
{
    qWarning( "ReplaceDialog::replaceInSelection(): Not implemented yet!" );
}

void ReplaceDialog::replace()
{
    qWarning( "ReplaceDialog::replace(): Not implemented yet!" );
}

void ReplaceDialog::replaceAll()
{
    qWarning( "ReplaceDialog::replaceAll(): Not implemented yet!" );
}

