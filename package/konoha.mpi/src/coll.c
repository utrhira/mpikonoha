#include "../konoha_mpi.h"

static int knh_MPI_BcastBytes(CTX ctx, knh_Object_t *c, knh_Object_t *odata, int count, int root_rank)
{
	COMM(comm, c);
	BA(data, odata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count == 0) {
				count = BA_size(data);
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			ret = MPI_Bcast(BA_buf(data), count, BA_Type, root_rank, KNH_MPI_COMM(comm));
		} else {
			if (count == 0) {
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			KNH_BA_EXPAND(data, count);
			ret = MPI_Bcast(BA_tail(data), count, BA_Type, root_rank, KNH_MPI_COMM(comm));
			BA_size(data) += count;
		}
	}
	return ret;
}

static int knh_MPI_BcastInt(CTX ctx, knh_Object_t *c, knh_Object_t *odata, int count, int root_rank)
{
	COMM(comm, c);
	IA(data, odata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count == 0) {
				count = IA_size(data);
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			ret = MPI_Bcast(IA_buf(data), count, IA_Type, root_rank, KNH_MPI_COMM(comm));
		} else {
			if (count == 0) {
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			KNH_IA_EXPAND(data, count);
			ret = MPI_Bcast(IA_tail(data), count, IA_Type, root_rank, KNH_MPI_COMM(comm));
			IA_size(data) += count;
		}
	}
	return ret;
}

static int knh_MPI_BcastFloat(CTX ctx, knh_Object_t *c, knh_Object_t *odata, int count, int root_rank)
{
	COMM(comm, c);
	FA(data, odata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count == 0) {
				count = FA_size(data);
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			ret = MPI_Bcast(FA_buf(data), count, FA_Type, root_rank, KNH_MPI_COMM(comm));
		} else {
			if (count == 0) {
				MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm)); /* bcast for buffer_length */
			}
			KNH_FA_EXPAND(data, count);
			ret = MPI_Bcast(FA_tail(data), count, FA_Type, root_rank, KNH_MPI_COMM(comm));
			FA_size(data) += count;
		}
	}
	return ret;
}

static int knh_MPI_ScatterBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count == 0) {
			if (KNH_MPI_RANK(comm) == root_rank) {
				count = BA_size(asdata) / KNH_MPI_SIZE(comm);
				if (BA_size(asdata) % KNH_MPI_SIZE(comm) != 0) count++;
			}
			MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
		}
		KNH_BA_EXPAND(ardata, count);
		ret = MPI_Scatter(BA_buf(asdata), count, BA_Type, BA_tail(ardata), count, BA_Type, root_rank, KNH_MPI_COMM(comm));
		BA_size(ardata) += count;
	}
	return ret;
}

static int knh_MPI_ScatterInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count == 0) {
			if (KNH_MPI_RANK(comm) == root_rank) {
				count = IA_size(asdata) / KNH_MPI_SIZE(comm);
				if (IA_size(asdata) % KNH_MPI_SIZE(comm) != 0) count++;
			}
			MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
		}
		KNH_IA_EXPAND(ardata, count);
		ret = MPI_Scatter(IA_buf(asdata), count, IA_Type, IA_tail(ardata), count, IA_Type, root_rank, KNH_MPI_COMM(comm));
		IA_size(ardata) += count;
	}
	return ret;
}

static int knh_MPI_ScatterFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count == 0) {
			if (KNH_MPI_RANK(comm) == root_rank) {
				count = FA_size(asdata) / KNH_MPI_SIZE(comm);
				if (FA_size(asdata) % KNH_MPI_SIZE(comm) != 0) count++;
			}
			MPI_Bcast(&count, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
		}
		KNH_FA_EXPAND(ardata, count);
		ret = MPI_Scatter(FA_buf(asdata), count, FA_Type, FA_tail(ardata), count, FA_Type, root_rank, KNH_MPI_COMM(comm));
		FA_size(ardata) += count;
	}
	return ret;
}

