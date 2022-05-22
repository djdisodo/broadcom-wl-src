#
# Makefile fragment for Linux 2.6
# Broadcom 802.11abg Networking Device Driver
#
# Copyright (C) 2015, Broadcom Corporation. All Rights Reserved.
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# $Id: Makefile_kbuild_portsrc 580354 2015-08-18 23:42:37Z $

ifneq ($(KERNELRELEASE),)

  LINUXVER_GOODFOR_CFG80211:=$(strip $(shell \
    if [ "$(VERSION)" -ge "2" -a "$(PATCHLEVEL)" -ge "6" -a "$(SUBLEVEL)" -ge "32" -o "$(VERSION)" -ge "3" ]; then \
      echo TRUE; \
    else \
      echo FALSE; \
    fi \
  ))

    LINUXVER_WEXT_ONLY:=$(strip $(shell \
    if [ "$(VERSION)" -ge "2" -a "$(PATCHLEVEL)" -ge "6" -a "$(SUBLEVEL)" -ge "17" ]; then \
      echo FALSE; \
    else \
      echo TRUE; \
    fi \
  ))

  ifneq ($(API),)
    ifeq ($(API), CFG80211)
      APICHOICE := FORCE_CFG80211
      $(info CFG80211 API specified in command line)
    else
      ifeq ($(API), WEXT)
        APICHOICE := FORCE_WEXT
        $(info Wireless Extension API specified in command line)
      else
        $(error Unknown API type)
      endif
    endif
  else
    ifeq ($(LINUXVER_GOODFOR_CFG80211),TRUE)
      APICHOICE := PREFER_CFG80211
      $(info CFG80211 API is prefered for this kernel version)
    else
      ifeq ($(LINUXVER_WEXT_ONLY),TRUE)
        APICHOICE := FORCE_WEXT
        $(info Wireless Extension is the only possible API for this kernel version)
      else
        APICHOICE := PREFER_WEXT
        $(info Wireless Extension API is prefered for this kernel version)
      endif
    endif
  endif

  ifeq ($(APICHOICE),FORCE_CFG80211)
    ifneq ($(CONFIG_CFG80211),)
      APIFINAL := CFG80211
    else
      $(error CFG80211 is specified but it is not enabled in kernel)
    endif
  endif

  ifeq ($(APICHOICE),FORCE_WEXT)
    APIFINAL := WEXT
  endif

  ifeq ($(APICHOICE),PREFER_CFG80211)
    ifneq ($(CONFIG_CFG80211),)
      APIFINAL := CFG80211
    else
      ifneq ($(CONFIG_WIRELESS_EXT),)
        APIFINAL := WEXT
      else
        $(warning Neither CFG80211 nor Wireless Extension is enabled in kernel)
      endif
    endif
  endif

  ifeq ($(APICHOICE),PREFER_WEXT)
    ifneq ($(CONFIG_WIRELESS_EXT),)
      APIFINAL := WEXT
    else
      ifneq ($(CONFIG_CFG80211),)
        APIFINAL := CFG80211
      else
        $(warning Neither CFG80211 nor Wireless Extension is enabled in kernel)
      endif
    endif
  endif

endif

#Check GCC version so we can apply -Wno-date-time if supported.  GCC >= 4.9
empty:=
space:= $(empty) $(empty)
GCCVERSIONSTRING := $(shell expr `$(CC) -dumpversion`)
#Create version number without "."
GCCVERSION := $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f1 -d.)
GCCVERSION += $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f2 -d.)
GCCVERSION += $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f3 -d.)
# Make sure the version number has at least 3 decimals
GCCVERSION += 00
# Remove spaces from the version number
GCCVERSION := $(subst $(space),$(empty),$(GCCVERSION))
# Crop the version number to 3 decimals.
GCCVERSION := $(shell expr `echo $(GCCVERSION)` | cut -b1-3)
GE_49 := $(shell expr `echo $(GCCVERSION)` \>= 490)

EXTRA_CFLAGS :=

ifeq ($(APIFINAL),CFG80211)
  EXTRA_CFLAGS += -DUSE_CFG80211
  $(info Using CFG80211 API)
