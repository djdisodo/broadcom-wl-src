#
# Copyright (C) 2009, Broadcom Corporation
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# $Id: wps_enr_lib.mk,v 1.11 2010/03/17 03:27:48 Exp $
#
# Linux Makefile
#

BLDTYPE = release
#BLDTYPE = debug

CC = gcc
export $(CC)

ifeq ($(CC), arm-linux-gcc)
CFLAGS += -mstructure-size-boundary=8
#LD = arm-linux-ld
LD = arm-linux-gcc
endif

ifeq ($(CC), mipsel-uclibc-gcc)
#LD = mipsel-uclibc-ld
LD = mipsel-uclibc-gcc
endif


ifeq ($(BLDTYPE),debug)
export CFLAGS = -Wall -Wnested-externs -g -D_TUDEBUGTRACE -DWPS_WIRELESS_ENROLLEE
export CXXFLAGS = -Wall -Wnested-externs -g -D_TUDEBUGTRACE -DWPS_WIRELESS_ENROLLEE
else
export CFLAGS = -Os -Wall -Wnested-externs -DWPS_WIRELESS_ENROLLEE
export CXXFLAGS = -Os -Wall -Wnested-externs -DWPS_WIRELESS_ENROLLEE
endif


export LDFLAGS = -r
export INCLUDE = -I$(SRCBASE)/include -I./include

CRYPTDIR = ../../bcmcrypto

LIBDIR = .

ifeq ($(EXTERNAL_OPENSSL),1)
export INCLUDE += -I$(EXTERNAL_OPENSSL_INC)
else
export INCLUDE += -I$(SRCBASE)/include/bcmcrypto
CRYPTOBJS := $(addprefix $(LIBDIR)/$(notdir $(CRYPTDIR))/, aes.o rijndael-alg-fst.o \
	 dh.o bn.o sha256.o hmac_sha256.o random.o)
endif


OBJS =  $(addprefix $(LIBDIR)/, enrollee/enr_api.o \
	sta/sta_eap_sm.o \
	enrollee/enr_reg_sm.o \
	registrar/reg_sm.o )




default: $(LIBDIR)/libwpsenr.a $(LIBDIR)/libbcmcrypto.a 

$(LIBDIR)/libbcmcrypto.a: $(CRYPTOBJS)
	$(AR) cr  $@ $^

$(LIBDIR)/libwpsenr.a: $(OBJS)
	$(AR) cr  $@ $^  

$(LIBDIR)/libbcmcrypto.so: $(CRYPTOBJS)
	$(LD) -shared -o $@ $^ 

$(LIBDIR)/libwpsenr.so: $(OBJS)
	$(LD) -shared -o $@ $^ 


$(LIBDIR)/bcmcrypto/%.o :  $(CRYPTDIR)/%.c
	$(CC) -c $(CFLAGS) -DAES_CBC_ONLY $(INCLUDE) $< -o $@


$(LIBDIR)/shared/%.o :  shared/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(LIBDIR)/sta/%.o :  sta/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(LIBDIR)/enrollee/%.o :  enrollee/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(LIBDIR)/registrar/%.o :  registrar/%.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@
