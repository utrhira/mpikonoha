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

/* ************************************************************************ */

#include"commons.h"
#include"../../include/konoha1/konohalang.h"
#ifdef K_USING_POSIX_
#include <unistd.h>
#endif

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------------------------------------ */
/* [opt] */

static void opt_W(CTX ctx, int mode, const char *optstr)
{

}

static kbool_t isCompileOnly = 0;
static kbool_t isInteractiveMode = 0;

static void opt_i(CTX ctx, int mode, const char *optstr)
{
	isInteractiveMode = 1;
}

kbool_t knh_isCompileOnly(CTX ctx)
{
	return isCompileOnly;
}

void knh_setCompileMode(CTX ctx, int mode)
{
	isCompileOnly = mode;;
}

static void opt_c(CTX ctx, int mode, const char *optstr)
{
	isCompileOnly = 1;
}

static void opt_g(CTX ctx, int mode, const char *optstr)
{
	CTX_setDebug(ctx, 1);
}

static void opt_O(CTX ctx, int mode, const char *optstr)
{
	DBG_P("OPTIMIZATION %d", mode);
	if(mode == 0) {
		//DP(ctx->gma)->cflag = 0; // NOTHING
	}
}

static kbool_t isActorMode = 0;
static int port = 0;

static void opt_p(CTX ctx, int mode, const char *optstr)
{
	isActorMode = 1;
	port = mode;
}

static kbool_t isMPIMode = 0;
static int kMPI_initialized = 0;
static int kMPI_worldRank;
static int kMPI_worldSize;

static void opt_mpi(CTX ctx, int mode, const char *optstr)
{
	MPI_Initialized(&kMPI_initialized);
	if(kMPI_initialized) {
		MPI_Comm_rank(MPI_COMM_WORLD, &kMPI_worldRank);
		MPI_Comm_size(MPI_COMM_WORLD, &kMPI_worldSize);
		if (kMPI_worldSize > 1) {
			static kMPITask head;
			head.world  = MPI_COMM_WORLD;
			head.script = NULL; // assigned @ this:konoha_main
			head.next   = NULL;
			static kMPITaskContext tctx;
			tctx.world = NULL; // assigned @ konoha.mpi/mpi.c:initWorld
			tctx.tasks = (struct kMPITask*)&head;
			ctx->wshare->mpictx = (struct kMPITaskContext*)&tctx;
			isMPIMode = 1;
		} else {
			KNH_NOTE("only 1 proc is running: MPI mode ignored");
		}
	} else {
		KNH_NOTE("process isn't initialized for MPI");
	}
}

/* ----------------------------------------------------------------------- */

void knh_loadScriptPackageList(CTX ctx, const char *pkglist)
{
	if(pkglist != NULL) {
		kbytes_t t = {{pkglist}, knh_strlen(pkglist)};
		char buf[256];
		size_t i = 0;
		int isExists = 0;
		L_NEXT:;
		isExists = 0;
		while(i < t.len + 1) {
			char *c = buf;
			while(i < t.len + 1) {
				int ch = t.ubuf[i];
				i++;
				if(ch ==':' || ch == ';' || ch == ',' || ch == 0) {
					*c = 0;
					DBG_P("loading '%s'", buf);
					if(!knh_loadPackage(ctx, B(buf)) && isExists == 0) {
						KNH_LOG("package not found: package=%s", buf+8);
					}
					goto L_NEXT;
				}
				else {
					if(ch == '?') {
						isExists = 1; continue;
					}
					*c = ch;
				}
				c++;
				if(!(c - buf < 256)) {
					KNH_LOG("too long name %s", pkglist);
					return ;
				}
			}
		}
	}
}

static void knh_setStartUpPackage(CTX ctx, int mode, const char *optstr)
{
	if(optstr != NULL) {
		knh_loadScriptPackageList(ctx, optstr);
	}
}

static void opt_version(CTX ctx, int mode, const char *optstr)
{
	if(mode == 0) {
#if defined(K_USING_MINGW_)
		fprintf(stdout, "konoha%d-%d\n", (int)sizeof(void*) * 8, (int)K_REVISION);
#else
		fprintf(stdout, "konoha%d-%d-%d\n", (int)sizeof(void*) * 8, (int)K_REVISION, (int)getppid());
#endif /* defined(K_USING_MINGW_) */
	}
	else {
		dump_sysinfo(NULL, NULL, 1/*ALL*/);
	}
	exit(0);
}