static int knh_MPI_GatherBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count > 0) {
				int rcount = count * KNH_MPI_SIZE(comm);
				KNH_BA_EXPAND(ardata, rcount);
				ret = MPI_Gather(BA_buf(asdata), count, BA_Type, BA_tail(ardata), count, BA_Type, root_rank, KNH_MPI_COMM(comm));
				BA_size(ardata) += rcount;
			} else {
				int rcounts[KNH_MPI_SIZE(comm)];
				int rdispls[KNH_MPI_SIZE(comm)];
				ret = MPI_Gather(&BA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					int i, rc, rcount;
					for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
						rc = rcounts[i];
						rdispls[i] = rcount;
					}
					KNH_BA_EXPAND(ardata, rcount);
					ret = MPI_Gatherv(BA_buf(asdata), BA_size(asdata), BA_Type,
									  BA_tail(ardata), rcounts, rdispls, BA_Type, root_rank, KNH_MPI_COMM(comm));
					BA_size(ardata) += rcount;
				}
			}
		} else {
			if (count > 0) {
				ret = MPI_Gather(BA_buf(asdata), count, BA_Type, NULL, count, BA_Type, root_rank, KNH_MPI_COMM(comm));
			} else {
				ret = MPI_Gather(&BA_size(asdata), 1, MPI_INT, NULL, 0, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					ret = MPI_Gatherv(BA_buf(asdata), BA_size(asdata), BA_Type, NULL, NULL, NULL, 0, root_rank, KNH_MPI_COMM(comm));
				}
			}
		}
	}
	return ret;
}

static int knh_MPI_GatherInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count > 0) {
				int rcount = count * KNH_MPI_SIZE(comm);
				KNH_IA_EXPAND(ardata, rcount);
				ret = MPI_Gather(IA_buf(asdata), count, IA_Type, IA_tail(ardata), count, IA_Type, root_rank, KNH_MPI_COMM(comm));
				IA_size(ardata) += rcount;
			} else {
				int rcounts[KNH_MPI_SIZE(comm)];
				int rdispls[KNH_MPI_SIZE(comm)];
				ret = MPI_Gather(&IA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					int i, rc, rcount;
					for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
						rc = rcounts[i];
						rdispls[i] = rcount;
					}
					KNH_IA_EXPAND(ardata, rcount);
					ret = MPI_Gatherv(IA_buf(asdata), IA_size(asdata), IA_Type,
									  IA_tail(ardata), rcounts, rdispls, IA_Type, root_rank, KNH_MPI_COMM(comm));
					IA_size(ardata) += rcount;
				}
			}
		} else {
			if (count > 0) {
				ret = MPI_Gather(IA_buf(asdata), count, IA_Type, NULL, count, IA_Type, root_rank, KNH_MPI_COMM(comm));
			} else {
				ret = MPI_Gather(&IA_size(asdata), 1, MPI_INT, NULL, 0, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					ret = MPI_Gatherv(IA_buf(asdata), IA_size(asdata), IA_Type, NULL, NULL, NULL, 0, root_rank, KNH_MPI_COMM(comm));
				}
			}
		}
	}
	return ret;
}

