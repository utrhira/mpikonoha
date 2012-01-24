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
//## @Static method void MPI.addTaskScript(MPIComm comm, String script);

extern kMPITaskContext *kmpi_global_tctx; // defined @src/main/runtime.c

KMETHOD MPI_addTaskScript(CTX ctx, ksfp_t *sfp _RIX)
{
	MPIC(tworld, sfp[1].o);
	if (MPIC_RANK(tworld) >= 0) {
		kString *script = sfp[2].s;
		kMPITask *task = MPIT_MALLOC();
		const char *tscript = strdup(S_totext(script)); // free @ MPICTX_TASKS_FREE
		MPIT_INITV(task, MPIC_COMM(tworld), tscript, S_size(script));
		kMPITask *tlist = MPICTX_THEAD(kmpi_global_tctx);
		if (tlist != NULL) {
			while(MPIT_NEXT(tlist) != NULL) {
				MPIT_NEXTV(tlist);
			}
			tlist->next = task;
		}
		else {
			KNH_NOTE("process is not running as MPI mode(use --mpi option)");
		}
	}
	RETURNvoid_();
}