static const char *HELPMSG =
		"usage: konoha [options]  [file | -]\n"
		"options and arguments (and corresponding environment variables):\n"
		"-a      : audit\n"
		"-c      : compile and verification only (never running)\n"
		"-g      : debug support for testing script\n"
		"-h      : print this help message and exit (also --help)\n"
		"-i      : invoke interactive shell after running script\n"
		"-l file : logging to file\n"
		"-O      : optimize generated bytecode slightly\n"
		"-p      : port number for actor\n"
		"-v      : verbose; -v0 is a strong system-level verbose\n"
		"-V      : show version and sysinfo and exit (also --version)\n"
		"-W arg  : warning control in compilation\n"
		"file    : program read from script file\n"
		"-       : program read from stdin\n"
		"\n"
		"Other environment variables:\n"
		"KONOHAHOME  : alternate to %s\n"
		"KONOHAENC   : alternate to %s\n"
		"\n";

static void opt_help(CTX ctx, int mode, const char *optstr)
{
	kString* home = knh_getPropertyNULL(ctx, STEXT("konoha.home.path"));
	fprintf(stdout, HELPMSG, S_totext(home), knh_getSystemEncoding());
	exit(0);
}

/* ----------------------------------------------------------------------- */

typedef void (*knh_Fsetopt)(CTX, int, const char *);

#define OPT_EMPTY    0
#define OPT_NUMBER   1
#define OPT_STRING   2
#define OPT_(O)      O, (sizeof(O)-1)

typedef struct {
	const char *name;
	size_t len;
	int type;
	void (*setopt)(CTX, int, const char *);
} knh_optdata_t ;

static void opt_dummy(CTX ctx, int mode, const char *optstr)
{
}

static knh_optdata_t optdata[] = {
	{OPT_("-a"), OPT_NUMBER, opt_dummy},
	{OPT_("-c"), OPT_EMPTY, opt_c},
	{OPT_("-i"), OPT_EMPTY, opt_i},
	{OPT_("-g"), OPT_NUMBER, opt_g},
	{OPT_("-v"), OPT_NUMBER, opt_dummy},
	{OPT_("-l"), OPT_STRING, opt_dummy},
	{OPT_("-O"), OPT_NUMBER, opt_O},
	{OPT_("-P"), OPT_STRING, knh_setStartUpPackage},
	{OPT_("-p"), OPT_NUMBER, opt_p},
	{OPT_("-W"), OPT_NUMBER, opt_W},
	{OPT_("-h"), OPT_EMPTY, opt_help},
	{OPT_("--verbose:gc"), OPT_EMPTY, opt_dummy},
	{OPT_("--verbose:lang"), OPT_EMPTY, opt_dummy},
	{OPT_("--verbose:pref"), OPT_EMPTY, opt_dummy},
//	{"--utest", OPT_EMPTY, opt_utest},
	{OPT_("--help"), OPT_EMPTY, opt_help},
	{OPT_("--enforce-security"), OPT_STRING, opt_dummy},
	{OPT_("--logcached"), OPT_STRING, opt_dummy},
	{OPT_("-V"), OPT_NUMBER, opt_version},
	{OPT_("--mpi"), OPT_EMPTY, opt_mpi},
	{OPT_("--version"), OPT_NUMBER, opt_version},
	{NULL, 0, OPT_EMPTY, NULL}, // END
};

static knh_optdata_t *knh_getoptdata(const char *name)
{
	knh_optdata_t *d = optdata;
	while(d->name != NULL) {
		if(knh_strncmp(d->name, name, d->len) == 0) {
			return d;
		}
		d++;
	}
	return NULL;
}

