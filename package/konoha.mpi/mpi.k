include "lib:gluelink";

@Native class MPIComm;
@Native class MPIData;
@Native class MPIRequest;
@Native class MPIOp;
@Native @Singleton class MPI;

/* ------------------------------------------------------------------------ */
/* General API */

@Native @Static void   MPI.preventLog();  // for ac
@Native @Static Float  MPI.getWtime();    // for MPI_Wtime
@Native @Static void   MPI.addTaskScript(MPIComm comm, String script);
@Native @Static String MPI.getTaskScript();
@Native @Static void   MPI.setTaskWorld(MPIComm comm);

/* ------------------------------------------------------------------------ */
/* Array Extra API */

@Public int[]   Array.getShape();
@Public void    Array.reshape(int[] shape);
@Public dynamic Array.cols(int start, int end);
@Public dynamic Array.rows(int start, int end);
@Public dynamic Array.cycle(int rank, int size);
@Public dynamic Array.trans();


/* ------------------------------------------------------------------------ */
/* MPI-1 Basic API */

@Native MPIComm MPIComm.getWorld();
@Native int     MPIComm.getRank();
@Native int     MPIComm.getSize();
@Native String  MPIComm.getProcessorName();
@Native int     MPIComm.barrier();
@Native MPIComm MPIComm.create(int[] ranks);

@Native MPIData MPIData.new (byte[] data, Class c);
@Native MPIData : (int i);
@Native MPIData : (int[] ia);
@Native MPIData : (float f);
@Native MPIData : (float[] fa);
@Native MPIData : (byte[] ba);
@Native int[]   : (MPIData md);
@Native float[] : (MPIData md);
@Native byte[]  : (MPIData md);
@Native int     MPIData.getDataType();
@Native Class   MPIData.getDataClass();
@Native int     MPIData.getSize();
@Native MPIData MPIData.opADD(int offset);

@Native boolean MPIRequest.test();
@Native boolean MPIRequest.wait();
@Native boolean MPIRequest.cancel();

@Native MPIOp MPIOp.new(Func<MPIData,MPIData> func, Boolean commutable);

/* ------------------------------------------------------------------------ */
/* Point-to-Point Communication API */

@Native boolean    MPIComm.send(MPIData sdata, int count, int dest_rank, int tag);
@Native boolean    MPIComm.recv(MPIData rdata, int count, int src_rank, int tag);
@Native boolean    MPIComm.sendrecv(MPIData sdata, int count, int dest_rank, int stag,
                                    MPIData rdata, int count, int src_rank, int rtag);
@Native MPIRequest MPIComm.iSend(MPIData sdata, int count, int dest_rank, int tag, MPIRequest _);
@Native MPIRequest MPIComm.iRecv(MPIData rdata, int count, int src_rank, int tag, MPIRequest _);


/* ------------------------------------------------------------------------ */
/* Collective Communication API */

@Native boolean MPIComm.bcast(MPIData sdata, int count, int root_rank);
@Native boolean MPIComm.scatter(MPIData sdata, int scount, MPIData rdata, int rcount, int root_rank);
@Native boolean MPIComm.gather(MPIData sdata, int scount, MPIData rdata, int rcount, int root_rank);
@Native boolean MPIComm.allGather(MPIData sdata, int scount, MPIData rdata, int rcount);
@Native boolean MPIComm.allToAll(MPIData sdata, int scount, MPIData rdata, int rcount);
@Native boolean MPIComm.reduce(MPIData sdata, MPIData rdata, int rcount, MPIOp op, int root_rank);
@Native boolean MPIComm.scan(MPIData sdata, MPIData rdata, int rcount, MPIOp op);
@Native boolean MPIComm.allReduce(MPIData sdata, MPIData rdata, int rcount, MPIOp op);
@Native boolean MPIComm.reduceScatter(MPIData sdata, MPIData rdata, int[] rcounts, MPIOp op);

/* ======================================================================== */
/* Utility Functions */

using konoha.io.*;

MPIData : (Object o) {
    byte[] bt = new byte[0];
    bt.writeMsgPack(o);
    return new MPIData(bt, o.class);
}
dynamic MPIData.decode(Class c) {
    return ((byte[])this).readMsgPack(0, 0, c);
}

/* ======================================================================== */
/* Vertiks */

using konoha.sugar.*;

/* ------------------------------------------------------------------------ */
// System

