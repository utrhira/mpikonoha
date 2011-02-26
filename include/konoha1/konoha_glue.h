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

#ifndef KONOHA_GLUE_H_
#define KONOHA_GLUE_H_

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* UTILS_API */

typedef const struct _knh_ExportsAPI_t {
	// memory
	void* (*malloc)(CTX, size_t);
	void  (*free)(CTX, void *, size_t);
	// stack
	void  (*setsfp)(CTX, knh_sfp_t *, void *);
	void  (*closeIterator)(CTX, knh_Iterator_t *);
	// evidence
	void  (*trace)(CTX, int, const char*, const char*, knh_sfp_t *, const char*, ...);
	void  (*perror)(CTX, knh_sfp_t *sfp, const char*, const char*);
	void  (*dbg_p)(const char*, const char*, int, const char*, ...);
	void  (*todo_p)(const char*, const char*, int, const char*, ...);
	// cwbbuf
	void (*cwb_clear)(knh_cwb_t *, size_t);
	const char* (*cwb_tochar)(CTX, knh_cwb_t *);
	// String
	knh_String_t* (*new_String)(CTX, const char*);
	const char* (*tochar)(CTX, knh_String_t*);
	// RawPtr
	knh_RawPtr_t* (*new_RawPtr)(CTX, void*, knh_FfreeRawPtr, knh_class_t, const char*);
	void (*RawPtr_init)(CTX, knh_RawPtr_t*, void *, knh_FfreeRawPtr);
	// Stream
	knh_InputStream_t* (*new_InputStreamNULL)(CTX, knh_String_t *, const char *mode);
	knh_OutputStream_t* (*new_OutputStreamNULL)(CTX, knh_String_t *, const char *mode);
	// OutputStream, Bytes
	void (*putcAPI)(CTX, void *, int);
	void (*writeAPI)(CTX, void *, const char *, size_t);
} knh_ExportsAPI_t;

#ifdef K_EXPORTS
//#define knh_cwb_open(ctx, cwbbuf)   ctx->api->cwb_open(ctx, cwbbuf)
//#define knh_cwb_clear(cwb, s)       ctx->api->cwb_clear(cwb, s)
#define knh_cwb_tochar(ctx, cwb)    ctx->api->cwb_tochar(ctx, cwb)
//#define knh_cwb_tobytes(cwb, s)     ctx->api->cwb_clear(cwb, s)
//#define new_String(ctx, s)          ctx->api->new_String(ctx, s)
//#define S_tochar(s)                 ctx->api->tochar(ctx, s)
#define new_RawPtr(ctx, p, f, cid, n)  ctx->api->new_RawPtr(ctx, p, f, cid, n)
#define knh_RawPtr_init(ctx, rp, p, f) ctx->api->RawPtr_init(ctx, rp, p, f)
#define new_InputStreamNULL(ctx, s, mode)  ctx->api->new_InputStreamNULL(ctx, s, mode)
#define new_OutputStreamNULL(ctx, s, mode)  ctx->api->new_OutputStreamNULL(ctx, s, mode)
//#define knh_putc(ctx, w, ch)         ctx->api->putc(ctx, w, ch)
//#define knh_write(ctx, w, s, len)    ctx->api->write(ctx, w, s, len)
#endif

/* ------------------------------------------------------------------------ */
/* driver */

typedef const struct {
	int   type;
	const char *name;
} knh_DSPI_t ;

#define K_DSPI_PATH             1
#define K_DSPI_CONVTO           2
#define K_DSPI_CONVFROM         3
#define K_DSPI_STREAM           4
#define K_DSPI_REGEX            5
#define K_DSPI_QUERY            6
#define K_DSPI_MAP              7

#define IS_DSPI(c)   (0 < c && c < 8)
#define K_DEFAULT_DSPI          STEXT("")

/* ------------------------------------------------------------------------ */
/* K_DSPI_PATH */

#define K_PATHHEAD_MAXSIZ    32
#define PATH_found          1
#define PATH_unknown        ((knh_uintptr_t)(-1))
#define PATH_isTyped(cid)   (cid == CLASS_Boolean || cid == CLASS_String)

typedef const struct _knh_PathDSPI_t {
	int   type;
	const char *name;
	knh_class_t cid;
	knh_class_t itrcid;
	knh_uintptr_t (*exists)(CTX, knh_bytes_t, knh_NameSpace_t *);
	knh_bool_t    (*isTyped)(CTX, knh_class_t);
	Object*       (*newObjectNULL)(CTX, knh_class_t, knh_String_t *, knh_NameSpace_t *);
} knh_PathDSPI_t;

/* ------------------------------------------------------------------------ */
/* K_BCONV_DSPI */

