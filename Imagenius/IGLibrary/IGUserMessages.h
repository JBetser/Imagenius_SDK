// imagenius thread messages
#define UM_IGROOT					(WM_USER + 1)
#define UM_ADDPICTURES				UM_IGROOT
#define UM_IMAGEPROCESSING			(UM_IGROOT + 1)
#define UM_DEEPZOOM_PROCESSING		(UM_IGROOT + 2)
#define UM_DEEPZOOM_ENDED			(UM_IGROOT + 3)
// imagenius socket messages
#define	UM_WINSOCK					(UM_IGROOT + 20)
// imagenius toolbox messages
#define UM_CUSTOMMOVE				(UM_IGROOT + 100)	
// imagenius progressbar messages
#define UM_PROGRESS_SETRANGE		(UM_IGROOT + 500)
#define UM_PROGRESS_SETSTEP			(UM_IGROOT + 501)
#define UM_PROGRESS_STEPIT			(UM_IGROOT + 502)
#define UM_PROGRESS_GETPROGRESS		(UM_IGROOT + 503)
#define UM_PROGRESS_SETMESSAGE		(UM_IGROOT + 504)
#define UM_PROGRESS_SETSUBRANGE		(UM_IGROOT + 505)
// imagenius hyperlink messages
#define UM_XHYPERLINK_CLICKED		(UM_IGROOT + 550)
// imagenius frame messages
#define UM_IGFRAME_LAYERCHANGED		(UM_IGROOT + 600)
