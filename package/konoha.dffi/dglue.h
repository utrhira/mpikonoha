#ifndef _DGLUE_H_
#define _DGLUE_H_

#define GLUE_TYPE_UNKNOWN 0
#define GLUE_TYPE_INTERNAL 1
#define GLUE_TYPE_LOCAL 2
#define GLUE_TYPE_REMOTE 3

/* ------------------------------------------------------------------------ */
/* [Dependable Glue] */

typedef struct knh_GlueSPI_t {
  kRawPtr* (*getFunc)(CTX, ksfp_t * _RIX);
  void (*component_free)(CTX, void *);
  void (*glue_free)(CTX, void *);
} knh_GlueSPI_t;

typedef struct knh_Glue_t {
  int glueType; 
  void *componentInfo;
  void *glueInfo;
  /* Glue Service Provider Interface */
  knh_GlueSPI_t *gapi;
} knh_Glue_t;

static knh_Glue_t *new_Glue(CTX ctx)
{
  knh_Glue_t *glue = (knh_Glue_t*)KNH_MALLOC(ctx, sizeof(knh_Glue_t));
  glue->glueType = GLUE_TYPE_UNKNOWN;
  // TODO: we need NullObject here
  glue->componentInfo = NULL; 
  glue->glueInfo = NULL;
  glue->gapi = (knh_GlueSPI_t*)KNH_MALLOC(ctx, sizeof(knh_GlueSPI_t));
  knh_GlueSPI_t *gapi = glue->gapi;
  gapi->component_free = NULL;
  gapi->glue_free = NULL;
  gapi->getFunc = NULL;
  return glue;
}

static void Glue_init(CTX ctx, kRawPtr *po)
{
  po->rawptr = NULL;
}

static void Glue_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
	  knh_Glue_t *glue = (knh_Glue_t*)po->rawptr;
	  knh_GlueSPI_t *gapi = glue->gapi;
	  if (glue->componentInfo != NULL) {
		//gapi->component_free(ctx, glue->componentInfo);
	  }
	  if (glue->glueInfo != NULL) gapi->glue_free(ctx, glue->glueInfo);
	  KNH_FREE(ctx, gapi, sizeof(knh_GlueSPI_t));
	  KNH_FREE(ctx, glue, sizeof(knh_Glue_t));
	  po->rawptr = NULL;
	}
}


DEFAPI(void) defGlue(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "Glue";
	cdef->init = Glue_init;
	cdef->free = Glue_free;
}

/* ------------------------------------------------------------------------ */

// @Native var Glue.getFunc(String symbol, Class_, Func _);
KMETHOD Glue_getFunc(CTX ctx, ksfp_t *sfp _RIX)
{
  knh_Glue_t *glue = (knh_Glue_t*)((sfp[0].p)->rawptr);
  knh_GlueSPI_t *gapi = glue->gapi;
  kRawPtr *ret = NULL;
  switch (glue->glueType) {
  case GLUE_TYPE_INTERNAL:
	ret = gapi->getFunc(ctx, sfp, K_RIX);
	break;
  case GLUE_TYPE_LOCAL:
	ret = gapi->getFunc(ctx, sfp, K_RIX);
	break;
  case GLUE_TYPE_REMOTE:

	break;
  default:
	break;
  }
  RETURN_(ret);
}

/* ------------------------------------------------------------------------ */

#endif /* _DGLUE_H_ */