static int knh_MPI_GatherFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata, int root_rank)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (KNH_MPI_RANK(comm) == root_rank) {
			if (count > 0) {
				int rcount = count * KNH_MPI_SIZE(comm);
				KNH_FA_EXPAND(ardata, rcount);
				ret = MPI_Gather(FA_buf(asdata), count, FA_Type, FA_tail(ardata), count, FA_Type, root_rank, KNH_MPI_COMM(comm));
				FA_size(ardata) += rcount;
			} else {
				int rcounts[KNH_MPI_SIZE(comm)];
				int rdispls[KNH_MPI_SIZE(comm)];
				ret = MPI_Gather(&FA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					int i, rc, rcount;
					for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
						rc = rcounts[i];
						rdispls[i] = rcount;
					}
					KNH_FA_EXPAND(ardata, rcount);
					ret = MPI_Gatherv(FA_buf(asdata), FA_size(asdata), FA_Type,
									  FA_tail(ardata), rcounts, rdispls, FA_Type, root_rank, KNH_MPI_COMM(comm));
					FA_size(ardata) += rcount;
				}
			}
		} else {
			if (count > 0) {
				ret = MPI_Gather(FA_buf(asdata), count, FA_Type, NULL, count, FA_Type, root_rank, KNH_MPI_COMM(comm));
			} else {
				ret = MPI_Gather(&FA_size(asdata), 1, MPI_INT, NULL, 0, MPI_INT, root_rank, KNH_MPI_COMM(comm));
				if (KNH_MPI_SUCCESS(ret)) {
					ret = MPI_Gatherv(FA_buf(asdata), FA_size(asdata), FA_Type, NULL, NULL, NULL, 0, root_rank, KNH_MPI_COMM(comm));
				}
			}
		}
	}
	return ret;
}

static int knh_MPI_AllgatherBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_BA_EXPAND(ardata, rcount);
			ret = MPI_Allgather(BA_buf(asdata), count, BA_Type, BA_tail(ardata), count, BA_Type, KNH_MPI_COMM(comm));
			BA_size(ardata) += rcount;
		} else {
			int rcounts[KNH_MPI_SIZE(comm)];
			int rdispls[KNH_MPI_SIZE(comm)];
			ret = MPI_Allgather(&BA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				int i, rc, rcount;
				for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
					rc = rcounts[i];
					rdispls[i] = rcount;
				}
				KNH_BA_EXPAND(ardata, rcount);
				ret = MPI_Allgatherv(BA_buf(asdata), BA_size(asdata), BA_Type, BA_tail(ardata), rcounts, rdispls, BA_Type, KNH_MPI_COMM(comm));
				BA_size(ardata) += rcount;
			}
		}
	}
	return ret;
}

static int knh_MPI_AllgatherInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_IA_EXPAND(ardata, rcount);
			ret = MPI_Allgather(IA_buf(asdata), count, IA_Type, IA_tail(ardata), count, IA_Type, KNH_MPI_COMM(comm));
			IA_size(ardata) += rcount;
		} else {
			int rcounts[KNH_MPI_SIZE(comm)];
			int rdispls[KNH_MPI_SIZE(comm)];
			ret = MPI_Allgather(&IA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				int i, rc, rcount;
				for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
					rc = rcounts[i];
					rdispls[i] = rcount;
				}
				KNH_IA_EXPAND(ardata, rcount);
				ret = MPI_Allgatherv(IA_buf(asdata), IA_size(asdata), IA_Type, IA_tail(ardata), rcounts, rdispls, IA_Type, KNH_MPI_COMM(comm));
				IA_size(ardata) += rcount;
			}
		}
	}
	return ret;
}

static int knh_MPI_AllgatherFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_FA_EXPAND(ardata, rcount);
			ret = MPI_Allgather(FA_buf(asdata), count, FA_Type, FA_tail(ardata), count, FA_Type, KNH_MPI_COMM(comm));
			FA_size(ardata) += rcount;
		} else {
			int rcounts[KNH_MPI_SIZE(comm)];
			int rdispls[KNH_MPI_SIZE(comm)];
			ret = MPI_Allgather(&FA_size(asdata), 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				int i, rc, rcount;
				for (i = 0, rcount = 0; i < KNH_MPI_SIZE(comm); i++, rcount += rc) {
					rc = rcounts[i];
					rdispls[i] = rcount;
				}
				KNH_FA_EXPAND(ardata, rcount);
				ret = MPI_Allgatherv(FA_buf(asdata), FA_size(asdata), FA_Type, FA_tail(ardata), rcounts, rdispls, FA_Type, KNH_MPI_COMM(comm));
				FA_size(ardata) += rcount;
			}
		}
	}
	return ret;
}

