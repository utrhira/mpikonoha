/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2011, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 *
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* ************************************************************************ */

#define USE_STEXT 1
#define USE_B     1
#define USE_bytes_first       1
#define USE_bytes_index       1
#define USE_bytes_last        1
#define USE_bytes_startsWith  1
#define USE_bytes_endsWith    1

#define USE_cwb_open      1
#define USE_cwb_tobytes   1

#include"commons.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [properties] */

static void knh_cwb_nzenvkey(CTX ctx, knh_cwb_t *cwb, knh_bytes_t t)
{
	size_t i;
	for(i = 0; i < t.len; i++) {
		knh_Bytes_putc(ctx, (cwb->ba), toupper(t.ustr[i]));
	}
}

/* ------------------------------------------------------------------------ */

knh_String_t* knh_getPropertyNULL(CTX ctx, knh_bytes_t key)
{
	if(knh_bytes_startsWith(key, STEXT("env."))) {
		knh_cwb_t cwbbuf, *cwb = knh_cwb_open(ctx, &cwbbuf);
		knh_cwb_nzenvkey(ctx, cwb, knh_bytes_last(key, 4));
		char *v = knh_getenv(knh_cwb_tochar(ctx, cwb));
		knh_cwb_close(cwb);
		if(v == NULL) return NULL;
		return new_S(ctx, B(v));
	}
	return (knh_String_t*)knh_DictMap_getNULL(ctx,  DP(ctx->sys)->props, key);
}

/* ------------------------------------------------------------------------ */

void knh_setProperty(CTX ctx, knh_String_t *key, dynamic *value)
{
	DictMap_set_(ctx, DP(ctx->sys)->props, key, value);
}

/* ------------------------------------------------------------------------ */

KNHAPI2(void) knh_setPropertyText(CTX ctx, char *key, char *value)
{
	knh_String_t *k = new_T(key);
	knh_String_t *v = new_T(value);
	DictMap_set_(ctx, DP(ctx->sys)->props, k, UPCAST(v));
}

/* ------------------------------------------------------------------------ */
/* [Constant Value] */

Object *knh_getClassConstNULL(CTX ctx, knh_class_t cid, knh_bytes_t name)
{
	DBG_ASSERT_cid(cid);
	if(ClassTBL(cid)->constDictCaseMapNULL == NULL) return NULL;
	knh_DictMap_t *cmap = ClassTBL(cid)->constDictCaseMapNULL;
	Object *value = NULL;
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	int res = knh_DictMap_index(cmap, name);
	if(res != -1) {
		value = knh_DictMap_valueAt(cmap, res);
	}
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	return value;
}

/* ------------------------------------------------------------------------ */

int knh_addClassConst(CTX ctx, knh_class_t cid, knh_String_t* name, Object *value)
{
	int ret;
	knh_DictMap_t *cmap = ClassTBL(cid)->constDictCaseMapNULL;
	DBG_ASSERT_cid(cid);
	if(cmap == NULL) {
		knh_ClassTBL_t *t = varClassTBL(cid);
		cmap = new_DictMap0(ctx, 0, 1/*isCaseMap*/, "ClassTBL.constDictMap");
		KNH_INITv(t->constDictCaseMapNULL, cmap);
	}
	DBG_ASSERT(IS_Map(cmap));
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	int idx = knh_DictMap_index(cmap, S_tobytes(name));
	if(idx != -1) {
		ret =  0;
		goto L_UNLOCK;
	}
	knh_DictMap_set(ctx, cmap, name, value);
	ret = 1;

	L_UNLOCK:
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	return ret;
}

/* ------------------------------------------------------------------------ */
/* [tfieldn, tmethodn] */

knh_fieldn_t knh_addname(CTX ctx, knh_String_t *s, knh_Fdictset f)
{
	knh_SystemEX_t *b = DP(ctx->sys);
	size_t n = knh_DictSet_size(b->nameDictCaseSet);
	if(n == b->namecapacity) {
		b->namecapacity = k_grow(n);
		b->nameinfo = (knh_nameinfo_t*)KNH_REALLOC(ctx, b->nameinfo, n, b->namecapacity, sizeof(knh_nameinfo_t));
	}
	DBG_ASSERT(n < b->namecapacity);
	KNH_INITv(b->nameinfo[n].name, s);
	if(unlikely(!(n+1 < K_FLAG_MN_SETTER))) {  /* Integer overflowed */
		KNH_SYSLOG(ctx, LOG_CRIT, "TooManyNames", "last nameid(fn)=%d < %d", (int)(n+1), (int)K_FLAG_MN_SETTER);
	}
	f(ctx, b->nameDictCaseSet, s, n + 1);
	return (knh_fieldn_t)(n);
}