endif

ifeq ($(APIFINAL),WEXT)
  EXTRA_CFLAGS += -DUSE_IW
  $(info Using Wireless Extension API)
endif

obj-m              += wl.o


WL_SRCS := $(wildcard src/wl/phy/*.c)

WL_OBJS            := $(patsubst %.c,%.o,$(WL_SRCS))

#$(info $$WL_OBJS is [${WL_OBJS}])

wl-objs := src/wl/sys/d11ucode_2w.o src/wl/sys/d11ucode_ge24.o src/wl/sys/d11ucode_ge40.o src/wl/sys/d11ucode_gt15.o src/wl/sys/d11ucode_le15.o src/wl/sys/d11ucode_p2p.o src/wl/sys/d11ucode_wowl.o src/wl/sys/wlc_11d.o src/wl/sys/wlc_11h.o src/wl/sys/wlc_11u.o src/wl/sys/wlc_alloc.o src/wl/sys/wlc_antsel.o src/wl/sys/wlc_ap.o src/wl/sys/wlc_apcs.o src/wl/sys/wlc_apps.o src/wl/sys/wlc_assoc.o src/wl/sys/wlc_bmac.o src/wl/sys/wlc_bsscfg.o src/wl/sys/wlc_bssload.o src/wl/sys/wlc.o src/wl/sys/wlc_cca.o src/wl/sys/wlc_channel.o src/wl/sys/wlc_cntry.o src/wl/sys/wlc_csa.o src/wl/sys/wlc_dfs.o src/wl/sys/wlc_diag.o src/wl/sys/wlc_dls.o src/wl/sys/wlc_event.o src/wl/sys/wlc_hrt.o src/wl/sys/wlc_hw.o src/wl/sys/wlc_interfere.o src/wl/sys/wlc_intr.o src/wl/sys/wlc_lq.o src/wl/sys/wlc_mbss.o src/wl/sys/wlc_offloads.o src/wl/sys/wlc_phy_shim.o src/wl/sys/wlc_probresp.o src/wl/sys/wlc_prot.o src/wl/sys/wlc_prot_g.o src/wl/sys/wlc_prot_n.o src/wl/sys/wlc_quiet.o src/wl/sys/wlc_rate.o src/wl/sys/wlc_rate_sel.o src/wl/sys/wlc_rm.o src/wl/sys/wlc_scan.o src/wl/sys/wlc_scb.o src/wl/sys/wlc_scb_ratesel.o src/wl/sys/wlc_stf.o src/wl/sys/wlc_tpc.o src/wl/sys/wlc_tso.o src/wl/sys/wlc_txbf.o src/wl/sys/wlc_utils.o src/wl/sys/wlc_vht.o src/wl/sys/wlc_vndr_ie_list.o src/wl/sys/wlc_wet.o src/wl/sys/wl_dslcpe_pktc.o src/wl/sys/wl_iw.o src/wl/sys/wl_linux.o
wl-objs += src/wl/phy/wlc_phy_abg.o src/wl/phy/wlc_phy_ac.o src/wl/phy/wlc_phy_cmn.o src/wl/phy/wlc_phy_ht.o src/wl/phy/wlc_phy_iovar.o src/wl/phy/wlc_phy_lcn40.o src/wl/phy/wlc_phy_lcn.o src/wl/phy/wlc_phy_lp.o src/wl/phy/wlc_phy_n.o src/wl/phy/wlc_phy_radio_n.o src/wl/phy/wlc_phy_ssn.o src/wl/phy/wlc_phytbl_ac.o src/wl/phy/wlc_phytbl_acdc.o src/wl/phy/wlc_phytbl_ht.o src/wl/phy/wlc_phytbl_lcn40.o src/wl/phy/wlc_phytbl_lcn.o src/wl/phy/wlc_phytbl_lp.o src/wl/phy/wlc_phytbl_n.o src/wl/phy/wlc_phytbl_ssn.o
wl-objs            += src/wl/ppr/src/wlc_ppr.o
wl-objs            += src/wl/clm/src/wlc_clm.o
wl-objs            += src/wl/clm/src/wlc_clm_data.o

#wl-objs            += src/wl/sys/wl_cfg80211_hybrid.o

wl-objs            += src/shared/linux_osl.o
wl-objs            += src/shared/bcmutils.o
wl-objs            += src/shared/siutils.o
wl-objs            += src/shared/sbutils.o
wl-objs            += src/shared/hndpmu.o
wl-objs            += src/shared/hnddma.o
wl-objs            += src/shared/nicpci.o
wl-objs            += src/shared/aiutils.o
wl-objs            += src/shared/bcmsrom.o
wl-objs            += src/shared/nvram_rw.o
wl-objs            += src/shared/nvram.o
wl-objs            += src/shared/bcmotp.o
wl-objs            += src/shared/flashutl.o
wl-objs            += src/shared/sflash.o
wl-objs            += src/shared/hndsoc.o
wl-objs            += src/shared/bcm_mpool.o
wl-objs            += src/shared/bcm_notif.o
wl-objs            += src/shared/qmath.o
wl-objs            += src/shared/bcmwpa.o
wl-objs            += src/shared/bcmwifi/src/bcmwifi_channels.o

wl-objs            += src/bcmcrypto/rc4.o
wl-objs            += src/bcmcrypto/wep.o



#wl-objs            += src/shared/bcmsromio.o
#wl-objs            += src/bcm963xx/board.o

wl-objs            += LZMA-SDK/C/LzmaDec.o

EXTRA_CFLAGS       += -I$(src)/src/include -I$(src)/LZMA-SDK/C
EXTRA_CFLAGS       += -I$(src)/src/wl/sys -I$(src)/src/phy
EXTRA_CFLAGS       += -I$(src)/src/shared/bcmwifi/include
EXTRA_CFLAGS       += -I$(src)/src/wl/phy
EXTRA_CFLAGS       += -I$(src)/src/wl/ppr/include
EXTRA_CFLAGS       += -I$(src)/src/wl/clm/include
EXTRA_CFLAGS       += -I$(src)/src/bcm963xx
EXTRA_CFLAGS       += -DBCMDRIVER -DWLC_LOW -nostdlib -DWL11H -DWLC_HIGH -DAP -DSTA -DDSLCPE_WLAN_VERSION="\"0\"" -DTYPEDEF_BOOL -DLINUX_PORT
EXTRA_CFLAGS       += -Wno-error=date-time -DBCMEXTNVM -DBCMHOSTVARS -DWLCNT -DWL11AC -DWL11N -DPPR_API -DUSE_IW -D_BCM96362_ -DWLTPC
EXTRA_CFLAGS       += -DUBUS_DISABLE -DWL_AP_TPC -DWLCSA

#idk wtf
EXTRA_CFLAGS       += -DCONFIG_MMC_MSM7X00A

#EXTRA_CFLAGS       += -DBCMDBG_ASSERT -DBCMDBG_ERR
ifeq "$(GE_49)" "1"
EXTRA_CFLAGS       += -Wno-date-time
endif

KBASE              ?= /lib/modules/`uname -r`
KBUILD_DIR         ?= $(KBASE)/build
MDEST_DIR          ?= $(KBASE)/kernel/drivers/net/wireless

# Cross compile setup.  Tool chain and kernel tree, replace with your own.
CROSS_TOOLS        = /home/sodo/openwrt/staging_dir/toolchain-arm_cortex-a9_gcc-11.2.0_musl_eabi/bin/arm-openwrt-linux-
CROSS_KBUILD_DIR   = /home/sodo/openwrt/build_dir/target-arm_cortex-a9_musl_eabi/linux-bcm53xx_generic/linux-5.10.113

all:
	KBUILD_NOPEDANTIC=1 make -C $(KBUILD_DIR) M=`pwd`

cross:
	
	KBUILD_NOPEDANTIC=1 ARCH=arm make CROSS_COMPILE=${CROSS_TOOLS} -C $(CROSS_KBUILD_DIR) M=`pwd`

clean:
	KBUILD_NOPEDANTIC=1 make -C $(KBUILD_DIR) M=`pwd` clean

install:
	install -D -m 755 wl.ko $(MDEST_DIR)