static int knh_MPI_AlltoallBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_BA_EXPAND(ardata, rcount);
			ret = MPI_Alltoall(BA_buf(asdata), count, BA_Type, BA_tail(ardata), count, BA_Type, KNH_MPI_COMM(comm));
			BA_size(ardata) += rcount;
		} else {
			size_t size = KNH_MPI_SIZE(comm);
			int scounts[size], sdispls[size], rcounts[size], rdispls[size];
			int r, wc, sum_count;
			int quot = BA_size(asdata) / size;
			int mod = BA_size(asdata) % size;
			for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
				wc = quot + ((r < mod) ? 1 : 0);
				scounts[r] = wc;
				sdispls[r] = sum_count;
			}
			ret = MPI_Alltoall(scounts, 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
					wc = rcounts[r];
					rdispls[r] = sum_count;
				}
				KNH_BA_EXPAND(ardata, sum_count);
				ret = MPI_Alltoallv(BA_buf(asdata), scounts, sdispls, BA_Type, BA_tail(ardata), rcounts, rdispls, BA_Type, KNH_MPI_COMM(comm));
				BA_size(ardata) += sum_count;
			}
		}
	}
	return ret;
}

static int knh_MPI_AlltoallInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_IA_EXPAND(ardata, rcount);
			ret = MPI_Alltoall(IA_buf(asdata), count, IA_Type, IA_tail(ardata), count, IA_Type, KNH_MPI_COMM(comm));
			IA_size(ardata) += rcount;
		} else {
			size_t size = KNH_MPI_SIZE(comm);
			int scounts[size], sdispls[size], rcounts[size], rdispls[size];
			int r, wc, sum_count;
			int quot = IA_size(asdata) / size;
			int mod = IA_size(asdata) % size;
			for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
				wc = quot + ((r < mod) ? 1 : 0);
				scounts[r] = wc;
				sdispls[r] = sum_count;
			}
			ret = MPI_Alltoall(scounts, 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
					wc = rcounts[r];
					rdispls[r] = sum_count;
				}
				KNH_IA_EXPAND(ardata, sum_count);
				ret = MPI_Alltoallv(IA_buf(asdata), scounts, sdispls, IA_Type, IA_tail(ardata), rcounts, rdispls, IA_Type, KNH_MPI_COMM(comm));
				IA_size(ardata) += sum_count;
			}
		}
	}
	return ret;
}

static int knh_MPI_AlltoallFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, int count, knh_Object_t *rdata)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	int ret = -1;
	if (KNH_ON_MPI(comm)) {
		if (count > 0) {
			int rcount = count * KNH_MPI_SIZE(comm);
			KNH_FA_EXPAND(ardata, rcount);
			ret = MPI_Alltoall(FA_buf(asdata), count, FA_Type, FA_tail(ardata), count, FA_Type, KNH_MPI_COMM(comm));
			FA_size(ardata) += rcount;
		} else {
			size_t size = KNH_MPI_SIZE(comm);
			int scounts[size], sdispls[size], rcounts[size], rdispls[size];
			int r, wc, sum_count;
			int quot = FA_size(asdata) / size;
			int mod = FA_size(asdata) % size;
			for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
				wc = quot + ((r < mod) ? 1 : 0);
				scounts[r] = wc;
				sdispls[r] = sum_count;
			}
			ret = MPI_Alltoall(scounts, 1, MPI_INT, rcounts, 1, MPI_INT, KNH_MPI_COMM(comm));
			if (KNH_MPI_SUCCESS(ret)) {
				for (r = 0, sum_count = 0; r < size; r++, sum_count += wc) {
					wc = rcounts[r];
					rdispls[r] = sum_count;
				}
				KNH_FA_EXPAND(ardata, sum_count);
				ret = MPI_Alltoallv(FA_buf(asdata), scounts, sdispls, FA_Type, FA_tail(ardata), rcounts, rdispls, FA_Type, KNH_MPI_COMM(comm));
				FA_size(ardata) += sum_count;
			}
		}
	}
	return ret;
}