static knh_fieldn_t knh_getname(CTX ctx, knh_bytes_t n, knh_fieldn_t def)
{
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	knh_index_t idx = knh_DictSet_index(DP(ctx->sys)->nameDictCaseSet, n);
	if(idx == -1) {
		if(def == FN_NEWID) {
			idx = knh_addname(ctx, new_S(ctx, n), knh_DictSet_set);
		}
		else {
			idx = def - MN_OPSIZE;
		}
	}
	else {
		idx = knh_DictSet_valueAt(DP(ctx->sys)->nameDictCaseSet, idx) - 1;
	}
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	return (knh_fieldn_t)idx + MN_OPSIZE;
}

knh_nameinfo_t *knh_getnameinfo(CTX ctx, knh_fieldn_t fn)
{
	size_t n = (FN_UNMASK(fn) - MN_OPSIZE);
	DBG_(
		size_t size = knh_DictSet_size(DP(ctx->sys)->nameDictCaseSet);
		DBG_ASSERT(n < size);
	);
	return DP(ctx->sys)->nameinfo + n;
}

/* ------------------------------------------------------------------------ */
/* [fn] */

knh_String_t *knh_getFieldName(CTX ctx, knh_fieldn_t fn)
{
	return knh_getnameinfo(ctx, fn)->name;
}

/* ------------------------------------------------------------------------ */

knh_fieldn_t knh_getfnq(CTX ctx, knh_bytes_t tname, knh_fieldn_t def)
{
	knh_fieldn_t mask = 0;
	knh_index_t idx = knh_bytes_index(tname, ':');
	if(idx > 0) {
		tname = knh_bytes_first(tname, idx);
	}
	else if(knh_bytes_startsWith(tname, STEXT("super."))) {
		mask = (def == FN_NONAME) ? 0 : K_FLAG_FN_SUPER;
		tname = knh_bytes_last(tname, 6);
	}
	else if(!knh_bytes_endsWith(tname, STEXT("__"))) {
		if(tname.ustr[0] == '_' && def != FN_NONAME) {
			mask = K_FLAG_FN_U1;
			tname = knh_bytes_last(tname, 1);
		}
		if(tname.ustr[0] == '_' && def != FN_NONAME) {
			mask = K_FLAG_FN_U2;
			tname = knh_bytes_last(tname, 1);
		}
		while(tname.ustr[0] == '_') {
			tname = knh_bytes_last(tname, 1);
		}
	}
	return knh_getname(ctx, tname, def) | mask;
}

/* ------------------------------------------------------------------------ */
/* [methodn] */

static knh_bytes_t knh_bytes_skipFMTOPT(knh_bytes_t t)
{
	size_t i;
	for(i = 1; i < t.len; i++) {
		if(isalnum(t.ustr[i])) break;
	}
	t.ustr = t.ustr + i;
	t.len = t.len - i;
	return t;
}

knh_methodn_t knh_getmn(CTX ctx, knh_bytes_t tname, knh_methodn_t def)
{
	knh_fieldn_t mask = 0;
	if(tname.ustr[0] == '%') {
		tname = knh_bytes_skipFMTOPT(tname);
		if(def != MN_NONAME) mask |= K_FLAG_MN_FMT;
	}
	else if(tname.ustr[0] == 'i' && tname.ustr[1] == 's') { /* is => get */
		tname = knh_bytes_last(tname, 2);
		if(def != MN_NONAME) mask |= K_FLAG_MN_ISBOOL;
	}
	else if(tname.ustr[0] == 'g' && tname.ustr[1] == 'e' && tname.ustr[2] == 't') {
		tname = knh_bytes_last(tname, 3);
		if(def != MN_NONAME) mask |= K_FLAG_MN_GETTER;
	}
	else if(tname.ustr[0] == 's' && tname.ustr[1] == 'e' && tname.ustr[2] == 't') {
		tname = knh_bytes_last(tname, 3);
		if(def != MN_NONAME) mask |= K_FLAG_MN_SETTER;
	}
	return knh_getname(ctx, tname, def) | mask;
}

