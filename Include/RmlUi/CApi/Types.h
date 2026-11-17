#ifndef RMLUI_CAPI_TYPES_H
#define RMLUI_CAPI_TYPES_H

#include "RmlUi/CApi/Header.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RmlContext RmlContext;
typedef struct RmlElement RmlElement;
typedef struct RmlElementDocument RmlElementDocument;
typedef struct RmlElementText RmlElementText;
typedef struct RmlElementForm RmlElementForm;
typedef struct RmlElementFormControl RmlElementFormControl;
typedef struct RmlElementFormControlInput RmlElementFormControlInput;
typedef struct RmlElementFormControlSelect RmlElementFormControlSelect;
typedef struct RmlElementFormControlTextArea RmlElementFormControlTextArea;
typedef struct RmlElementProgress RmlElementProgress;
typedef struct RmlElementTabSet RmlElementTabSet;
typedef struct RmlEvent RmlEvent;
typedef struct RmlEventListener RmlEventListener;
typedef struct RmlVariant RmlVariant;
typedef struct RmlDictionary RmlDictionary;
typedef struct RmlVariantList RmlVariantList;
typedef struct RmlElementList RmlElementList;
typedef struct RmlStringList RmlStringList;
typedef struct RmlSystemInterface RmlSystemInterface;
typedef struct RmlFileInterface RmlFileInterface;
typedef struct RmlRenderInterface RmlRenderInterface;
typedef struct RmlFontEngineInterface RmlFontEngineInterface;
typedef struct RmlTextInputHandler RmlTextInputHandler;
typedef struct RmlTextInputContext RmlTextInputContext;
typedef struct RmlPlugin RmlPlugin;
typedef struct RmlDataTypeDefinition RmlDataTypeDefinition;
typedef struct RmlDecorator RmlDecorator;
typedef struct RmlFilter RmlFilter;
typedef struct RmlFontEffect RmlFontEffect;
typedef struct RmlElementInstancer RmlElementInstancer;
typedef struct RmlDecoratorInstancer RmlDecoratorInstancer;
typedef struct RmlFilterInstancer RmlFilterInstancer;
typedef struct RmlFontEffectInstancer RmlFontEffectInstancer;
typedef struct RmlEventInstancer RmlEventInstancer;
typedef struct RmlEventListenerInstancer RmlEventListenerInstancer;
typedef struct RmlPropertyDefinition RmlPropertyDefinition;
typedef struct RmlPropertyDictionary RmlPropertyDictionary;
typedef struct RmlStyleSheetContainer RmlStyleSheetContainer;
typedef struct RmlPropertyIdSet RmlPropertyIdSet;
typedef struct RmlTexturedMeshList RmlTexturedMeshList;
typedef struct RmlDecoratorInstancerInterface RmlDecoratorInstancerInterface;
typedef struct RmlTextureSource RmlTextureSource;
typedef struct RmlTextureGenerator RmlTextureGenerator;
typedef struct RmlGeometry RmlGeometry;
typedef struct RmlCompiledShader RmlCompiledShader;

typedef struct RmlVector2i {
	int x;
	int y;
} RmlVector2i;

typedef struct RmlVector2f {
	float x;
	float y;
} RmlVector2f;

typedef struct RmlVector3f {
	float x;
	float y;
	float z;
} RmlVector3f;

typedef struct RmlVector4f {
	float x;
	float y;
	float z;
	float w;
} RmlVector4f;

typedef struct RmlColourb {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} RmlColourb;

typedef struct RmlColourbPremultiplied {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} RmlColourbPremultiplied;

typedef struct RmlColourf {
	float red;
	float green;
	float blue;
	float alpha;
} RmlColourf;

typedef struct RmlRectanglei {
	RmlVector2i p0;
	RmlVector2i p1;
} RmlRectanglei;

typedef struct RmlRectanglef {
	RmlVector2f p0;
	RmlVector2f p1;
} RmlRectanglef;

