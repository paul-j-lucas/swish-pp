#=======================================================================
#@V@:Note: File automatically generated by VIDE: (09:42:02 08 Sep 2000) (g++).
#  This file regenerated each time you run VIDE, so save under a
#    new name if you hand edit, or it will be overwritten.
#=======================================================================

# Standard defines:
CC  	=	g++

WRES	=	windres

HOMEV	=	
VPATH	=	$(HOMEV)/include
oDir	=	.
Bin	=	.
Src	=	.
libDirs	=
incDirs	=
LIBS	=	-liberty
C_FLAGS	=	-O2\
	-DWIN32

SRCS	=\
	$(Src)/bcd.c\
	$(Src)/mmap_file.c\
	$(Src)/conf_bool.c\
	$(Src)/conf_int.c\
	$(Src)/conf_set.c\
	$(Src)/conf_string.c\
	$(Src)/conf_var.c\
	$(Src)/file_list.c\
	$(Src)/search.c\
	$(Src)/stem_word.c\
	$(Src)/option_stream.c\
	$(Src)/file_info.c\
	$(Src)/util.c\
	$(Src)/word_util.c\
	$(Src)/token.c\
	$(Src)/index_segment.c

EXOBJS	=\
	$(oDir)/bcd.o\
	$(oDir)/mmap_file.o\
	$(oDir)/conf_bool.o\
	$(oDir)/conf_int.o\
	$(oDir)/conf_set.o\
	$(oDir)/conf_string.o\
	$(oDir)/conf_var.o\
	$(oDir)/file_list.o\
	$(oDir)/search.o\
	$(oDir)/stem_word.o\
	$(oDir)/option_stream.o\
	$(oDir)/file_info.o\
	$(oDir)/util.o\
	$(oDir)/word_util.o\
	$(oDir)/token.o\
	$(oDir)/index_segment.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/search
ALLTGT	=	$(Bin)/search

# User defines:

#@# Targets follow ---------------------------------

all:	$(ALLTGT)

objs:	$(ALLOBJS)

cleanobjs:
	rm -f $(ALLOBJS)

cleanbin:
	rm -f $(ALLBIN)

clean:	cleanobjs cleanbin

cleanall:	cleanobjs cleanbin

#@# User Targets follow ---------------------------------


#@# Dependency rules follow -----------------------------

$(Bin)/search: $(EXOBJS)
	$(CC) -o $(Bin)/search $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)

$(oDir)/bcd.o: bcd.c fake_ansi.h platform.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/conf_bool.o: conf_bool.c conf_bool.h conf_var.h fake_ansi.h platform.h \
 less.h exit_codes.h util.h config.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/conf_int.o: conf_int.c conf_int.h conf_var.h fake_ansi.h platform.h \
 less.h exit_codes.h util.h config.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/conf_set.o: conf_set.c conf_set.h conf_var.h fake_ansi.h platform.h \
 less.h my_set.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/conf_string.o: conf_string.c conf_string.h conf_var.h fake_ansi.h \
 platform.h less.h exit_codes.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/conf_var.o: conf_var.c config.h conf_var.h fake_ansi.h platform.h \
 less.h exit_codes.h mmap_file.h util.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/mmap_file.o: mmap_file.c fake_ansi.h platform.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/file_list.o: file_list.c bcd.h fake_ansi.h platform.h file_list.h \
 index_segment.h mmap_file.h word_info.h html.h my_set.h less.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/search.o: search.c bcd.h config.h exit_codes.h file_info.h my_set.h \
 fake_ansi.h platform.h less.h file_list.h index_segment.h \
 mmap_file.h word_info.h html.h IndexFile.h conf_string.h conf_var.h \
 option_stream.h ResultsMax.h conf_int.h search.h stem_word.h \
 StemWords.h conf_bool.h token.h util.h version.h WordFilesMax.h \
 WordPercentMax.h word_util.h search_options.c
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/stem_word.o: stem_word.c stem_word.h less.h fake_ansi.h platform.h \
 word_util.h config.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/option_stream.o: option_stream.c option_stream.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/file_info.o: file_info.c bcd.h config.h fake_ansi.h platform.h \
 file_info.h my_set.h less.h FilesReserve.h conf_int.h conf_var.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/util.o: util.c config.h conf_var.h fake_ansi.h platform.h less.h \
 util.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/word_util.o: word_util.c config.h word_util.h fake_ansi.h platform.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/token.o: token.c config.h platform.h token.h fake_ansi.h util.h \
 mmap_file.h word_util.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/index_segment.o: index_segment.c fake_ansi.h platform.h \
 index_segment.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
