//
//@Virtual void QGraphicsTransform.applyTo(QMatrix4x4 matrix);
KMETHOD QGraphicsTransform_applyTo(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QGraphicsTransform *  qp = RawPtr_to(QGraphicsTransform *, sfp[0]);
	if (qp != NULL) {
		QMatrix4x4*  matrix = RawPtr_to(QMatrix4x4*, sfp[1]);
		qp->applyTo(matrix);
	}
	RETURNvoid_();
}


DummyQGraphicsTransform::DummyQGraphicsTransform()
{
	self = NULL;
	event_map = new map<string, knh_Func_t *>();
	slot_map = new map<string, knh_Func_t *>();
}

void DummyQGraphicsTransform::setSelf(knh_RawPtr_t *ptr)
{
	DummyQGraphicsTransform::self = ptr;
	DummyQObject::setSelf(ptr);
}

bool DummyQGraphicsTransform::eventDispatcher(QEvent *event)
{
	bool ret = true;
	switch (event->type()) {
	default:
		ret = DummyQObject::eventDispatcher(event);
		break;
	}
	return ret;
}

bool DummyQGraphicsTransform::addEvent(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQGraphicsTransform::event_map->bigin();
	if ((itr = DummyQGraphicsTransform::event_map->find(str)) == DummyQGraphicsTransform::event_map->end()) {
		bool ret;
		ret = DummyQObject::addEvent(callback_func, str);
		return ret;
	} else {
		KNH_INITv((*event_map)[str], callback_func);
		return true;
	}
}

bool DummyQGraphicsTransform::signalConnect(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQGraphicsTransform::slot_map->bigin();
	if ((itr = DummyQGraphicsTransform::event_map->find(str)) == DummyQGraphicsTransform::slot_map->end()) {
		bool ret;
		ret = DummyQObject::signalConnect(callback_func, str);
		return ret;
	} else {
		KNH_INITv((*slot_map)[str], callback_func);
		return true;
	}
}


KQGraphicsTransform::KQGraphicsTransform(QObject* parent) : QGraphicsTransform(parent)
{
	self = NULL;
}

KMETHOD QGraphicsTransform_addEvent(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQGraphicsTransform *qp = RawPtr_to(KQGraphicsTransform *, sfp[0]);
	const char *event_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->event_map->find(event_name) == qp->event_map->end()) {
//			fprintf(stderr, "WARNING:[QGraphicsTransform]unknown event name [%s]\n", event_name);
//			return;
//		}
		string str = string(event_name);
//		KNH_INITv((*(qp->event_map))[event_name], callback_func);
		if (!qp->DummyQGraphicsTransform::addEvent(callback_func, str)) {
			fprintf(stderr, "WARNING:[QGraphicsTransform]unknown event name [%s]\n", event_name);
			return;
		}
	}
	RETURNvoid_();
}

KMETHOD QGraphicsTransform_signalConnect(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQGraphicsTransform *qp = RawPtr_to(KQGraphicsTransform *, sfp[0]);
	const char *signal_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->slot_map->find(signal_name) == qp->slot_map->end()) {
//			fprintf(stderr, "WARNING:[QGraphicsTransform]unknown signal name [%s]\n", signal_name);
//			return;
//		}
		string str = string(signal_name);
//		KNH_INITv((*(qp->slot_map))[signal_name], callback_func);
		if (!qp->DummyQGraphicsTransform::signalConnect(callback_func, str)) {
			fprintf(stderr, "WARNING:[QGraphicsTransform]unknown signal name [%s]\n", signal_name);
			return;
		}
	}
	RETURNvoid_();
}

static void QGraphicsTransform_free(CTX ctx, knh_RawPtr_t *p)
{
	(void)ctx;
	if (p->rawptr != NULL) {
		KQGraphicsTransform *qp = (KQGraphicsTransform *)p->rawptr;
		(void)qp;
		//delete qp;
	}
}
static void QGraphicsTransform_reftrace(CTX ctx, knh_RawPtr_t *p FTRARG)
{
	(void)ctx; (void)p; (void)tail_;
	if (p->rawptr != NULL) {
		KQGraphicsTransform *qp = (KQGraphicsTransform *)p->rawptr;
		(void)qp;
	}
}

static int QGraphicsTransform_compareTo(knh_RawPtr_t *p1, knh_RawPtr_t *p2)
{
	return (p1->rawptr == p2->rawptr ? 0 : 1);
}

bool KQGraphicsTransform::event(QEvent *event)
{
	if (!DummyQGraphicsTransform::eventDispatcher(event)) {
		QGraphicsTransform::event(event);
		return false;
	}
	return true;
}

DEFAPI(void) defQGraphicsTransform(CTX ctx, knh_class_t cid, knh_ClassDef_t *cdef)
{
	(void)ctx; (void) cid;
	cdef->name = "QGraphicsTransform";
	cdef->free = QGraphicsTransform_free;
	cdef->reftrace = QGraphicsTransform_reftrace;
	cdef->compareTo = QGraphicsTransform_compareTo;
}

