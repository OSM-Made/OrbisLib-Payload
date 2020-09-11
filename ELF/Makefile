CC		:=	g++
AS		:=	g++
OBJCOPY	:=	objcopy
ODIR	:=	build
SDIR	:=	source
IDIRS	:=	-Iinclude -I$(FREEBSD_INLUDES) -I, -Iinclude -I$(SDIR)/Util -I, -Iinclude -I$(SDIR)/Util/Resolver -I
CFLAGS	:=	$(IDIRS) -O3 -s -w -std=gnu++11 -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables -nostartfiles -nostdlib -w -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small -mstackrealign -fPIE
LFLAGS	:=	-Xlinker -T ./Linker -Wl,--build-id=none -mstackrealign -pie
CFILES	:=	$(wildcard $(SDIR)/*.cpp)
SFILES	:=	$(wildcard $(SDIR)/*.s)
UTIL    :=  $(patsubst $(SDIR)/Util/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/Util/*.cpp))
UTILR   :=  $(patsubst $(SDIR)/Util/Resolver/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/Util/Resolver/*.cpp))
UTILH	:=  $(patsubst $(SDIR)/Util/Helpers/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/Util/Helpers/*.cpp))
OLIB    :=  $(patsubst $(SDIR)/OrbisLib/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/OrbisLib/*.cpp))
OBJS	:=	$(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES)) $(UTIL) $(UTILR) $(UTILH) $(OLIB)

TARGET = OrbisLib.elf

$(TARGET): $(ODIR) $(OBJS)
	$(CC) crt0.s $(ODIR)/*.o -o $(TARGET) $(CFLAGS) $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/Util/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/Util/Resolver/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/Util/Helpers/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/OrbisLib/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.s
	$(AS) -c -o $@ $< $(SFLAGS)

$(ODIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(ODIR)/*.o