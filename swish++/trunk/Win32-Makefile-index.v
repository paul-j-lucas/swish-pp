#=======================================================================
#@V@:Note: File automatically generated by VIDE: (09:41:13 08 Sep 2000) (g++).
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
	$(Src)/directory.c\
	$(Src)/elements.c\
	$(Src)/entities.c\
	$(Src)/file_info.c\
	$(Src)/file_list.c\
	$(Src)/FilesGrow.c\
	$(Src)/filter.c\
	$(Src)/html.c\
	$(Src)/index.c\
	$(Src)/index_segment.c\
	$(Src)/itoa.c\
	$(Src)/option_stream.c\
	$(Src)/postscript.c\
	$(Src)/stem_word.c\
	$(Src)/stop_words.c\
	$(Src)/token.c\
	$(Src)/util.c\
	$(Src)/word_util.c\
	$(Src)/word_info.c\
	$(Src)/FilterFile.c\
	$(Src)/IncludeFile.c\
	$(Src)/ExcludeFile.c

EXOBJS	=\
	$(oDir)/bcd.o\
	$(oDir)/mmap_file.o\
	$(oDir)/conf_bool.o\
	$(oDir)/conf_int.o\
	$(oDir)/conf_set.o\
	$(oDir)/conf_string.o\
	$(oDir)/conf_var.o\
	$(oDir)/directory.o\
	$(oDir)/elements.o\
	$(oDir)/entities.o\
	$(oDir)/file_info.o\
	$(oDir)/file_list.o\
	$(oDir)/FilesGrow.o\
	$(oDir)/filter.o\
	$(oDir)/html.o\
	$(oDir)/index.o\
	$(oDir)/index_segment.o\
	$(oDir)/itoa.o\
	$(oDir)/option_stream.o\
	$(oDir)/postscript.o\
	$(oDir)/stem_word.o\
	$(oDir)/stop_words.o\
	$(oDir)/token.o\
	$(oDir)/util.o\
	$(oDir)/word_util.o\
	$(oDir)/word_info.o\
	$(oDir)/FilterFile.o\
	$(oDir)/IncludeFile.o\
	$(oDir)/ExcludeFile.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/index
ALLTGT	=	$(Bin)/index

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

$(Bin)/index: $(EXOBJS)
	$(CC) -o $(Bin)/index $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)

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

$(oDir)/directory.o: directory.c directory.h platform.h RecurseSubdirs.h \
 conf_bool.h conf_var.h fake_ansi.h less.h util.h config.h \
 mmap_file.h Verbosity.h conf_int.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/elements.o: elements.c elements.h my_set.h fake_ansi.h platform.h \
 less.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/entities.o: entities.c entities.h less.h fake_ansi.h platform.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/file_info.o: file_info.c bcd.h config.h fake_ansi.h platform.h \
 file_info.h my_set.h less.h FilesReserve.h conf_int.h conf_var.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/file_list.o: file_list.c bcd.h fake_ansi.h platform.h file_list.h \
 index_segment.h mmap_file.h word_info.h html.h my_set.h less.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/mmap_file.o: mmap_file.c fake_ansi.h platform.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/FilesGrow.o: FilesGrow.c FilesGrow.h config.h conf_int.h conf_var.h \
 fake_ansi.h platform.h less.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/filter.o: filter.c config.h filter.h fake_ansi.h platform.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/html.o: html.c config.h elements.h my_set.h fake_ansi.h platform.h \
 less.h entities.h ExcludeClass.h conf_set.h conf_var.h ExcludeMeta.h \
 html.h mmap_file.h IncludeMeta.h index.h meta_map.h TitleLines.h \
 conf_int.h util.h word_util.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/index.o: index.c bcd.h config.h directory.h platform.h elements.h \
 my_set.h fake_ansi.h less.h ExcludeClass.h conf_set.h conf_var.h \
 ExcludeFile.h pattern_map.h fnmatch.h ExcludeMeta.h exit_codes.h \
 file_info.h file_list.h index_segment.h mmap_file.h word_info.h \
 html.h FilesGrow.h conf_int.h FilesReserve.h FilterFile.h filter.h \
 IncludeFile.h IncludeMeta.h Incremental.h conf_bool.h IndexFile.h \
 conf_string.h index.h itoa.h meta_map.h option_stream.h \
 RecurseSubdirs.h StopWordFile.h stop_words.h TempDirectory.h \
 TitleLines.h util.h Verbosity.h version.h WordFilesMax.h \
 WordPercentMax.h word_util.h do_file.c
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/index_segment.o: index_segment.c fake_ansi.h platform.h \
 index_segment.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/itoa.o: itoa.c fake_ansi.h platform.h util.h config.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/option_stream.o: option_stream.c option_stream.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/postscript.o: postscript.c postscript.h my_set.h fake_ansi.h \
 platform.h less.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/stem_word.o: stem_word.c stem_word.h less.h fake_ansi.h platform.h \
 word_util.h config.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/stop_words.o: stop_words.c config.h exit_codes.h mmap_file.h \
 index_segment.h stop_words.h my_set.h fake_ansi.h platform.h less.h \
 util.h word_util.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/token.o: token.c config.h platform.h token.h fake_ansi.h util.h \
 mmap_file.h word_util.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/util.o: util.c config.h conf_var.h fake_ansi.h platform.h less.h \
 util.h mmap_file.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/word_util.o: word_util.c config.h word_util.h fake_ansi.h platform.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/word_info.o: word_info.c bcd.h word_info.h fake_ansi.h platform.h \
 html.h mmap_file.h my_set.h less.h util.h config.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/FilterFile.o: FilterFile.c exit_codes.h FilterFile.h conf_var.h \
 fake_ansi.h platform.h less.h filter.h pattern_map.h fnmatch.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/IncludeFile.o: IncludeFile.c IncludeFile.h conf_var.h fake_ansi.h \
 platform.h less.h pattern_map.h fnmatch.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/ExcludeFile.o: ExcludeFile.c ExcludeFile.h conf_var.h fake_ansi.h \
 platform.h less.h pattern_map.h fnmatch.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
