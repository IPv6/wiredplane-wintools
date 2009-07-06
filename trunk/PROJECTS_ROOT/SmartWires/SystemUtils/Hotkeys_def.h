#if !defined(IPV6_HOTKEYS_DEF)
#define IPV6_HOTKEYS_DEF
#define	MAX_KEYS				16
#define	EVENT_KEYS				5
#define MAXHOTKEY_NAMELEN		32
#define CODE_1PREFIX			0x0000
#define CODE_2PREFIX			0x4000
#define CODE_3PREFIX			0x8000
#define CODE_4PREFIX			0xC000
#define CODE_NOPREFIX			(~0xC000)
//#define HOTKEYF_SHIFT           0x01
//#define HOTKEYF_CONTROL         0x02
//#define HOTKEYF_ALT             0x04
//#define HOTKEYF_EXT             0x08
//#define HOTKEYF_PRNHOLD       0x0010
#define HOTKEYF_LSHIFT          0x00000100
#define HOTKEYF_RSHIFT          0x00000200
#define HOTKEYF_LCONTROL        0x00000400
#define HOTKEYF_RCONTROL        0x00000800
#define HOTKEYF_LALT            0x00001000
#define HOTKEYF_RALT            0x00002000
#define HOTKEYF_LEXT            0x00004000
#define HOTKEYF_REXT            0x00008000
// Additional Options
#define ADDOPT_LOWLEVEL			0x0001
#define ADDOPT_SOFT_EVENTS		0x0002
#define ADDOPT_ON_UNPRESS		0x0004
#define ADDOPT_TRANSPARENT		0x0008
#define ADDOPT_SEQUENCE			0x0010
#define ADDOPT_ON_WPRESSED		0x0020
#define ADDOPT_EVENT			0x0040
// Hotkey classes
#define	DOWNONLY_KEYS			0x0001
//#define	EXTEVENT_KEYS		0x0002
// Used in the WKKeyHook dll too!!!
#define KBBUFFER_SIZE			256
#define KBBUFFER_DSIZE			KBBUFFER_SIZE*2
// Special scancodes for handheld KEYS (Mouse buttons for example)
// Do not change them! They are hardcoded into misc dlls too!!!
#define VK_FJ_KEY				KBBUFFER_DSIZE-167
#define VK_MOB_MLES				KBBUFFER_DSIZE-30
#define VK_NULL					KBBUFFER_DSIZE-29
#define VK_MOB_HLP				KBBUFFER_DSIZE-28
#define VK_3X					KBBUFFER_DSIZE-27
#define VK_2X					KBBUFFER_DSIZE-26
#define VK_MOB_ICO				KBBUFFER_DSIZE-25
#define VK_MOB_MAX				KBBUFFER_DSIZE-24
#define VK_MOB_MIN				KBBUFFER_DSIZE-23
#define VK_MOB_CLOSE			KBBUFFER_DSIZE-22
#define VK_MO_TITLE				KBBUFFER_DSIZE-21
#define VK_PRESSNHOLD			KBBUFFER_DSIZE-20
#define VK_RBT_DBL				KBBUFFER_DSIZE-19
#define VK_MBT_DBL				KBBUFFER_DSIZE-18
#define VK_LBT_DBL				KBBUFFER_DSIZE-17
#define VK_MBORDER_L			KBBUFFER_DSIZE-16
#define VK_MBORDER_R			KBBUFFER_DSIZE-15
#define VK_MBORDER_B			KBBUFFER_DSIZE-14
#define VK_MBORDER_T			KBBUFFER_DSIZE-13
#define VK_EVENTKEY				KBBUFFER_DSIZE-12// Особая клавиша - ничего не делает, лишь сбрасывает состояние кнопок движения мыши и др.
#define VK_MCORNER_LB			KBBUFFER_DSIZE-11
#define VK_MCORNER_LT			KBBUFFER_DSIZE-10
#define VK_MCORNER_RB			KBBUFFER_DSIZE-9
#define VK_MCORNER_RT			KBBUFFER_DSIZE-8
#define VK_XBTTNWK4				KBBUFFER_DSIZE-7
#define VK_XBTTNWK3				KBBUFFER_DSIZE-6
#define VK_XBTTNWK2				KBBUFFER_DSIZE-5
#define VK_XBTTNWK1				KBBUFFER_DSIZE-4
#define WHEEL_CL				KBBUFFER_DSIZE-3
#define WHEEL_DO				KBBUFFER_DSIZE-2
#define WHEEL_UP				KBBUFFER_DSIZE-1
//-----------------------------------------------
// Коды для предобработки горячих клавиш
#define BLOCKSTATE_PROCESS		0x80000000
#define BLOCKSTATE_BLOCK		0x40000000
#define BLOCKSTATE_ALLOW		0x20000000
#define BLOCKSTATE_REPLACE		0x10000000
#endif
