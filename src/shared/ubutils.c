/*
 * Misc utility routines for accessing chip-specific features
 * of the SiliconBackplane-based Broadcom chips.
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <hndsoc.h>
#include <sbchipc.h>
#include <pcicfg.h>

#include "siutils_priv.h"

#include <bcmdevs.h>

#if defined(DSLCPE) && defined(DSLCPE_WOC)
#include <bcm_map_part.h>
#undef  GOODREGS
#define GOODREGS(regs) 1
#endif /* defined(DSLCPE) && defined(DSLCPE_WOC) */

/* EROM parsing */

#define FAKEWRAP_ADDR_CORE_BASE	(CPHYSADDR(&WLAN_SHIM->CcControl))

typedef volatile struct _ubdmp {
	uint32	ioctrl;		/* 0x408 */
	uint32	iostatus;	/* 0x500 */
} ubdmp_t;



static uint32
get_erom_ent(si_t *sih, uint32 **eromptr, uint32 mask, uint32 match)
{
	uint32 ent;
	uint inv = 0, nom = 0;

	while (TRUE) {
		ent = R_REG(si_osh(sih), *eromptr);
		(*eromptr)++;

		if (mask == 0)
			break;

		if ((ent & ER_VALID) == 0) {
			inv++;
			continue;
		}

		if (ent == (ER_END | ER_VALID))
			break;

		if ((ent & mask) == match)
			break;

		nom++;
	}

	SI_VMSG(("%s: Returning ent 0x%08x\n", __FUNCTION__, ent));
	if (inv + nom) {
		SI_VMSG(("  after %d invalid and %d non-matching entries\n", inv, nom));
	}
	return ent;
}

static uint32
get_asd(si_t *sih, uint32 **eromptr, uint sp, uint ad, uint st, uint32 *addrl, uint32 *addrh,
        uint32 *sizel, uint32 *sizeh)
{
	uint32 asd, sz, szd;
	
	asd = get_erom_ent(sih, eromptr, ER_VALID, ER_VALID);
	if (((asd & ER_TAG1) != ER_ADD) ||
	    (((asd & AD_SP_MASK) >> AD_SP_SHIFT) != sp) ||
	    ((asd & AD_ST_MASK) != st)) {
		/* This is not what we want, "push" it back */
		(*eromptr)--;
		return 0;
	}
	*addrl = asd & AD_ADDR_MASK;
	if (asd & AD_AG32)
		*addrh = get_erom_ent(sih, eromptr, 0, 0);
	else
		*addrh = 0;
	*sizeh = 0;
	sz = asd & AD_SZ_MASK;
	if (sz == AD_SZ_SZD) {
		szd = get_erom_ent(sih, eromptr, 0, 0);
		*sizel = szd & SD_SZ_MASK;
		if (szd & SD_SG32)
			*sizeh = get_erom_ent(sih, eromptr, 0, 0);
	} else
		*sizel = AD_SZ_BASE << (sz >> AD_SZ_SHIFT);

	SI_VMSG(("  SP %d, ad %d: st = %d, 0x%08x_0x%08x @ 0x%08x_0x%08x\n",
	        sp, ad, st, *sizeh, *sizel, *addrh, *addrl));

	return asd;
}

