KVERS = $(shell uname -r )
obj-m += mcp2515.o

# EXTRA_CFLAGS=-g -O0
build: kernel_modules

# make 编译
kernel_modules: 
	make -C /lib/modules/${KVERS}/build M=${CURDIR} modules

# make clean 清除
clean:
	make -C /lib/modules/${KVERS}/build M=${CURDIR} clean 

# make print-info 打印调试信息
print-info:
	@echo $(KVERS)
	@echo ${CURDIR}