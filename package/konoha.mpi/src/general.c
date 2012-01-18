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

KMETHOD MPI_addTaskScript(CTX ctx, ksfp_t *sfp _RIX)
{
	kMPITaskContext *tctx = (kMPITaskContext*)ctx->share->mpictx;
	if (tctx != NULL) {
		MPIC(tworld, sfp[1].o);
		if (MPIC_RANK(tworld) >= 0) {
			kString *script = sfp[2].s;
			kMPITask *task = (kMPITask*)malloc(sizeof(kMPITask));
			task->world = MPIC_COMM(tworld);
			task->script = strdup(S_totext(script));
			task->scriptsize = S_size(script);
			task->next = NULL;
			kMPITask *t;
			for (t = tctx->tasks; t->next != NULL; t = (kMPITask*)t->next) ;
			t->next = (struct kMPITask*)task;
		}
	} else {
		KNH_NOTE("konoha isn't initialized as MPI mode");
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Static method void MPI.setTaskInitScript(String script);

KMETHOD MPI_setTaskInitScript(CTX ctx, ksfp_t *sfp _RIX)
{
	kMPITaskContext *tctx = (kMPITaskContext*)ctx->share->mpictx;
	if (tctx != NULL) {
		kString *script = sfp[1].s;
		tctx->initscript = strdup(S_totext(script));
		tctx->initsiz = S_size(script);
	} else {
		KNH_NOTE("konoha isn't initialized as MPI mode");
	}
	RETURNvoid_();
}
