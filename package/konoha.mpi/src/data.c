#include "../konoha_mpi.h"

/* ------------------------------------------------------------------------ */
/* TYPEMAP */

TYPEMAP Int_MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->i = new_Int(ctx, sfp[1].ivalue);
	MPID_TYPE(data) = MPI_LONG;
	MPID_CID(data) = CLASS_Int;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP int___MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->a = sfp[1].a;
	MPID_TYPE(data) = MPI_LONG;
	MPID_CID(data) = CLASS_Array;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP Float_MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->f = new_Float_(ctx, CLASS_Float, sfp[1].fvalue);
	MPID_TYPE(data) = MPI_DOUBLE;
	MPID_CID(data) = CLASS_Float;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP float___MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->a = sfp[1].a;
	MPID_TYPE(data) = MPI_DOUBLE;
	MPID_CID(data) = CLASS_Array;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP Bytes_MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->ba = sfp[1].ba;
	MPID_TYPE(data) = MPI_CHAR;
	MPID_CID(data) = CLASS_Bytes;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP String_MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	data->s = sfp[1].s;
	MPID_TYPE(data) = MPI_CHAR;
	MPID_CID(data) = CLASS_String;
	MPID_DCID(data) = O_cid(sfp[1].o);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

TYPEMAP MPIData_int__(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[1].o);
	if (MPID_CID(data) != CLASS_Array || MPID_DCID(data) != CLASS_ArrayInt) {
		NTHROW_CAST_MPID(ctx, sfp, data);
	}
	RETURN_(data->a);
}

TYPEMAP MPIData_float__(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[1].o);
	//if (MPID_CID(data) != CLASS_Array || MPID_DCID(data) != CLASS_ArrayFloat) {
	if (MPID_CID(data) != CLASS_Array || MPID_TYPE(data) != MPI_DOUBLE) {
		NTHROW_CAST_MPID(ctx, sfp, data);
	}
	RETURN_(data->a);
}

TYPEMAP MPIData_Bytes(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[1].o);
	if (MPID_CID(data) != CLASS_Bytes || MPID_DCID(data) != CLASS_Bytes) {
		NTHROW_CAST_MPID(ctx, sfp, data);
	}
	RETURN_(data->ba);
}

/* ------------------------------------------------------------------------ */

static kMethod* knh_loadMethod(CTX ctx, kclass_t cid, kbytes_t mtdnm)
{
	kmethodn_t mn = knh_getmn(ctx, mtdnm, MN_NONAME);
	return knh_NameSpace_getMethodNULL(ctx, NULL, cid, mn);
}

static kBytes* knh_MPIData_serialize(CTX ctx, ksfp_t *sfp, kObject* target)
{
	size_t init = K_FASTMALLOC_SIZE;
	kBytes *ba = new_Bytes(ctx, "mpiobj", init);
	knh_bzero(ba->bu.ubuf, init);
	ba->dim = new_dim(ctx, init, 1);
	{
		kMethod *wmsg = knh_loadMethod(ctx, knh_getcid(ctx, STEXT("Bytes")), STEXT("writeMsgPack"));
		if (wmsg == NULL) {
			KNH_NTHROW2(ctx, sfp, "Script!!", "method 'Bytes.writeMsgPack' not found", K_FAILED, KNH_LDATA0);
		}
		CLOSURE_start(1);
		KNH_SETv(ctx, lsfp[K_CALLDELTA+0].o, ba);
		KNH_SETv(ctx, lsfp[K_CALLDELTA+1].o, target);
		KNH_SCALL(ctx, lsfp, 0, wmsg, 1);
		CLOSURE_end();
	}
	return ba;
}

static kObject* knh_MPIData_deserialize(CTX ctx, ksfp_t *sfp, kBytes* target, kclass_t cid)
{
	kMethod *rmsg = knh_loadMethod(ctx, knh_getcid(ctx, STEXT("Bytes")), STEXT("readMsgPack"));
	if (rmsg == NULL) {
		KNH_NTHROW2(ctx, sfp, "Script!!", "method 'Bytes.readMsgPack' not found", K_FAILED, KNH_LDATA0);
	}
	CLOSURE_start(3);
	KNH_SETv(ctx, lsfp[K_CALLDELTA+0].o, target);
	lsfp[K_CALLDELTA+1].ivalue = 0;
	lsfp[K_CALLDELTA+2].ivalue = 0;
	KNH_SETv(ctx, lsfp[K_CALLDELTA+3].o, new_Type(ctx, cid));
	KNH_SCALL(ctx, lsfp, 0, rmsg, 3);
	kObject *obj = lsfp[K_RTNIDX].o;
	CLOSURE_end();
	return obj;
}