typedef const struct _knh_ConvDSPI_t {
	int  type;
	const char *name;
	knh_conv_t* (*open)(CTX, const char*, const char*);
	knh_bool_t (*conv)(CTX, knh_conv_t *, knh_bytes_t t, knh_Bytes_t *);
	knh_bool_t (*enc)(CTX, knh_conv_t *, knh_bytes_t t, knh_Bytes_t *);
	knh_bool_t (*dec)(CTX, knh_conv_t *, knh_bytes_t t, knh_Bytes_t *);
	knh_bool_t (*sconv)(CTX, knh_conv_t *, knh_bytes_t t, knh_Bytes_t *);
	void (*close)(CTX ctx, knh_conv_t*);
	void (*setparam)(CTX ctx, knh_conv_t *, void *, void *);
} knh_ConvDSPI_t;

/* ------------------------------------------------------------------------ */
/* K_DSPI_STREAM */

typedef void   (*knh_Fclose)(CTX, knh_io_t);

typedef const struct _knh_StreamDSPI_t {
	int type;
	const char *name;
	size_t bufsiz;  /* knh_io_t == FILE* if bufsiz == 0 */
	knh_io_t (*fopen)(CTX, knh_bytes_t, const char *);
	knh_io_t (*wopen)(CTX, knh_bytes_t, const char *);
	knh_intptr_t (*fread)(CTX, knh_io_t, char *, size_t);
	knh_intptr_t (*fwrite)(CTX, knh_io_t, const char *, size_t);
	void   (*fclose)(CTX, knh_io_t);
	int (*feof)(CTX, knh_io_t);
	int (*fgetc)(CTX, knh_io_t);
	const char* (*getContentType)(CTX, knh_io_t);
	const char* (*getCharset)(CTX, knh_io_t);
} knh_StreamDSPI_t;

/* ------------------------------------------------------------------------ */
/* K_DSPI_QUERY */

typedef const struct _knh_QueryDPI_t {
	int   type;
	const char *name;
	knh_qconn_t* (*qopen)(CTX ctx, knh_bytes_t);
	knh_qcur_t* (*qexec)(CTX ctx, knh_qconn_t *, knh_bytes_t, knh_ResultSet_t*);
	void   (*qclose)(CTX ctx, knh_qconn_t *);
	int    (*qcurnext)(CTX, knh_qcur_t *, knh_ResultSet_t*);
	void   (*qcurfree)(knh_qcur_t *);
} knh_QueryDSPI_t;

/* ------------------------------------------------------------------------ */
/* K_DSPI_MAP */

typedef const struct _knh_MapDSPI_t {
	int   type;
	const char *name;
	const struct _knh_MapDSPI_t* (*config)(CTX, knh_class_t, knh_class_t);
	knh_map_t* (*init)(CTX, size_t, const char*, void *);
	void (*reftrace)(CTX, knh_map_t* FTRARG);
	void (*freemap)(CTX, knh_map_t*);
	knh_bool_t (*get)(CTX, knh_map_t*, knh_sfp_t*, knh_sfp_t *);
	void (*set)(CTX, knh_map_t*, knh_sfp_t *);
	void (*remove)(CTX, knh_map_t*, knh_sfp_t *);
	size_t (*size)(CTX, knh_map_t*);
	knh_bool_t (*setIterator)(CTX, knh_map_t*, knh_Iterator_t *);
} knh_MapDSPI_t;

/* ------------------------------------------------------------------------ */
/* REGEX_SPI */

#ifndef K_REGEX_MATCHSIZE
#define K_REGEX_MATCHSIZE    16
#endif

typedef struct {
	int rm_so;   /* start of match */
	int rm_eo;   /* end of match */
	knh_bytes_t rm_name;  /* {NULL, 0}, if not NAMED */
} knh_regmatch_t;

typedef struct _knh_RegexSPI_t {
	const char *name;
	knh_regex_t* (*regmalloc)(CTX, knh_String_t *);
	int (*parse_cflags)(CTX, const char *opt);
	int (*parse_eflags)(CTX, const char *opt);
	int (*regcomp)(CTX, knh_regex_t *, const char *, int);
	int (*regexec)(CTX, knh_regex_t *, const char *, size_t, knh_regmatch_t*, int);
	size_t (*regerror)(int, knh_regex_t *, char *, size_t);
	void (*regfree)(CTX, knh_regex_t *);
} knh_RegexSPI_t;

/* ------------------------------------------------------------------------ */
/* ConstData  */
/* ------------------------------------------------------------------------ */

typedef const struct {
	const char *name;
	knh_int_t ivalue;
} knh_IntData_t;

typedef const struct {
	const char *name;
	knh_float_t fvalue;
} knh_FloatData_t;

typedef const struct {
	const char *name;
	const char *value;
} knh_StringData_t;

typedef const struct {
	const char *name;
	void *ptr;
} knh_NamedPointerData_t;

#define _DATA(s)    (knh_intptr_t)(s)
#define DATA_END      0
#define DATA_STRUCT0   1
#define DATA_CLASS0    2
#define DATA_CPARAM    3
#define DATA_GENERICS  4
#define DATA_EXPT      5
#define DATA_METHOD0   6
#define DATA_METHOD    7
#define DATA_TCAST     8
#define DATA_PARAM     9

