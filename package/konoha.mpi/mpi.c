#include "konoha_mpi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* ClassDef */

static void knh_MPIComm_init(CTX ctx, kRawPtr *o)
{
	MPIC(comm, o);
	MPIC_COMM(comm) = 0;
	MPIC_RANK(comm) = -1;
	MPIC_SIZE(comm) = -1;
	MPIC_PROC(comm) = NULL;
	int init = 0;
	MPI_Initialized(&init);
	if (init) {
		char pname[MPI_MAX_PROCESSOR_NAME] = {0};
		int len = 0;
		MPI_Get_processor_name(pname, &len);
		if (len) MPIC_PROC(comm) = strdup(pname); // to be free
	}
}

static void knh_MPIComm_free(CTX ctx, kRawPtr *o)
{
	MPIC(comm, o);
	if (MPIC_PROC(comm)) free(comm->proc_name); // allocated by strdup
}

static void knh_MPIData_init(CTX ctx, kRawPtr *o)
{
	MPID(data, o);
	data->o = NULL;
	MPID_TYPE(data) = MPI_BYTE;
	MPID_DCID(data) = CLASS_Object;
	MPID_POFS(data) = 0;
}

static void knh_MPIData_reftrace(CTX ctx, kRawPtr *p FTRARG)
{
	MPID(data, p);
	if (data->o != NULL) {
		KNH_ENSUREREF(ctx, 1);
		KNH_ADDREF(ctx, data->o);
		KNH_SIZEREF(ctx);
	}
}

static void knh_MPIData_p(CTX ctx, kOutputStream *w, kRawPtr *o, int level)
{
	MPID(data, o);
	ClassTBL(MPID_DCID(data))->cdef->p(ctx, w, data->r, level);
	if (MPID_POFS(data) > 0) knh_printf(ctx, w, "(+%d)offset", MPID_POFS(data));
}

static void knh_MPIOp_init(CTX ctx, kRawPtr *o)
{
	MPIO(op, o);
	MPIO_OP(op) = 0;
	MPIO_OPFUNC(op) = NULL;
}

static void knh_MPIOp_reftrace(CTX ctx, kRawPtr *p FTRARG)
{
	MPIO(op, p);
	if (MPIO_OPFUNC(op) != NULL) {
		KNH_ENSUREREF(ctx, 1);
		KNH_ADDREF(ctx, MPIO_OPFUNC(op));
		KNH_SIZEREF(ctx);
	}
}

static void knh_MPIRequest_init(CTX ctx, kRawPtr *o)
{
	MPIR(req, o);
	MPIR_INC(req) = 0;
}

DEFAPI(void) defMPI(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "MPI";
}

DEFAPI(void) defMPIComm(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "MPIComm";
	cdef->init = knh_MPIComm_init;
	cdef->free = knh_MPIComm_free;
}

DEFAPI(void) defMPIData(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "MPIData";
	cdef->init = knh_MPIData_init;
	cdef->reftrace = knh_MPIData_reftrace;
	cdef->p = knh_MPIData_p;
}

DEFAPI(void) defMPIRequest(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "MPIRequest";
	cdef->init = knh_MPIRequest_init;
}

DEFAPI(void) defMPIOp(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "MPIOp";
	cdef->init = knh_MPIOp_init;
	cdef->reftrace = knh_MPIOp_reftrace;
}

/* ------------------------------------------------------------------------ */
/* Const */

static void knh_MPI_initWorld(CTX ctx, kclass_t cid)
{
	MPIC(world, new_O(MPIComm, cid));
	MPIC_COMM(world) = MPI_COMM_WORLD;
	MPI_Comm_rank(MPIC_COMM(world), &MPIC_RANK(world));
	MPI_Comm_size(MPIC_COMM(world), &MPIC_SIZE(world));
	knh_addClassConst(ctx, cid, new_String(ctx, "WORLD"), (kObject*)world);
	kMPITaskContext *tctx = (kMPITaskContext*)ctx->wshare->mpictx;
	if (tctx) tctx->world = world; // only used by mpikonoha
}

static knh_IntData_t MPIConstOp[] = {
	{"MAX",  (kint_t)MPI_MAX},
	{"MIN",  (kint_t)MPI_MIN},
	{"SUM",  (kint_t)MPI_SUM},
	{"PROD", (kint_t)MPI_PROD},
	{"LAND", (kint_t)MPI_LAND},
	{"BAND", (kint_t)MPI_BAND},
	{"LOD",  (kint_t)MPI_LOR},
	{"BOR",  (kint_t)MPI_BOR},
	{"LXOR", (kint_t)MPI_LXOR},
	{"BXOR", (kint_t)MPI_BXOR},
	{NULL, 0},
};

static void knh_MPI_initOp(CTX ctx, kclass_t cid)
{
	knh_IntData_t *d;
	for (d = &MPIConstOp[0]; d->ivalue > 0; d++) {
		MPIO(op, new_O(MPIOp, cid));
		MPIO_OP(op) = (MPI_Op)d->ivalue;
		MPIO_OPFUNC(op) = NULL;
		knh_addClassConst(ctx, cid, new_String(ctx, d->name), (Object*)op);
	}
}

DEFAPI(void) constMPI(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	/* dummy */;
}

DEFAPI(void) constMPIComm(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	int init = 0;
	MPI_Initialized(&init);
	if (init) knh_MPI_initWorld(ctx, cid);
}

DEFAPI(void) constMPIData(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	/* dummy */;
}

DEFAPI(void) constMPIRequest(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	/* dummy */;
}

DEFAPI(void) constMPIOp(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	int init = 0;
	MPI_Initialized(&init);
	if (init) knh_MPI_initOp(ctx, cid);
}

void knh_MPI_initArrayFuncData(CTX ctx);
void knh_MPI_initArrayPrintFunc(CTX ctx);

static const struct kcontext_t *global_ctx;

void knh_MPI_errhandler(MPI_Comm *comm, int *err, ...)
{
	if (*err != MPI_SUCCESS) {
		char errstr[MPI_MAX_ERROR_STRING] = {0};
		int errlen = 0;
		MPI_Error_string(*err, errstr, &errlen);
		knh_ldata_t ldata[] = {LOG_s("errmsg", errstr), LOG_END};
		KNH_NTRACE(global_ctx, "MPI_Error", K_FAILED, ldata);
	}
}

DEFAPI(const knh_PackageDef_t*) init(CTX ctx, knh_LoaderAPI_t *kapi)
{
	int init = 0;
	MPI_Initialized(&init);
	if (init) {
		//MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
		global_ctx = ctx;
		MPI_Errhandler errfn;
		MPI_Comm_create_errhandler(knh_MPI_errhandler, &errfn);
		MPI_Errhandler_set(MPI_COMM_WORLD, errfn);
	} else {
		KNH_NOTE("process is not initialized for MPI: MPI functions are NOT available");
	}
	knh_MPI_initArrayFuncData(ctx);
	knh_MPI_initArrayPrintFunc(ctx);
	kapi->setPackageProperty(ctx, "name", "mpi");
	kapi->setPackageProperty(ctx, "version", "1.0");
	RETURN_PKGINFO("konoha.mpi");
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
