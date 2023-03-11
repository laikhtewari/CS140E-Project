COMMON_SRC := magstripe.c
PROGS := test-sender.c
CFLAGS_EXTRA  = -Iexternal-code

STAFF_OBJS += $(CS140E_2023_PATH)/libpi/staff-objs/kmalloc.o

BOOTLOADER = my-install
RUN = 1

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

clean::
	rm -f tests/*.list tests/*.o tests/*.elf tests/*~ tests/*.bin
	make -C external-code clean