typedef knh_intptr_t knh_data_t;

/* ------------------------------------------------------------------------ */

typedef const struct _knh_PackageLoaderAPI_t {
	/* global */
	void (*loadData)(CTX, knh_data_t *, knh_ParamArray_t **);
	void (*loadIntData)(CTX, knh_IntData_t *);
	void (*loadFloatData)(CTX, knh_FloatData_t *);
	void (*loadStringData)(CTX, knh_StringData_t *);
	/* namespace */
	void (*setRegexSPI)(CTX, const knh_RegexSPI_t *);
	void (*addPathDSPI)(CTX, const char*, knh_PathDSPI_t *, int);
	void (*addStreamDSPI)(CTX, const char*, knh_StreamDSPI_t *, int);
	void (*addQueryDSPI)(CTX, const char *, knh_QueryDSPI_t *, int);
	void (*addConverterDSPI)(CTX, const char *, knh_ConvDSPI_t*, int);
} knh_PackageLoaderAPI_t;

typedef struct {
	const char *name;     /* pacakge name */
	const char *version;  /* pacakge version*/
	const char *url;      /* package url */
	const char *info;     /* package info */
	int buildid;
	int (*importpkg)(CTX, knh_PackageLoaderAPI_t *);
	int (*usingpkg)(CTX, knh_PackageLoaderAPI_t *, knh_NameSpace_t *ns);
} knh_PackageDef_t;

typedef void (*knh_Fsetuppkg)(CTX, knh_PackageLoaderAPI_t *, const char *, int);
typedef const knh_PackageDef_t* (*knh_Fopenpkg)(void);

#define knh_isSelectedDSPI(c, T)   (c == NULL || strstr(c, ":" T ":") != NULL)

typedef struct {
	const char *name;
	knh_ObjectSPI2_t *cspi;  // if cspi is NULL, rawptr is be used instead.
	knh_Fdefnull      fdefault;
} knh_ClassData_t;

typedef const knh_ClassData_t* (*knh_Fclass)(void);



/* ------------------------------------------------------------------------ */
/* new version */

#define Boolean_to(T, a)         ((T)a.bvalue)
#define Int_to(T, a)             ((T)a.ivalue)
#define Float_to(T, a)           ((T)a.fvalue)
#define String_to(T, a)          ((T)ctx->api->tochar(ctx, a.s))
#define StringNull_to(T, a, def) ((T)(IS_bString(a.o) ? ctx->api->tochar(ctx, a.s) : def))
#define RawPtr_to(T, a)            ((T)((a.p)->ptr))
#define RawPtrNull_to(T, a, def)   (IS_bRawPtr(a.o) ? ((T)((a.p)->ptr)) : (def))
#define Class_tocid(a)           ((a.c)->cid)

/* ------------------------------------------------------------------------ */

#ifdef K_EXPORTS

#define RETURN_(vv) {\
		ctx->api->setsfp(ctx, sfp+rix, vv);\
		return; \
	}\

#define RETURNa_(v) {\
		Object *vv_ = (Object*)v;\
		ctx->api->setsfp(ctx, sfp+rix, vv_);\
		sfp[rix].ndata = O_data(vv_);\
		return; \
	}\

#else
#define RETURN_(vv) {\
		KNH_SETv(ctx, sfp[rix].o, vv);\
		return; \
	}\

#define RETURNa_(v) {\
		Object *vv_ = (Object*)v;\
		KNH_SETv(ctx, sfp[rix].o, vv_);\
		sfp[rix].ndata = O_data(vv_);\
		return; \
	}\

#endif

#define RETURNd_(d) {\
		sfp[rix].ndata = d; \
		return; \
	}\

#define RETURNb_(c) {\
		sfp[rix].bvalue = c; \
		return; \
	}\

#define RETURNi_(c) {\
		sfp[rix].ivalue = c; \
		return; \
	}\

#define RETURNf_(c) {\
		sfp[rix].fvalue = c; \
		return; \
	}\

#define RETURNvoid_() {\
		return; \
	}\


#define ITREND_() {\
		ctx->api->closeIterator(ctx, sfp[0].it);\
		return 0; \
	}\

#ifdef K_EXPORTS

#define ITRNEXT_(vv) {\
		ctx->api->setsfp(ctx, sfp+rtnidx, vv);\
		return 1; \
	}\

#else

#define ITRNEXT_(vv) {\
		KNH_SETv(ctx, sfp[rtnidx].o, vv);\
		return 1; \
	}\

#endif

#define ITRNEXTd_(d) {\
		sfp[rtnidx].ndata = d;\
		return 1; \
	}\

#define ITRNEXTi_(n) {\
		sfp[rtnidx].ivalue = n;\
		return 1; \
	}\

#define ITRNEXTf_(n) {\
		sfp[rtnidx].fvalue = n;\
		return 1; \
	}\

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif /*KONOHA_GLUE_H_*/