typedef struct RmlMatrix4f {
	float data[16];
} RmlMatrix4f;

typedef struct RmlVertex {
	RmlVector2f position;
	RmlColourbPremultiplied colour;
	RmlVector2f tex_coord;
} RmlVertex;

typedef struct RmlTouch {
	uintptr_t identifier;
	RmlVector2f position;
} RmlTouch;

typedef struct RmlStringView {
	const char* data;
	size_t size;
} RmlStringView;

typedef struct RmlOwnedString {
	char* data;
	size_t size;
} RmlOwnedString;

typedef struct RmlDataModelConstructor {
	void* model_;
	void* type_register_;
} RmlDataModelConstructor;

typedef struct RmlDataModelHandle {
	void* model_;
} RmlDataModelHandle;

typedef struct RmlDataVariable {
	RmlDataTypeDefinition* definition;
	void* ptr;
} RmlDataVariable;

typedef enum RmlDataVariableType {
	RML_DATA_VARIABLE_TYPE_SCALAR = 0,
	RML_DATA_VARIABLE_TYPE_ARRAY = 1,
	RML_DATA_VARIABLE_TYPE_STRUCT = 2,
} RmlDataVariableType;

#define RML_PLUGIN_EVENT_BASIC 1
#define RML_PLUGIN_EVENT_DOCUMENT 2
#define RML_PLUGIN_EVENT_ELEMENT 4
#define RML_PLUGIN_EVENT_DATA_MODEL 8
#define RML_PLUGIN_EVENT_ALL 15

typedef enum RmlShorthandType {
	RML_SHORTHAND_TYPE_FALL_THROUGH = 0,
	RML_SHORTHAND_TYPE_REPLICATE = 1,
	RML_SHORTHAND_TYPE_BOX = 2,
	RML_SHORTHAND_TYPE_RECURSIVE_REPEAT = 3,
	RML_SHORTHAND_TYPE_RECURSIVE_COMMA_SEPARATED = 4,
	RML_SHORTHAND_TYPE_FLEX = 5,
} RmlShorthandType;

typedef struct RmlFontMetrics {
	int size;
	float ascent;
	float descent;
	float line_spacing;
	float x_height;
	float underline_position;
	float underline_thickness;
	bool has_ellipsis;
} RmlFontMetrics;

typedef enum RmlTextDirection {
	RML_TEXT_DIRECTION_AUTO = 0,
	RML_TEXT_DIRECTION_LTR = 1,
	RML_TEXT_DIRECTION_RTL = 2
} RmlTextDirection;

typedef enum RmlFontKerning {
	RML_FONT_KERNING_AUTO = 0,
	RML_FONT_KERNING_NORMAL = 1,
	RML_FONT_KERNING_NONE = 2
} RmlFontKerning;

/* language is borrowed and valid only for the duration of the callback that receives this struct. */
typedef struct RmlTextShapingContext {
	RmlStringView language;
	RmlTextDirection text_direction;
	RmlFontKerning font_kerning;
	float letter_spacing;
} RmlTextShapingContext;

typedef uintptr_t RmlTextureHandle;
typedef uintptr_t RmlCompiledGeometryHandle;
typedef uintptr_t RmlCompiledFilterHandle;
typedef uintptr_t RmlCompiledShaderHandle;
typedef uintptr_t RmlLayerHandle;
typedef uintptr_t RmlFileHandle;
typedef uintptr_t RmlFontFaceHandle;
typedef uintptr_t RmlFontEffectsHandle;
typedef uintptr_t RmlDecoratorDataHandle;

/* A copyable view of a texture loaded through a render manager, like Rml::Texture. Treat the fields as private.
   A zero-initialised value means no texture. The view does not own the texture and stays valid while the render
   manager that produced it exists (until the context is removed or rml_release_render_managers/rml_shutdown is called),
   and, for callback textures, while the texture source that produced it exists. */