static int knh_parseopt(CTX ctx, int argc, const char **argv)
{
	int n;
	for(n = 1; n < argc; n++) {
		const char *t = argv[n];
		if(t[0] == '-' && (isalnum(t[1]) || t[1] == '-')) {
			knh_optdata_t *d = knh_getoptdata(t);
			int optnum = 1;              // default
			const char* optstr = NULL;   // default
			if(d == NULL) {
				opt_help(ctx, optnum, optstr);
			}
			if(d->type == OPT_NUMBER) {
				t += d->len;
				if(t[0] == '=') t++;
				if(isalnum(t[0])) {
					kint_t v = 0;
					knh_bytes_parseint(B((char*)t), &v);
					optnum = (int)v;
				}
			}
			else if(d->type == OPT_STRING) {
				t += d->len;
				if(t[0] == '=') {
					optstr = t + 1;
				}
				else if(t[0] != 0) {
					optstr = t;
				}
				else if(n + 1 < argc) {
					n++;
					optstr = argv[n];
					if(optstr[0] == '-') {
						n--; optstr = NULL;
					}
				}
			}
			d->setopt(ctx, optnum, optstr);
			continue;
		}
		break;
	}
	return n;
}

static kbytes_t knh_bytes_nsname(kbytes_t t)
{
	size_t i, s = 0;
	for(i = t.len - 1; i > 0; i--) {
		if(t.utext[i] == '/' || t.utext[i] == '\\') {
			s = i + 1;
			break;
		}
	}
	for(i = s; i < t.len; i++) {
		if(t.utext[i] == '.') {
			t.utext = t.utext + s;
			t.len = i - s;
			return t;
		}
	}
	t.utext = t.utext + s;
	t.len = t.len - s;
	return t;
}

static void knh_parsearg(CTX ctx, int argc, const char **argv)
{
	int i;
	kArray *a = new_Array(ctx, CLASS_String, argc);
	for(i = 1; i < argc; i++) {
		knh_Array_add(ctx, a, new_String2(ctx, CLASS_String, argv[i], knh_strlen(argv[i]), SPOL_TEXT|SPOL_POOLALWAYS));
	}
	knh_DictMap_set(ctx, ctx->share->props, new_T("script.argv"), a);
	if(argc > 0) {
		kString *s = new_T(argv[0]);
		knh_DictMap_set(ctx, ctx->share->props, new_T("script.name"), s);
		kbytes_t t = knh_bytes_nsname(S_tobytes(s));
		knh_Script_setNSName(ctx, ctx->script, new_String2(ctx, CLASS_String, t.text, t.len, SPOL_TEXT|SPOL_POOLALWAYS));
	}
	else {
		knh_Script_setNSName(ctx, ctx->script, TS_main);
	}
}

/* ------------------------------------------------------------------------ */

static int knh_runMain(CTX ctx, int argc, const char **argv)
{
	KONOHA_BEGIN(ctx);
	kMethod *mtd = ClassTBL_getMethodNULL(ctx, O_cTBL(ctx->script), MN_main);
	int res = 0;
	if(mtd != NULL) {
		int thisidx = 1 + K_CALLDELTA;
		BEGIN_LOCAL(ctx, lsfp, 5);
		lsfp[1].ivalue = 0;
		lsfp[thisidx+K_PCIDX].pc = NULL;
		klr_setmtdNC(ctx,lsfp[thisidx+K_MTDIDX], mtd);
		KNH_SETv(ctx, lsfp[thisidx].o, ctx->script);
		KNH_SETv(ctx, lsfp[thisidx+1].o, knh_getPropertyNULL(ctx, STEXT("script.argv")));
		klr_setesp(ctx, lsfp + thisidx+2);
		if(knh_VirtualMachine_launch(ctx, lsfp + thisidx)) {
			res = (int)lsfp[1].ivalue;
		}
		END_LOCAL(ctx, lsfp);
	}
	KONOHA_END(ctx);
	return res;
}

/* ------------------------------------------------------------------------ */
/* [shell] */

static void knh_showWelcome(CTX ctx, kOutputStream *w)
{
	const knh_sysinfo_t *sysinfo = knh_getsysinfo();
	knh_printf(ctx, w, "%s%s %s(%s) %s (rev:%d, %s %s)%s\n",
		TERM_BBOLD(ctx),
		sysinfo->konoha_type, sysinfo->konoha_version, sysinfo->konoha_codename,
		K_DISTTYPE, ((kintptr_t)K_REVISION), __DATE__, __TIME__, TERM_EBOLD(ctx));
	//knh_printf(ctx, w, "[%s] on %s (%d, %s)\n", CC_TYPE, sysinfo->kern_ostype, sysinfo->konoha_systembits, knh_getSystemEncoding());
	knh_printf(ctx, w, "options: %sused_memory:%d kb\n",
		sysinfo->konoha_options, (kintptr_t)(ctx->stat->usedMemorySize / 1024));
}

