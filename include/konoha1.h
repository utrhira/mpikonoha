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

#ifndef KONOHA_H_
#define KONOHA_H_
#ifndef K_INTERNAL
#define K_EXPORTS   1
#endif

#ifdef HAVE_CONFIG_H
#include"config.h"
#endif

/* configuration and dependency */

#ifndef K_USED_TO_BE_ON_LKM
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>
#include<string.h>
#include<setjmp.h>
#include<stdarg.h>
#endif

#include"konoha1/konoha_config.h"

#if defined(K_USING_SIGNAL)
#include<signal.h>
#endif

/* types, symbols */
#include"konoha1/konoha_t.h"
#include"konoha1/konoha_class.h"

/* functionality */
#include"konoha1/konoha_debug.h"
#include"konoha1/konoha_glue.h"
#include"konoha1/konoha_gc.h"

/* konohaapi, prototypes */

#include"konoha1/konoha_api.h"

#ifndef K_INTERNAL
konoha_t konoha_open(size_t stacksize);
void konoha_main(konoha_t, int, const char**);
void konoha_close(konoha_t konoha);
#endif

#endif /*KONOHA_H_*/