/* parse the enumeration rom to identify all cores */
void
ub_scan(si_t *sih, void *regs, uint devid)
{
	si_info_t *sii = SI_INFO(sih);
	chipcregs_t *cc = (chipcregs_t *)regs;
	uint32 erombase, *eromptr, *eromlim;

	erombase = R_REG(sii->osh, &cc->eromptr);

	switch (BUSTYPE(sih->bustype)) {
	case SI_BUS:
		eromptr = (uint32 *)REG_MAP(erombase, SI_CORE_SIZE);
		break;

	default:
		SI_ERROR(("Don't know how to do AXI enumertion on bus %d\n", sih->bustype));
		ASSERT(0);
		return;
	}
	eromlim = eromptr + (ER_REMAPCONTROL / sizeof(uint32));

	SI_VMSG(("ub_scan: regs = 0x%p, erombase = 0x%08x, eromptr = 0x%p, eromlim = 0x%p\n",
	         regs, erombase, eromptr, eromlim));
	while (eromptr < eromlim) {
		uint32 cia, cib, cid, mfg, crev, nmw, nsw, nmp, nsp;
		uint32 mpd, asd, addrl, addrh, sizel, sizeh;
		uint32 *base;
		uint i, j, idx;
		bool br;

		br = FALSE;

		/* Grok a component */
		cia = get_erom_ent(sih, &eromptr, ER_TAG, ER_CI);
		if (cia == (ER_END | ER_VALID)) {
			SI_VMSG(("Found END of erom after %d cores\n", sii->numcores));
#if defined(DSLCPE) && defined(DSLCPE_WOC)
			/* set wrapper addr : operations later require curwrap */
			for (i=0;i<sii->numcores; i++) {
				/* assign shim_reg_xxx_control to wrapper */
				sii->wrapba[i] = FAKEWRAP_ADDR_CORE_BASE + i*(sizeof(ubdmp_t));
				SI_VMSG(("wrapba(%d) = 0x%4x \n", i, sii->wrapba[i]));
			}
#endif /* defined(DSLCPE) && defined(DSLCPE_WOC) */		
			return;
		}
		base = eromptr - 1;
		cib = get_erom_ent(sih, &eromptr, 0, 0);

		if ((cib & ER_TAG) != ER_CI) {
			SI_ERROR(("CIA not followed by CIB\n"));
			goto error;
		}

		cid = (cia & CIA_CID_MASK) >> CIA_CID_SHIFT;
		mfg = (cia & CIA_MFG_MASK) >> CIA_MFG_SHIFT;
		crev = (cib & CIB_REV_MASK) >> CIB_REV_SHIFT;
		nmw = (cib & CIB_NMW_MASK) >> CIB_NMW_SHIFT;
		nsw = (cib & CIB_NSW_MASK) >> CIB_NSW_SHIFT;
		nmp = (cib & CIB_NMP_MASK) >> CIB_NMP_SHIFT;
		nsp = (cib & CIB_NSP_MASK) >> CIB_NSP_SHIFT;

		SI_VMSG(("Found component 0x%04x/0x%04x rev %d at erom addr 0x%p, with nmw = %d, "
		         "nsw = %d, nmp = %d & nsp = %d\n",
		         mfg, cid, crev, base, nmw, nsw, nmp, nsp));

		if (((mfg == MFGID_ARM) && (cid == DEF_AI_COMP)) || (nsp == 0))
			continue;

#if defined(DSLCPE) && defined(DSLCPE_WOC)
		    if (((mfg == MFGID_BRCM) && (cid == DEF_SHIM_COMP))|| (nsp == 0))
				continue;
#else
		if ((nmw + nsw == 0)) {
			/* A component which is not a core */
			/* XXX: Should record some info */
			if (cid == OOB_ROUTER_CORE_ID) {
				asd = get_asd(sih, &eromptr, 0, 0, AD_ST_SLAVE,
					&addrl, &addrh, &sizel, &sizeh);
				if (asd != 0) {
					sii->oob_router = addrl;
				}
			}
			continue;
		}
#endif /* defined(DSLCPE) && defined(DSLCPE_WOC) */ 
		idx = sii->numcores;
/*		sii->eromptr[idx] = base; */
		sii->cia[idx] = cia;
		sii->cib[idx] = cib;
		sii->coreid[idx] = cid;

		for (i = 0; i < nmp; i++) {
			mpd = get_erom_ent(sih, &eromptr, ER_VALID, ER_VALID);
			if ((mpd & ER_TAG) != ER_MP) {
				SI_ERROR(("Not enough MP entries for component 0x%x\n", cid));
				goto error;
			}
			SI_VMSG(("  Master port %d, mp: %d id: %d\n", i,
			         (mpd & MPD_MP_MASK) >> MPD_MP_SHIFT,
			         (mpd & MPD_MUI_MASK) >> MPD_MUI_SHIFT));
		}

		/* First Slave Address Descriptor should be port 0:
		 * the main register space for the core
		 */
		asd = get_asd(sih, &eromptr, 0, 0, AD_ST_SLAVE, &addrl, &addrh, &sizel, &sizeh);
		if (asd == 0) {
			/* Try again to see if it is a bridge */
			asd = get_asd(sih, &eromptr, 0, 0, AD_ST_BRIDGE, &addrl, &addrh,
			              &sizel, &sizeh);
			if (asd != 0)
				br = TRUE;
			else
				if ((addrh != 0) || (sizeh != 0) || (sizel != SI_CORE_SIZE)) {
					SI_ERROR(("First Slave ASD for core 0x%04x malformed "
					          "(0x%08x)\n", cid, asd));
					goto error;
				}
		}
		sii->coresba[idx] = addrl;
		sii->coresba_size[idx] = sizel;
		/* Get any more ASDs in port 0 */
		j = 1;
		do {
			asd = get_asd(sih, &eromptr, 0, j, AD_ST_SLAVE, &addrl, &addrh,
			              &sizel, &sizeh);
			if ((asd != 0) && (j == 1) && (sizel == SI_CORE_SIZE))
				sii->coresba2[idx] = addrl;
				sii->coresba2_size[idx] = sizel;
			j++;
		} while (asd != 0);

		/* Go through the ASDs for other slave ports */
		for (i = 1; i < nsp; i++) {
			j = 0;
			do {
				asd = get_asd(sih, &eromptr, i, j++, AD_ST_SLAVE, &addrl, &addrh,
				              &sizel, &sizeh);
			} while (asd != 0);
			if (j == 0) {
				SI_ERROR((" SP %d has no address descriptors\n", i));
				goto error;
			}
		}

		/* Now get master wrappers */
		for (i = 0; i < nmw; i++) {
			asd = get_asd(sih, &eromptr, i, 0, AD_ST_MWRAP, &addrl, &addrh,
			              &sizel, &sizeh);
			if (asd == 0) {
				SI_ERROR(("Missing descriptor for MW %d\n", i));
				goto error;
			}
			if ((sizeh != 0) || (sizel != SI_CORE_SIZE)) {
				SI_ERROR(("Master wrapper %d is not 4KB\n", i));
				goto error;
			}
			if (i == 0)
				sii->wrapba[idx] = addrl;
		}

		/* And finally slave wrappers */
		for (i = 0; i < nsw; i++) {
			uint fwp = (nsp == 1) ? 0 : 1;
			asd = get_asd(sih, &eromptr, fwp + i, 0, AD_ST_SWRAP, &addrl, &addrh,
			              &sizel, &sizeh);
			if (asd == 0) {
				SI_ERROR(("Missing descriptor for SW %d\n", i));
				goto error;
			}
			if ((sizeh != 0) || (sizel != SI_CORE_SIZE)) {
				SI_ERROR(("Slave wrapper %d is not 4KB\n", i));
				goto error;
			}
			if ((nmw == 0) && (i == 0))
				sii->wrapba[idx] = addrl;
		}

		/* Don't record bridges */
		if (br)
			continue;

		/* Done with core */
		sii->numcores++;
	}

	SI_ERROR(("Reached end of erom without finding END"));

error:
	sii->numcores = 0;
	return;
}