static int knh_MPI_ReduceBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o, int root_rank)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&BA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		if (KNH_MPI_RANK(comm) == root_rank) {
			KNH_BA_EXPAND(ardata, count);
			ret = MPI_Reduce(BA_buf(asdata), BA_tail(ardata), count, BA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
			BA_size(ardata) += count;
		} else {
			ret = MPI_Reduce(BA_buf(asdata), NULL, count, BA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
		}
	}
	return ret;
}

static int knh_MPI_ReduceInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o, int root_rank)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&IA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		if (KNH_MPI_RANK(comm) == root_rank) {
			KNH_IA_EXPAND(ardata, count);
			ret = MPI_Reduce(IA_buf(asdata), IA_tail(ardata), count, IA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
			IA_size(ardata) += count;
		} else {
			ret = MPI_Reduce(IA_buf(asdata), NULL, count, IA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
		}
	}
	return ret;
}

static int knh_MPI_ReduceFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o, int root_rank)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&FA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		if (KNH_MPI_RANK(comm) == root_rank) {
			KNH_FA_EXPAND(ardata, count);
			ret = MPI_Reduce(FA_buf(asdata), FA_tail(ardata), count, FA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
			FA_size(ardata) += count;
		} else {
			ret = MPI_Reduce(FA_buf(asdata), NULL, count, FA_Type, KNH_MPI_OP(op), root_rank, KNH_MPI_COMM(comm));
		}
	}
	return ret;
}

static int knh_MPI_AllreduceBytes(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o)
{
	COMM(comm, c);
	BA(asdata, sdata);
	BA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&BA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		KNH_BA_EXPAND(ardata, count);
		ret = MPI_Allreduce(BA_buf(asdata), BA_tail(ardata), count, BA_Type, KNH_MPI_OP(op), KNH_MPI_COMM(comm));
		BA_size(ardata) += count;
	}
	return ret;
}

static int knh_MPI_AllreduceInt(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o)
{
	COMM(comm, c);
	IA(asdata, sdata);
	IA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&IA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		KNH_IA_EXPAND(ardata, count);
		ret = MPI_Allreduce(IA_buf(asdata), IA_tail(ardata), count, IA_Type, KNH_MPI_OP(op), KNH_MPI_COMM(comm));
		IA_size(ardata) += count;
	}
	return ret;
}

static int knh_MPI_AllreduceFloat(CTX ctx, knh_Object_t *c, knh_Object_t *sdata, knh_Object_t *rdata, int count, knh_Object_t *o)
{
	COMM(comm, c);
	FA(asdata, sdata);
	FA(ardata, rdata);
	OP(op, o);
	int ret = -1;
	if (KNH_ON_MPI(comm) && !KNH_MPI_OP_IS_NULL(op)) {
		if (count == 0) {
			ret = MPI_Allreduce(&FA_size(asdata), &count, 1, MPI_INT, MPI_MIN, KNH_MPI_COMM(comm)); /* get mininum data cont */
			if (!KNH_MPI_SUCCESS(ret)) return ret;
		}
		KNH_FA_EXPAND(ardata, count);
		ret = MPI_Allreduce(FA_buf(asdata), FA_tail(ardata), count, FA_Type, KNH_MPI_OP(op), KNH_MPI_COMM(comm));
		FA_size(ardata) += count;
	}
	return ret;
}

/* ------------------------------------------------------------------------ */