typedef struct RmlTexture {
	const void* internal_render_manager;
	uint32_t internal_file_index;
	uint32_t internal_callback_index;
} RmlTexture;

typedef enum RmlLogType {
	RML_LOG_TYPE_ALWAYS = 0,
	RML_LOG_TYPE_ERROR,
	RML_LOG_TYPE_ASSERT,
	RML_LOG_TYPE_WARNING,
	RML_LOG_TYPE_INFO,
	RML_LOG_TYPE_DEBUG,
	RML_LOG_TYPE_MAX
} RmlLogType;

typedef enum RmlKeyIdentifier {
	RML_KEY_UNKNOWN = 0,
	RML_KEY_SPACE = 1,
	RML_KEY_0 = 2,
	RML_KEY_1 = 3,
	RML_KEY_2 = 4,
	RML_KEY_3 = 5,
	RML_KEY_4 = 6,
	RML_KEY_5 = 7,
	RML_KEY_6 = 8,
	RML_KEY_7 = 9,
	RML_KEY_8 = 10,
	RML_KEY_9 = 11,
	RML_KEY_A = 12,
	RML_KEY_B = 13,
	RML_KEY_C = 14,
	RML_KEY_D = 15,
	RML_KEY_E = 16,
	RML_KEY_F = 17,
	RML_KEY_G = 18,
	RML_KEY_H = 19,
	RML_KEY_I = 20,
	RML_KEY_J = 21,
	RML_KEY_K = 22,
	RML_KEY_L = 23,
	RML_KEY_M = 24,
	RML_KEY_N = 25,
	RML_KEY_O = 26,
	RML_KEY_P = 27,
	RML_KEY_Q = 28,
	RML_KEY_R = 29,
	RML_KEY_S = 30,
	RML_KEY_T = 31,
	RML_KEY_U = 32,
	RML_KEY_V = 33,
	RML_KEY_W = 34,
	RML_KEY_X = 35,
	RML_KEY_Y = 36,
	RML_KEY_Z = 37,
	RML_KEY_OEM_1 = 38,
	RML_KEY_OEM_PLUS = 39,
	RML_KEY_OEM_COMMA = 40,
	RML_KEY_OEM_MINUS = 41,
	RML_KEY_OEM_PERIOD = 42,
	RML_KEY_OEM_2 = 43,
	RML_KEY_OEM_3 = 44,
	RML_KEY_OEM_4 = 45,
	RML_KEY_OEM_5 = 46,
	RML_KEY_OEM_6 = 47,
	RML_KEY_OEM_7 = 48,
	RML_KEY_OEM_8 = 49,
	RML_KEY_OEM_102 = 50,
	RML_KEY_NUMPAD0 = 51,
	RML_KEY_NUMPAD1 = 52,
	RML_KEY_NUMPAD2 = 53,
	RML_KEY_NUMPAD3 = 54,
	RML_KEY_NUMPAD4 = 55,
	RML_KEY_NUMPAD5 = 56,
	RML_KEY_NUMPAD6 = 57,
	RML_KEY_NUMPAD7 = 58,
	RML_KEY_NUMPAD8 = 59,
	RML_KEY_NUMPAD9 = 60,
	RML_KEY_NUMPADENTER = 61,
	RML_KEY_MULTIPLY = 62,
	RML_KEY_ADD = 63,
	RML_KEY_SEPARATOR = 64,
	RML_KEY_SUBTRACT = 65,
	RML_KEY_DECIMAL = 66,
	RML_KEY_DIVIDE = 67,
	RML_KEY_OEM_NEC_EQUAL = 68,
	RML_KEY_BACK = 69,
	RML_KEY_TAB = 70,
	RML_KEY_CLEAR = 71,
	RML_KEY_RETURN = 72,
	RML_KEY_PAUSE = 73,
	RML_KEY_CAPITAL = 74,
	RML_KEY_KANA = 75,
	RML_KEY_HANGUL = 76,
	RML_KEY_JUNJA = 77,
	RML_KEY_FINAL = 78,
	RML_KEY_HANJA = 79,
	RML_KEY_KANJI = 80,
	RML_KEY_ESCAPE = 81,
	RML_KEY_CONVERT = 82,
	RML_KEY_NONCONVERT = 83,
	RML_KEY_ACCEPT = 84,
	RML_KEY_MODECHANGE = 85,
	RML_KEY_PRIOR = 86,
	RML_KEY_NEXT = 87,
	RML_KEY_END = 88,
	RML_KEY_HOME = 89,
	RML_KEY_LEFT = 90,
	RML_KEY_UP = 91,
	RML_KEY_RIGHT = 92,
	RML_KEY_DOWN = 93,
	RML_KEY_SELECT = 94,
	RML_KEY_PRINT = 95,
	RML_KEY_EXECUTE = 96,
	RML_KEY_SNAPSHOT = 97,
	RML_KEY_INSERT = 98,
	RML_KEY_DELETE = 99,
	RML_KEY_HELP = 100,
	RML_KEY_LWIN = 101,
	RML_KEY_RWIN = 102,
	RML_KEY_APPS = 103,
	RML_KEY_POWER = 104,
	RML_KEY_SLEEP = 105,
	RML_KEY_WAKE = 106,
	RML_KEY_F1 = 107,
	RML_KEY_F2 = 108,
	RML_KEY_F3 = 109,
	RML_KEY_F4 = 110,
	RML_KEY_F5 = 111,
	RML_KEY_F6 = 112,
	RML_KEY_F7 = 113,
	RML_KEY_F8 = 114,
	RML_KEY_F9 = 115,
	RML_KEY_F10 = 116,
	RML_KEY_F11 = 117,
	RML_KEY_F12 = 118,
	RML_KEY_F13 = 119,
	RML_KEY_F14 = 120,
	RML_KEY_F15 = 121,
	RML_KEY_F16 = 122,
	RML_KEY_F17 = 123,
	RML_KEY_F18 = 124,
	RML_KEY_F19 = 125,
	RML_KEY_F20 = 126,
	RML_KEY_F21 = 127,
	RML_KEY_F22 = 128,
	RML_KEY_F23 = 129,
	RML_KEY_F24 = 130,
	RML_KEY_NUMLOCK = 131,
	RML_KEY_SCROLL = 132,
	RML_KEY_OEM_FJ_JISHO = 133,
	RML_KEY_OEM_FJ_MASSHOU = 134,
	RML_KEY_OEM_FJ_TOUROKU = 135,
	RML_KEY_OEM_FJ_LOYA = 136,
	RML_KEY_OEM_FJ_ROYA = 137,
	RML_KEY_LSHIFT = 138,
	RML_KEY_RSHIFT = 139,
	RML_KEY_LCONTROL = 140,
	RML_KEY_RCONTROL = 141,
	RML_KEY_LMENU = 142,
	RML_KEY_RMENU = 143,
	RML_KEY_BROWSER_BACK = 144,
	RML_KEY_BROWSER_FORWARD = 145,
	RML_KEY_BROWSER_REFRESH = 146,
	RML_KEY_BROWSER_STOP = 147,
	RML_KEY_BROWSER_SEARCH = 148,
	RML_KEY_BROWSER_FAVORITES = 149,
	RML_KEY_BROWSER_HOME = 150,
	RML_KEY_VOLUME_MUTE = 151,
	RML_KEY_VOLUME_DOWN = 152,
	RML_KEY_VOLUME_UP = 153,
	RML_KEY_MEDIA_NEXT_TRACK = 154,
	RML_KEY_MEDIA_PREV_TRACK = 155,
	RML_KEY_MEDIA_STOP = 156,
	RML_KEY_MEDIA_PLAY_PAUSE = 157,
	RML_KEY_LAUNCH_MAIL = 158,
	RML_KEY_LAUNCH_MEDIA_SELECT = 159,
	RML_KEY_LAUNCH_APP1 = 160,
	RML_KEY_LAUNCH_APP2 = 161,
	RML_KEY_OEM_AX = 162,
	RML_KEY_ICO_HELP = 163,
	RML_KEY_ICO_00 = 164,
	RML_KEY_PROCESSKEY = 165,
	RML_KEY_ICO_CLEAR = 166,
	RML_KEY_ATTN = 167,
	RML_KEY_CRSEL = 168,
	RML_KEY_EXSEL = 169,
	RML_KEY_EREOF = 170,
	RML_KEY_PLAY = 171,
	RML_KEY_ZOOM = 172,
	RML_KEY_PA1 = 173,
	RML_KEY_OEM_CLEAR = 174,
	RML_KEY_LMETA = 175,
	RML_KEY_RMETA = 176,
	RML_KEY_FIRST_CUSTOM_KEY = 177,
	RML_KEY_LAST_CUSTOM_KEY = 250
} RmlKeyIdentifier;

