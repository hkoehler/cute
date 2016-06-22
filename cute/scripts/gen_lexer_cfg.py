# (C) Copyright 2003, Heiko Köhler
# This cute script generates default lexer config files

from config import *
from cute import *


for l in dir(lang):
	if str(l)[0] is not "_":
		file = open("/home/heiko/cute-0.2.8/cute/langs/"+str(l)+".py", "w")
		file.write("#"+str(l)+" properties\n\n")
		for p in dir(getattr(lang,l)):
			if str(p) is "fileExtensions":
				file.write(str(l)+"."+"fileExtensions = \""+getattr(getattr(lang,l),p)+"\"\n")
				continue
			if str(p) is "streamCommentStart":
				file.write(str(l)+"."+"streamCommentStart = \""+getattr(getattr(lang,l),p)+"\"\n")
				continue
			if str(p) is "streamCommentEnd":
				file.write(str(l)+"."+"streamCommentEnd = \""+getattr(getattr(lang,l),p)+"\"\n")
				continue
			if str(p) is "blockCommentStart":
				file.write(str(l)+"."+"blockCommentStart = \""+getattr(getattr(lang,l),p)+"\"\n")
				continue
			if str(p) is "wordCharacters":
				file.write(str(l)+"."+"wordCharacters = \""+getattr(getattr(lang,l),p)+"\"\n\n")
				continue
		for p in dir(getattr(lang,l)):
			if str(p) is "fileExtensions":
				continue
			if str(p) is "streamCommentStart":
				continue
			if str(p) is "streamCommentEnd":
				continue
			if str(p) is "blockCommentStart":
				continue
			if str(p) is "wordCharacters":
				continue
			if str(p)[0] is not"_":
				prop = getattr(getattr(lang,l),p)
				file.write(str(l)+"."+str(p)+".fore = "+hex(prop.fore)+"\n")
				file.write(str(l)+"."+str(p)+".back = "+hex(prop.back)+"\n")
				file.write(str(l)+"."+str(p)+".font = "+"'"+str(prop.font)+"'"+"\n")
				file.write(str(l)+"."+str(p)+".size = "+str(prop.size)+"\n")
				file.write(str(l)+"."+str(p)+".eolfilled = "+str(prop.eolfilled)+"\n")
				file.write(str(l)+"."+str(p)+".bold = "+str(prop.bold)+"\n")
				file.write(str(l)+"."+str(p)+".underline = "+str(prop.underline)+"\n")
				file.write(str(l)+"."+str(p)+".italic = "+str(prop.italic)+"\n")
				file.write("\n")
		file.close()
