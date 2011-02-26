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

#define USE_B     1
#define USE_STEXT 1
#define USE_bytes_startsWith 1
#define USE_cwb_open      1

#include"commons.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [object] */

//knh_String_t *knh_Object_getkey(CTX ctx, Object *o)
//{
//	knh_sfp_t lsfp;
//	lsfp.o = o;
//	lsfp.data = ((knh_Int_t*)o)->n.data;
//	return (knh_String_t*)ClassTBL(O_bcid(o))->cspi->hashkey(ctx, &lsfp, KNH_FOBJECT_KEY);
//}

//knh_hashcode_t knh_Object_hashCode(CTX ctx, Object *o)
//{
//	knh_sfp_t lsfp;
//	lsfp.o = o;
//	lsfp.data = ((knh_Int_t*)o)->n.data;
//	return (knh_hashcode_t)ClassTBL(O_bcid(o))->cspi->hashkey(ctx, &lsfp, KNH_FOBJECT_HASH);
//}

/* ------------------------------------------------------------------------ */

int knh_Object_compareTo(Object *o1, Object *o2)
{
	knh_class_t cid1 = O_cid(o1);
	knh_class_t cid2 = O_cid(o2);
	int res;
	if(cid1 == cid2) {
		res = O_cTBL(o1)->cspi2->compareTo(o1, o2);
	}
	else {
		res = (int)(o1 - o2);
		DBG_P("compared %s %s res=%d", O__(o1), O__(o2), res);
	}
	return res;
}

/* ------------------------------------------------------------------------ */

//int knh_Object_compareTo2(CTX ctx, Object **o1, Object **o2)
//{
//	Object* _o1 = *((Object**) o1);
//	Object* _o2 = *((Object**) o2);
//	knh_class_t bcid1 = _o1->h.bcid;
//	knh_class_t bcid2 = _o2->h.bcid;
//	int res;
//	if(bcid1 == bcid2) {
//		res = ClassTBL(bcid1)->cspi->compareTo(ctx, _o1, _o2);
//	}
//	else {
//		res = (int)(_o1 - _o2);
//		DBG_P("compared %s %s res=%d", O__(_o1), O__(_o2), res);
//	}
//	return res;
//}

/* ------------------------------------------------------------------------ */

//knh_bool_t knh_Object_equals(CTX ctx, Object *o1, Object *o2)
//{
//	return (knh_Object_compareTo(ctx, o1, o2) == 0);
//}

///* ------------------------------------------------------------------------ */
//
//knh_bytes_t knh_Object_tobytes(CTX ctx, Object *o)
//{
//	switch(o->h.bcid) {
//		case CLASS_String : return S_tobytes((knh_String_t*)o);
//		case CLASS_Bytes : return BA_tobytes((knh_Bytes_t*)o);
//	}
//	return STEXT("");
//}

/* ------------------------------------------------------------------------ */

void knh_Object_toNULL_(CTX ctx, Object *o)
{
	Object_setNullObject(o, 1);
}

/* ------------------------------------------------------------------------ */
/* [ClassTBL] */

#ifdef K_USING_DEBUG
const char *SAFESTRUCT__(CTX ctx, knh_class_t bcid)
{
	if(bcid < ctx->share->sizeClassTBL) {
		return ClassTBL(bcid)->cspi2->name;
	}
	KNH_P("unknown bcid=%d", bcid);
	return "UNKNOWN";
}

const char *SAFETYPE__(CTX ctx, knh_type_t type)
{
	knh_class_t cid = CLASS_t(type);
	if(type == TYPE_void) return "void";
	if(type == TYPE_var)  return "var";
	if(cid < ctx->share->sizeClassTBL) {
		return S_tochar(ClassTBL(cid)->sname);
	}
	else {
		static char unsafebuf[20];
		knh_snprintf(unsafebuf, sizeof(unsafebuf), "type=%d", (int)type);
		return (const char*)unsafebuf;
	}
}

const knh_ClassTBL_t* DBG_ClassTBL(CTX ctx, knh_class_t cid)
{
	if(cid < ctx->share->sizeClassTBL) {
		return ctx->share->ClassTBL[cid];
	}
	DBG_ABORT("NOT CORRECT: cid=%d", cid);
	return NULL;
}

#endif

const char *SAFECLASS__(CTX ctx, knh_class_t cid)
{
	if(cid < ctx->share->sizeClassTBL) {
		return S_tochar(ClassTBL(cid)->lname);
	}
	else {
		return "unknown";
	}
}

/* ------------------------------------------------------------------------ */

knh_Class_t *new_Type(CTX ctx, knh_type_t type)
{
	knh_class_t cid = CLASS_t(type);
	const knh_ClassTBL_t *t = ClassTBL(cid);
	if(t->typeNULL == NULL) {
		knh_Class_t *o = new_(Class);
		o->cid = cid;
		o->type = cid;
		o->cTBL = t;
		KNH_INITv(((knh_ClassTBL_t*)t)->typeNULL, o);
	}
	return t->typeNULL;
}

/* ------------------------------------------------------------------------ */

void knh_write_cid(CTX ctx, knh_OutputStream_t *w, knh_class_t cid)
{
	const char *tname = NULL;
	switch(cid) {
	case TYPE_dynamic:  tname = "dynamic";    break;
	case TYPE_void: tname = "void";       break;
	case TYPE_var:  tname = "var";        break;
	case TYPE_This: tname = "This";       break;
	case TYPE_TEXT: tname = "@Secure String"; break;
	default :{
			if(cid > TYPE_This) {
				knh_write(ctx, w, STEXT("T"));
				knh_write_ifmt(ctx, w, K_INT_FMT, cid - TYPE_This);
			}
			else {
				knh_write(ctx, w, S_tobytes(ClassTBL(cid)->lname));
			}
			return ;
		}
	}
}

void knh_write_type(CTX ctx, knh_OutputStream_t *w, knh_type_t type)
{
	const char *tname = NULL;
	//DBG_P("type=%d", type);
	switch(type) {
	case TYPE_Boolean: tname = "boolean"; break;
	case TYPE_Int:    tname = "int";      break;
	case TYPE_Float:  tname = "float";    break;
	case TYPE_dynamic:  tname = "dynamic";    break;
	case TYPE_void: tname = "void";       break;
	case TYPE_var:  tname = "var";        break;
	case TYPE_This: tname = "This";       break;
	case TYPE_TEXT: tname = "@Secure String"; break;
	default :{
			knh_class_t cid = CLASS_t(type);
			if(cid > TYPE_This) {
				knh_write(ctx, w, STEXT("T"));
				knh_write_ifmt(ctx, w, K_INT_FMT, cid - TYPE_This);
			}
			else {
				knh_write(ctx, w, S_tobytes(ClassTBL(cid)->sname));
			}
			return ;
		}
	}
	knh_write_text(ctx, w, tname);
}

/* ------------------------------------------------------------------------ */

static knh_Object_t *Fdefnull_newValue(CTX ctx, knh_class_t cid);