/* This function changes the logical "focus" to the indicated core.
 * Return the current core's virtual address.
 */
void *
ub_setcoreidx(si_t *sih, uint coreidx)
{
	si_info_t *sii = SI_INFO(sih);
	uint32 addr = sii->coresba[coreidx];
	uint32 wrap = sii->wrapba[coreidx];
	void *regs;

	if (coreidx >= sii->numcores)
		return (NULL);

	/*
	 * If the user has provided an interrupt mask enabled function,
	 * then assert interrupts are disabled before switching the core.
	 */
	ASSERT((sii->intrsenabled_fn == NULL) || !(*(sii)->intrsenabled_fn)((sii)->intr_arg));

	switch (BUSTYPE(sih->bustype)) {
	case SI_BUS:
		/* map new one */
		if (!sii->regs[coreidx]) {
			sii->regs[coreidx] = REG_MAP(addr, SI_CORE_SIZE);
			ASSERT(GOODREGS(sii->regs[coreidx]));
		}
		sii->curmap = regs = sii->regs[coreidx];
		if (!sii->wrappers[coreidx]) {
			sii->wrappers[coreidx] = REG_MAP(wrap, SI_CORE_SIZE);
			ASSERT(GOODREGS(sii->wrappers[coreidx]));
		}
		sii->curwrap = sii->wrappers[coreidx];
		break;
	default:
		ASSERT(0);
		regs = NULL;
		break;
	}

	sii->curmap = regs;
	sii->curidx = coreidx;

	return regs;
}

