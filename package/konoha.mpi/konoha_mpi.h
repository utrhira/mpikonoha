
#ifndef KNH_INCLUDE_MPI
#define KNH_INCLUDE_MPI

#define BEGIN_EXTERN_C \
	extern "C" {

#define END_EXTERN_C \
	}

#ifndef K_INTERNAL
#define K_INTERNAL
#endif
#include <konoha1.h>
#include <konoha1/inlinelibs.h>
#include <mpi.h>

/* ------------------------------------------------------------------------ */
/* Konoha Array */

//## type ArrayFloat Array 0 Float;

#ifndef IS_ArrayFloat
#define IS_ArrayFloat(o) (IS_bArray(o) && O_p1(o) == CLASS_Float)
#endif

void knh_MPI_initArrayFuncData(CTX ctx);
void knh_MPI_initArrayPrintFunc(CTX ctx);

static inline kBytes* new_B(CTX ctx, const char *name, size_t init)
{
	kBytes *ba = new_Bytes(ctx, name, init);
	knh_bzero(ba->bu.ubuf, init);
	ba->dim = new_dim(ctx, init, 1);
	return ba;
}

/* ------------------------------------------------------------------------ */
/* MPI Communicator */

//## class MPIComm Object;

typedef struct {
	kObjectHeader h;
	MPI_Comm comm;
	int myrank;
	int numprocs;
	char *proc_name;
} kMPIComm;

#define MPIC_COMM(c) ((c)->comm)
#define MPIC_RANK(c) ((c)->myrank)
#define MPIC_SIZE(c) ((c)->numprocs)
#define MPIC_PROC(d) ((d)->proc_name)
#define MPIC(v, o) kMPIComm *v = ((kMPIComm*)o)
#define MPIC_INITV(o, c) \
	{ \
		MPIC_COMM(o) = (c); \
		MPI_Comm_rank((c), &MPIC_RANK(o)); \
		MPI_Comm_size((c), &MPIC_SIZE(o)); \
	}

/* ------------------------------------------------------------------------ */
/* MPI Data */

//## class MPIData Object;

typedef struct {
	kObjectHeader h;
	union {
		kObject *o;
		kRawPtr *r;
		kInt    *i;
		kFloat  *f;
		kArray  *a;
		kBytes  *ba;
		kString *s;
	};
	MPI_Datatype type;
	kclass_t cid;  // object external class
	kclass_t dcid; // internal data class
	size_t offset;
} kMPIData;

#define MPID_ADDR(d) knh_MPIData_getAddr(d)
#define MPID_TYPE(d) ((d)->type)
#define MPID_CID(d) ((d)->cid)
#define MPID_DCID(d) ((d)->dcid)
#define MPID_POFS(d) ((d)->offset)
#define MPID_SIZE(d) knh_MPIData_getSize(d)
#define MPID_INIT(d, obj, type, cid, dcid) \
	{ \
		(d)->o = UPCAST(obj); \
		MPID_TYPE(d) = (type); \
		MPID_CID(d) = (cid); \
		MPID_DCID(d) = (dcid); \
	}

#define MPID_WCHK(d) \
	{\
		if (knh_MPIData_getCapacity(d) == -1) {\
			KNH_NTHROW2(ctx, sfp, "Script!!", "MPIData unwritable", K_FAILED, \
					KNH_LDATA(LOG_p("unwritable data", d)));\
		}\
	}

#define NTHROW_CAST_MPID(ctx, sfp, data) \
	KNH_NTHROW2((ctx), (sfp), "Script!!", "invalid type casting", K_FAILED, \
		KNH_LDATA( \
		LOG_s("content class", SAFECLASS__(ctx, MPID_CID(data))), \
		LOG_s("internal class", SAFECLASS__(ctx, MPID_DCID(data))) \
	));

#define MPID_CCHK(d, c)							\
	{\
		int psize = MPID_SIZE(d) - MPID_POFS(d);\
		if (c > psize || c <= 0) c = psize;\
	}

#define MPID(v, o) kMPIData *v = ((kMPIData*)o)

void* knh_MPIData_getAddr(kMPIData *data);
void  knh_MPIData_expand(CTX ctx, kMPIData *data, int *count, int *inc);
int   knh_MPIData_getSize(kMPIData *data);
int   knh_MPIData_incSize(kMPIData *data, int count);
int   knh_MPIData_getCapacity(kMPIData *data);

/* ------------------------------------------------------------------------ */
/* MPI Request (+ MPI Status) */

//## class MPIRequest Object;

typedef struct {
	kObjectHeader h;
	MPI_Request reqt;
	kMPIData *data;
	int incflag;
} kMPIRequest;

#define MPIR_REQ(req)  ((req)->reqt)
#define MPIR_DATA(req) ((req)->data)
#define MPIR_TYPE(req) (MPID_TYPE(MPIR_DATA(req)))
#define MPIR_INC(req)  ((req)->incflag)
#define MPIR(v, o) kMPIRequest *v = ((kMPIRequest*)o)

#define MPIS_SRC(stat)  ((stat).MPI_SOURCE)
#define MPIS_TAG(stat)  ((stat).MPI_TAG)
#define MPIS_ERR(stat)  ((stat).MPI_ERROR)

/* ------------------------------------------------------------------------ */
/* MPI Operator */

//## class MPIOp Object;

typedef struct {
	kObjectHeader h;
	MPI_Op op;
	MPI_User_function *func;
} kMPIOp;

//## type MPIOpFunc    Func  0 MPIData MPIData Int;

#define MPIO_OP(o) ((o)->op)
#define MPIO_OPFUNC(o) ((o)->func)
#define KNH_MPI_OP_NULL(op) (MPIO_OP(op) = 0)
#define KNH_MPI_OP_IS_NULL(op) (MPIO_OP(op) == 0)
#define MPIO(v, op) kMPIOp *v = (kMPIOp*)op

/* ------------------------------------------------------------------------ */
/* MPITask Struct */

typedef struct kMPITask kMPITask;
struct kMPITask {
	MPI_Comm  comm;
	kbytes_t  script;
	kMPITask *next;
};

#define MPIT_COMM(o) (o)->comm
#define MPIT_NEXT(o) (o)->next
#define MPIT_SCRIPT(o) (o)->script
#define MPIT_NEXTV(v) (v = MPIT_NEXT(v))
#define MPIT_INITV(o, c, t, s) \
	{ \
		MPIT_COMM(o) = (c); \
		MPIT_SCRIPT(o) = new_bytes2((t), (s)); \
		MPIT_NEXT(o) = NULL; \
	}

#define MPIT_MALLOC() (kMPITask*)malloc(sizeof(kMPITask))
#define MPIT_FREE(o) \
	{ \
		free((void*)(MPIT_SCRIPT(o).buf)); \
		free((void*)o); \
	}

typedef struct kMPITaskContext kMPITaskContext;
struct kMPITaskContext {
	kMPIComm *kworld;
	kbytes_t  taskscript;
	kMPITask *tasks;
};

#define MPICTX_THEAD(o) (o)->tasks
#define MPICTX_TSCRIPT(o) (o)->taskscript
#define MPICTX_TWORLD(o) (o)->kworld
#define MPICTX_TASKS_FREE(o) \
	{ \
		kMPITask *_next, *_task = MPICTX_THEAD(o); \
		while (_task != NULL) { \
			_next = MPIT_NEXT(_task); \
			MPIT_FREE(_task); \
			_task = _next; \
		} \
	}

/* ------------------------------------------------------------------------ */

#endif /* KNH_INCLUDE_MPI */
