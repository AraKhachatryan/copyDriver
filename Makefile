BINARY     := copy
KERNEL      := /lib/modules/$(shell uname -r)/build
ARCH        := x86
C_FLAGS     := -Wall
KMOD_DIR    := $(shell pwd)
TARGET_PATH := /lib/modules/$(shell uname -r)/kernel/drivers/char

OBJECTS := main.o

ccflags-y += $(C_FLAGS)

obj-m += $(BINARY).o

$(BINARY)-y := $(OBJECTS)

$(BINARY).ko:
	make -C $(KERNEL) M=$(KMOD_DIR) modules

sign:
	openssl req -new -x509 -newkey rsa:2048 -keyout MOK.priv -outform DER -out MOK.der -nodes -days 36500 -subj "/CN=copyDriver/"
	sudo /usr/src/linux-headers-$(shell uname -r)/scripts/sign-file sha256 ./MOK.priv ./MOK.der $(shell modinfo -n copy)
	modinfo -n copy
	sudo mokutil --import MOK.der

install:
	sudo cp $(BINARY).ko $(TARGET_PATH)
	sudo depmod -a

load:
	sudo insmod copy.ko
	major="$(grep "copy" /proc/devices | cut -d ' ' -f 1)"
	sudo mknod -m 0666 /dev/copy c "$major" 0

unload:
	sudo rmmod ./$(BINARY).ko

uninstall:
	sudo rm $(TARGET_PATH)/$(BINARY).ko
	sudo depmod -a

clean:
	make -C $(KERNEL) M=$(KMOD_DIR) clean