/* ------------------------------------------------------------------------ */

const char* knh_getmnname(CTX ctx, knh_methodn_t mn)
{
	mn = MN_toFN(mn);
	if(mn < MN_OPSIZE) {
		return knh_getopname(mn);
	}
	return S_tochar(knh_getFieldName(ctx, mn));
}

/* ------------------------------------------------------------------------ */
/* [uri] */

knh_uri_t knh_getURI(CTX ctx, knh_bytes_t t)
{
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	knh_index_t idx = knh_DictSet_index(DP(ctx->sys)->urnDictSet, t);
	if(idx == -1) {
		knh_String_t *s = new_S(ctx, t);
		idx = knh_Array_size(DP(ctx->sys)->urns);
		knh_DictSet_set(ctx, DP(ctx->sys)->urnDictSet, s, idx);
		knh_Array_add(ctx, DP(ctx->sys)->urns, s);
		KNH_SYSLOG(ctx, LOG_INFO, "NEW_URI", "URI=%d, URN='%B'", idx, S_tobytes(s));
	}
	else {
		idx = knh_DictSet_valueAt(DP(ctx->sys)->urnDictSet, idx);
	}
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	return (knh_uri_t)idx;
}

/* ------------------------------------------------------------------------ */

knh_String_t *knh_getURN(CTX ctx, knh_uri_t uri)
{
	size_t n = URI_UNMASK(uri);
	knh_Array_t *a = DP(ctx->sys)->urns;
	if(n < knh_Array_size(a)) {
		return (knh_String_t*)(a)->list[n];
	}
	else {
		DBG_ASSERT(uri == URI_unknown);
		return TS_EMPTY;
	}
}

/* ------------------------------------------------------------------------ */
/* [Driver] */

void knh_addDSPI(CTX ctx, const char *scheme, const knh_DSPI_t* p)
{
	const char *name = (scheme == NULL) ? p->name : scheme;
	knh_cwb_t cwbbuf, *cwb = knh_cwb_open(ctx, &cwbbuf);
	knh_write(ctx, cwb->w, B(name));
	knh_putc(ctx, cwb->w, ':');
	knh_write_ifmt(ctx, cwb->w, K_INT_FMT, p->type);
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	knh_DictSet_set(ctx, DP(ctx->sys)->dspiDictSet, knh_cwb_newString(ctx, cwb), (knh_uintptr_t)p);
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
}

/* ------------------------------------------------------------------------ */

const knh_DSPI_t *knh_getDSPINULL(CTX ctx, int type, knh_bytes_t path)
{
	knh_cwb_t cwbbuf, *cwb = knh_cwb_open(ctx, &cwbbuf);
	knh_index_t idx = knh_bytes_index(path, ':');
	if(idx == -1) {
		knh_write(ctx, cwb->w, path);
	}
	else {
		knh_write(ctx, cwb->w, knh_bytes_first(path, idx));
	}
	knh_putc(ctx, cwb->w, ':');
	knh_write_ifmt(ctx, cwb->w, K_INT_FMT, type);
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	const knh_DSPI_t *p = (const knh_DSPI_t*)knh_DictSet_get(ctx, DP(ctx->sys)->dspiDictSet, knh_cwb_tobytes(cwb));
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	knh_cwb_close(cwb);
	return p;
}

/* ------------------------------------------------------------------------ */

knh_PathDSPI_t *knh_NameSpace_getPathDSPINULL(CTX ctx, knh_NameSpace_t *ns, knh_bytes_t path)
{
	return (knh_PathDSPI_t *)knh_getDSPINULL(ctx, K_DSPI_PATH, path);
}

/* ------------------------------------------------------------------------ */

knh_ConvDSPI_t *knh_NameSpace_getConvTODSPINULL(CTX ctx, knh_NameSpace_t *ns, knh_bytes_t path)
{
	return (knh_ConvDSPI_t *)knh_getDSPINULL(ctx, K_DSPI_CONVTO, path);
}

/* ------------------------------------------------------------------------ */

knh_ConvDSPI_t *knh_NameSpace_getConvFROMDSPINULL(CTX ctx, knh_NameSpace_t *ns, knh_bytes_t path)
{
	return (const knh_ConvDSPI_t *)knh_getDSPINULL(ctx, K_DSPI_CONVFROM, path);
}

/* ------------------------------------------------------------------------ */

void knh_exit(CTX ctx, int status)
{
	exit(status);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif