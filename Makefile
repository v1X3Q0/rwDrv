obj-m+=mychar.o


ifeq ($(ANDROID), )
BUILD_PATH=$(KERNEL_PATH)/lib/modules/$(shell uname -r)/build/
else
ifndef KERNEL_PATH
$(error need definition for KERNEL_PATH)
endif
ifeq ($(KERNEL_VERS), )
$(error need definition for KERNEL_VERS)
else
BUILD_PATH=$(KERNEL_PATH)/out/android-msm-pixel-$(KERNEL_VERS)/private/msm-google/
endif
endif

ifeq ($(TARGET2), )
	TARGET2 := $(TARGET)gcc
endif

all:
	make -C $(BUILD_PATH) M=$(PWD) modules
	$(TARGET2)++ testebbchar.cpp ebbHelper.cpp -o test --static -g
clean:
	make -C $(BUILD_PATH) M=$(PWD) clean
	rm test
