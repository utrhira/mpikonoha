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

#ifdef KNH_MPI_VERTIKS
#if 0
{
#endif

extern const char* kMPI_argv0;
static kMPITaskContext _tctx;
static kMPITaskContext *kMPI_global_tctx = &_tctx;

static int bytes_isempty(kbytes_t t)
{
	size_t i;
	for(i = 0; i < t.len; i++) {
		if(!isspace(t.utext[i])) return 0;
	}
	return 1;
}

static kline_t Bytes_addQUOTE(CTX ctx, kBytes *ba, kInputStream *in, int quote, kline_t line)
{
	int ch, prev = quote;
	while((ch = knh_InputStream_getc(ctx, in)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		knh_Bytes_putc(ctx, ba, ch);
		if(ch == quote && prev != '\\') {
			return line;
		}
		prev = ch;
	}
	return line;
}

static kline_t Bytes_addCOMMENT(CTX ctx, kBytes *ba, kInputStream *in, kline_t line)
{
	int ch, prev = 0, level = 1;
	while((ch = knh_InputStream_getc(ctx, in)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		knh_Bytes_putc(ctx, ba, ch);
		if(prev == '*' && ch == '/') level--;
		if(prev == '/' && ch == '*') level++;
		if(level == 0) return line;
		prev = ch;
	}
	return line;
}

static kline_t readchunk(CTX ctx, kInputStream *in, kline_t line, kBytes *ba)
{
	int ch;
	int prev = 0, isBLOCK = 0;
	while((ch = knh_InputStream_getc(ctx, in)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		knh_Bytes_putc(ctx, ba, ch);
		if(prev == '/' && ch == '*') {
			line = Bytes_addCOMMENT(ctx, ba, in, line);
			continue;
		}
		if(ch == '\'' || ch == '"' || ch == '`') {
			line = Bytes_addQUOTE(ctx, ba, in, ch, line);
			continue;
		}
		if(isBLOCK != 1 && prev == '\n' && ch == '\n') {
			break;
		}
		if(prev == '{') {
			isBLOCK = 1;
		}
		if(prev == '\n' && ch == '}') {
			isBLOCK = 0;
		}
		prev = ch;
	}
	return line;
}

KMETHOD MPI_vload(CTX ctx, ksfp_t *sfp _RIX)
{
	kString *ret = new_String(ctx, "");
	if (kMPI_argv0 != NULL) {
		const char *tscript = NULL;
		size_t tsize = 0;
		int myrank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
		if (myrank == 0/*root*/) {
			kNameSpace *ns = K_GMANS;
			kline_t uline = 1;
			CWB_t cwbbuf, *cwb = CWB_open(ctx, &cwbbuf);
			knh_buff_addospath(ctx, cwb->ba, cwb->pos, 0, B(kMPI_argv0));
			FILE *fp = fopen(CWB_totext(ctx, cwb), "r");
			if(fp != NULL) {
				kBytes *bscript = new_Bytes(ctx, "bscript", K_PAGESIZE);
				kuri_t uri = knh_getURI(ctx, CWB_tobytes(cwb));
				KNH_SETv(ctx, ns->path, new_Path(ctx, knh_buff_newRealPathString(ctx, cwb->ba, cwb->pos)));
				kInputStream *in = new_InputStream(ctx, new_FILE(ctx, fp, 256), ns->path);
				ULINE_setURI(uline, uri);
				{
					BEGIN_LOCAL(ctx, lsfp, 3);
					LOCAL_NEW(ctx, lsfp, 0, kBytes*, ba, new_Bytes(ctx, "chunk", K_PAGESIZE));
					KNH_SETv(ctx, lsfp[1].o, in);
					kline_t linenum = uline;
					do {
						knh_Bytes_clear(ba, 0);
						if(!io2_isClosed(ctx, in->io2)) {
							uline = linenum;
							linenum = readchunk(ctx, in, linenum, ba);
						}
						if(!bytes_isempty(ba->bu)) {
							knh_Bytes_write2(ctx, bscript, ba->bu.buf, BA_size(ba));
						}
					} while(BA_size(ba) > 0);
					KNH_NTRACE2(ctx, "MPI.vload", K_NOTICE, KNH_LDATA(LOG_s("urn", S_totext(in->path->urn))));
					END_LOCAL(ctx, lsfp);
				}
				tscript = strdup(knh_Bytes_ensureZero(ctx, bscript));
				tsize = strlen(tscript);
			}
			else {
				KNH_NOTE("script not found: %s", kMPI_argv0);
			}
			CWB_close(ctx, cwb);
		}
		MPI_Bcast((void*)&tsize, 1, MPI_INT, 0/*root*/, MPI_COMM_WORLD);
		if (tsize > 0) {
			if (myrank > 0/*not root*/) {
				tscript = (const char*)malloc(tsize);
			}
			MPI_Bcast((void*)&tscript[0], tsize, MPI_CHAR, 0/*root*/, MPI_COMM_WORLD);
			ret = new_String2(ctx, CLASS_String, tscript, tsize, 0);
			free((void*)tscript);
		}
	}
	RETURN_(ret);
}

KMETHOD MPI_vmainloop(CTX ctx, ksfp_t *sfp _RIX)
{
	kbool_t ret = 0;
	{
		kclass_t comm_cid = knh_getcid(ctx, STEXT("MPIComm"));
		MPIC(tworld, new_O(MPIComm, comm_cid));
		MPIC_INITV(tworld, MPI_COMM_WORLD);
		knh_addClassConst(ctx, comm_cid, new_String(ctx, "TWORLD"), (kObject*)tworld);
		MPICTX_TWORLD(kMPI_global_tctx) = tworld;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	kMPITask *task = MPICTX_THEAD(kMPI_global_tctx);
	if (task != NULL) {
		int myrank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
		do {
			MPIC_INITV(MPICTX_TWORLD(kMPI_global_tctx), MPIT_COMM(task));
			MPI_Barrier(MPIT_COMM(task));
			kbytes_t bscript = MPIT_SCRIPT(task);
//			fprintf(stderr,
//					"--<rank:%d>-------------------------------------------------------------------\n"
//					"%s\n"
//					"------------------------------------------------------------------------------\n", myrank, bscript.text);
			double _begin = MPI_Wtime();
			kInputStream *bin = new_BytesInputStream(ctx, bscript.text, bscript.len);
			ret = knh_beval(ctx, bin, 1);
			double _finish = MPI_Wtime();
			double _duration = _finish - _begin;
			KNH_NTRACE2(ctx, "Vertiks:eval", K_NOTICE,
						KNH_LDATA(LOG_f("begin", _begin), LOG_f("finish", _finish),
								  LOG_f("duration", _duration), LOG_i("myrank", myrank)));
		} while (MPIT_NEXTV(task) != NULL);
		MPICTX_TASKS_FREE(kMPI_global_tctx);
	}
	else {
		KNH_NOTE("no registered tasks");
	}
	RETURNb_(ret);
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

#if 0
}
#endif
#endif /* KNH_MPI_VERTIKS */
