# Animate build.
#
# The host make starts one container; make inside the container performs the
# complete build with xcc, xas, and cpmdisk.

IMAGE		=	wischner/xcc-z80-idp:latest

ifeq ($(IN_CONTAINER),)

CONTAINER_WORKDIR	=	/work
DOCKER_RUN		=	docker run --rm \
					--user "$(shell id -u):$(shell id -g)" \
					-v "$(CURDIR):$(CONTAINER_WORKDIR)" \
					-w $(CONTAINER_WORKDIR) \
					$(IMAGE)

.PHONY: all clean
all clean:
	$(DOCKER_RUN) make --no-print-directory IN_CONTAINER=1 $@

else

BUILD_DIR	=	build
BIN_DIR		=	bin
INC_DIR		=	src
DATA_DIR	=	data
PLATFORM	=	cpm3
CFLAGS		=	-Os
TARGET		=	animate
DISK_TYPE	=	fdd
FLOPPY		=	$(BIN_DIR)/fddb.img

vpath %.c src
vpath %.s src

C_SRCS		=	$(wildcard src/*.c)
S_SRCS		=	$(wildcard src/*.s)
OBJS		=	$(addprefix $(BUILD_DIR)/,$(notdir $(C_SRCS:.c=.rel)) $(notdir $(S_SRCS:.s=.rel)))
ANIMATIONS	=	$(wildcard $(DATA_DIR)/animations/*.A)

.PHONY: all
all: $(FLOPPY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/%.rel: %.c | $(BUILD_DIR)
	xcc --platform $(PLATFORM) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

$(BUILD_DIR)/%.rel: %.s | $(BUILD_DIR)
	xas -I$(INC_DIR) -o $@ $<

$(BIN_DIR)/$(TARGET).com: $(OBJS) | $(BIN_DIR)
	xcc --platform $(PLATFORM) --oformat=binary -lsdk -lugpx -o $@ $(OBJS)

$(FLOPPY): $(BIN_DIR)/$(TARGET).com $(ANIMATIONS)
	rm -f $(FLOPPY)
	cpmdisk create $(FLOPPY) $(DISK_TYPE)
	cpmdisk add $(FLOPPY) -u 0 $(BIN_DIR)/$(TARGET).com $(ANIMATIONS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

endif