TYPEMAP Object_MPIData(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	kObject *obj = sfp[1].o;
	kBytes *serialized = knh_MPIData_serialize(ctx, sfp, obj);
	if (serialized == NULL) {
		KNH_NTHROW2(ctx, sfp, "Script!!", "object serialization failed", K_FAILED,
					KNH_LDATA(LOG_s("class", SAFECLASS__(ctx, O_cid(obj)))));
	}
	data->ba = serialized;
	MPID_TYPE(data) = MPI_CHAR;
	MPID_CID(data) = CLASS_Bytes;
	MPID_DCID(data) = O_cid(obj);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

KMETHOD MPIData_decode(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	if (MPID_CID(data) != CLASS_Bytes || MPID_DCID(data) == CLASS_Bytes) {
		NTHROW_CAST_MPID(ctx, sfp, data);
	}
	kclass_t dcid = MPID_DCID(data);
	kObject *obj = knh_MPIData_deserialize(ctx, sfp, data->ba, dcid);
	if (obj == NULL) {
		KNH_NTHROW2(ctx, sfp, "Script!!", "object serialization failed", K_FAILED,
					KNH_LDATA(LOG_s("class", SAFECLASS__(ctx, dcid))));
	}
	RETURN_(obj);
}

/* ------------------------------------------------------------------------ */

void* knh_MPIData_getAddr(kMPIData *data)
{
	switch (MPID_CID(data)) {
	case CLASS_Int:
		return &O_data(data->i);
	case CLASS_Float:
		return &O_data(data->f);
	case CLASS_Array:
		if (MPID_DCID(data) == CLASS_ArrayInt)
			return data->a->ilist + MPID_POFS(data);
		//else if (MPID_DCID(data) == CLASS_ArrayFloat)
		else if (MPID_TYPE(data) == MPI_DOUBLE)
			return data->a->flist + MPID_POFS(data);
	case CLASS_Bytes:
		return data->ba->bu.buf + MPID_POFS(data);
	case CLASS_String:
		return data->s->str.buf + MPID_POFS(data);
	default:
		return (data->ba) ? data->ba->bu.buf + MPID_POFS(data) : NULL;
	}
}

void knh_MPIData_expand(CTX ctx, kMPIData *data, int *count, int *inc)
{
	if (count <= 0) {
		*count = 0;  // invalid param
		*inc = 0;
		return;
	}
	int cur_size = MPID_SIZE(data);
	int new_size = MPID_POFS(data) + *count;
	int capacity = knh_MPIData_getCapacity(data);
	if (new_size <= cur_size) {
		*inc = 0; // no need to incl
		return;
	}
	if (new_size > capacity) {
		switch (MPID_CID(data)) {
		case CLASS_Bytes: {
			int exp_size = k_grow(capacity);
			if (exp_size < new_size) exp_size = k_goodsize(new_size);
			knh_Bytes_expands(ctx, data->ba, exp_size);
			break;
		}
		case CLASS_Array: {
			knh_Array_grow(ctx, data->a, new_size, 0);
			break;
		}
		default: {
			*count = 0;
			*inc = 0;
			return;
		}
		}
	}
	*inc = new_size - cur_size;
}

int knh_MPIData_getSize(kMPIData *data)
{
	switch (MPID_CID(data)) {
	case CLASS_Array:
		return knh_Array_size(data->a);
	case CLASS_Bytes:
		return BA_size(data->ba);
	case CLASS_String:
		return S_size(data->s);
	default:
		return (data->ba) ? BA_size(data->ba) : 0;
	}
}

int knh_MPIData_incSize(kMPIData *data, int count)
{
	if (count <= 0) return 0;
	switch (MPID_CID(data)) {
	case CLASS_Array: {
		knh_Array_size(data->a) += count;
		return count;
	}
	case CLASS_Bytes: {
		BA_size(data->ba) += count;
		return count;
	}
	}
	return 0;
}

int knh_MPIData_getCapacity(kMPIData *data)
{
	switch (MPID_CID(data)) {
	case CLASS_Array:
		return data->a->dim->capacity;
	case CLASS_Bytes:
		return data->ba->dim->capacity;
	default:
		break;
	}
	return -1;
}

void  knh_MPIData_checkCount(kMPIData *data, int *count)
{
	int size = MPID_SIZE(data);
	int pofs = MPID_POFS(data);
	if (*count > size - pofs) {
		// cut down count number.
		*count = size - pofs;
	}
}

/* ------------------------------------------------------------------------ */
//## method MPIData MPIData.new(Class class);

KMETHOD MPIData_new(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	size_t init = K_FASTMALLOC_SIZE;
	kBytes *ba = new_Bytes(ctx, "mpiobj", init);
	knh_bzero(ba->bu.ubuf, init);
	ba->dim = new_dim(ctx, init, 1);
	data->ba = ba;
	MPID_TYPE(data) = MPI_CHAR;
	MPID_CID(data) = CLASS_Bytes;
	MPID_DCID(data) = knh_Class_cid(sfp[1].c);
	MPID_POFS(data) = 0;
	RETURN_(data);
}

/* ------------------------------------------------------------------------ */
//## method int MPIData.getDataType();

KMETHOD MPIData_getDataType(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	RETURNi_((kint_t)MPID_TYPE(data));
}

/* ------------------------------------------------------------------------ */
//## method Class MPIData.getDataClass();

KMETHOD MPIData_getDataClass(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	kclass_t cid = MPID_CID(data);
	kclass_t dcid = MPID_DCID(data);
	RETURN_(new_Type(ctx, (cid == CLASS_Bytes || cid == CLASS_Array) ? dcid : cid));
}

/* ------------------------------------------------------------------------ */
//## medhot int MPIData.getSize();

KMETHOD MPIData_getSize(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	RETURNi_(MPID_SIZE(data));
}

/* ------------------------------------------------------------------------ */
//## medhot dynamic MPIData.get(int n);

KMETHOD MPIData_get(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	kObject *ret = UPCAST(data);
	if (MPID_CID(data) == CLASS_Array) {
		int idx = Int_to(int, sfp[1]);
		if (idx < 0) {
			KNH_NTHROW2(ctx, sfp, "Script!!", "invalid index", K_FAILED, KNH_LDATA(LOG_i("index", idx)));
		}
		kArray *a = data->a;
		if (MPID_DCID(data) == CLASS_ArrayInt) {
			ret = UPCAST(new_Int(ctx, a->ilist[idx]));
		}
		//else if (MPID_DCID(data) == CLASS_ArrayFloat) {
		else if (MPID_TYPE(data) == MPI_DOUBLE) {
			ret = UPCAST(new_Float(ctx, a->flist[idx]));
		}
		else {
			// TODO: general Array
		}
	}
	else {
		// FIXME: index access not allowed for (type of xxx)
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */
//## medhot dynamic MPIData.set(int n, dynamic d);

KMETHOD MPIData_set(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	MPID_WCHK(data); // NTHROW except types of Array or Bytes
	int idx = Int_to(int, sfp[1]);
	if (idx < 0) {
		KNH_NTHROW2(ctx, sfp, "Script!!", "invalid index", K_FAILED, KNH_LDATA(LOG_i("index", idx)));
	}
	if (MPID_CID(data) == CLASS_Array) {
		kArray *a = data->a;
		if (MPID_DCID(data) == CLASS_ArrayInt) {
			kint_t val = Int_to(kint_t, sfp[2]);
			a->ilist[idx] = val;
		}
		//else if (MPID_DCID(data) == CLASS_ArrayFloat) {
		else if (MPID_TYPE(data) == MPI_DOUBLE) {
			kfloat_t val = Float_to(kfloat_t, sfp[2]);
			a->flist[idx] = val;
		}
		else {
			// TODO: general Array
		}
	}
	else {
		kBytes *b = data->ba;
		// TODO: Bytes
		(void)b;
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method MPIData MPIData.opADD(int offset);

KMETHOD MPIData_opADD(CTX ctx, ksfp_t *sfp _RIX)
{
	MPID(data, sfp[0].o);
	MPID(newdata, new_O(MPIData, knh_getcid(ctx, B("konoha.mpi.MPIData"))));
	newdata->o = data->o;
	MPID_TYPE(newdata) = MPID_TYPE(data);
	MPID_CID(newdata) = MPID_CID(data);
	MPID_DCID(newdata) = MPID_DCID(data);
	int ofs = Int_to(int, sfp[1]);
	if (ofs < 0 || MPID_SIZE(data) < ofs) {
		ofs = 0;
		KNH_NTHROW2(ctx, sfp, "Script!!", "invalid offset value", K_FAILED, KNH_LDATA(LOG_i("offset", ofs)));
	}
	MPID_POFS(newdata) = MPID_POFS(data) + ofs;
	RETURN_(newdata);
}