@Public void    System.p(dynamic d)               { OUT << d << EOL }
@Public void    System.e(dynamic d)               { ERR << d << EOL }
@Public void    System.x(int i, String s)         { if (i == 0) { p(s) } else { e(s) } exit(i) }
@Public void    System.todo(String s, dynamic d)  { OUT << "TODO: " + s; p(d) }
@Public void    System.eexit(boolean b, String s) { if (!b) x(1, "failed to launch: "+ s) }

/* ------------------------------------------------------------------------ */
// Token @using konoha.sugar

@Public String Token.rawText() {
    return (this.type in? [Token.TEXT2, Token.STEXT]) ? `"`+this.text+`"` :
           (this.type == Token.CODE) ? "{ "+this.text+" }" :
           (this.text == ";" && this.type == Token.OPERATOR) ? this.text + "\n" : this.text;
}

// only use for Token[]
String : (Token[] ta) {
    ret = "";
    if (|ta| > 0) {
        ret = ta[0].text;
        if (|ta| > 1) for (token : ta[1 to -1]) { ret += " " + token.rawText() }
    }
    return ret;
}

// only use for Token[]
@Public Token[] Array.tuntil(Func<Token=>boolean> condf) {
    Token[] ret = [];
    if (this instanceof Token[]) {
        for (token : this) { if (condf(token)) { break } else { ret << token } }
    }
    return ret;
}

// only use for int[]
String : (Array<int> ia) {
    ret = "";
    if (|ia| > 0) {
        ret = "%s"(ia[0]);
        if (|ia| > 1) for (i : ia[1 to -1]) { ret += ", " + i }
    }
    return ret;
}

/* ------------------------------------------------------------------------ */
// TaskFunc

class TaskFunc {
    String name;
    String sign;
    String sparam;
    String source;
    Map param;
    int[] ranks;
    TaskFunc(String name, String sign, String param, String source) {
        _name = name;
        _sign = sign;
        _sparam = param;
        _source = source;
        _param = {};
        _ranks = [];
    }
    void evalParam(TaskScript ts) {
        if (|_sparam| > 0) {
            for (stt : _sparam.split(",")) {
                kv = stt.trim().convert(to:lower).split("=");
                key = kv[0].trim();
                value = kv[1].trim();
                value = value.replace("SIZE", "%d"(ts.numprocs));
                value = value.replace("RANK", "%d"(ts.myrank));
                switch(key) {
                case "size" :
                    if ("OTHERS" in? value) {
                        value = value.replace("OTHERS", "0");
                        ivalue = (int)eval(value);
                        _param[key] = ivalue;
                    }
                    else {
                        ivalue = (int)eval(value);
                        eexit(ivalue > 0 && ivalue <= ts.numprocs, "illegal target task size");
                        _param[key] = ivalue;
                    }
                    break;
                case "rank" :
                    ivalue = (int)eval(value);
                    eexit(ivalue < ts.numprocs, "specified rank over");
                    _param[key] = ivalue;
                    _ranks << ivalue;
                    break;
                case "out" :
                case "err" :
                    _param[key] = value;
                    todo("specified out/err of task", value);
                    break;
                default: break;
                }
            }
        }
    }
    String getId() {
        return "task:"+ this.hashCode() + "(%s{#0})"(_name);
    }
    int getSize() {
        return ("size" in? _param) ? _param["size"] :
               ("rank" in? _param) ? |_ranks| : 1;
    }
}

/* ------------------------------------------------------------------------ */
// TaskScript @depends TaskFunc, Token

class TaskScript {
    int numprocs;
    int myrank;
    String script;
    Map<String,TaskFunc[]> tfmap;

    TaskScript() {
        _numprocs = MPIComm.world.size;
        _myrank = MPIComm.world.rank;
        _script = "";
        _tfmap  = {};
    }

    void exec() {
        parse();
        attach();
        run();
    }