/* Return the number of address spaces in current core */
int
ub_numaddrspaces(si_t *sih)
{
	return 2;
}

/* Return the address of the nth address space in the current core */
uint32
ub_addrspace(si_t *sih, uint asidx)
{
	si_info_t *sii;
	uint cidx;

	sii = SI_INFO(sih);
	cidx = sii->curidx;

	if (asidx == 0)
		return sii->coresba[cidx];
	else if (asidx == 1)
		return sii->coresba2[cidx];
	else {
		SI_ERROR(("%s: Need to parse the erom again to find addr space %d\n",
		          __FUNCTION__, asidx));
		return 0;
	}
}

/* Return the size of the nth address space in the current core */
uint32
ub_addrspacesize(si_t *sih, uint asidx)
{
	si_info_t *sii;
	uint cidx;

	sii = SI_INFO(sih);
	cidx = sii->curidx;

	if (asidx == 0)
		return sii->coresba_size[cidx];
	else if (asidx == 1)
		return sii->coresba2_size[cidx];
	else {
		SI_ERROR(("%s: Need to parse the erom again to find addr space %d\n",
		          __FUNCTION__, asidx));
		return 0;
	}
}

uint
ub_flag(si_t *sih)
{
		return 0;
}

void
ub_setint(si_t *sih, int siflag)
{
}

uint
ub_corevendor(si_t *sih)
{
	si_info_t *sii;
	uint32 cia;

	sii = SI_INFO(sih);
	cia = sii->cia[sii->curidx];
	return ((cia & CIA_MFG_MASK) >> CIA_MFG_SHIFT);
}

uint
ub_corerev(si_t *sih)
{
	si_info_t *sii;
	uint32 cib;

	sii = SI_INFO(sih);
	cib = sii->cib[sii->curidx];
	return ((cib & CIB_REV_MASK) >> CIB_REV_SHIFT);
}

bool
ub_iscoreup(si_t *sih)
{	
	
	si_info_t *sii;
	ubdmp_t *ai;

	sii = SI_INFO(sih);
	ai = sii->curwrap;

	return ((R_REG(sii->osh, &ai->ioctrl) & (SICF_WOC_CORE_RESET |SICF_FGC | SICF_CLOCK_EN)) == SICF_CLOCK_EN);
}

/*
 * Switch to 'coreidx', issue a single arbitrary 32bit register mask&set operation,
 * switch back to the original core, and return the new value.
 *
 * When using the silicon backplane, no fidleing with interrupts or core switches are needed.
 *
 * Also, when using pci/pcie, we can optimize away the core switching for pci registers
 * and (on newer pci cores) chipcommon registers.
 */
uint
ub_corereg(si_t *sih, uint coreidx, uint regoff, uint mask, uint val)
{
	uint origidx = 0;
	uint32 *r = NULL;
	uint w;
	uint intr_val = 0;
	bool fast = FALSE;
	si_info_t *sii;

	sii = SI_INFO(sih);

	ASSERT(GOODIDX(coreidx));
	ASSERT(regoff < SI_CORE_SIZE);
	ASSERT((val & ~mask) == 0);

	if (BUSTYPE(sih->bustype) == SI_BUS) {
		/* If internal bus, we can always get at everything */
		fast = TRUE;
		/* map if does not exist */
		if (!sii->regs[coreidx]) {
			sii->regs[coreidx] = REG_MAP(sii->coresba[coreidx],
			                            SI_CORE_SIZE);
			ASSERT(GOODREGS(sii->regs[coreidx]));
		}
		r = (uint32 *)((uchar *)sii->regs[coreidx] + regoff);
	}

	if (!fast) {
		INTR_OFF(sii, intr_val);

		/* save current core index */
		origidx = si_coreidx(&sii->pub);

		/* switch core */
		r = (uint32*) ((uchar*) ub_setcoreidx(&sii->pub, coreidx) + regoff);
	}
	ASSERT(r != NULL);

	/* mask and set */
	if (mask || val) {
		w = (R_REG(sii->osh, r) & ~mask) | val;
		W_REG(sii->osh, r, w);
	}

	/* readback */
	w = R_REG(sii->osh, r);

	if (!fast) {
		/* restore core index */
		if (origidx != coreidx)
			ub_setcoreidx(&sii->pub, origidx);

		INTR_RESTORE(sii, intr_val);
	}

	return (w);
}