static int shell_checkstmt(kbytes_t t)
{
	size_t i = 0;
	int ch, quote = 0, nest = 0;
	int hasDOC = 0;
	L_NORMAL:
	for(; i < t.len; i++) {
		ch = t.ubuf[i];
		if(ch == '{' || ch == '[' || ch == '(') nest++;
		if(ch == '}' || ch == ']' || ch == ')') nest--;
		if(ch == '\'' || ch == '"' || ch == '`') {
			if(t.ubuf[i+1] == ch && t.ubuf[i+2] == ch) {
				quote = ch; i+=2;
				goto L_TQUOTE;
			}
		}
		if(ch == '\n') hasDOC = 0;
		if(ch == '#') {
			hasDOC = 1;
		}
	}
	return (hasDOC == 1) ? 1 : nest;
	L_TQUOTE:
	DBG_ASSERT(i > 0);
	for(; i < t.len; i++) {
		ch = t.ubuf[i];
		if(t.ubuf[i-1] != '\\' && ch == quote) {
			if(t.ubuf[i+1] == ch && t.ubuf[i+2] == ch) {
				i+=2;
				goto L_NORMAL;
			}
		}
	}
	return 1;
}

static void shell_restart(CTX ctx)
{
//	kNameSpace *ns = new_NameSpace(ctx, NULL);
//	DBG_ASSERT(ns->b->aliasDictMapNULL == NULL);
//	ctx->wshare->sysAliasDictMapNULL = DP(ctx->share->rootns)->aliasDictMapNULL;
//	DP(ctx->share->rootns)->aliasDictMapNULL = NULL;
//	KNH_SETv(ctx, ((kshare_t*)ctx->share)->rootns, ns);
//	KNH_SETv(ctx, ((kcontext_t*)ctx)->script, new_(Script));
//	{
//		kGammaBuilder *newgma = new_(GammaBuilder);
//		KNH_SETv(ctx, ((kcontext_t*)ctx)->gma, newgma);
//		KNH_INITv(DP(newgma)->symbolDictMap, new_DictMap0(ctx, 256, 0/*isCaseMap*/, "GammaBuilder.symbolDictMap"));
//		KNH_INITv(DP(newgma)->constPools, new_Array0(ctx, 0));
//		KNH_INITv(DP(newgma)->script, ctx->script);
//	}
	KNH_TODO("restart");
}

void knh_dumpKeyword(CTX ctx, kOutputStream *w);

static kstatus_t shell_command(CTX ctx, const char *cmd)
{
	kbytes_t t = {{cmd}, knh_strlen(cmd)};
	if(B_equals(t, "quit") || B_equals(t, "exit") || B_equals(t, "bye")) {
		return K_BREAK;
	}
	if(B_equals(t, "keyword")) {
		knh_dumpKeyword(ctx, KNH_STDOUT);
		return K_REDO;
	}
	if(B_equals(t, "restart")) {
		fputs(
"=============================================================================="
"\n\n\n", stdout);
		shell_restart(ctx);
		return K_REDO;
	}
	return K_CONTINUE;
}

/* ------------------------------------------------------------------------ */
/* standard shell api */

static void* shell_init(CTX ctx, const char *msg, const char *optstr)
{
	fputs(msg, stdout);
	CTX_setInteractive(ctx, 1);
	CTX_setDebug(ctx, 1);
	return NULL; // nostatus
}

static void shell_display(CTX ctx, void *status, const char *msg)
{
	fputs(msg, stdout);
}

static void shell_cleanup(CTX ctx, void *status)
{
	CTX_setInteractive(ctx, 0);
}

#ifdef K_PREVIEW
static const char* SecurityAlertMessage =  "** FOR EVALUATION/DEVELOPMENT USE ONLY **";
#else
static const char* SecurityAlertMessage = NULL;
#endif