//## method Int MPICommunicator.bcast(dynamic data, Int root_rank);
KMETHOD MPICommunicator_bcast(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *odata = sfp[1].o;
	int ret = -1;
	if (IS_Bytes(odata)) {
		ret = knh_MPI_BcastBytes(ctx, sfp[0].o, odata, 0, Int_to(int, sfp[2]));
	} else if (IS_IArray(odata)) {
		ret = knh_MPI_BcastInt(ctx, sfp[0].o, odata, 0, Int_to(int, sfp[2]));
	} else if (IS_FArray(odata)) {
		ret = knh_MPI_BcastFloat(ctx, sfp[0].o, odata, 0, Int_to(int, sfp[2]));
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.bcastBytes(Bytes sdata, Int count, Int root_rank);
KMETHOD MPICommunicator_bcastBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_BcastBytes(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), Int_to(int, sfp[3])));
}

//## method Int MPICommunicator.bcastInt(IArray sdata, Int count, Int root_rank);
KMETHOD MPICommunicator_bcastInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_BcastInt(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), Int_to(int, sfp[3])));
}

//## method Int MPICommunicator.bcastFloat(FArray sdata, Int count, Int root_rank);
KMETHOD MPICommunicator_bcastFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_BcastFloat(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), Int_to(int, sfp[3])));
}

//## method Int MPICommunicator.scatter(dynamic sdata, dynamic rdata, Int root_rank);
KMETHOD MPICommunicator_scatter(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_ScatterBytes(ctx, sfp[0].o, sdata, 0, rdata, Int_to(int, sfp[3]));
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_ScatterInt(ctx, sfp[0].o, sdata, 0, rdata, Int_to(int, sfp[3]));
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_ScatterFloat(ctx, sfp[0].o, sdata, 0, rdata, Int_to(int, sfp[3]));
	}
	RETURNi_(0);
}