typedef enum RmlKeyModifier {
	RML_KEY_MODIFIER_CTRL = 1 << 0,
	RML_KEY_MODIFIER_SHIFT = 1 << 1,
	RML_KEY_MODIFIER_ALT = 1 << 2,
	RML_KEY_MODIFIER_META = 1 << 3,
	RML_KEY_MODIFIER_CAPSLOCK = 1 << 4,
	RML_KEY_MODIFIER_NUMLOCK = 1 << 5,
	RML_KEY_MODIFIER_SCROLLLOCK = 1 << 6
} RmlKeyModifier;

typedef enum RmlFontStyle {
	RML_FONT_STYLE_NORMAL = 0,
	RML_FONT_STYLE_ITALIC = 1
} RmlFontStyle;

typedef enum RmlFontWeight {
	RML_FONT_WEIGHT_AUTO = 0,
	RML_FONT_WEIGHT_NORMAL = 400,
	RML_FONT_WEIGHT_BOLD = 700
} RmlFontWeight;

typedef enum RmlScrollBehavior {
	RML_SCROLL_BEHAVIOR_AUTO = 0,
	RML_SCROLL_BEHAVIOR_SMOOTH = 1,
	RML_SCROLL_BEHAVIOR_INSTANT = 2
} RmlScrollBehavior;

