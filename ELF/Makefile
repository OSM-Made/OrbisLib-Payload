CC		:=	gcc
AS		:=	gcc
OBJCOPY	:=	objcopy
ODIR	:=	build
SDIR	:=	source
IDIRS	:=	-I$(LIBPS4)/include -I. -Iinclude -I$(SDIR)/FreeBSD/include -I.
LDIRS	:=	-L$(LIBPS4) -L. -Llib
CFLAGS	:=	$(IDIRS) $(LDIRS) -O3 -s -w -std=gnu++11 -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables -nostartfiles -nostdlib -w -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small -mstackrealign -fPIE
LFLAGS	:=	-Xlinker -T Linker -Wl,--build-id=none -mstackrealign -pie
CFILES	:=	$(wildcard $(SDIR)/*.cpp)
SFILES	:=	$(wildcard $(SDIR)/*.s)
UTIL    :=  $(patsubst $(SDIR)/Util/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/Util/*.cpp))
UTILR   :=  $(patsubst $(SDIR)/Util/Resolver/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/Util/Resolver/*.cpp))
OLIB    :=  $(patsubst $(SDIR)/OrbisLib/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/OrbisLib/*.cpp))
OBJS	:=	$(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES)) $(UTIL) $(UTILR) $(OLIB)

TARGET = OrbisLib.elf

$(TARGET): $(ODIR) $(OBJS)
	$(CC) crt0.s $(ODIR)/*.o -o $(TARGET) $(CFLAGS) $(LFLAGS)
	
$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/Util/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/Util/Resolver/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/OrbisLib/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(ODIR)/*.o