void knh_setClassName(CTX ctx, knh_class_t cid, knh_String_t *lname, knh_String_t *snameNULL)
{
	DBG_ASSERT_cid(cid);
	knh_ClassTBL_t *t = varClassTBL(cid);
	DBG_ASSERT(t->lname == NULL);
	KNH_INITv(t->lname, lname);
	if(snameNULL != NULL) {
		KNH_INITv(t->sname, snameNULL);
	}
	else if(t->bcid == cid) {
		KNH_INITv(t->sname, new_T(t->cspi2->name));
	}
	if(t->sname == NULL) {
		KNH_INITv(t->sname, t->lname);
	}
	OLD_LOCK(ctx, LOCK_SYSTBL, NULL);
	{
		if(S_startsWith(lname, STEXT("konoha."))) {
			knh_DictSet_append(ctx, DP(ctx->sys)->ClassNameDictSet, t->sname, cid+1);
		}
		knh_DictSet_set(ctx, DP(ctx->sys)->ClassNameDictSet, lname, cid+1);
	}
	OLD_UNLOCK(ctx, LOCK_SYSTBL, NULL);
	if(cid > CLASS_Gamma) {
		DBG_SYSLOG("class=%d, lname=%s, sname=%s", cid, S_tochar(lname), S_tochar(t->sname));
	}
	if(t->fdefnull == NULL) {
		t->fdefnull = Fdefnull_newValue;
	}
}

/* ------------------------------------------------------------------------ */
/* [name] */

knh_class_t knh_getcid(CTX ctx, knh_bytes_t lname)
{
	knh_uintptr_t cid = knh_DictSet_get(ctx, DP(ctx->sys)->ClassNameDictSet, lname);
	if(cid > 0) return (knh_class_t)(cid-1);
#if defined(K_USING_SEMANTICS)
	if(lname.buf[lname.len-1] == '}') {
		return knh_findcidx(ctx, lname);
	}
#endif/*K_USING_SEMANTICS*/
	return CLASS_unknown;
}

/* ------------------------------------------------------------------------ */
/* [default] */

static knh_Object_t *Fdefnull_getConst(CTX ctx, knh_class_t cid)
{
	return ClassTBL(cid)->defnull;
}

static knh_Object_t *Fdefnull_newValue(CTX ctx, knh_class_t cid)
{
	knh_ClassTBL_t *ct = varClassTBL(cid);
	Object *v = new_Object_init2(ctx, (const knh_ClassTBL_t*)ct);
	Object_setNullObject(v, 1);
	DBG_P("create new default value of %s", CLASS__(cid));
	KNH_ASSERT(ct->defnull == NULL);
	KNH_INITv(ct->defnull, v);
	ct->fdefnull = Fdefnull_getConst;
	return ct->defnull;
}

void knh_setClassDefaultValue_(CTX ctx, knh_class_t cid, Object *value, knh_Fdefnull f)
{
	DBG_ASSERT_cid(cid);
	knh_ClassTBL_t *t = varClassTBL(cid);
	if(value == NULL) {
		if(f == NULL) f = Fdefnull_newValue;
	}
	else {
		KNH_ASSERT(t->defnull == NULL);
		KNH_INITv(t->defnull, value);
		if(f == NULL) f = Fdefnull_getConst;
	}
	t->fdefnull = f;
}

Object *knh_getClassDefaultValue(CTX ctx, knh_class_t cid)
{
	return ClassTBL(cid)->fdefnull(ctx, cid);
}

void knh_ClassTBL_setCSPI2(knh_ClassTBL_t *ct, const knh_ObjectSPI2_t *cspi2)
{
	ct->cspi2 = cspi2;
	if(cspi2->getDefaultNull != NULL) {
		ct->fdefnull = cspi2->getDefaultNull;
	}
	ct->fast_reftrace = cspi2->reftrace;
}

/* ------------------------------------------------------------------------ */

//static int knh_ClassMap_util_cmp(const knh_TypeMap_t *m1, const knh_TypeMap_t *m2)
//{
//	int res = DP(m1)->flag - DP(m2)->flag;
//	return (res == 0) ? SP(m2)->tcid - SP(m1)->tcid : res;
//}

///* ------------------------------------------------------------------------ */
//
//void knh_ClassMap_sort(CTX ctx, knh_ClassMap_t *cmap)
//{
//	if(!ClassMap_isSorted(cmap)) {
//		knh_qsort((cmap)->maplist, (cmap)->size, sizeof(knh_TypeMap_t*),
//					(int (*)(const void*, const void*))knh_ClassMap_util_cmp);
//		ClassMap_setSorted(cmap, 1);
//	}
//}

///* ------------------------------------------------------------------------ */
//
//static knh_Array_t* knh_Class_domain(CTX ctx)
//{
//	knh_Array_t *a = new_Array(ctx, CLASS_Class, 0);
//	size_t cid = 0;
//	for(cid = 0; cid < ctx->share->sizeClassTBL; cid++) {
//		if(class_isPrivate(cid) || class_isTypeVariable(cid)) continue;
//		knh_Array_add_(ctx, a, UPCAST(new_Type(ctx, cid)));
//	}
//	return a;
//}
//
///* ------------------------------------------------------------------------ */
//
//static knh_Array_t* knh_Method_domain(CTX ctx)
//{
//	knh_Array_t *a = new_Array(ctx, CLASS_Method, 0);
//	size_t cid = 0;
//	for(cid = 0; cid < ctx->share->sizeClassTBL; cid++) {
//		knh_Array_t *ma = ClassTBL(cid)->methods;
//		size_t i;
//		for(i = 0; i < knh_Array_size(ma); i++) {
//			knh_Method_t *mtd = (knh_Method_t*)knh_Array_n(ma, i);
//			if((mtd)->cid == cid) {
//				knh_Array_add_(ctx, a, UPCAST(mtd));
//			}
//		}
//	}
//	return a;
//}
//
///* ------------------------------------------------------------------------ */
//
//knh_Array_t* knh_getClassDomain(CTX ctx, knh_class_t cid)
//{
//	knh_Array_t *a = NULL;
//	switch(cid) {
//	case CLASS_Class:
//		a = knh_Class_domain(ctx);
//		break;
//	case CLASS_Method:
//		a = knh_Method_domain(ctx);
//		break;
//	default:
//		a = NULL;
//	}
//	if(a == NULL) {
//		DBG_P("Empty domain cid=%s", CLASS__(cid));
//		a = new_Array0(ctx, 0);
//	}
//	return a;
//}

/* ------------------------------------------------------------------------ */
/* [instaceof] */