typedef enum RmlScrollAlignment {
	RML_SCROLL_ALIGNMENT_START = 0,
	RML_SCROLL_ALIGNMENT_CENTER = 1,
	RML_SCROLL_ALIGNMENT_END = 2,
	RML_SCROLL_ALIGNMENT_NEAREST = 3,
	RML_SCROLL_ALIGNMENT_ADAPTIVE = 4
} RmlScrollAlignment;

typedef enum RmlScrollParentage {
	RML_SCROLL_PARENTAGE_ALL = 0,
	RML_SCROLL_PARENTAGE_CLOSEST = 1
} RmlScrollParentage;

typedef struct RmlScrollIntoViewOptions {
	RmlScrollAlignment vertical;
	RmlScrollAlignment horizontal;
	RmlScrollBehavior behavior;
	RmlScrollParentage parentage;
} RmlScrollIntoViewOptions;

typedef enum RmlModalFlag {
	RML_MODAL_FLAG_NONE = 0,
	RML_MODAL_FLAG_MODAL = 1,
	RML_MODAL_FLAG_KEEP = 2
} RmlModalFlag;

typedef enum RmlFocusFlag {
	RML_FOCUS_FLAG_NONE = 0,
	RML_FOCUS_FLAG_DOCUMENT = 1,
	RML_FOCUS_FLAG_KEEP = 2,
	RML_FOCUS_FLAG_AUTO = 3
} RmlFocusFlag;