void
ub_core_disable(si_t *sih, uint32 bits)
{
	si_info_t *sii;
	volatile uint32 dummy;
	ubdmp_t *ai;

	sii = SI_INFO(sih);
	
	ai = sii->curwrap;

	if(R_REG(sii->osh, &ai->ioctrl) & SICF_WOC_CORE_RESET)
		return;
			
	W_REG(sii->osh, &ai->ioctrl, bits);
	dummy = R_REG(sii->osh, &ai->ioctrl);
	OSL_DELAY(10);	
	W_REG(sii->osh, &ai->ioctrl, dummy|SICF_WOC_CORE_RESET);
	OSL_DELAY(10); /* Add delay to workaround the CR5750 */
}

/* reset and re-enable a core
 * inputs:
 * bits - core specific bits that are set during and after reset sequence
 * resetbits - core specific bits that are set only during reset sequence
 */
void
ub_core_reset(si_t *sih, uint32 bits, uint32 resetbits)
{
	si_info_t *sii;
	ubdmp_t *ai;
	volatile uint32 dummy;
	
	sii = SI_INFO(sih);		
	ASSERT(GOODREGS(sii->curwrap));

	ai = sii->curwrap;                	
	/*
	 * Must do the disable sequence first to work for arbitrary current core state.
	 */
	ub_core_disable(sih, (bits | resetbits));
	W_REG(sii->osh, &ai->ioctrl, (bits | SICF_FGC | SICF_CLOCK_EN));
	dummy = R_REG(sii->osh, &ai->ioctrl);

	W_REG(sii->osh, &ai->ioctrl, dummy & ~SICF_WOC_CORE_RESET);
	OSL_DELAY(1);
	W_REG(sii->osh, &ai->ioctrl, (bits | SICF_CLOCK_EN));
	dummy = R_REG(sii->osh, &ai->ioctrl);	 		
	OSL_DELAY(10); /* Add delay to workaround the CR5750 */
}


void
ub_core_cflags_wo(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	ubdmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);

	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);

	if (mask || val) {
		w = ((R_REG(sii->osh, &ai->ioctrl) & ~mask) | val);
		W_REG(sii->osh, &ai->ioctrl, w);
	}
}

uint32
ub_core_cflags(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	ubdmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);

	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);

	if (mask || val) {
		w = ((R_REG(sii->osh, &ai->ioctrl) & ~mask) | val);
		W_REG(sii->osh, &ai->ioctrl, w);
	}

	return R_REG(sii->osh, &ai->ioctrl);
}

uint32
ub_core_sflags(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	ubdmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);

	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);
	ASSERT((mask & ~SISF_CORE_BITS) == 0);

	if (mask || val) {
		w = ((R_REG(sii->osh, &ai->iostatus) & ~mask) | val);
		W_REG(sii->osh, &ai->iostatus, w);
	}

	return R_REG(sii->osh, &ai->iostatus);
}

#if defined(BCMDBG) || defined(BCMDBG_DUMP)
/* print interesting ubdmp registers */
void
ub_dumpregs(si_t *sih, struct bcmstrbuf *b)
{
	si_info_t *sii;
	osl_t *osh;
	ubdmp_t *ai;
	uint i;

	sii = SI_INFO(sih);
	osh = sii->osh;

	for (i = 0; i < sii->numcores; i++) {
		si_setcoreidx(&sii->pub, i);
		ai = sii->curwrap;

		bcm_bprintf(b, "core 0x%x: \n", sii->coreid[i]);

		//TBD: if (sih->chip == BCM6362_CHIP_ID)		
		bcm_bprintf(b, " ioctrl 0x%x iostatus 0x%x \n", R_REG(osh, &ai->ioctrl), R_REG(osh, &ai->iostatus));

	}
}
#endif	/* BCMDBG || BCMDBG_DUMP */

#ifdef BCMDBG
void
ub_view(si_t *sih, bool verbose)
{
	if (1) { //TBD:
		SI_ERROR(("not implemented\n"));
		return;		
	}
}
#endif	/* BCMDBG */
