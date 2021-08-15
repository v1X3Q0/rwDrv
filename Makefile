obj-m+=mychar.o


ifeq ($(ANDROID), )
BUILD_PATH=$(KERNEL_PATH)/lib/modules/$(shell uname -r)/build/
else
ifndef KERNEL_PATH
$(error need definition for KERNEL_PATH)
endif
BUILD_PATH=$(KERNEL_PATH)/out/android-msm-pixel-4.9/private/msm-google/
endif

ifeq ($(TARGET2), )
	TARGET2 := $(TARGET)gcc
endif

all:
	make -C $(BUILD_PATH) M=$(PWD) modules
	$(TARGET2) testebbchar.c ebbHelper.c -o test --static -g
clean:
	make -C $(BUILD_PATH) M=$(PWD) clean
	rm test
