# (C) Copyright 2003, Heiko Köhler
# counts all C/C++ lines in a dir without comments or empty lines

import os
import string
import dircache
import re
from cute import *

lines = 0
inComment = 0
total = 0

if question("Count in current dir?"):
	dir = os.getcwd()
else:
	dir = getExistingDirectory()

clearOutput()
re_cpp = re.compile(r".*\.cpp$")
re_c = re.compile(r".*\.c$")
re_h = re.compile(r".*\.h$")
for file in dircache.listdir(dir):
	if re_cpp.match(file,1):
		f=open(file)
	elif re_c.match(file,1):
		f=open(file)
	elif re_h.match(file,1):
		f=open(file)
	else:
		continue
	while 1:
		line = f.readline()
		if not line:
			break
		line = line.strip()
		if not line:
			continue
		if inComment == 1:
			if line.count("*/"):
				if inComment:
					inComment = 0
			continue
		if string.find(line, "//") == 0:
			continue
		if string.find(line, "/*") == 0:
			inComment = 1
			if line.count("*/"):
				inComment = 0
			continue
		lines += 1
	output( "%s:%d" % ( file, lines ))
	total += lines
	lines = 0

output("There are %d lines" % (total,))
message("There are %d lines" % (total,))
