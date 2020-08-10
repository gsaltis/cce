FileUtils.o: FileUtils.c String.h MemoryManager.h
main.o: main.c String.h MemoryManager.h ascii.h ANSIColors.h
MemoryManager.o: MemoryManager.c MemoryManager.h
String.o: String.c String.h MemoryManager.h ascii.h
