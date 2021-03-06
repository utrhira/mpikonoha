//QStaticText QStaticText.new();
KMETHOD QStaticText_new(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQStaticText *ret_v = new KQStaticText();
	knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v, NULL);
	ret_v->setSelf(rptr);
	RETURN_(rptr);
}

/*
//QStaticText QStaticText.new(String text);
KMETHOD QStaticText_new(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	const QString text = String_to(const QString, sfp[1]);
	KQStaticText *ret_v = new KQStaticText(text);
	knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v, NULL);
	ret_v->setSelf(rptr);
	RETURN_(rptr);
}
*/
/*
//QStaticText QStaticText.new(QStaticText other);
KMETHOD QStaticText_new(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	const QStaticText  other = *RawPtr_to(const QStaticText *, sfp[1]);
	KQStaticText *ret_v = new KQStaticText(other);
	knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v, NULL);
	ret_v->setSelf(rptr);
	RETURN_(rptr);
}
*/
//int QStaticText.getPerformanceHint();
KMETHOD QStaticText_getPerformanceHint(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		QStaticText::PerformanceHint ret_v = qp->performanceHint();
		RETURNi_(ret_v);
	} else {
		RETURNi_(0);
	}
}

//void QStaticText.prepare(QTransform matrix, QFont font);
KMETHOD QStaticText_prepare(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		const QTransform  matrix = *RawPtr_to(const QTransform *, sfp[1]);
		const QFont  font = *RawPtr_to(const QFont *, sfp[2]);
		qp->prepare(matrix, font);
	}
	RETURNvoid_();
}

//void QStaticText.setPerformanceHint(int performanceHint);
KMETHOD QStaticText_setPerformanceHint(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		QStaticText::PerformanceHint performanceHint = Int_to(QStaticText::PerformanceHint, sfp[1]);
		qp->setPerformanceHint(performanceHint);
	}
	RETURNvoid_();
}

//void QStaticText.setText(String text);
KMETHOD QStaticText_setText(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		const QString text = String_to(const QString, sfp[1]);
		qp->setText(text);
	}
	RETURNvoid_();
}

//void QStaticText.setTextFormat(int textFormat);
KMETHOD QStaticText_setTextFormat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		Qt::TextFormat textFormat = Int_to(Qt::TextFormat, sfp[1]);
		qp->setTextFormat(textFormat);
	}
	RETURNvoid_();
}

//void QStaticText.setTextOption(QTextOption textOption);
KMETHOD QStaticText_setTextOption(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		const QTextOption  textOption = *RawPtr_to(const QTextOption *, sfp[1]);
		qp->setTextOption(textOption);
	}
	RETURNvoid_();
}

//void QStaticText.setTextWidth(float textWidth);
KMETHOD QStaticText_setTextWidth(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		qreal textWidth = Float_to(qreal, sfp[1]);
		qp->setTextWidth(textWidth);
	}
	RETURNvoid_();
}

//QSizeF QStaticText.size();
KMETHOD QStaticText_size(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		QSizeF ret_v = qp->size();
		QSizeF *ret_v_ = new QSizeF(ret_v);
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v_, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//String QStaticText.getText();
KMETHOD QStaticText_getText(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		QString ret_v = qp->text();
		const char *ret_c = ret_v.toLocal8Bit().data();
		RETURN_(new_String(ctx, ret_c));
	} else {
		RETURN_(KNH_NULL);
	}
}

//int QStaticText.getTextFormat();
KMETHOD QStaticText_getTextFormat(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		Qt::TextFormat ret_v = qp->textFormat();
		RETURNi_(ret_v);
	} else {
		RETURNi_(0);
	}
}

//QTextOption QStaticText.getTextOption();
KMETHOD QStaticText_getTextOption(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		QTextOption ret_v = qp->textOption();
		QTextOption *ret_v_ = new QTextOption(ret_v);
		knh_RawPtr_t *rptr = new_ReturnCppObject(ctx, sfp, ret_v_, NULL);
		RETURN_(rptr);
	} else {
		RETURN_(KNH_NULL);
	}
}

//float QStaticText.getTextWidth();
KMETHOD QStaticText_getTextWidth(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *  qp = RawPtr_to(QStaticText *, sfp[0]);
	if (qp) {
		qreal ret_v = qp->textWidth();
		RETURNf_(ret_v);
	} else {
		RETURNf_(0.0f);
	}
}

//Array<String> QStaticText.parents();
KMETHOD QStaticText_parents(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	QStaticText *qp = RawPtr_to(QStaticText*, sfp[0]);
	if (qp != NULL) {
		int size = 10;
		knh_Array_t *a = new_Array0(ctx, size);
		const knh_ClassTBL_t *ct = sfp[0].p->h.cTBL;
		while(ct->supcid != CLASS_Object) {
			ct = ct->supTBL;
			knh_Array_add(ctx, a, (knh_Object_t *)ct->lname);
		}
		RETURN_(a);
	} else {
		RETURN_(KNH_NULL);
	}
}

DummyQStaticText::DummyQStaticText()
{
	CTX lctx = knh_getCurrentContext();
	(void)lctx;
	self = NULL;
	event_map = new map<string, knh_Func_t *>();
	slot_map = new map<string, knh_Func_t *>();
}
DummyQStaticText::~DummyQStaticText()
{
	delete event_map;
	delete slot_map;
	event_map = NULL;
	slot_map = NULL;
}