void knh_setSecurityAlertMessage(const char *msg, int isNeedFree)
{
	static int isSecurityAlertNeedFree = 0;
	if(SecurityAlertMessage != NULL && isSecurityAlertNeedFree) {
		free((void*)SecurityAlertMessage);
	}
	SecurityAlertMessage = msg;
	isSecurityAlertNeedFree = isNeedFree;
}

static void knh_showSecurityAlert(CTX ctx, kOutputStream *w)
{
	if(SecurityAlertMessage != NULL /*&& CTX_isInteractive(ctx)*/) {
		knh_write_ascii(ctx, w, TERM_BBOLD(ctx));
		knh_write_ascii(ctx, w, "SECURITY ALERT: ");
		knh_write_ascii(ctx, w, SecurityAlertMessage);
		knh_write_ascii(ctx, w, TERM_EBOLD(ctx));
		knh_write_EOL(ctx, w); knh_write_EOL(ctx, w);
		knh_setSecurityAlertMessage(NULL, 0);
	}
}

static kstatus_t readstmt(CTX ctx, CWB_t *cwb)
{
	int line = 1;
	kstatus_t status = K_CONTINUE;
	CWB_clear(cwb, 0);
	fputs(TERM_BBOLD(ctx), stdout);
	while(1) {
		int check;
		char *ln = ctx->spi->readline(line == 1 ? ">>> " : "    ");
		if(ln == NULL) {
			CWB_clear(cwb, 0);
			status = K_BREAK;
			break;
		}
		if(line > 1) knh_Bytes_putc(ctx, cwb->ba, '\n');
		knh_Bytes_write(ctx, cwb->ba, B(ln));
		free(ln);
		if((check = shell_checkstmt(CWB_tobytes(cwb))) > 0) {
			line++;
			continue;
		}
		if(check < 0) {
			fputs("(Cancelled)...\n", stdout);
			CWB_clear(cwb, 0);
		}
		break;
	}
	if(CWB_size(cwb) > 0) {
		const char *p = CWB_totext(ctx, cwb);
		ctx->spi->add_history(p);
	}
	fputs(TERM_EBOLD(ctx), stdout);
	fflush(stdout);
	return status;
}

#ifdef K_USING_READLINE
extern char *readline(const char *) __attribute__ ((weak_import));
extern int  add_history(const char *) __attribute__ ((weak_import));

#else

static char* readline(const char* prompt)
{
	static int checkCTL = 0;
	int ch, pos = 0;
	static char linebuf[1024]; // THREAD-UNSAFE
	fputs(prompt, stdout);
	while((ch = fgetc(stdin)) != EOF) {
		//fprintf(stderr, "%d: %c\n", ch, ch);
		if(ch == '\r') continue;
		if(ch == 27) {
			/* ^[[A */;
			fgetc(stdin); fgetc(stdin);
			if(checkCTL == 0) {
				fprintf(stdout, " - readline would provide you with better shell experience.\n");
				checkCTL = 1;
			}
			continue;
		}
		if(ch == '\n' || pos == sizeof(linebuf) - 1) {
			linebuf[pos] = 0;
			break;
		}
		linebuf[pos] = ch;
		pos++;
	}
	if(ch == EOF) return NULL;
	{
		char *p = (char*)malloc(pos+1);
		knh_memcpy(p, linebuf, pos+1);
		return p;
	}
}

static int add_history(const char* line)
{
	return 0;
}
#endif

static void knh_linkDynamicReadline(CTX ctx)
{
	if(ctx->spi->readline == NULL) {
		void *handler = knh_dlopen(ctx, "libreadline" K_OSDLLEXT);
		if(handler != NULL) {
			void *f = knh_dlsym(ctx, handler, "readline", NULL, 0/*isTest*/);
			if(f != NULL) {
				((knh_ServiceSPI_t*)ctx->spi)->readlinespi = "libreadline";
				((knh_ServiceSPI_t*)ctx->spi)->readline = (char* (*)(const char*))f;
			}
			else {
				goto L_STDIN;
			}
			f = knh_dlsym(ctx, handler, "add_history", NULL, 0/*isTest*/);
			if(f != NULL) {
				((knh_ServiceSPI_t*)ctx->spi)->add_history = (int (*)(const char*))f;
			}
			else {
				((knh_ServiceSPI_t*)ctx->spi)->add_history = add_history;
			}
			return;
		}
		L_STDIN:;
		((knh_ServiceSPI_t*)ctx->spi)->readlinespi = "stdin";
		((knh_ServiceSPI_t*)ctx->spi)->readline = readline;
		((knh_ServiceSPI_t*)ctx->spi)->add_history = add_history;
	}
}

