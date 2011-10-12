#ifndef QTEXTFRAME
#define QTEXTFRAME
class DummyQTextFrame : public DummyQTextObject {
public:
	knh_RawPtr_t *self;
	std::map<std::string, knh_Func_t *> *event_map;
	std::map<std::string, knh_Func_t *> *slot_map;
	DummyQTextFrame();
	void setSelf(knh_RawPtr_t *ptr);
	bool eventDispatcher(QEvent *event);
	bool addEvent(knh_Func_t *callback_func, std::string str);
	bool signalConnect(knh_Func_t *callback_func, std::string str);
};

class KQTextFrame : public QTextFrame, public DummyQTextFrame {
//	Q_OBJECT;
public:
	knh_RawPtr_t *self;
	KQTextFrame(QTextDocument* document);
	bool event(QEvent *event);
};

#endif //QTEXTFRAME

