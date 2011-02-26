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

#define USE_cwb_tobytes 1

#include"commons.h"

#ifdef K_USED_TO_BE_ON_LKM
#include<linux/string.h>
#else
#include<string.h>
#endif

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [memory] */

size_t k_goodsize(size_t ss)
{
	size_t s = ss;
	if(s == 0) return 0;  /* added */
	if(s <= K_FASTMALLOC_SIZE) return K_FASTMALLOC_SIZE;
	if(s > 64 * 1024) return s;
	s |= s >> 1;
	s |= s >> 2;
	s |= s >> 4;
	s |= s >> 8;
	s |= s >> 16;
	return ((s + 1) == ss * 2) ? ss : s + 1;
}

size_t k_goodsize2(size_t ss, size_t wsize)
{
	return k_goodsize(ss * wsize) / wsize;
}

const knh_dim_t *new_dim(CTX ctx, size_t capacity, size_t wsize)
{
	knh_dim_t *dim = (knh_dim_t*)KNH_MALLOC(ctx, sizeof(knh_dim_t));
	dim->capacity = capacity;
	dim->wsize = wsize;
	dim->dim = 1;
	dim->x = dim->xy = dim->xyz = capacity;
	return (const knh_dim_t*)dim;
}

void knh_dimfree(CTX ctx, void *p, const knh_dim_t *dim)
{
	size_t size = dim->capacity * dim->wsize;
	if(size > 0) {
		KNH_FREE(ctx, p, size);
		KNH_FREE(ctx, (void*)dim, sizeof(knh_dim_t));
	}
}

/* ------------------------------------------------------------------------ */

static void Bytes_checkstack(CTX ctx, knh_uchar_t*oldstart, knh_uchar_t *oldend, knh_uchar_t* newstart)
{
	knh_uchar_t **cstack_top = (knh_uchar_t**)(&ctx);
	knh_uchar_t **p = (knh_uchar_t**)ctx->cstack_bottom;
	DBG_P("cstack_bottom=%p, cstack_top=%p basedata=%p", p[0], cstack_top, &oldstart);
	if(!(p < cstack_top)) {
		knh_uchar_t **tmp = cstack_top;
		cstack_top = p; p = tmp;
	}
	while(p <= cstack_top) {
		if((oldstart <= p[0] && p[0] < oldend)) {
			knh_uchar_t *newc = p[0] + (newstart - oldstart);
			KNH_SYSLOG(ctx, LOG_DEBUG, "RewrittenCharPointer", "*oldptr=%p, newptr=%p", p[0], newc);
			p[0] = newc;
		}
		p++;
	}
}

static void Bytes_expands(CTX ctx, knh_Bytes_t *ba, size_t newsize)
{
	if(ba->dim->capacity == 0) {
		newsize = k_goodsize(newsize);
		ba->bu.ubuf = (knh_uchar_t*)KNH_MALLOC(ctx, newsize);
		ba->dim = new_dim(ctx, newsize, 1);
	}
	else {
		knh_uchar_t *ubuf = ba->bu.ubuf;
		ba->bu.ubuf = (knh_uchar_t*)KNH_REALLOC(ctx, ba->bu.ubuf, ba->dim->capacity, newsize, 1);
		((knh_dim_t*)ba->dim)->capacity = newsize;
		if(unlikely(ctx->bufa == ba)) {
			KNH_SYSLOG(ctx, LOG_INFO, "ExtendedContextBuffer", "*newsize=%ld, pointer=(%p => %p)", newsize, ubuf, ba->bu.ubuf);
			Bytes_checkstack(ctx, ubuf, ubuf + ba->bu.len, ba->bu.ubuf);
		}
	}
}

/* ------------------------------------------------------------------------ */
/* [Bytes] */

knh_Bytes_t* new_Bytes(CTX ctx, size_t capacity)
{
	knh_Bytes_t *ba = new_(Bytes);
	if(capacity > 0) {
		Bytes_expands(ctx, ba, capacity);
	}
	return ba;
}


void knh_Bytes_clear(knh_Bytes_t *ba, size_t pos)
{
	DBG_ASSERT(!Bytes_isStatic(ba));
	if(pos < BA_size(ba)) {
		knh_bzero(ba->bu.ubuf + pos, BA_size(ba) - pos);
		BA_size(ba) = pos;
	}
}

void knh_Bytes_ensureSize(CTX ctx, knh_Bytes_t *ba, size_t len)
{
	DBG_ASSERT(!Bytes_isStatic(ba));
	if(ba->dim->capacity < len) {
		Bytes_expands(ctx, ba, len);
	}
	BA_size(ba) = len;
}

const char *knh_Bytes_ensureZero(CTX ctx, knh_Bytes_t *ba)
{
	size_t size = BA_size(ba);
	size_t capacity = ba->dim->capacity;
	if(size == capacity) {
		Bytes_expands(ctx, ba, k_grow(capacity));
	}
	ba->bu.ubuf[BA_size(ba)] = 0;
	return ba->bu.text;
}

void knh_Bytes_putc(CTX ctx, knh_Bytes_t *ba, int ch)
{
	size_t capacity = ba->dim->capacity;
	if(BA_size(ba) == capacity) {
		Bytes_expands(ctx, ba, k_grow(capacity));
	}
	ba->bu.ubuf[BA_size(ba)] = ch;
	BA_size(ba) += 1;
}

void knh_Bytes_unputc(knh_Bytes_t *ba)
{
	if(BA_size(ba) > 0) {
		BA_size(ba) -= 1;
		ba->bu.ubuf[BA_size(ba)] = '\0';
	}
}

void knh_Bytes_write(CTX ctx, knh_Bytes_t *ba, knh_bytes_t t)
{
	size_t capacity = ba->dim->capacity;
	if(t.len == 0) return ;
	if(BA_size(ba) + t.len >= capacity) {
		size_t newsize = k_grow(capacity);
		if(newsize < BA_size(ba) + t.len) newsize = k_goodsize(BA_size(ba) + t.len);
		Bytes_expands(ctx, ba, newsize);
	}
	knh_memcpy(ba->bu.ubuf + BA_size(ba), t.ustr, t.len);
	BA_size(ba) += t.len;
}

/* ------------------------------------------------------------------------ */

knh_String_t *knh_cwb_newString(CTX ctx, knh_cwb_t *cwb)
{
	if(cwb->pos == (cwb->ba)->bu.len) {
		return TS_EMPTY;
	}
	else {
		knh_bytes_t t = knh_cwb_tobytes(cwb);
		knh_String_t *s = new_S(ctx, t);
		knh_cwb_close(cwb);
		return s;
	}
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif