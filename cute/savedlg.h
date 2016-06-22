#include <qlistbox.h>
#include "ui/savedialog.h"
#include "cuteview.h"

/** This dialog is shown when CUTE quits. It shows all unsaved files */
class SaveDlg : public SaveDialog
{
public:
	SaveDlg(QWidget *parent=0, const char *name=0, bool saveProject=false);
	/** saves all selected files */
	void saveFiles();
protected:
	QList<CUTEView> viewList;
};
