def okayClicked():
    if (checkBox.isChecked()):
        print "checked"
    else:
        print "not checked"
    dialog.close()

def cancelClicked():
    dialog.close()

dialog = newDialog(1)
dialog.setMaximumSize(800, 600)
dialog.setMinimumSize(300, 200)

layout = newGridLayout(dialog, 2, 3, 11, 6)

label = newLabel(dialog, "Edit box:")
textEdit = newTextEdit(dialog, "Hello, World!")
checkBox = newCheckBox(dialog, "check here")
spacer = newSpacerItem(1, 1, 7, 1);
okay = newPushButton(dialog, "Okay");
cancel = newPushButton(dialog, "Cancel");

layout.addMultiCellWidget(label, 0, 0, 0, 3)
layout.addMultiCellWidget(textEdit, 1, 1, 0, 3)
layout.addWidget(checkBox, 2, 0)
layout.addItem(spacer, 2, 1)
layout.addWidget(okay, 2, 2)
layout.addWidget(cancel, 2, 3)
layout.addWidget(checkBox, 2, 0)

okay.hook(okayClicked, "clicked")
cancel.hook(cancelClicked, "clicked")

dialog.show()
