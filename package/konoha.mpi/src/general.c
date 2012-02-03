#include "../konoha_mpi.h"

/* ------------------------------------------------------------------------ */
//## @Static method void MPI.preventLog();

KMETHOD MPI_preventLog(CTX ctx, ksfp_t *sfp _RIX)
{
	knh_closelog();
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Static method Float MPI.getWtime();
KMETHOD MPI_getWtime(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNf_(MPI_Wtime());
}

/* ------------------------------------------------------------------------ */
//## @Static method String MPI.getTaskScript();

extern kMPITaskContext *kMPI_global_tctx; // defined @src/main/runtime.c

KMETHOD MPI_getTaskScript(CTX ctx, ksfp_t *sfp _RIX)
{
	kString *script;
	if (kMPI_global_tctx != NULL) {
		kbytes_t bscript = MPICTX_TSCRIPT(kMPI_global_tctx);
		script = new_String2(ctx, CLASS_String, bscript.text, bscript.len, 0);
	}
	else {
		script = (kString*)KNH_NULVAL(CLASS_String);
	}
	RETURN_(script);
}

/* ------------------------------------------------------------------------ */
//## @Static method void MPI.addTaskScript(MPIComm comm, String script);

KMETHOD MPI_addTaskScript(CTX ctx, ksfp_t *sfp _RIX)
{
	MPIC(tworld, sfp[1].o);
	if (MPIC_RANK(tworld) >= 0) {
		kString *script = sfp[2].s;
		kMPITask *task = MPIT_MALLOC();
		const char *tscript = strdup(S_totext(script)); // free @ MPICTX_TASKS_FREE
		MPIT_INITV(task, MPIC_COMM(tworld), tscript, S_size(script));
		kMPITask *tlist = MPICTX_THEAD(kMPI_global_tctx);
		if (tlist != NULL) {
			while(MPIT_NEXT(tlist) != NULL) {
				MPIT_NEXTV(tlist);
			}
			MPIT_NEXT(tlist) = task;
		}
		else {
			MPICTX_THEAD(kMPI_global_tctx) = task;
		}
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Native @Static void  MPI.setTaskWorld(MPIComm comm);

KMETHOD MPI_setTaskWorld(CTX ctx, ksfp_t *sfp _RIX)
{
	MPIC(tworld, sfp[1].o);
	MPIC_INITV(MPICTX_TWORLD(kMPI_global_tctx), MPIC_COMM(tworld));
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Native @Static void  MPI.setTaskOut(OutputStream out);

KMETHOD MPI_setTaskOut(CTX ctx, ksfp_t *sfp _RIX)
{
	kOutputStream *out = sfp[1].w;
	knh_OutputStream_flush(ctx, ctx->out);
	KNH_SETv(ctx, WCTX(ctx)->out, out);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Native @Static void  MPI.setTaskErr(OutputStream err);

KMETHOD MPI_setTaskErr(CTX ctx, ksfp_t *sfp _RIX)
{
	kOutputStream *err = sfp[1].w;
	knh_OutputStream_flush(ctx, ctx->err);
	KNH_SETv(ctx, WCTX(ctx)->err, err);
	RETURNvoid_();
}