//## method Int MPICommunicator.scatterBytes(Bytes sdata, Int count, Bytes rdata, Int root_rank);
KMETHOD MPICommunicator_scatterBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ScatterBytes(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.scatterInt(IArray sdata, Int count, IArray rdata, Int root_rank);
KMETHOD MPICommunicator_scatterInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ScatterInt(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.scatterFloat(FArray sdata, Int count, FArray rdata, Int root_rank);
KMETHOD MPICommunicator_scatterFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ScatterFloat(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.gather(dynamic sdata, dynamic rdata, Int root_rank);
KMETHOD MPICommunicator_gather(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_GatherBytes(ctx, sfp[0].o, sdata, 0, sfp[2].o, Int_to(int, sfp[3]));
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_GatherInt(ctx, sfp[0].o, sdata, 0, sfp[2].o, Int_to(int, sfp[3]));
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_GatherFloat(ctx, sfp[0].o, sdata, 0, sfp[2].o, Int_to(int, sfp[3]));
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.gatherBytes(Bytes sdata, Int count, Bytes rdata, Int root_rank);
KMETHOD MPICommunicator_gatherBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_GatherBytes(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.gatherInt(IArray sdata, Int count, IArray rdata, Int root_rank);
KMETHOD MPICommunicator_gatherInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_GatherInt(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.gatherFloat(FArray sdata, Int count, FArray rdata, Int root_rank);
KMETHOD MPICommunicator_gatherFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_GatherFloat(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o, Int_to(int, sfp[4])));
}

//## method Int MPICommunicator.allGather(dynamic sdata, dynamic rdata);
KMETHOD MPICommunicator_allGather(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_AllgatherBytes(ctx, sfp[0].o, sdata, 0, rdata);
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_AllgatherInt(ctx, sfp[0].o, sdata, 0, rdata);
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_AllgatherFloat(ctx, sfp[0].o, sdata, 0, rdata);
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.allGatherBytes(Bytes sdata, Int count, Bytes rdata);
KMETHOD MPICommunicator_allGatherBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllgatherBytes(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.allGatherInt(IArray sdata, Int count, IArray rdata);
KMETHOD MPICommunicator_allGatherInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllgatherInt(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.allGatherFloat(FArray sdata, Int count, FArray rdata);
KMETHOD MPICommunicator_allGatherFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllgatherFloat(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.allToAll(dynamic sdata, dynamic rdata);
KMETHOD MPICommunicator_allToAll(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_AlltoallBytes(ctx, sfp[0].o, sfp[1].o, 0, sfp[2].o);
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_AlltoallInt(ctx, sfp[0].o, sfp[1].o, 0, sfp[2].o);
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_AlltoallFloat(ctx, sfp[0].o, sfp[1].o, 0, sfp[2].o);
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.allToAllBytes(Bytes sdata, Int count, Bytes rdata);
KMETHOD MPICommunicator_allToAllBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AlltoallBytes(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.allToAllInt(IArray sdata, Int count, IArray rdata);
KMETHOD MPICommunicator_allToAllInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AlltoallInt(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.allToAllFloat(FArray sdata, Int count, FArray rdata);
KMETHOD MPICommunicator_allToAllFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AlltoallFloat(ctx, sfp[0].o, sfp[1].o, Int_to(int, sfp[2]), sfp[3].o));
}

//## method Int MPICommunicator.reduce(dynamic sdata, dynamic rdata, MPIOp op, Int root_rank);
KMETHOD MPICommunicator_reduce(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_ReduceBytes(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o, Int_to(int, sfp[4]));
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_ReduceInt(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o, Int_to(int, sfp[4]));
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_ReduceFloat(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o, Int_to(int, sfp[4]));
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.reduceBytes(Bytes sdata, Bytes rdata, Int count, MPIOp op, Int root_rank);
KMETHOD MPICommunicator_reduceBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ReduceBytes(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o, Int_to(int, sfp[5])));
}

//## method Int MPICommunicator.reduceInt(IArray sdata, IArray rdata, Int count, MPIOp op, Int root_rank);
KMETHOD MPICommunicator_reduceInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ReduceInt(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o, Int_to(int, sfp[5])));
}

//## method Int MPICommunicator.reduceFloat(FArray sdata, FArray rdata, Int count, MPIOp op, Int root_rank);
KMETHOD MPICommunicator_reduceFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_ReduceFloat(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o, Int_to(int, sfp[5])));
}

//## method Int MPICommunicator.allReduce(dynamic sdata, dynamic rdata, MPIOp op);
KMETHOD MPICommunicator_allReduce(CTX ctx, knh_sfp_t *sfp _RIX)
{
	knh_Object_t *sdata = sfp[1].o;
	knh_Object_t *rdata = sfp[2].o;
	int ret = -1;
	if (IS_Bytes(sdata) && IS_Bytes(rdata)) {
		ret = knh_MPI_AllreduceBytes(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o);
	} else if (IS_IArray(sdata) && IS_IArray(rdata)) {
		ret = knh_MPI_AllreduceInt(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o);
	} else if (IS_FArray(sdata) && IS_FArray(rdata)) {
		ret = knh_MPI_AllreduceFloat(ctx, sfp[0].o, sdata, rdata, 0, sfp[3].o);
	}
	RETURNi_(ret);
}

//## method Int MPICommunicator.allReduceBytes(Bytes sdata, Bytes rdata, Int count, MPIOp op);
KMETHOD MPICommunicator_allReduceBytes(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllreduceBytes(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o));
}

//## method Int MPICommunicator.allReduceInt(IArray sdata, IArray rdata, Int count, MPIOp op);
KMETHOD MPICommunicator_allReduceInt(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllreduceInt(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o));
}

//## method Int MPICommunicator.allReduceFloat(FArray sdata, FArray rdata, Int count, MPIOp op);
KMETHOD MPICommunicator_allReduceFloat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	RETURNi_(knh_MPI_AllreduceFloat(ctx, sfp[0].o, sfp[1].o, sfp[2].o, Int_to(int, sfp[3]), sfp[4].o));
}