void DummyQStaticText::setSelf(knh_RawPtr_t *ptr)
{
	DummyQStaticText::self = ptr;
}

bool DummyQStaticText::eventDispatcher(QEvent *event)
{
	bool ret = true;
	switch (event->type()) {
	default:
		ret = false;
		break;
	}
	return ret;
}

bool DummyQStaticText::addEvent(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQStaticText::event_map->bigin();
	if ((itr = DummyQStaticText::event_map->find(str)) == DummyQStaticText::event_map->end()) {
		bool ret = false;
		return ret;
	} else {
		KNH_INITv((*event_map)[str], callback_func);
		return true;
	}
}

bool DummyQStaticText::signalConnect(knh_Func_t *callback_func, string str)
{
	std::map<string, knh_Func_t*>::iterator itr;// = DummyQStaticText::slot_map->bigin();
	if ((itr = DummyQStaticText::slot_map->find(str)) == DummyQStaticText::slot_map->end()) {
		bool ret = false;
		return ret;
	} else {
		KNH_INITv((*slot_map)[str], callback_func);
		return true;
	}
}

knh_Object_t** DummyQStaticText::reftrace(CTX ctx, knh_RawPtr_t *p FTRARG)
{
	(void)ctx; (void)p; (void)tail_;
//	fprintf(stderr, "DummyQStaticText::reftrace p->rawptr=[%p]\n", p->rawptr);


	return tail_;
}

void DummyQStaticText::connection(QObject *o)
{
	QStaticText *p = dynamic_cast<QStaticText*>(o);
	if (p != NULL) {
	}
}

KQStaticText::KQStaticText() : QStaticText()
{
	magic_num = G_MAGIC_NUM;
	self = NULL;
	dummy = new DummyQStaticText();
}

KQStaticText::~KQStaticText()
{
	delete dummy;
	dummy = NULL;
}
KMETHOD QStaticText_addEvent(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQStaticText *qp = RawPtr_to(KQStaticText *, sfp[0]);
	const char *event_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->event_map->find(event_name) == qp->event_map->end()) {
//			fprintf(stderr, "WARNING:[QStaticText]unknown event name [%s]\n", event_name);
//			return;
//		}
		string str = string(event_name);
//		KNH_INITv((*(qp->event_map))[event_name], callback_func);
		if (!qp->dummy->addEvent(callback_func, str)) {
			fprintf(stderr, "WARNING:[QStaticText]unknown event name [%s]\n", event_name);
			return;
		}
	}
	RETURNvoid_();
}
KMETHOD QStaticText_signalConnect(CTX ctx, knh_sfp_t *sfp _RIX)
{
	(void)ctx;
	KQStaticText *qp = RawPtr_to(KQStaticText *, sfp[0]);
	const char *signal_name = String_to(const char *, sfp[1]);
	knh_Func_t *callback_func = sfp[2].fo;
	if (qp != NULL) {
//		if (qp->slot_map->find(signal_name) == qp->slot_map->end()) {
//			fprintf(stderr, "WARNING:[QStaticText]unknown signal name [%s]\n", signal_name);
//			return;
//		}
		string str = string(signal_name);
//		KNH_INITv((*(qp->slot_map))[signal_name], callback_func);
		if (!qp->dummy->signalConnect(callback_func, str)) {
			fprintf(stderr, "WARNING:[QStaticText]unknown signal name [%s]\n", signal_name);
			return;
		}
	}
	RETURNvoid_();
}

static void QStaticText_free(CTX ctx, knh_RawPtr_t *p)
{
	(void)ctx;
	if (!exec_flag) return;
	if (p->rawptr != NULL) {
		KQStaticText *qp = (KQStaticText *)p->rawptr;
		if (qp->magic_num == G_MAGIC_NUM) {
			delete qp;
			p->rawptr = NULL;
		} else {
			delete (QStaticText*)qp;
			p->rawptr = NULL;
		}
	}
}
static void QStaticText_reftrace(CTX ctx, knh_RawPtr_t *p FTRARG)
{
	if (p->rawptr != NULL) {
//		KQStaticText *qp = (KQStaticText *)p->rawptr;
		KQStaticText *qp = static_cast<KQStaticText*>(p->rawptr);
		qp->dummy->reftrace(ctx, p, tail_);
	}
}

static int QStaticText_compareTo(knh_RawPtr_t *p1, knh_RawPtr_t *p2)
{
	return (*static_cast<QStaticText*>(p1->rawptr) == *static_cast<QStaticText*>(p2->rawptr) ? 0 : 1);
}

void KQStaticText::setSelf(knh_RawPtr_t *ptr)
{
	self = ptr;
	dummy->setSelf(ptr);
}

static knh_IntData_t QStaticTextConstInt[] = {
	{"ModerateCaching", QStaticText::ModerateCaching},
	{"AggressiveCaching", QStaticText::AggressiveCaching},
	{NULL, 0}
};

DEFAPI(void) constQStaticText(CTX ctx, knh_class_t cid, const knh_LoaderAPI_t *kapi) {
	kapi->loadClassIntConst(ctx, cid, QStaticTextConstInt);
}


DEFAPI(void) defQStaticText(CTX ctx, knh_class_t cid, knh_ClassDef_t *cdef)
{
	(void)ctx; (void) cid;
	cdef->name = "QStaticText";
	cdef->free = QStaticText_free;
	cdef->reftrace = QStaticText_reftrace;
	cdef->compareTo = QStaticText_compareTo;
}


