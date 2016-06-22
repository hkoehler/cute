#include <qfiledialog.h>

#include "cute.h"
#include "savedlg.h"
#include "cuteview.h"
#include "project.h"

SaveDlg::SaveDlg(QWidget *parent, const char *name, bool saveProject) :
	SaveDialog(parent, name)
{
	CUTE *mainFrm = static_cast<CUTE*>(parent);
	WidgetList list = mainFrm->windows();

	docs->setSelectionMode(QListBox::Multi);
	CUTEView *view;
	int i = 0;
	for(QWidget *iter = list.first(); iter; iter = list.next()){
		view = static_cast<CUTEView*>(iter);
		QFile f(static_cast<CUTEView*>(iter)->fileName());
		if( saveProject && !mainFrm->project->hasFile(f) )
			continue;
		if(view->scintilla()->isModified()){
			if(view->fileName() == QString::null)
				docs->insertItem(view->caption());
			else
				docs->insertItem(view->fileName());
			docs->setSelected(i++, true);
			viewList.append(view);
		}
	}
	setWFlags(WShowModal);
}

void SaveDlg::saveFiles()
{
	QListBoxItem *i = docs->firstItem();
	CUTEView *view = viewList.first();
	for(;;){
		if( i->isSelected() )
			if(view->fileName() == QString::null ){
				QString fileName = QFileDialog::getSaveFileName(QDir::currentDirPath());
				if(fileName == QString::null){
					reject();
					return;
				}
				view->saveFile(fileName);
			}
			else
				view->saveFile();
		i = i->next();
		if(!i)
			break;
		view = viewList.next();
	}
	accept();
}