static void knh_shell(CTX ctx)
{
	void *shell_status = NULL;
	BEGIN_LOCAL(ctx, lsfp, 2);
//	LOCAL_NEW(ctx, lsfp, 0, kInputStream *, bin, new_BytesInputStream(ctx, new_Bytes(ctx, "shell", K_PAGESIZE)));
	{
		CWB_t cwbbuf, *cwb = CWB_open(ctx, &cwbbuf);
		knh_showWelcome(ctx, cwb->w);
		knh_showSecurityAlert(ctx, cwb->w);
		shell_status = shell_init(ctx, CWB_totext(ctx, cwb), NULL);
		CWB_close(ctx, cwb);
	}
	while(1) {
		{
			CWB_t cwbbuf, *cwb = CWB_open(ctx, &cwbbuf);
			kstatus_t status = readstmt(ctx, cwb);
			if(status == K_BREAK) {
				CWB_close(ctx, cwb);
				break;
			}
			if(CWB_size(cwb) == 0) {
				CWB_close(ctx, cwb);
				continue;
			}
			status = shell_command(ctx, CWB_totext(ctx, cwb));
			if(status == K_BREAK) {
				CWB_close(ctx, cwb);
				break;
			}
			if(status == K_REDO) {
				CWB_close(ctx, cwb);
				continue;
			}
#ifdef K_USING_SUGAR
			kString *script = CWB_newString(ctx, cwb, 0);
			KNH_SETv(ctx, lsfp[0].o, script);
			knh_beval2(ctx, S_totext(script), 1);
#else
			kInputStream *bin = new_BytesInputStream(ctx, CWB_totext(ctx, cwb), CWB_size(cwb));
			KNH_SETv(ctx, lsfp[0].o, bin);
			knh_beval(ctx, bin, 1);
#endif
		}
		knh_OutputStream_flush(ctx, ctx->out);
		if(ctx->isEvaled == 1) {
			CWB_t cwbbuf, *cwb = CWB_open(ctx, &cwbbuf);
			knh_write_Object(ctx, cwb->w, ctx->evaled, FMT_dump);
			knh_showSecurityAlert(ctx, cwb->w);
			if(CWB_size(cwb) !=0) {
				shell_display(ctx, shell_status, CWB_totext(ctx, cwb));
			}
			CWB_close(ctx, cwb);
			WCTX(ctx)->isEvaled = 0;
		}
	}
	shell_cleanup(ctx, shell_status);
	END_LOCAL(ctx, lsfp);
}

static int konoha_shell(CTX ctx, char *optstr)
{
	KONOHA_BEGIN(ctx);
	knh_loadScriptPackageList(ctx, "konoha.i?");
#ifdef K_USING_SECURITY_ALERT
	knh_askSecurityAlert(ctx);
#endif
	CTX_setInteractive(ctx, 1);
	knh_shell(ctx);
	KONOHA_END(ctx);
	return 0;
}

/* ------------------------------------------------------------------------- */

typedef knh_iconv_t (*ficonv_open)(const char *, const char *);
typedef size_t (*ficonv)(knh_iconv_t , char **, size_t *, char **, size_t *);
typedef int    (*ficonv_close)(knh_iconv_t);

static void knh_linkDynamicIconv(CTX ctx)
{
	knh_ServiceSPI_t *spi = ((knh_ServiceSPI_t*)ctx->spi);
	void *handler = knh_dlopen(ctx, "libiconv" K_OSDLLEXT);
	void *f = NULL;
	if(handler != NULL) {
		f = knh_dlsym(ctx, handler, "iconv_open", "libiconv_open", 1/*isTest*/);
		if (f != NULL) {
			spi->iconvspi       = "libiconv";
			spi->iconv_openSPI  = (ficonv_open)f;
			spi->iconvSPI = (ficonv)knh_dlsym(ctx, handler, "iconv", "libiconv", 0/*isTest*/);
			spi->iconv_closeSPI = (ficonv_close)knh_dlsym(ctx, handler, "libiconv_close", "iconv_close", 0);
			KNH_ASSERT(spi->iconvSPI != NULL && spi->iconv_closeSPI != NULL);
			return ; // OK
		}
	}
	PleaseLetUsKnowYourOS("libiconv is not available");
}