typedef enum RmlScrollFlag {
	RML_SCROLL_FLAG_NONE = 0,
	RML_SCROLL_FLAG_AUTO = 1
} RmlScrollFlag;

typedef enum RmlClipMaskOperation {
	RML_CLIP_MASK_OPERATION_SET = 0,
	RML_CLIP_MASK_OPERATION_SET_INVERSE = 1,
	RML_CLIP_MASK_OPERATION_INTERSECT = 2
} RmlClipMaskOperation;

typedef enum RmlBlendMode {
	RML_BLEND_MODE_BLEND = 0,
	RML_BLEND_MODE_REPLACE = 1
} RmlBlendMode;

typedef enum RmlEventPhase {
	RML_EVENT_PHASE_NONE = 0,
	RML_EVENT_PHASE_CAPTURE = 1,
	RML_EVENT_PHASE_TARGET = 2,
	RML_EVENT_PHASE_BUBBLE = 4
} RmlEventPhase;

typedef enum RmlDefaultActionPhase {
	RML_DEFAULT_ACTION_PHASE_NONE = 0,
	RML_DEFAULT_ACTION_PHASE_TARGET = 2,
	RML_DEFAULT_ACTION_PHASE_TARGET_AND_BUBBLE = 6
} RmlDefaultActionPhase;

typedef enum RmlBoxArea {
	RML_BOX_AREA_MARGIN = 0,
	RML_BOX_AREA_BORDER = 1,
	RML_BOX_AREA_PADDING = 2,
	RML_BOX_AREA_CONTENT = 3,
	RML_BOX_AREA_AUTO = 4
} RmlBoxArea;

typedef enum RmlVariantType {
	RML_VARIANT_TYPE_NONE = '-',
	RML_VARIANT_TYPE_BOOL = 'B',
	RML_VARIANT_TYPE_BYTE = 'b',
	RML_VARIANT_TYPE_CHAR = 'c',
	RML_VARIANT_TYPE_FLOAT = 'f',
	RML_VARIANT_TYPE_DOUBLE = 'd',
	RML_VARIANT_TYPE_INT = 'i',
	RML_VARIANT_TYPE_INT64 = 'I',
	RML_VARIANT_TYPE_UINT = 'u',
	RML_VARIANT_TYPE_UINT64 = 'U',
	RML_VARIANT_TYPE_STRING = 's',
	RML_VARIANT_TYPE_VECTOR2 = '2',
	RML_VARIANT_TYPE_VECTOR3 = '3',
	RML_VARIANT_TYPE_VECTOR4 = '4',
	RML_VARIANT_TYPE_COLOURF = 'g',
	RML_VARIANT_TYPE_COLOURB = 'h',
	RML_VARIANT_TYPE_SCRIPTINTERFACE = 'p',
	RML_VARIANT_TYPE_TRANSFORMPTR = 't',
	RML_VARIANT_TYPE_TRANSITIONLIST = 'T',
	RML_VARIANT_TYPE_ANIMATIONLIST = 'A',
	RML_VARIANT_TYPE_DECORATORSPTR = 'D',
	RML_VARIANT_TYPE_FILTERSPTR = 'F',
	RML_VARIANT_TYPE_FONTEFFECTSPTR = 'E',
	RML_VARIANT_TYPE_COLORSTOPLIST = 'C',
	RML_VARIANT_TYPE_BOXSHADOWLIST = 'S',
	RML_VARIANT_TYPE_VOIDPTR = '*'
} RmlVariantType;

static inline RmlStringView rml_sv(const char* str)
{
	RmlStringView view;
	view.data = str;
	view.size = str ? strlen(str) : 0;
	return view;
}

RMLUI_CAPI_API void rml_owned_string_free(RmlOwnedString str);
RMLUI_CAPI_API void rml_owned_string_set(RmlOwnedString* out, RmlStringView view);
RMLUI_CAPI_API bool rml_matrix_is_row_major(void);

#ifdef __cplusplus
}
#endif

#endif
