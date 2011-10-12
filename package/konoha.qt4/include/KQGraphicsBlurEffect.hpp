#ifndef QGRAPHICSBLUREFFECT
#define QGRAPHICSBLUREFFECT
class DummyQGraphicsBlurEffect : public DummyQGraphicsEffect {
public:
	knh_RawPtr_t *self;
	std::map<std::string, knh_Func_t *> *event_map;
	std::map<std::string, knh_Func_t *> *slot_map;
	DummyQGraphicsBlurEffect();
	void setSelf(knh_RawPtr_t *ptr);
	bool eventDispatcher(QEvent *event);
	bool addEvent(knh_Func_t *callback_func, std::string str);
	bool signalConnect(knh_Func_t *callback_func, std::string str);
};

class KQGraphicsBlurEffect : public QGraphicsBlurEffect, public DummyQGraphicsBlurEffect {
//	Q_OBJECT;
public:
	knh_RawPtr_t *self;
	KQGraphicsBlurEffect(QObject* parent);
	bool event(QEvent *event);
};

#endif //QGRAPHICSBLUREFFECT