knh_bool_t ClassTBL_isa_(CTX ctx, const knh_ClassTBL_t *ct, const knh_ClassTBL_t *ct2)
{
	knh_class_t reqt = ct2->cid;
	if(reqt == CLASS_Object || reqt == CLASS_dynamic) return 1;
	DBG_ASSERT(ct->cid != CLASS_Tvoid); DBG_ASSERT(ct->cid != CLASS_Tvar);
	//if(cid == CLASS_Tvoid) return 0;
//  if(ClassTBL(scid)->bcid == tcid) return 1; /* Int:km Int */
	if(ct->bcid == CLASS_Tuple && ct2->bcid == CLASS_Tuple) {
		if(ct2->cparam->psize < ct->cparam->psize) {
			size_t i;
			for(i = 0; i < ct2->cparam->psize; i++) {
				knh_param_t *p = knh_ParamArray_get(ct->cparam, i);
				knh_param_t *p2 = knh_ParamArray_get(ct2->cparam, i);
				if(p->type != p2->type) return 0;
			}
			return 1;
		}
		return 0;
	}
	if(ct->bcid == CLASS_Func && ct2->bcid == CLASS_Func) {
		if(ct2->cparam->psize == ct->cparam->psize) {
			size_t i;
			for(i = 0; i < ct2->cparam->psize; i++) {
				knh_param_t *p = knh_ParamArray_get(ct->cparam, i);
				knh_param_t *p2 = knh_ParamArray_get(ct2->cparam, i);
				if(!class_isa(p->type, p2->type)) {
					return 0;
				}
			}
			if(ct->cparam->rsize == 0) return 1;  // Func<int=>int> of Func<int>
			if(ct2->cparam->rsize == ct->cparam->rsize) {
				if(ct->cparam->rsize == 1) {
					knh_param_t *r = knh_ParamArray_rget(ct->cparam, 0);
					knh_param_t *r2 = knh_ParamArray_rget(ct2->cparam, 0);
					if(r->type != r2->type) return 0;
				}
				return 1;
			}
		}
		return 0;
	}
	while(ct->cid != CLASS_Object) {
		knh_Array_t *a = ct->tmaps;
		size_t i;
		for(i = 0; i < knh_Array_size(a); i++) {
			knh_TypeMap_t *tmap = a->trans[i];
			if(SP(tmap)->tcid == reqt && DP(tmap)->flag == FLAG_TypeMap_Interface) return 1;
		}
		if(ct == ct->supTBL) return 0;
		ct = ct->supTBL;
		if(ct->cid == reqt) return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
/* [class param] */

int class_isGenerics(CTX ctx, knh_class_t cid)
{
	DBG_ASSERT_cid(cid);
	return (ClassTBL(cid)->cparam != NULL && IS_ParamArray(ClassTBL(cid)->cparam));
}

knh_class_t knh_class_p(CTX ctx, knh_class_t cid, size_t n)
{
	knh_param_t *p = knh_ParamArray_get(ClassTBL(cid)->cparam, n);
	return p->type;
}

static void knh_write_cname(CTX ctx, knh_OutputStream_t *w, knh_class_t cid, knh_Fwritecid f)
{
	if(cid < ctx->share->sizeClassTBL) {
		knh_class_t bcid = knh_class_bcid(cid);
		if(class_isGenerics(ctx, cid) && cid != bcid) {
			size_t i;
			knh_ParamArray_t *pa = ClassTBL(cid)->cparam;
			f(ctx, w, bcid);
			knh_putc(ctx, w, '<');
			for(i = 0; i < pa->psize; i++) {
				knh_param_t *p = knh_ParamArray_get(pa, i);
				if(i > 0) knh_putc(ctx, w, ',');
				knh_write_cname(ctx, w, CLASS_t(p->type), f);
			}
			if(pa->rsize > 0) {
				if(pa->psize == 0) knh_write(ctx, w, STEXT("void"));
				knh_write(ctx, w, STEXT("=>"));
			}
			for(i = 0; i < pa->rsize; i++) {
				knh_param_t *p = knh_ParamArray_rget(pa, i);
				if(i > 0) knh_putc(ctx, w, ',');
				knh_write_cname(ctx, w, CLASS_t(p->type), f);
			}
			knh_putc(ctx, w, '>');
			return;
		}
	}
	f(ctx, w, cid);
}

/* ------------------------------------------------------------------------ */

static void SimTBL_append(CTX ctx, knh_class_t cid, knh_ClassTBL_t *newt)
{
	knh_ClassTBL_t *t = varClassTBL(cid);
	while(t->simTBL != NULL) {
		t = (knh_ClassTBL_t*)t->simTBL;
	}
	t->simTBL = (const knh_ClassTBL_t*)newt;
}

void knh_setClassParam(CTX ctx, knh_ClassTBL_t *t, knh_ParamArray_t *pa)
{
	size_t i, size = pa->psize + pa->rsize;
	KNH_INITv(t->cparam, pa);
	if(pa->psize > 0) {
		knh_param_t *p = knh_ParamArray_get(pa, 0);
		t->p1 = CLASS_t(p->type);
	}
	if(pa->psize > 1) {
		knh_param_t *p = knh_ParamArray_get(pa, 1);
		t->p2 = CLASS_t(p->type);
	}
	for(i = 0; i < size; i++) {
		knh_param_t *p = knh_ParamArray_get(pa, i);
		if((p->type) >= TYPE_T1) {
			t->cflag |= FLAG_Class_TypeVariable;
		}
	}
}

static void write_tuplecid(CTX ctx, knh_OutputStream_t *w, knh_class_t cid)
{
	if(cid == CLASS_Tuple) {
		knh_write_text(ctx, w, "konoha.Tuple");
	}
	else {
		knh_write_cid(ctx, w, cid);
	}
}

static void write_tupletype(CTX ctx, knh_OutputStream_t *w, knh_class_t cid)
{
	if(cid == CLASS_Tuple) {
		knh_write_text(ctx, w, "Tuple");
	}
	else {
		knh_write_type(ctx, w, cid);
	}
}

knh_class_t knh_addGenericsClass(CTX ctx, knh_class_t cid, knh_class_t bcid, knh_ParamArray_t *pa)
{
	knh_Fwritecid write_cid = knh_write_cid;
	knh_Fwritecid write_type = knh_write_type;
	if(cid == CLASS_newid) cid = new_ClassId(ctx);
	DBG_ASSERT(bcid < cid);
	{
		knh_ClassTBL_t *ct = varClassTBL(cid);
		const knh_ClassTBL_t *bct = ClassTBL(bcid);
		ct->magicflag  = bct->magicflag;
		ct->cflag  = bct->cflag;
		knh_ClassTBL_setCSPI2(ct, bct->cspi2);
		ct->bcid   = bcid;
		ct->baseTBL = bct;
		ct->supcid = bct->supcid;
		ct->supTBL = ClassTBL(ct->supcid);
		knh_setClassParam(ctx, ct, pa);
		SimTBL_append(ctx, bcid, ct);
		if(bcid == CLASS_Tuple && cid != CLASS_Tuple) {
			size_t i, fi = 0;
			knh_Tuple_t *tpl = (knh_Tuple_t*)new_hObject_(ctx, (knh_ClassTBL_t*)ct);
			Object_setNullObject(tpl, 1);
			knh_setClassDefaultValue(ctx, cid, tpl, NULL);
			ct->fcapacity = pa->psize;
			DBLNDATA_(for(i = 0; i < pa->psize; i++) {
				knh_param_t *p = knh_ParamArray_get(pa, i);
				if(IS_Tunbox(p->type)) ct->fcapacity += 1;
			})
			ct->fsize = ct->fcapacity;
			ct->fields = (knh_fields_t*)KNH_MALLOC(ctx, sizeof(knh_fields_t) * ct->fcapacity);
			if(ct->fcapacity <= K_SMALLOBJECT_FIELDSIZE) {
				tpl->fields = &(tpl->smallobject);
			}
			else {
				tpl->fields = (knh_Object_t**)KNH_MALLOC(ctx, sizeof(knh_Object_t*) * ct->fcapacity);
			}
			knh_bzero(tpl->fields, sizeof(knh_Object_t*) * ct->fcapacity);
			for(i = 0; i < pa->psize; i++) {
				knh_param_t *p = knh_ParamArray_get(pa, i);
				ct->fields[fi].flag = FLAG_Field_Getter;
				ct->fields[fi].type = p->type;
				ct->fields[fi].fn = FN_index(ctx, i);
				ct->fields[fi].israw = 0;
				if(IS_Tunbox(p->type)) {
					ct->fields[fi].israw = 1;
					DBLNDATA_(fi++;
					ct->fields[fi].flag = 0;
					ct->fields[fi].fn = FN_NONAME;
					ct->fields[fi].type = TYPE_void;
					ct->fields[fi].israw = 1;)
				}
				else {
					KNH_INITv(tpl->fields[fi], KNH_NULVAL(CLASS_t(p->type)));
				}
				fi++;
			}
			KNH_INITv(ct->methods, KNH_EMPTYLIST);
			write_cid = write_tuplecid;
			write_type = write_tupletype;
		}
		else {
			ct->fields = bct->fields;
			ct->fsize  = bct->fsize;
			ct->fcapacity = bct->fcapacity;
			KNH_INITv(ct->methods, bct->methods);
		}
		KNH_INITv(ct->tmaps, KNH_EMPTYLIST);
	}
	{
		knh_cwb_t cwbbuf, *cwb = knh_cwb_open(ctx, &cwbbuf);
		knh_String_t *lname, *sname;
		knh_write_cname(ctx, cwb->w, cid, write_cid);
		lname = knh_cwb_newString(ctx, cwb);
		knh_write_cname(ctx, cwb->w, cid, write_type);
		sname = knh_cwb_newString(ctx, cwb);
		knh_setClassName(ctx, cid, lname, sname);
	}
	return cid;
}

/* ------------------------------------------------------------------------ */

knh_class_t knh_class_Generics(CTX ctx, knh_class_t bcid, knh_ParamArray_t *pa)
{
	const knh_ClassTBL_t *t = ClassTBL(bcid);
	while(t != NULL) {
		if(t->cparam != NULL && knh_ParamArray_equalsType(pa, t->cparam)) {
			return t->cid;
		}
		t = t->simTBL;
	}
	return knh_addGenericsClass(ctx, CLASS_newid, bcid, pa);
}

/* ------------------------------------------------------------------------ */

knh_class_t knh_class_P1(CTX ctx, knh_class_t bcid, knh_type_t p1)
{
	const knh_ClassTBL_t *t = ClassTBL(bcid);
	while(t != NULL) {
		if(t->p1 == p1) return t->cid;
		t = t->simTBL;
	}
	{
		knh_ParamArray_t *bpa = ClassTBL(bcid)->cparam;
		knh_param_t *bp = knh_ParamArray_get(bpa, 0);
		knh_ParamArray_t *pa = new_ParamArray(ctx);
		knh_param_t p = {p1, bp->fn};
		knh_ParamArray_add(ctx, pa, p);
		return knh_addGenericsClass(ctx, CLASS_newid, bcid, pa);
	}
}

/* ------------------------------------------------------------------------ */

knh_bool_t knh_ParamArray_hasTypeVar(knh_ParamArray_t *pa)
{
	size_t i, size = pa->psize + pa->rsize;
	for(i = 0; i < size; i++) {
		knh_param_t *p = knh_ParamArray_get(pa, i);
		if(!(p->type < TYPE_T0)) return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

void knh_ParamArray_tocid(CTX ctx, knh_ParamArray_t *pa, knh_class_t this_cid, knh_ParamArray_t *npa)
{
	size_t i, size = pa->psize + pa->rsize;
	for(i = 0; i < size; i++) {
		knh_param_t *cp = knh_ParamArray_get(pa, i);
		knh_param_t np = *cp;
		np.type = knh_type_tocid(ctx, np.type, this_cid);
		knh_ParamArray_add(ctx, npa, np);
	}
	npa->psize = pa->psize;
	npa->rsize = pa->rsize;
}

/* ------------------------------------------------------------------------ */

knh_class_t knh_type_tocid(CTX ctx, knh_type_t ptype, knh_class_t this_cid)
{
	if(ptype == TYPE_This) {
		return this_cid;
	}
	else if(ptype > TYPE_This) {
		long ti = (long)ptype - (long)TYPE_T1;
		knh_ParamArray_t *tpa = ClassTBL(this_cid)->cparam;
		if(tpa != NULL && ti < (tpa)->psize + (tpa)->rsize) {
			knh_param_t *p = knh_ParamArray_get(tpa, ti);
			//DBG_P("this_cid=%d, p%d=%d", this_cid, ti+1, p->type);
			return CLASS_t(p->type);
		}
		return CLASS_Tvoid;
	}
	else {
		knh_class_t pcid = CLASS_t(ptype);
		if(class_isGenerics(ctx, pcid)) { /* Iterator<This> */
			BEGIN_LOCAL(ctx, lsfp, 1);
			knh_ParamArray_t *npa = new_ParamArray(ctx);
			KNH_SETv(ctx, lsfp[0].o, npa);
			knh_ParamArray_tocid(ctx, ClassTBL(pcid)->cparam, this_cid, npa);
			pcid = knh_class_Generics(ctx, knh_class_bcid(pcid), npa);
			END_LOCAL_(ctx, lsfp);
		}
		return pcid;
	}
}

/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* [ParamArray] */

knh_ParamArray_t *new_ParamArrayR0(CTX ctx, knh_type_t t)
{
	knh_ParamArray_t *pa = new_ParamArray(ctx);
	knh_param_t p = {t, FN_return};
	knh_ParamArray_radd(ctx, pa, p);
	return pa;
}

/* ------------------------------------------------------------------------ */

knh_ParamArray_t *new_ParamArrayP1(CTX ctx, knh_type_t rtype, knh_type_t p1, knh_fieldn_t fn1)
{
	knh_ParamArray_t *mp = new_ParamArray(ctx);
	knh_param_t p = {p1, fn1}, 	r = {rtype, FN_return};
	knh_ParamArray_add(ctx, mp, p);
	knh_ParamArray_radd(ctx, mp, r);
	return mp;
}

/* ------------------------------------------------------------------------ */

void knh_ParamArray_add(CTX ctx, knh_ParamArray_t *pa, knh_param_t p)
{
	size_t size = pa->psize + pa->rsize;
	if(size == 0) {
		pa->p0 = p;
		pa->p2 = p;  // last
	}
	else if(size == 1) {
		pa->p1 = p;
		pa->p2 = p;  // last
	}
	else if(size == 2) {
		pa->p2 = p;
	}
	else if(size == 3) {
		knh_param_t *params = (knh_param_t*)KNH_MALLOC(ctx, K_FASTMALLOC_SIZE);
		params[0] = pa->p0;
		params[1] = pa->p1;
		params[2] = pa->p2;
		params[3] = p;
		pa->capacity = K_FASTMALLOC_SIZE / sizeof(knh_param_t);
		pa->params = params;
	}
	else {
		if(pa->capacity == size) {
			pa->params = (knh_param_t*)KNH_REALLOC(ctx, pa->params, pa->capacity, pa->capacity*2, sizeof(knh_param_t));
			pa->capacity *= 2;
		}
		pa->params[size] = p;
	}
	pa->psize += 1;
}

/* ------------------------------------------------------------------------ */

void knh_ParamArray_radd(CTX ctx, knh_ParamArray_t *pa, knh_param_t p)
{
	knh_ParamArray_add(ctx, pa, p);
	pa->psize -= 1;
	pa->rsize += 1;
}

/* ------------------------------------------------------------------------ */

knh_param_t* knh_ParamArray_get(knh_ParamArray_t *pa, size_t n)
{
	size_t size = pa->psize + pa->rsize;
	DBG_ASSERT(n < size);
	if(size <= 3) {
		switch(n) {
			case 0: return &pa->p0;
			case 1: return &pa->p1;
			case 2: return &pa->p2;
			default: return &pa->p2;
		}
	}
	else {
		return pa->params + n;
	}
}

/* ------------------------------------------------------------------------ */

knh_type_t knh_ParamArray_getptype(knh_ParamArray_t *pa, size_t n)
{
	if(!(n < pa->psize)) {
		if(pa->psize == 0) return TYPE_dynamic;
		n = pa->psize-1;
	}
	return knh_ParamArray_get(pa, n)->type;
}

/* ------------------------------------------------------------------------ */

knh_bool_t knh_ParamArray_equalsType(knh_ParamArray_t *pa, knh_ParamArray_t *pa2)
{
	size_t i;
	if(pa->psize != pa2->psize || pa->rsize != pa2->rsize) return 0;
	for(i = 0; i < pa->psize; i++) {
		knh_param_t *p = knh_ParamArray_get(pa, i);
		knh_param_t *p2 = knh_ParamArray_get(pa2, i);
		if(p->type != p2->type) return 0;
	}
	for(i = 0; i < pa->rsize; i++) {
		knh_param_t *p = knh_ParamArray_rget(pa, i);
		knh_param_t *p2 = knh_ParamArray_rget(pa2, i);
		if(p->type != p2->type) return 0;
	}
	return 1;
}

/* ------------------------------------------------------------------------ */

knh_type_t knh_ParamArray_rtype(knh_ParamArray_t *pa)
{
	if(pa->rsize != 0) {
		knh_param_t *p = knh_ParamArray_rget(pa, 0);
		return p->type;
	}
	if(ParamArray_isRVAR(pa)) return TYPE_var;
	return TYPE_void;
}

/* ------------------------------------------------------------------------ */

void knh_write_mn(CTX ctx, knh_OutputStream_t *w, knh_methodn_t mn)
{
	knh_bytes_t t = B(MN__(mn));
	if(MN_isFMT(mn)) {
		knh_putc(ctx, w, '%');
		knh_write(ctx, w, t);
	}
	else if(MN_isISBOOL(mn)) {
		knh_write(ctx, w, STEXT("is"));
		knh_write_cap(ctx, w, t);
	}
	else if(MN_isGETTER(mn)) {
		knh_write(ctx, w, STEXT("get"));
		knh_write_cap(ctx, w, t);
	}
	else if(MN_isSETTER(mn)) {
		knh_write(ctx, w, STEXT("set"));
		knh_write_cap(ctx, w, t);
	}
	else {
		knh_write(ctx, w, t);
	}
}

/* ------------------------------------------------------------------------ */
/* [Method] */

static METHOD Fmethod_abstract(CTX ctx, knh_sfp_t *sfp, long rix)
{
	knh_Method_t *mtd = sfp[K_MTDIDX].mtdNC;
	knh_type_t rtype = knh_type_tocid(ctx, knh_ParamArray_rtype(DP(mtd)->mp), O_cid(sfp[0].o));
	//KNH_SYSLOG(ctx, LOG_NOTICE, "AbstractMethod", "%C.%M", mtd->cid, mtd->mn);
	if(rtype != TYPE_void) {
		if(IS_Tunbox(rtype)) {
			RETURNi_(K_INT0);
		}
		else {
			RETURN_(KNH_NULVAL(CLASS_t(rtype)));
		}
	}
}

/* ------------------------------------------------------------------------ */

knh_bool_t Method_isAbstract(knh_Method_t *mtd)
{
	return (mtd->fcall_1 == Fmethod_abstract);
}

void Method_setFunc(CTX ctx, knh_Method_t *mtd, knh_Fmethod func)
{
	func = (func == NULL) ? Fmethod_abstract : func;
	(mtd)->fcall_1 = func;
	(mtd)->pc_start = CODE_FUNCCALL;
	DBG_ASSERT((mtd)->pc_start != NULL);
}

void knh_Method_toAbstract(CTX ctx, knh_Method_t *mtd)
{
	KNH_SETv(ctx, DP(mtd)->kcode, KNH_NULL);
	Method_setFunc(ctx, mtd, Fmethod_abstract);
}

/* ------------------------------------------------------------------------ */

knh_Method_t* new_Method(CTX ctx, knh_flag_t flag, knh_class_t cid, knh_methodn_t mn, knh_Fmethod func)
{
	knh_Method_t* mtd = new_(Method);
	DP(mtd)->flag  = flag;
	(mtd)->cid     = cid;
	(mtd)->mn      = mn;
	Method_setFunc(ctx, mtd, func);
	return mtd;
}

/* ------------------------------------------------------------------------ */
/* [VirtualField] */

static METHOD Fmethod_getter(CTX ctx, knh_sfp_t *sfp, long rix)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	RETURN_((sfp[0].ox)->fields[delta]);
}
static METHOD Fmethod_ngetter(CTX ctx, knh_sfp_t *sfp, long rix)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	knh_ndata_t *data = (knh_ndata_t*)(&(sfp[0].ox->fields[delta]));
	RETURNd_(data[0]);
}
static METHOD Fmethod_setter(CTX ctx, knh_sfp_t *sfp, long rix)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	KNH_SETv(ctx, (sfp[0].ox)->fields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static METHOD Fmethod_nsetter(CTX ctx, knh_sfp_t *sfp, long rix)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	knh_ndata_t *ndata = (knh_ndata_t*)(&((sfp[0].ox)->fields[delta]));
	ndata[0] = sfp[1].ndata;
	RETURNd_(ndata[0]);
}

static knh_Method_t *new_GetterMethod(CTX ctx, knh_class_t cid, knh_methodn_t mn, knh_type_t type, int idx)
{
	knh_Fmethod f = (IS_Tunbox(type)) ? Fmethod_ngetter : Fmethod_getter;
	knh_Method_t *mtd = new_Method(ctx, 0, cid, mn, f);
	DP(mtd)->delta = idx;
	KNH_SETv(ctx, DP(mtd)->mp, new_ParamArrayR0(ctx, type));
	return mtd;
}

static knh_Method_t *new_SetterMethod(CTX ctx, knh_class_t cid, knh_methodn_t mn, knh_type_t type, int idx)
{
	knh_Fmethod f = (IS_Tunbox(type)) ? Fmethod_nsetter : Fmethod_setter;
	knh_Method_t *mtd = new_Method(ctx, 0, cid, mn, f);
	DP(mtd)->delta = idx;
	KNH_SETv(ctx, DP(mtd)->mp, new_ParamArrayP1(ctx, type, type, FN_v));
	return mtd;
}

knh_index_t knh_Method_indexOfGetterField(knh_Method_t *o)
{
	knh_Fmethod f = SP(o)->fcall_1;
	if(f== Fmethod_getter || f == Fmethod_ngetter) {
		return (knh_index_t)DP(o)->delta;
	}
	return -1;
}

knh_index_t knh_Method_indexOfSetterField(knh_Method_t *o)
{
	knh_Fmethod f = SP(o)->fcall_1;
	if(f == Fmethod_setter || f == Fmethod_nsetter) {
		return (knh_index_t)DP(o)->delta;
	}
	return -1;
}

knh_fieldn_t FN_index(CTX ctx, size_t idx)
{
	switch(idx) {
		case 0: return FN_first;
		case 1: return FN_second;
		case 2: return FN_third;
		default: {
			knh_bytes_t t;
			char buf[20];
			knh_snprintf(buf, sizeof(buf), "%d", (int)idx);
			t.text = (const char*)buf; t.len = knh_strlen(buf);
			return knh_getfnq(ctx, t, FN_NEWID);
		}
	}
}

knh_Method_t* knh_ClassTBL_getTupleGetter(CTX ctx, const knh_ClassTBL_t *t, knh_methodn_t mn, size_t idx)
{
	size_t i = 0, ti = 0;
	for(i = 0; i < t->fsize; i++) {
		if(ti == idx) {
			knh_Method_t *mtd = new_GetterMethod(ctx, t->cid, mn, t->fields[i].type, i);
			knh_ClassTBL_addMethod(ctx, t, mtd, 0);
			return mtd;
		}
		if(t->fields[i].type == TYPE_void) continue;
		ti++;
	}
	return NULL;
}

/* ------------------------------------------------------------------------ */

static METHOD Fmethod_NoSuchMethod(CTX ctx, knh_sfp_t *sfp, long rix)
{
	knh_Method_t *mtd = sfp[K_MTDIDX].mtdNC;
	KNH_ASSERT(IS_Method(mtd));
	knh_class_t cid = Method_isStatic(mtd) ? (mtd)->cid : O_cid(sfp[0].o);
	THROW_NoSuchMethod(ctx, sfp, cid, (mtd)->mn);
}

static knh_Method_t* new_NoSuchMethod(CTX ctx, knh_class_t cid, knh_methodn_t mn)
{
	knh_Method_t *mtd = new_Method(ctx, 0, cid, mn, Fmethod_NoSuchMethod);
	return mtd;
}

/* ------------------------------------------------------------------------ */

static inline knh_Method_t *Cache_setMethod(knh_mtdcache_t *mcache, knh_class_t cid, knh_methodn_t mn, knh_Method_t *mtd)
{
	DBG_ASSERT(mtd != NULL);
	knh_hashcode_t h = hashcode_mtd(cid, mn, K_MTDCACHE_SIZE);
	mcache = mcache + h;
	DBG_(
	if(mcache->cid != cid || mcache->mn != mn) {
		if(mcache->cid != 0 && mcache->mn != 0) {
			//DBG_P("MISSED h=%d mcache=%d.%d %d.%d", h, mcache->cid, mcache->mn, cid, mn);
		}
	})
	mcache->cid = cid;
	mcache->mn  = mn;
	mcache->mtd = mtd;
	return mtd;
}

void knh_ClassTBL_addMethod(CTX ctx, const knh_ClassTBL_t *t, knh_Method_t *mtd, int isCHECK)
{
	DBG_ASSERT(IS_Method(mtd));
	knh_Array_t *a = t->methods;
	if(unlikely(a == KNH_EMPTYLIST)) {
		KNH_ASSERT(knh_Array_size(a) == 0);
		a = new_Array0(ctx, K_FASTMALLOC_SIZE/sizeof(knh_Method_t*));
		KNH_SETv(ctx, ((knh_ClassTBL_t*)t)->methods, a);
	}
	if(isCHECK) {
		size_t i;
		for(i = 0; i < knh_Array_size(a); i++) {
			knh_Method_t *mtd2 = a->methods[i];
			if((mtd2)->mn == (mtd)->mn) {
				KNH_SYSLOG(ctx, LOG_WARNING, "MethodRedefinition", "%C.%M", (mtd)->cid, (mtd)->mn);
				return ;
			}
		}
	}
	if(FLAG_is(t->cflag, FLAG_Class_Singleton)) {
		DP(mtd)->flag = DP(mtd)->flag | FLAG_Method_Static;
	}
	knh_Array_add(ctx, a, mtd);
	Cache_setMethod(ctx->mtdcache, mtd->cid, mtd->mn, mtd);
}

knh_Method_t* knh_ClassTBL_findMethodNULL(CTX ctx, const knh_ClassTBL_t *t, knh_methodn_t mn, int isGEN)
{
	{
		knh_class_t cid = t->cid;
		knh_hashcode_t h = hashcode_mtd(cid, mn, K_MTDCACHE_SIZE);
		knh_mtdcache_t *mcache = ctx->mtdcache + h;
		if(mcache->cid == t->cid && mcache->mn == mn) {
			//DBG_P("HIT h=%d mcache=%d.%d %d.%d", h, mcache->cid, mcache->mn, t->cid, mn);
			KNH_ASSERT(IS_Method(mcache->mtd));
			return mcache->mtd;
		}
	}
	{
		const knh_ClassTBL_t *p, *t0 = t;
		do {
			size_t i;
			knh_Array_t *a = t0->methods;
			for(i = 0; i < knh_Array_size(a); i++) {
				knh_Method_t *mtd = a->methods[i];
				if((mtd)->mn == mn) {
					return Cache_setMethod(ctx->mtdcache, t->cid, mn, mtd);
				}
			}
			p = t0;
			t0 = t0->supTBL;
		}
		while(p != t0);
	}
	if(t->fsize > 0) {
		if(MN_isGETTER(mn) || MN_isISBOOL(mn)) {
			knh_index_t idx;
			knh_fieldn_t fn = MN_toFN(mn);
			for(idx = (knh_index_t)t->fsize - 1; idx >= 0 ; idx--) {
				knh_fields_t *cf = t->fields + idx;
				if(cf->fn == fn) {
					if(MN_isISBOOL(mn) && !IS_Tbool(cf->type)) {
						continue;
					}
					if(!FLAG_is(cf->flag, FLAG_Field_Getter)) {
						continue;
					}
					knh_Method_t *mtd = new_GetterMethod(ctx, t->cid, mn, cf->type, idx);
					knh_ClassTBL_addMethod(ctx, t, mtd, 0);
					return Cache_setMethod(ctx->mtdcache, t->cid, mn, mtd);
				}
			}
			goto L_NoSuchMethod;
		}
		if(MN_isSETTER(mn)) {
			knh_index_t idx;
			knh_fieldn_t fn = MN_toFN(mn);
			for(idx = (knh_index_t)t->fsize - 1; idx >= 0 ; idx--) {
				knh_fields_t *cf = t->fields + idx;
				if(cf->fn == fn) {
					if(!FLAG_is(cf->flag, FLAG_Field_Setter)) {
						continue;
					}else {
						knh_Method_t *mtd = new_SetterMethod(ctx, t->cid, mn, cf->type, idx);
						knh_ClassTBL_addMethod(ctx, t, mtd, 0);
						return Cache_setMethod(ctx->mtdcache, t->cid, mn, mtd);
					}
				}
			}
		}
	}
	L_NoSuchMethod:;
	if(isGEN) {
		if(MN_isFMT(mn)) {
			DBG_P("Generating %%empty: cid=%s mn=%%%s", CLASS__(t->cid), MN__(mn));
			return ClassTBL_getMethodNULL(ctx, t, MN__empty);
		}
		else {
			knh_Method_t *mtd = new_NoSuchMethod(ctx, t->cid, mn);
			knh_ClassTBL_addMethod(ctx, t, mtd, 0);
			return Cache_setMethod(ctx->mtdcache, t->cid, mn, mtd);
		}
	}
	else {
		return NULL;
	}
}

knh_Method_t *knh_ClassTBL_getFmtNULL(CTX ctx, const knh_ClassTBL_t *t, knh_methodn_t mn0)
{
	knh_methodn_t mn = mn0;
	if(mn == MN__dump) {
		knh_Method_t *mtd = knh_ClassTBL_findMethodNULL(ctx, t, mn, 0);
		if(mtd != NULL) {
			if(mtd->cid != CLASS_Object || t->bcid == CLASS_Object) {
				return Cache_setMethod(ctx->mtdcache, t->cid, mn0, mtd);
			}
		}
		mn = MN__k;
	}
	if(mn == MN__k) {
		knh_Method_t *mtd = knh_ClassTBL_findMethodNULL(ctx, t, mn, 0);
		if(mtd != NULL) {
			if(mtd->cid != CLASS_Object || t->bcid == CLASS_Object) {
				return Cache_setMethod(ctx->mtdcache, t->cid, mn0, mtd);
			}
		}
		mn = MN__s;
	}
	{
		knh_Method_t *mtd = knh_ClassTBL_findMethodNULL(ctx, t, mn, 0);
		if(mtd != NULL) {
			return Cache_setMethod(ctx->mtdcache, t->cid, mn0, mtd);
		}
	}
	return NULL;
}

knh_Method_t *knh_ClassTBL_getFmt(CTX ctx, const knh_ClassTBL_t *t, knh_methodn_t mn0)
{
	knh_Method_t *mtd = knh_ClassTBL_getFmtNULL(ctx, t, mn0);
	if(mtd == NULL) {
		mtd = knh_ClassTBL_findMethodNULL(ctx, ClassTBL(CLASS_Object), MN__empty, 1);
		mtd = Cache_setMethod(ctx->mtdcache, t->cid, mn0, mtd);
	}
	//DBG_P("%s.%s => %s.%s", CLASS__(t->cid), MN__(mn0), CLASS__(mtd->cid), MN__(mtd->mn));
	return mtd;
}

/* ------------------------------------------------------------------------ */

void knh_NameSpace_addMethod(CTX ctx, knh_class_t mtd_cid, knh_Method_t *mtd)
{
	DBG_ASSERT(mtd_cid == mtd->cid);
	if(Method_isPrivate(mtd)) {
		KNH_TODO("Private Method");
	}
	else {
		//DBG_P("mtd=%p, %s.%s", mtd, CLASS__(mtd_cid), MN__(mtd->mn));
		knh_ClassTBL_addMethod(ctx, ClassTBL(mtd_cid), mtd, 0);
	}
}

knh_Method_t* knh_NameSpace_getMethodNULL(CTX ctx, knh_class_t cid, knh_methodn_t mn)
{
	knh_Method_t *mtd = ClassTBL_getMethodNULL(ctx, ClassTBL(cid), mn);
	return mtd;
}

knh_Method_t* knh_NameSpace_getFmtNULL(CTX ctx, knh_NameSpace_t *ns, knh_class_t cid, knh_methodn_t mn)
{
	L_TAIL:;
	if(DP(ns)->formattersNULL != NULL) {
		knh_Array_t *a = DP(ns)->formattersNULL;
		size_t i;
		for(i = 0; i < knh_Array_size(a); i++) {
			knh_Method_t *mtd = a->methods[i];
			if((mtd)->cid == cid && (mtd)->mn == mn) {
				return mtd;
			}
		}
	}
	if(DP(ns)->parentNULL != NULL) {
		ns = DP(ns)->parentNULL;
		goto L_TAIL;
	}
	return knh_ClassTBL_getFmtNULL(ctx, ClassTBL(cid), mn);
}

void knh_NameSpace_addFmt(CTX ctx, knh_NameSpace_t *ns, knh_Method_t *mtd)
{
	if(DP(ns)->formattersNULL == NULL) {
		KNH_INITv(DP(ns)->formattersNULL, new_Array0(ctx, 0));
	}
	knh_Array_add(ctx, DP(ns)->formattersNULL, mtd);
}

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/* [func] */

static void knh_readyTransMapList(CTX ctx, knh_class_t cid)
{
	DBG_ASSERT_cid(cid);
	knh_ClassTBL_t *t = varClassTBL(cid);
	if(t->tmaps == KNH_EMPTYLIST) {
		KNH_ASSERT(knh_Array_size(t->tmaps) == 0);
		KNH_SETv(ctx, t->tmaps, new_Array0(ctx, 1));
		//KNH_SYSLOG(ctx, LOG_INFO, "[NEW_TMAPLIST] cid=%s", CLASS__(cid));
	}
}

/* ------------------------------------------------------------------------ */

KNHAPI2(void) knh_addTypeMap(CTX ctx, knh_TypeMap_t *trl)
{
	knh_class_t cid = SP(trl)->scid;
	knh_readyTransMapList(ctx, cid);
	knh_Array_add(ctx, ClassTBL(cid)->tmaps, trl);
}

/* ------------------------------------------------------------------------ */

void knh_addTypeMapFunc(CTX ctx, knh_flag_t flag, knh_type_t stype, knh_type_t ttype, knh_Ftmapper ftcast, Object *mapdata)
{
	knh_class_t cid = CLASS_t(stype);
	DBG_ASSERT_cid(cid);
	knh_readyTransMapList(ctx, cid);
	knh_Array_add(ctx, ClassTBL(cid)->tmaps,
		new_TypeMap(ctx, flag, CLASS_t(stype), CLASS_t(ttype), ftcast));
}

/* ------------------------------------------------------------------------ */

static TCAST knh_Ftmapper_null(CTX ctx, knh_sfp_t *sfp, long rix)
{
	KNH_TODO("null translator");
//	DBG_(
//	knh_TypeMap_t *trl = sfp[0].trlNC;
//	DBG_P("trl: %s ==> %s", CLASS__(SP(trl)->scid), CLASS__(SP(trl)->tcid));
//	);
//	KNH_THROW__T(ctx, "ClassCast!!");
}

///* ------------------------------------------------------------------------ */
//
//static TCAST knh_Ftmapper_method(CTX ctx, knh_sfp_t *sfp, long rix)
//{
//	knh_Method_t *mtd = DP(sfp[0].trlNC)->mtd;
//	DBG_ASSERT(IS_Method(mtd));
//	KNH_SETv(ctx, sfp[1+K_CALLDELTA].o, sfp[0].o);
//	KNH_SCALL(ctx, sfp, 1, mtd, /*argc*/, 0);
//	RETURN(ctx, sfp, rtnidx, sfp[1+K_CALLDELTA].o);
//}

/* ------------------------------------------------------------------------ */

KNHAPI2(knh_TypeMap_t*) new_TypeMap(CTX ctx, knh_flag_t flag, knh_class_t scid, knh_class_t tcid, knh_Ftmapper ftcast)
{
	knh_TypeMap_t* trl = new_(TypeMap);
	if(class_isFinal(scid)) {
		DP(trl)->flag = flag | FLAG_TypeMap_Final;
	}
	else {
		DP(trl)->flag = flag;
	}
	SP(trl)->scid = scid;
	SP(trl)->tcid = tcid;
	if(ftcast == NULL) ftcast = knh_Ftmapper_null;
	trl->ftcast_1 = ftcast;
	KNH_SETv(ctx, DP(trl)->mapdata, KNH_NULL);
	return trl;
}

///* ------------------------------------------------------------------------ */
///* [MapMap] */
//
//static TCAST knh_Ftmapper_mapmap(CTX ctx, knh_sfp_t *sfp, long rix)
//{
//	knh_TypeMap_t *trl = sfp[K_TRLIDX].trlNC;
//	DBG_ASSERT(IS_TypeMap(trl));
//	knh_TypeMap_t *m1 = DP(trl)->m1, *m2 = DP(trl)->m2;
//	DBG_ASSERT(IS_TypeMap(m1));
//	KNH_SETv(ctx, sfp[1].o, m1);
//	(m1)->ftcast_1(ctx, sfp);
//	if(IS_NOTNULL(sfp[0].o)) {
//		DBG_ASSERT(IS_TypeMap(m2));
//		KNH_SETv(ctx, sfp[1].o, m2);
//		(m2)->ftcast_1(ctx, sfp);
//	}
//}
//
///* ------------------------------------------------------------------------ */
//
static knh_TypeMap_t* new_MapMap(CTX ctx, knh_TypeMap_t *m1, knh_TypeMap_t *m2)
{
//	knh_TypeMap_t* trl = (knh_TypeMap_t*)new_Object_bcid(ctx, CLASS_TypeMap, 0);
//	KNH_SYSLOG(ctx, LOG_NOTICE, "generated mapper: %C => %C => %C", DP(m1)->scid, DP(m1)->tcid, DP(m2)->tcid);
//	DP(trl)->size = 0;
//	DP(trl)->flag = DP(m1)->flag | DP(m2)->flag;
//	SP(trl)->scid = DP(m1)->scid;
//	SP(trl)->tcid = DP(m2)->tcid;
//	DBG_ASSERT(IS_TypeMap(m1));
//	KNH_SETv(ctx, DP(trl)->m1, m1);
//	DBG_ASSERT(IS_TypeMap(m2));
//	KNH_SETv(ctx, DP(trl)->m2, m2);
//	trl->h.flag = m1->h.flag & m2->h.flag;
//	trl->ftcast_1 = knh_Ftmapper_mapmap;
//	return trl;
	return NULL;
}

/* ------------------------------------------------------------------------ */
/* [cache] */

static inline knh_TypeMap_t *Cache_setTypeMap(CTX ctx, knh_class_t scid, knh_class_t tcid, knh_TypeMap_t *tmap)
{
	knh_hashcode_t h = hashcode_tmap(scid, tcid, K_TMAPCACHE_SIZE);
	knh_tmapcache_t *cache = ctx->tmapcache + h;
	cache->scid = scid;
	cache->tcid = tcid;
	cache->tmap = tmap;
	return tmap;
}

/* ------------------------------------------------------------------------ */

static TCAST Ftmap_asis(CTX ctx, knh_sfp_t *sfp, long rix)
{
	KNH_SWAPMOV(ctx, sfp, rix, 0);
}

/* ------------------------------------------------------------------------ */

static knh_TypeMap_t* new_TypeMapASIS(CTX ctx, knh_class_t scid, knh_class_t tcid)
{
	return new_TypeMap(ctx, 0, scid, tcid, Ftmap_asis);
}

/* ------------------------------------------------------------------------ */

static knh_TypeMap_t* new_TypeMap__NoSuchMapping(CTX ctx, knh_class_t scid, knh_class_t tcid)
{
	return new_TypeMap(ctx, 0, scid, tcid, NULL);
}

/* ------------------------------------------------------------------------ */

knh_bool_t TypeMap_isNoSuchMapping(knh_TypeMap_t *trl)
{
	return ((trl)->ftcast_1 == knh_Ftmapper_null);
}

/* ------------------------------------------------------------------------ */

knh_TypeMap_t *knh_findTypeMapNULL(CTX ctx, knh_class_t scid, knh_class_t tcid, int isGEN)
{
	knh_TypeMap_t *tmap = NULL;
	DBG_P("finding.. %s ==> %s",CLASS__(scid), CLASS__(tcid));
	if(scid == CLASS_Tvoid) return NULL;
	DBG_ASSERT(scid != CLASS_Tvar);
	{
		knh_hashcode_t h = hashcode_tmap(scid, tcid, K_TMAPCACHE_SIZE);
		knh_tmapcache_t *cache = ctx->tmapcache + h;
		if(cache->scid  == scid && cache->tcid == tcid) {
			return cache->tmap;
		}
	}
	DBG_ASSERT_cid(scid); DBG_ASSERT_cid(tcid);
	{
		knh_class_t sbcid = scid;
		while(1) {
			size_t i;
			knh_Array_t *a = ClassTBL(sbcid)->tmaps;
			for(i = 0; i < (a)->size; i++) {
				tmap = (a)->trans[i];
				if(SP(tmap)->tcid == tcid) {
					goto L_SETCACHE;
				}
			}
			for(i = 0; i < (a)->size; i++) {
				tmap = (a)->trans[i];
				if(class_isa(SP(tmap)->tcid, tcid)) {
					goto L_SETCACHE;
				}
			}
			if(sbcid == CLASS_Object) break;
			for(i = 0; i < (a)->size; i++) {
				size_t j;
				knh_class_t mcid = SP((a)->trans[i])->tcid;
				DBG_ASSERT_cid(mcid);
				if(mcid <= CLASS_String) {
					DBG_P("forbid lowlevel transitivity %s", CLASS__(mcid));
					continue;   /* Stop lowlevel inference */
				}
				knh_Array_t *a2 = ClassTBL(mcid)->tmaps;
				for(j = 0; j < (a2)->size; j++) {
					tmap = (a2)->trans[j];
					if(SP(tmap)->tcid == tcid) {
						tmap = new_MapMap(ctx, (a)->trans[i], tmap);
						goto L_ADDTRL;
					}
				}
				for(j = 0; j < (a2)->size; j++) {
					tmap = (a2)->trans[j];
					if(class_isa(SP(tmap)->tcid, tcid)) {
						tmap = new_MapMap(ctx, (a)->trans[i], tmap);
						goto L_ADDTRL;
					}
				}
			}
			if(class_isGenerics(ctx, sbcid) && ClassTBL(sbcid)->bcid != sbcid) {
				sbcid = ClassTBL(sbcid)->bcid;
			}
			else {
				sbcid = ClassTBL(sbcid)->supcid;
			}
		}
	}
	/* GENERATIVE PART */
	if(class_isa(scid, tcid)) {  /* default */
		tmap = new_TypeMapASIS(ctx, scid, tcid);
		goto L_ADDTRL;
	}

	tmap = ClassTBL(scid)->cspi2->findTypeMapNULL(ctx, scid, tcid, 1);
	if(tmap != NULL) {
		goto L_ADDTRL;
	}

	if(isGEN) {
		tmap = new_TypeMap__NoSuchMapping(ctx, scid, tcid);
		goto L_ADDTRL;
	}
	return NULL;

	L_ADDTRL:
	knh_addTypeMap(ctx, tmap);
	L_SETCACHE:
	return Cache_setTypeMap(ctx, scid, tcid, tmap);
}

/* ------------------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif