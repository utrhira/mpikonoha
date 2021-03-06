#include <gwt.hpp>

#ifdef __cplusplus
extern "C" {
#endif

GamGroup::GamGroup(void)
{
	setObjectName("GamGroup");
}

KMETHOD Group_new(CTX ctx, knh_sfp_t *sfp _RIX)
{
	NO_WARNING();
	GamGroup *g = new GamGroup();
	knh_RawPtr_t *p = new_ReturnCppObject(ctx, sfp, g, NULL);
	RETURN_(p);
}

KMETHOD Group_addToGroup(CTX ctx, knh_sfp_t *sfp _RIX)
{
	NO_WARNING();
	GamGroup *g = RawPtr_to(GamGroup *, sfp[0]);
	QGraphicsItem *i = QGraphicsItem_to(sfp[1]);
	g->addToGroup(i);
	RETURNvoid_();
}

static void Group_free(CTX ctx, knh_RawPtr_t *p)
{
	(void)ctx;
#ifdef DEBUG_MODE
	fprintf(stderr, "Group:free\n");
#endif
	//GamGroup *g = (GamGroup *)p->rawptr;
	//delete g;
}

static void Group_reftrace(CTX ctx, knh_RawPtr_t *p FTRARG)
{
	(void)ctx;
	(void)p;
	(void)tail_;
#ifdef DEBUG_MODE
	fprintf(stderr, "Group:reftrace\n");
#endif
	//QApplication *app = (QApplication *)p->rawptr;
}

DEFAPI(void) defGroup(CTX ctx, knh_class_t cid, knh_ClassDef_t *cdef)
{
	NO_WARNING2();
	cdef->name = "Group";
	cdef->free = Group_free;
	cdef->reftrace = Group_reftrace;
}

DEFAPI(void) constGroup(CTX ctx, knh_class_t cid, const knh_LoaderAPI_t *kapi)
{
	(void)ctx;
	(void)cid;
	(void)kapi;
	//kapi->loadIntClassConst(ctx, cid, TimeLineConstInt);
}

#ifdef __cplusplus
}
#endif
