#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
DEBUG ?= 0
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# Project configuration
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/rooms
DATA		:=	data
INCLUDES	:=	include source source/rooms

ASSETS		:=	resources
GRAPHICS	:=	$(ASSETS)/gfx
TIMELINES	:=	$(ASSETS)/timelines
INVENTORY	:=	$(ASSETS)/inventory
ROMFS		:=	romfs

# These directories are copied as-is from resources/ to romfs/.
RAW_ASSET_DIRS	:=	audio lang states

# Graphics that must be linked directly into the executable.
# Everything else under GRAPHICS will be placed in RomFS.
MEMGFX		:=	$(GRAPHICS)/hud/gfx_hud.t3s

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	:=	-g -Wall -O2 -mword-relocations \
			-ffunction-sections \
			$(ARCH)

ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG
endif
CFLAGS	+=	$(INCLUDE) -D__3DS__

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lcitro2d -lcitro3d -lctru -lm -lvorbisidec -logg

#---------------------------------------------------------------------------------
# Libraries
#---------------------------------------------------------------------------------
LIBDIRS := $(CTRULIB) $(DEVKITPRO)/portlibs/3ds

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add
# additional rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# Source files
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# Graphics
#---------------------------------------------------------------------------------

# Find every .t3s recursively under resources/gfx.
GFXFILES	:=	$(shell if [ -d "$(GRAPHICS)" ]; then find "$(GRAPHICS)" -type f -name '*.t3s'; fi)

# Everything except MEMGFX goes to RomFS.
ROMGFX		:=	$(filter-out $(MEMGFX),$(GFXFILES))

# Directories containing .t3s files, used by VPATH.
GFXDIRS		:=	$(sort $(dir $(GFXFILES)))

# Graphics linked into the executable.
MEM_T3XFILES	:=	$(notdir $(MEMGFX:.t3s=.t3x))
MEM_HFILES	:=	$(notdir $(MEMGFX:.t3s=.h))

# Graphics generated into RomFS.
ROM_T3XFILES	:=	$(foreach file,$(ROMGFX),$(ROMFS)/gfx/$(notdir $(file:.t3s=.t3x)))

ROM_HFILES	:=	$(foreach file,$(ROMGFX),$(BUILD)/$(notdir $(file:.t3s=.h)))

T3XHFILES	:=	$(MEM_HFILES:%=$(BUILD)/%) $(ROM_HFILES)

#---------------------------------------------------------------------------------
# Timelines
#---------------------------------------------------------------------------------