/*************************************************************************** */

struct konoha_module_driver {
	const char *name;
	void (*init)(int argc, int n, const char **argv);
	void (*exit)(void);
};

static void ac_init(int argc, int n, const char **argv)
{
//	if(uout != NULL) {
//		fprintf(uout, "testing: %s\n", argv[n]);
//		fflush(uout);
//	}
}

static void ac_exit(void)
{
//	if(uout != NULL) {
//		fclose(uout);
//	}
}

#ifdef K_USING_LLVM
extern void knh_llvm_init(int, int, const char **);
extern void knh_llvm_exit(void);
#endif
struct konoha_module_driver konoha_modules[] = {
	{"ac", ac_init, ac_exit},
#ifdef K_USING_LLVM
	{"llvm", knh_llvm_init, knh_llvm_exit},
#endif
	{"null", NULL, NULL}
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

int konoha_main(konoha_t konoha, int argc, const char **argv)
{
	CTX ctx = (CTX)konoha;
	int i, ret = 0, n = knh_parseopt(ctx, argc, argv);
	knh_linkDynamicReadline(ctx);
	knh_linkDynamicRegex(ctx);
	knh_linkDynamicIconv(ctx);
	for (i = 0; konoha_modules[i].init != NULL; ++i) {
		konoha_modules[i].init(argc, n, argv);
	}
	argc = argc - n;
	argv = argv + n;
	if(isActorMode) {
		char portstr[6] = {0};
		knh_snprintf(portstr, sizeof(portstr), "%d", port);
		const char *argv_actor[3] = {"actsrv", portstr, NULL};
		argc = 2;
		argv = argv_actor;
	}
	knh_parsearg(ctx, argc, argv);
	if(argc == 0) {
		ret = konoha_shell(ctx, NULL);
	}
	else if(isMPIMode) {
		// using konoha.mpi
		// if (rank == 0) load initscript
		///*
		if(kMPI_worldRank == 0 &&
		   knh_startScript(ctx, argv[0]) == K_CONTINUE) {
			ret = knh_runMain(ctx, argc, argv);
		}
		//*/
		kMPITaskContext *tctx = (kMPITaskContext*)ctx->wshare->mpictx;
		size_t initsiz = tctx->initsiz;
		MPI_Bcast(&initsiz, 1, MPI_INT, 0/*root*/, MPI_COMM_WORLD);
		if(initsiz > 0) {
			const char *initscript = (kMPI_worldRank == 0) ?
				tctx->initscript : (const char*)malloc(initsiz);
			MPI_Bcast((void*)&initscript[0], initsiz, MPI_CHAR, 0/*root*/, MPI_COMM_WORLD);
			kbytes_t bscript = {{initscript}, initsiz};
			if(knh_startBytesScript(ctx, bscript) == K_CONTINUE) {
				ret = knh_runMain(ctx, argc, argv);
			}
		}
		kMPITask *task = (kMPITask*)tctx->tasks; // head
		kMPIComm *tworld = tctx->world;
		while(task->next != NULL) {
			task = (kMPITask*)task->next;
			MPIC_COMM(tworld) = task->world;
			MPIC_init_rank(tworld);
			MPIC_init_size(tworld);
			MPI_Barrier(MPIC_COMM(tworld));
			kbytes_t bscript = {{task->script}, task->scriptsize};
			if(knh_startBytesScript(ctx, bscript) == K_CONTINUE) {
				ret = knh_runMain(ctx, argc, argv);
			}
		}
	}
	else {
		if(knh_startScript(ctx, argv[0]) == K_CONTINUE && !knh_isCompileOnly(ctx)) {
			ret = knh_runMain(ctx, argc, argv);
			if(isInteractiveMode) {
				konoha_shell(ctx, NULL);
			}
		}
	}
	for (i = 0; konoha_modules[i].exit != NULL; ++i) {
		konoha_modules[i].exit();
	}
	return ret;
}

/*************************************************************************** */

#ifdef __cplusplus
}
#endif