    void parse() {
        tokens = Lang.tokenize(MPI.taskScript);
        paramf = def(Token t) { return t.text == ")" }
        argsf  = def(Token t) { return (t.text != "{" && t.type != Token.CODE) }
        codef  = def(Token t) { return t.text == "}" }

        for (int i = 0; i < |tokens|; i++) {
            token = tokens[i];
            if (token.type == Token.META && token.text == "@Task") {
                fparam = "";
                j = i;
                if (tokens[++j].text == "(") {
                    tpary = tokens[++j to -1].tuntil(paramf);
                    fparam = (String)tpary;
                    j += (|tpary| + 1);
                }
                rtype = tokens[j].text;
                fname = tokens[++j].text;
                sign = "%s{#0} %s{#1}"(rtype, fname);
                for (t : tokens[++j to -1]) {
                    if (t.text == "{" || t.type == Token.CODE) break;
                    sign += " %s{#0}"(t.text);
                    j++;
                }
                fsrc = "";
                if (tokens[j].type == Token.CODE) { // this may be always true
                    fsrc = tokens[j].text;
                }
                else { // *** may be ignored
                    tcary = tokens[++j to -1].tuntil(codef);
                    fsrc = "\n    " + (String)tcary;
                    j += (|tcary|);
                }
                tfunc = new TaskFunc(fname, sign, fparam, fsrc);
                if (fname in? _tfmap == false) {
                    _script += "%s{#0};\n"(tfunc.sign);
                    _tfmap[fname] = new TaskFunc[0];
                }
                tfary = (TaskFunc[])_tfmap[fname];
                tfary << tfunc;
                _script += "/* %s{#0} */\n"(tfunc.id);
                i = j;
            }
            else {
                _script += " " + token.rawText();
            }
        }
        trimscript = "";
        foreach (line in _script.split("\n")) {
            if (line.startsWith(" ")) {
                line = (|line|>1) ? line[1 to -1] : "";
            }
            trimscript += line + "\n";
        }
        _script = trimscript;
    }

    void attach() {
        for (tfname : _tfmap.keys()) {
            tfary = (TaskFunc[])tfmap[tfname];
            proc_reserved = new boolean[_numprocs];
            tf_remain = new TaskFunc[0];
            for (tf : tfary) {
                tf.evalParam(this);
                if (|tf.ranks| > 0) {
                    treserved = 0;
                    for (r : tf.ranks) {
                        eexit(!proc_reserved[r], "requested rank duplicated in [%s{#0}]"(tfname));
                        proc_reserved[r] = true;
                        treserved++;
                    }
                    if (treserved == tf.size) continue;
                }
                tf_remain << tf;
            }
            proc_remain = new int[0];
            for (int i = 0; i < _numprocs; i++) {
                if (!proc_reserved[i]) proc_remain << i;
            }
            for (tf : tf_remain) {
                tremain = (tf.size > 0) ? (tf.size - |tf.ranks|) : |proc_remain| + (tf.size);
                eexit(tremain >= 0, "requested task size < 0 int [%s{#0}]"(tfname));
                for (int i = 0; i < tremain; i++) {
                    eexit(|proc_remain| > 0, "requested task size > numprocs in [%s{#0}]"(tfname));
                    r = proc_remain[0];
                    proc_remain.remove(0);
                    tf.ranks << r;
                }
            }
        }
    }

    void run() {
        tf_procs = new TaskFunc[][_numprocs];
        for (int i = 0; i < _numprocs; i++) {
            tf_procs[i] = new TaskFunc[0];
        }
        for (tfname : tfmap.keys()) {
            tfary = (TaskFunc[])_tfmap[tfname];
            for (tf : tfary) {
                for (tr : tf.ranks) {
                    tf_procs[tr] << tf;
                }
            }
        }
        phead = "MPI.setTaskWorld(MPIComm.WORLD);\n";
        comms = [];
        myscript = "";
        MPIComm mycomm;
        for (int i = 0; i < _numprocs; i++) {
            tfs = (TaskFunc[])tf_procs[i];
            pscript = _script;
            for (tf : tfs) {
                pcomm = "comm_%s{#0}"("%s"(tf.hashCode()));
                if (pcomm in? comms == false) {
                    phead += "%s{#0} = MPIComm.WORLD.create([%s{#1}]);\n"(pcomm, (String)tf.ranks);
                    comms << pcomm;
                }
                tscript = "%s{#0}\n{\n    MPI.setTaskWorld(%s{#1});%s{#2}    MPI.setTaskWorld(MPIComm.WORLD);\n}"(tf.sign, pcomm, tf.source);
                pscript = pscript.replace("/* %s{#0} */"(tf.id), tscript);
            }
            comm = MPIComm.world.create([i]);
            if (i == _myrank) {
                myscript = pscript;
                mycomm = comm;
            }
        }
        MPI.addTaskScript(mycomm, phead + "\n" + myscript);
    }
}

// EOF