TIMELINE_GFX	:=	$(wildcard $(TIMELINES)/*/gfx.t3s)
TIMELINE_DIRS	:=	$(sort $(dir $(TIMELINE_GFX)))

TIMELINE_FILES := $(foreach dir,$(TIMELINE_DIRS),\
	$(ROMFS)/timelines/$(notdir $(patsubst %/,%,$(dir)))/gfx.t3x \
	$(ROMFS)/timelines/$(notdir $(patsubst %/,%,$(dir)))/gfx.h \
	$(ROMFS)/timelines/$(notdir $(patsubst %/,%,$(dir)))/timeline)

#---------------------------------------------------------------------------------
# Inventory
#---------------------------------------------------------------------------------

INVENTORY_FILES := \
	$(ROMFS)/inventory/inventory \
	$(ROMFS)/inventory/gfx.t3x \
	$(ROMFS)/inventory/gfx.h

#---------------------------------------------------------------------------------
# Assets copied directly to RomFS
#---------------------------------------------------------------------------------

RAW_ASSET_FILES := $(foreach dir,$(RAW_ASSET_DIRS),\
	$(shell if [ -d "$(ASSETS)/$(dir)" ]; then find "$(ASSETS)/$(dir)" -type f; fi))

ROMFS_RAW_FILES := $(patsubst $(ASSETS)/%,$(ROMFS)/%,$(RAW_ASSET_FILES))

#---------------------------------------------------------------------------------
# Search paths
#---------------------------------------------------------------------------------
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
				$(foreach dir,$(GFXDIRS),$(CURDIR)/$(dir)) \
				$(foreach dir,$(DATA),$(CURDIR)/$(dir))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

#---------------------------------------------------------------------------------
# Object files
#---------------------------------------------------------------------------------
export OFILES_SOURCES :=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

# IMPORTANT:
# Only MEM_T3XFILES is converted with bin2o and linked into the executable.
# RomFS graphics are NOT included here.
export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
				$(PICAFILES:.v.pica=.shbin.o) \
				$(SHLISTFILES:.shlist=.shbin.o) \
				$(addsuffix .o,$(MEM_T3XFILES))

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

#---------------------------------------------------------------------------------
# Generated headers
#---------------------------------------------------------------------------------
export HFILES := $(PICAFILES:.v.pica=_shbin.h) \
			 $(SHLISTFILES:.shlist=_shbin.h) \
			 $(addsuffix .h,$(subst .,_,$(BINFILES))) \
			 $(notdir $(T3XHFILES))

#---------------------------------------------------------------------------------
# Include / library paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
				$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
				-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(if $(NO_SMDH),,$(OUTPUT).smdh)

#---------------------------------------------------------------------------------
# Icon
#---------------------------------------------------------------------------------
ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

#---------------------------------------------------------------------------------
# SMDH
#---------------------------------------------------------------------------------
ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
endif

#---------------------------------------------------------------------------------
# RomFS
#---------------------------------------------------------------------------------
ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: all clean lint

#---------------------------------------------------------------------------------
# Main outer target
#---------------------------------------------------------------------------------
all: $(BUILD) \
	 $(MEM_T3XFILES:%=$(BUILD)/%) \
	 $(ROM_T3XFILES) \
	 $(T3XHFILES) \
	 $(TIMELINE_FILES) \
	 $(INVENTORY_FILES) \
	 $(ROMFS_RAW_FILES)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
# Directories
#---------------------------------------------------------------------------------
$(BUILD):
	@mkdir -p $@

#---------------------------------------------------------------------------------
# Clean
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -rf $(BUILD) $(ROMFS)
	@rm -f $(TARGET).3dsx $(OUTPUT).smdh $(TARGET).elf PRN

#---------------------------------------------------------------------------------
# Lint
#---------------------------------------------------------------------------------

lint:
	@$(MAKE) CFLAGS="$(CFLAGS) -fanalyzer"

#---------------------------------------------------------------------------------
# Assets copied directly to RomFS
#---------------------------------------------------------------------------------
$(ROMFS)/%: $(ASSETS)/%
	@mkdir -p $(dir $@)
	@cp $< $@

#---------------------------------------------------------------------------------
# Graphics linked directly into the executable.
# MEMGFX currently contains gfx_hud.t3s.
#---------------------------------------------------------------------------------
$(MEM_T3XFILES:%=$(BUILD)/%) $(MEM_HFILES:%=$(BUILD)/%) &: $(MEMGFX)
	@echo $(notdir $<)
	@mkdir -p $(BUILD)
	@tex3ds -i $< \
		-H $(BUILD)/$(patsubst %.t3s,%.h,$(notdir $<)) \
		-d $(DEPSDIR)/$(patsubst %.t3s,%.d,$(notdir $<)) \
		-o $(BUILD)/$(patsubst %.t3s,%.t3x,$(notdir $<))

#---------------------------------------------------------------------------------
# Timelines
#---------------------------------------------------------------------------------
$(ROMFS)/timelines/%/gfx.t3x $(ROMFS)/timelines/%/gfx.h &: $(TIMELINES)/%/gfx.t3s
	@echo $(notdir $<)
	@mkdir -p $(ROMFS)/timelines/$*
	@tex3ds -i $< \
		-H $(ROMFS)/timelines/$*/gfx.h \
		-d $(DEPSDIR)/gfx_timeline_$*.d \
		-o $(ROMFS)/timelines/$*/gfx.t3x

$(ROMFS)/timelines/%/timeline: $(TIMELINES)/%/timeline
	@mkdir -p $(ROMFS)/timelines/$*
	@cp $< $@

#---------------------------------------------------------------------------------
# Inventory
#---------------------------------------------------------------------------------
$(ROMFS)/inventory/gfx.t3x $(ROMFS)/inventory/gfx.h &: $(INVENTORY)/gfx.t3s
	@echo $(notdir $<)
	@mkdir -p $(ROMFS)/inventory
	@tex3ds -i $< \
		-H $(ROMFS)/inventory/gfx.h \
		-d $(DEPSDIR)/gfx_inventory.d \
		-o $(ROMFS)/inventory/gfx.t3x

$(ROMFS)/inventory/inventory: $(INVENTORY)/inventory
	@mkdir -p $(ROMFS)/inventory
	@cp $< $@

#---------------------------------------------------------------------------------
# All other graphics:
# Build the .t3x into RomFS.
# The generated header remains in BUILD.
#---------------------------------------------------------------------------------
$(ROMFS)/gfx/%.t3x $(BUILD)/%.h &: %.t3s
	@echo $(notdir $<)
	@mkdir -p $(ROMFS)/gfx $(BUILD)
	@tex3ds -i $< \
		-H $(BUILD)/$*.h \
		-d $(DEPSDIR)/$*.d \
		-o $(ROMFS)/gfx/$*.t3x

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
# Inner make
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(_3DSXDEPS)

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# Binary data
#---------------------------------------------------------------------------------
%.bin.o %_bin.h : %.bin
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# Only .t3x files present in BUILD are converted with bin2o.
#---------------------------------------------------------------------------------
.PRECIOUS	:	%.t3x %.shbin

%.t3x.o %_t3x.h : %.t3x
	$(SILENTMSG) $(notdir $<)
	$(bin2o)

#---------------------------------------------------------------------------------
# Shaders
#---------------------------------------------------------------------------------
%.shbin.o %_shbin.h : %.shbin
	$(SILENTMSG) $(notdir $<)
	$(bin2o)

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
