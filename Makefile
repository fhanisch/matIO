EXEC:
	cl /nologo /W3 matio.c

LIB:
	cl /nologo /W3 /c /DLIB matio.c
	lib /nologo /out:matio.lib matio.obj
	