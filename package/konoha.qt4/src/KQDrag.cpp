//QDrag QDrag.new(QWidget dragSource);
KMETHOD QDrag_new(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QWidget*  dragSource = RawPtr_to(QWidget*, sfp[1]);
	KQDrag *ret_v = new KQDrag(dragSource);
	knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v, NULL);
	ret_v->self = rptr;
	ret_v->setSelf(rptr);
	RETURN_(rptr);
}

//int QDrag.exec(int supportedActions);
KMETHOD QDrag_exec(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		Qt::DropActions supportedActions = Int_to(Qt::DropActions, sfp[1]);
		Qt::DropAction ret_v = qp->exec(supportedActions);
		RETURNi_(ret_v);
	} else {
		RETURNi_(0);
	}
}

/*
//int QDrag.exec(int supportedActions, int defaultDropAction);
KMETHOD QDrag_exec(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		Qt::DropActions supportedActions = Int_to(Qt::DropActions, sfp[1]);
		Qt::DropAction defaultDropAction = Int_to(Qt::DropAction, sfp[2]);
		Qt::DropAction ret_v = qp->exec(supportedActions, defaultDropAction);
		RETURNi_(ret_v);
	} else {
		RETURNi_(0);
	}
}
*/
//QPoint QDrag.getHotSpot();
KMETHOD QDrag_getHotSpot(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QPoint ret_v = qp->hotSpot();
		QPoint *ret_v_ = new QPoint(ret_v);
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v_, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//QMimeData QDrag.getMimeData();
KMETHOD QDrag_getMimeData(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QMimeData* ret_v = qp->mimeData();
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, (QMimeData*)ret_v, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//QPixmap QDrag.getPixmap();
KMETHOD QDrag_getPixmap(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QPixmap ret_v = qp->pixmap();
		QPixmap *ret_v_ = new QPixmap(ret_v);
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v_, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//void QDrag.setDragCursor(QPixmap cursor, int action);
KMETHOD QDrag_setDragCursor(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		const QPixmap  cursor = *RawPtr_to(const QPixmap *, sfp[1]);
		Qt::DropAction action = Int_to(Qt::DropAction, sfp[2]);
		qp->setDragCursor(cursor, action);
	}
	RETURNvoid_();
}

//void QDrag.setHotSpot(QPoint hotspot);
KMETHOD QDrag_setHotSpot(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		const QPoint  hotspot = *RawPtr_to(const QPoint *, sfp[1]);
		qp->setHotSpot(hotspot);
	}
	RETURNvoid_();
}

//void QDrag.setMimeData(QMimeData data);
KMETHOD QDrag_setMimeData(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QMimeData*  data = RawPtr_to(QMimeData*, sfp[1]);
		qp->setMimeData(data);
	}
	RETURNvoid_();
}

//void QDrag.setPixmap(QPixmap pixmap);
KMETHOD QDrag_setPixmap(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		const QPixmap  pixmap = *RawPtr_to(const QPixmap *, sfp[1]);
		qp->setPixmap(pixmap);
	}
	RETURNvoid_();
}

//QWidget QDrag.source();
KMETHOD QDrag_source(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QWidget* ret_v = qp->source();
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, (QWidget*)ret_v, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//QWidget QDrag.target();
KMETHOD QDrag_target(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QDrag *  qp = RawPtr_to(QDrag *, sfp[0]);
	if (qp != NULL) {
		QWidget* ret_v = qp->target();
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, (QWidget*)ret_v, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}


DummyQDrag::DummyQDrag()
{
	self = NULL;
	event_map = new map<string, knh_Func_t *>();
	slot_map = new map<string, knh_Func_t *>();
}

void DummyQDrag::setSelf(knh_RawPtr_t *ptr)
{
	DummyQDrag::self = ptr;
	DummyQObject::setSelf(ptr);
}

bool DummyQDrag::eventDispatcher(QEvent *event)
{
	bool ret = true;
	switch (event->type()) {
	default:
		ret = DummyQObject::eventDispatcher(event);
		break;
	}
	return ret;
}

bool DummyQDrag::addEvent(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQDrag::event_map->bigin();
	if ((itr = DummyQDrag::event_map->find(str)) == DummyQDrag::event_map->end()) {
		bool ret;
		ret = DummyQObject::addEvent(callback_func, str);
		return ret;
	} else {
		KNH_INITv((*event_map)[str], callback_func);
		return true;
	}
}

bool DummyQDrag::signalConnect(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQDrag::slot_map->bigin();
	if ((itr = DummyQDrag::event_map->find(str)) == DummyQDrag::slot_map->end()) {
		bool ret;
		ret = DummyQObject::signalConnect(callback_func, str);
		return ret;
	} else {
		KNH_INITv((*slot_map)[str], callback_func);
		return true;
	}
}


KQDrag::KQDrag(QWidget* dragSource) : QDrag(dragSource)
{
	self = NULL;
}

KMETHOD QDrag_addEvent(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQDrag *qp = RawPtr_to(KQDrag *, sfp[0]);
	const char *event_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->event_map->find(event_name) == qp->event_map->end()) {
//			fprintf(stderr, "WARNING:[QDrag]unknown event name [%s]\n", event_name);
//			return;
//		}
		string str = string(event_name);
//		KNH_INITv((*(qp->event_map))[event_name], callback_func);
		if (!qp->DummyQDrag::addEvent(callback_func, str)) {
			fprintf(stderr, "WARNING:[QDrag]unknown event name [%s]\n", event_name);
			return;
		}
	}
	RETURNvoid_();
}

KMETHOD QDrag_signalConnect(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQDrag *qp = RawPtr_to(KQDrag *, sfp[0]);
	const char *signal_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->slot_map->find(signal_name) == qp->slot_map->end()) {
//			fprintf(stderr, "WARNING:[QDrag]unknown signal name [%s]\n", signal_name);
//			return;
//		}
		string str = string(signal_name);
//		KNH_INITv((*(qp->slot_map))[signal_name], callback_func);
		if (!qp->DummyQDrag::signalConnect(callback_func, str)) {
			fprintf(stderr, "WARNING:[QDrag]unknown signal name [%s]\n", signal_name);
			return;
		}
	}
	RETURNvoid_();
}

static void QDrag_free(CTX ctx, knh_RawPtr_t *p)
{
	(void)ctx;
	if (p->rawptr != NULL) {
		KQDrag *qp = (KQDrag *)p->rawptr;
		(void)qp;
		//delete qp;
	}
}
static void QDrag_reftrace(CTX ctx, knh_RawPtr_t *p FTRARG)
{
	(void)ctx; (void)p; (void)tail_;
	if (p->rawptr != NULL) {
		KQDrag *qp = (KQDrag *)p->rawptr;
		(void)qp;
	}
}

static int QDrag_compareTo(knh_RawPtr_t *p1, knh_RawPtr_t *p2)
{
	return (p1->rawptr == p2->rawptr ? 0 : 1);
}

bool KQDrag::event(QEvent *event)
{
	if (!DummyQDrag::eventDispatcher(event)) {
		QDrag::event(event);
		return false;
	}
	return true;
}

DEFAPI(void) defQDrag(CTX ctx, knh_class_t cid, knh_ClassDef_t *cdef)
{
	(void)ctx; (void) cid;
	cdef->name = "QDrag";
	cdef->free = QDrag_free;
	cdef->reftrace = QDrag_reftrace;
	cdef->compareTo = QDrag_compareTo;
}

