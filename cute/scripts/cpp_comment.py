# comments selected text in C++ style
sel = selection()
if sel.columnTo == 0:
	t = 0
else:
	t = 1
beginUndoAction()
for line in range(sel.lineFrom, sel.lineTo+t):
	insert("//", line, 0)
endUndoAction()
del sel
del t
