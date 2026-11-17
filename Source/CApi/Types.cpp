#include <RmlUi/CApi/Types.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/DataVariable.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/ScrollTypes.h>
#include <RmlUi/Core/FontMetrics.h>
#include <RmlUi/Core/PropertySpecification.h>
#include <RmlUi/Core/StyleTypes.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Variant.h>
#include <RmlUi/Core/Vertex.h>
#include <cstddef>

static_assert(sizeof(RmlVector2i) == sizeof(Rml::Vector2i));
static_assert(alignof(RmlVector2i) == alignof(Rml::Vector2i));
static_assert(offsetof(RmlVector2i, x) == offsetof(Rml::Vector2i, x));
static_assert(offsetof(RmlVector2i, y) == offsetof(Rml::Vector2i, y));

static_assert(sizeof(RmlVector2f) == sizeof(Rml::Vector2f));
static_assert(alignof(RmlVector2f) == alignof(Rml::Vector2f));
static_assert(offsetof(RmlVector2f, x) == offsetof(Rml::Vector2f, x));
static_assert(offsetof(RmlVector2f, y) == offsetof(Rml::Vector2f, y));

static_assert(sizeof(RmlVector3i) == sizeof(Rml::Vector3i));
static_assert(alignof(RmlVector3i) == alignof(Rml::Vector3i));
static_assert(offsetof(RmlVector3i, x) == offsetof(Rml::Vector3i, x));
static_assert(offsetof(RmlVector3i, y) == offsetof(Rml::Vector3i, y));
static_assert(offsetof(RmlVector3i, z) == offsetof(Rml::Vector3i, z));

static_assert(sizeof(RmlVector3f) == sizeof(Rml::Vector3f));
static_assert(alignof(RmlVector3f) == alignof(Rml::Vector3f));
static_assert(offsetof(RmlVector3f, x) == offsetof(Rml::Vector3f, x));
static_assert(offsetof(RmlVector3f, y) == offsetof(Rml::Vector3f, y));
static_assert(offsetof(RmlVector3f, z) == offsetof(Rml::Vector3f, z));

static_assert(sizeof(RmlVector4i) == sizeof(Rml::Vector4i));
static_assert(alignof(RmlVector4i) == alignof(Rml::Vector4i));
static_assert(offsetof(RmlVector4i, x) == offsetof(Rml::Vector4i, x));
static_assert(offsetof(RmlVector4i, y) == offsetof(Rml::Vector4i, y));
static_assert(offsetof(RmlVector4i, z) == offsetof(Rml::Vector4i, z));
static_assert(offsetof(RmlVector4i, w) == offsetof(Rml::Vector4i, w));

static_assert(sizeof(RmlVector4f) == sizeof(Rml::Vector4f));
static_assert(alignof(RmlVector4f) == alignof(Rml::Vector4f));
static_assert(offsetof(RmlVector4f, x) == offsetof(Rml::Vector4f, x));
static_assert(offsetof(RmlVector4f, y) == offsetof(Rml::Vector4f, y));
static_assert(offsetof(RmlVector4f, z) == offsetof(Rml::Vector4f, z));
static_assert(offsetof(RmlVector4f, w) == offsetof(Rml::Vector4f, w));

static_assert(sizeof(RmlColourb) == sizeof(Rml::Colourb));
static_assert(alignof(RmlColourb) == alignof(Rml::Colourb));
static_assert(offsetof(RmlColourb, red) == offsetof(Rml::Colourb, red));
static_assert(offsetof(RmlColourb, green) == offsetof(Rml::Colourb, green));
static_assert(offsetof(RmlColourb, blue) == offsetof(Rml::Colourb, blue));
static_assert(offsetof(RmlColourb, alpha) == offsetof(Rml::Colourb, alpha));

static_assert(sizeof(RmlColourbPremultiplied) == sizeof(Rml::ColourbPremultiplied));
static_assert(alignof(RmlColourbPremultiplied) == alignof(Rml::ColourbPremultiplied));
static_assert(offsetof(RmlColourbPremultiplied, red) == offsetof(Rml::ColourbPremultiplied, red));
static_assert(offsetof(RmlColourbPremultiplied, green) == offsetof(Rml::ColourbPremultiplied, green));
static_assert(offsetof(RmlColourbPremultiplied, blue) == offsetof(Rml::ColourbPremultiplied, blue));
static_assert(offsetof(RmlColourbPremultiplied, alpha) == offsetof(Rml::ColourbPremultiplied, alpha));

static_assert(sizeof(RmlColourf) == sizeof(Rml::Colourf));
static_assert(alignof(RmlColourf) == alignof(Rml::Colourf));
static_assert(offsetof(RmlColourf, red) == offsetof(Rml::Colourf, red));
static_assert(offsetof(RmlColourf, green) == offsetof(Rml::Colourf, green));
static_assert(offsetof(RmlColourf, blue) == offsetof(Rml::Colourf, blue));
static_assert(offsetof(RmlColourf, alpha) == offsetof(Rml::Colourf, alpha));

static_assert(sizeof(RmlRectanglei) == sizeof(Rml::Rectanglei));
static_assert(alignof(RmlRectanglei) == alignof(Rml::Rectanglei));
static_assert(offsetof(RmlRectanglei, p0) == offsetof(Rml::Rectanglei, p0));
static_assert(offsetof(RmlRectanglei, p1) == offsetof(Rml::Rectanglei, p1));

static_assert(sizeof(RmlRectanglef) == sizeof(Rml::Rectanglef));
static_assert(alignof(RmlRectanglef) == alignof(Rml::Rectanglef));
static_assert(offsetof(RmlRectanglef, p0) == offsetof(Rml::Rectanglef, p0));
static_assert(offsetof(RmlRectanglef, p1) == offsetof(Rml::Rectanglef, p1));

static_assert(sizeof(RmlMatrix4f) == sizeof(Rml::Matrix4f));
static_assert(alignof(RmlMatrix4f) == alignof(Rml::Matrix4f));
static_assert(offsetof(RmlMatrix4f, data) == 0);
// Matrix4's storage is a private vectors[4] as the first member; data() points at its first float.
static_assert(sizeof(Rml::Matrix4f) == sizeof(float) * 16);

static_assert(sizeof(RmlVertex) == sizeof(Rml::Vertex));
static_assert(alignof(RmlVertex) == alignof(Rml::Vertex));
static_assert(offsetof(RmlVertex, position) == offsetof(Rml::Vertex, position));
static_assert(offsetof(RmlVertex, colour) == offsetof(Rml::Vertex, colour));
static_assert(offsetof(RmlVertex, tex_coord) == offsetof(Rml::Vertex, tex_coord));

static_assert(sizeof(RmlTouch) == sizeof(Rml::Touch));
static_assert(alignof(RmlTouch) == alignof(Rml::Touch));
static_assert(offsetof(RmlTouch, identifier) == offsetof(Rml::Touch, identifier));
static_assert(offsetof(RmlTouch, position) == offsetof(Rml::Touch, position));

static_assert(RML_LOG_TYPE_ALWAYS == static_cast<int>(Rml::Log::Type::LT_ALWAYS));
static_assert(RML_LOG_TYPE_ERROR == static_cast<int>(Rml::Log::Type::LT_ERROR));
static_assert(RML_LOG_TYPE_ASSERT == static_cast<int>(Rml::Log::Type::LT_ASSERT));
static_assert(RML_LOG_TYPE_WARNING == static_cast<int>(Rml::Log::Type::LT_WARNING));
static_assert(RML_LOG_TYPE_INFO == static_cast<int>(Rml::Log::Type::LT_INFO));
static_assert(RML_LOG_TYPE_DEBUG == static_cast<int>(Rml::Log::Type::LT_DEBUG));
static_assert(RML_LOG_TYPE_MAX == static_cast<int>(Rml::Log::Type::LT_MAX));

#define RML_ASSERT_KEY(name) static_assert(RML_KEY_##name == static_cast<int>(Rml::Input::KI_##name), #name)
RML_ASSERT_KEY(UNKNOWN);
RML_ASSERT_KEY(SPACE);
RML_ASSERT_KEY(0);
RML_ASSERT_KEY(1);
RML_ASSERT_KEY(2);
RML_ASSERT_KEY(3);
RML_ASSERT_KEY(4);
RML_ASSERT_KEY(5);
RML_ASSERT_KEY(6);
RML_ASSERT_KEY(7);
RML_ASSERT_KEY(8);
RML_ASSERT_KEY(9);
RML_ASSERT_KEY(A);
RML_ASSERT_KEY(B);
RML_ASSERT_KEY(C);
RML_ASSERT_KEY(D);
RML_ASSERT_KEY(E);
RML_ASSERT_KEY(F);
RML_ASSERT_KEY(G);
RML_ASSERT_KEY(H);
RML_ASSERT_KEY(I);
RML_ASSERT_KEY(J);
RML_ASSERT_KEY(K);
RML_ASSERT_KEY(L);
RML_ASSERT_KEY(M);
RML_ASSERT_KEY(N);
RML_ASSERT_KEY(O);
RML_ASSERT_KEY(P);
RML_ASSERT_KEY(Q);
RML_ASSERT_KEY(R);
RML_ASSERT_KEY(S);
RML_ASSERT_KEY(T);
RML_ASSERT_KEY(U);
RML_ASSERT_KEY(V);
RML_ASSERT_KEY(W);
RML_ASSERT_KEY(X);
RML_ASSERT_KEY(Y);
RML_ASSERT_KEY(Z);
RML_ASSERT_KEY(OEM_1);
RML_ASSERT_KEY(OEM_PLUS);
RML_ASSERT_KEY(OEM_COMMA);
RML_ASSERT_KEY(OEM_MINUS);
RML_ASSERT_KEY(OEM_PERIOD);
RML_ASSERT_KEY(OEM_2);
RML_ASSERT_KEY(OEM_3);
RML_ASSERT_KEY(OEM_4);
RML_ASSERT_KEY(OEM_5);
RML_ASSERT_KEY(OEM_6);
RML_ASSERT_KEY(OEM_7);
RML_ASSERT_KEY(OEM_8);
RML_ASSERT_KEY(OEM_102);
RML_ASSERT_KEY(NUMPAD0);
RML_ASSERT_KEY(NUMPAD1);
RML_ASSERT_KEY(NUMPAD2);
RML_ASSERT_KEY(NUMPAD3);
RML_ASSERT_KEY(NUMPAD4);
RML_ASSERT_KEY(NUMPAD5);
RML_ASSERT_KEY(NUMPAD6);
RML_ASSERT_KEY(NUMPAD7);
RML_ASSERT_KEY(NUMPAD8);
RML_ASSERT_KEY(NUMPAD9);
RML_ASSERT_KEY(NUMPADENTER);
RML_ASSERT_KEY(MULTIPLY);
RML_ASSERT_KEY(ADD);
RML_ASSERT_KEY(SEPARATOR);
RML_ASSERT_KEY(SUBTRACT);
RML_ASSERT_KEY(DECIMAL);
RML_ASSERT_KEY(DIVIDE);
RML_ASSERT_KEY(OEM_NEC_EQUAL);
RML_ASSERT_KEY(BACK);
RML_ASSERT_KEY(TAB);
RML_ASSERT_KEY(CLEAR);
RML_ASSERT_KEY(RETURN);
RML_ASSERT_KEY(PAUSE);
RML_ASSERT_KEY(CAPITAL);
RML_ASSERT_KEY(KANA);
RML_ASSERT_KEY(HANGUL);
RML_ASSERT_KEY(JUNJA);
RML_ASSERT_KEY(FINAL);
RML_ASSERT_KEY(HANJA);
RML_ASSERT_KEY(KANJI);
RML_ASSERT_KEY(ESCAPE);
RML_ASSERT_KEY(CONVERT);
RML_ASSERT_KEY(NONCONVERT);
RML_ASSERT_KEY(ACCEPT);
RML_ASSERT_KEY(MODECHANGE);
RML_ASSERT_KEY(PRIOR);
RML_ASSERT_KEY(NEXT);
RML_ASSERT_KEY(END);
RML_ASSERT_KEY(HOME);
RML_ASSERT_KEY(LEFT);
RML_ASSERT_KEY(UP);
RML_ASSERT_KEY(RIGHT);
RML_ASSERT_KEY(DOWN);
RML_ASSERT_KEY(SELECT);
RML_ASSERT_KEY(PRINT);
RML_ASSERT_KEY(EXECUTE);
RML_ASSERT_KEY(SNAPSHOT);
RML_ASSERT_KEY(INSERT);
RML_ASSERT_KEY(DELETE);
RML_ASSERT_KEY(HELP);
RML_ASSERT_KEY(LWIN);
RML_ASSERT_KEY(RWIN);
RML_ASSERT_KEY(APPS);
RML_ASSERT_KEY(POWER);
RML_ASSERT_KEY(SLEEP);
RML_ASSERT_KEY(WAKE);
RML_ASSERT_KEY(F1);
RML_ASSERT_KEY(F2);
RML_ASSERT_KEY(F3);
RML_ASSERT_KEY(F4);
RML_ASSERT_KEY(F5);
RML_ASSERT_KEY(F6);
RML_ASSERT_KEY(F7);
RML_ASSERT_KEY(F8);
RML_ASSERT_KEY(F9);
RML_ASSERT_KEY(F10);
RML_ASSERT_KEY(F11);
RML_ASSERT_KEY(F12);
RML_ASSERT_KEY(F13);
RML_ASSERT_KEY(F14);
RML_ASSERT_KEY(F15);
RML_ASSERT_KEY(F16);
RML_ASSERT_KEY(F17);
RML_ASSERT_KEY(F18);
RML_ASSERT_KEY(F19);
RML_ASSERT_KEY(F20);
RML_ASSERT_KEY(F21);
RML_ASSERT_KEY(F22);
RML_ASSERT_KEY(F23);
RML_ASSERT_KEY(F24);
RML_ASSERT_KEY(NUMLOCK);
RML_ASSERT_KEY(SCROLL);
RML_ASSERT_KEY(OEM_FJ_JISHO);
RML_ASSERT_KEY(OEM_FJ_MASSHOU);
RML_ASSERT_KEY(OEM_FJ_TOUROKU);
RML_ASSERT_KEY(OEM_FJ_LOYA);
RML_ASSERT_KEY(OEM_FJ_ROYA);
RML_ASSERT_KEY(LSHIFT);
RML_ASSERT_KEY(RSHIFT);
RML_ASSERT_KEY(LCONTROL);
RML_ASSERT_KEY(RCONTROL);
RML_ASSERT_KEY(LMENU);
RML_ASSERT_KEY(RMENU);
RML_ASSERT_KEY(BROWSER_BACK);
RML_ASSERT_KEY(BROWSER_FORWARD);
RML_ASSERT_KEY(BROWSER_REFRESH);
RML_ASSERT_KEY(BROWSER_STOP);
RML_ASSERT_KEY(BROWSER_SEARCH);
RML_ASSERT_KEY(BROWSER_FAVORITES);
RML_ASSERT_KEY(BROWSER_HOME);
RML_ASSERT_KEY(VOLUME_MUTE);
RML_ASSERT_KEY(VOLUME_DOWN);
RML_ASSERT_KEY(VOLUME_UP);
RML_ASSERT_KEY(MEDIA_NEXT_TRACK);
RML_ASSERT_KEY(MEDIA_PREV_TRACK);
RML_ASSERT_KEY(MEDIA_STOP);
RML_ASSERT_KEY(MEDIA_PLAY_PAUSE);
RML_ASSERT_KEY(LAUNCH_MAIL);
RML_ASSERT_KEY(LAUNCH_MEDIA_SELECT);
RML_ASSERT_KEY(LAUNCH_APP1);
RML_ASSERT_KEY(LAUNCH_APP2);
RML_ASSERT_KEY(OEM_AX);
RML_ASSERT_KEY(ICO_HELP);
RML_ASSERT_KEY(ICO_00);
RML_ASSERT_KEY(PROCESSKEY);
RML_ASSERT_KEY(ICO_CLEAR);
RML_ASSERT_KEY(ATTN);
RML_ASSERT_KEY(CRSEL);
RML_ASSERT_KEY(EXSEL);
RML_ASSERT_KEY(EREOF);
RML_ASSERT_KEY(PLAY);
RML_ASSERT_KEY(ZOOM);
RML_ASSERT_KEY(PA1);
RML_ASSERT_KEY(OEM_CLEAR);
RML_ASSERT_KEY(LMETA);
RML_ASSERT_KEY(RMETA);
RML_ASSERT_KEY(FIRST_CUSTOM_KEY);
RML_ASSERT_KEY(LAST_CUSTOM_KEY);
#undef RML_ASSERT_KEY

static_assert(RML_KEY_MODIFIER_CTRL == static_cast<int>(Rml::Input::KM_CTRL));
static_assert(RML_KEY_MODIFIER_SHIFT == static_cast<int>(Rml::Input::KM_SHIFT));
static_assert(RML_KEY_MODIFIER_ALT == static_cast<int>(Rml::Input::KM_ALT));
static_assert(RML_KEY_MODIFIER_META == static_cast<int>(Rml::Input::KM_META));
static_assert(RML_KEY_MODIFIER_CAPSLOCK == static_cast<int>(Rml::Input::KM_CAPSLOCK));
static_assert(RML_KEY_MODIFIER_NUMLOCK == static_cast<int>(Rml::Input::KM_NUMLOCK));
static_assert(RML_KEY_MODIFIER_SCROLLLOCK == static_cast<int>(Rml::Input::KM_SCROLLLOCK));

static_assert(RML_FONT_STYLE_NORMAL == static_cast<int>(Rml::Style::FontStyle::Normal));
static_assert(RML_FONT_STYLE_ITALIC == static_cast<int>(Rml::Style::FontStyle::Italic));

static_assert(RML_TEXT_DIRECTION_AUTO == static_cast<int>(Rml::Style::Direction::Auto));
static_assert(RML_TEXT_DIRECTION_LTR == static_cast<int>(Rml::Style::Direction::Ltr));
static_assert(RML_TEXT_DIRECTION_RTL == static_cast<int>(Rml::Style::Direction::Rtl));
static_assert(RML_FONT_KERNING_AUTO == static_cast<int>(Rml::Style::FontKerning::Auto));
static_assert(RML_FONT_KERNING_NORMAL == static_cast<int>(Rml::Style::FontKerning::Normal));
static_assert(RML_FONT_KERNING_NONE == static_cast<int>(Rml::Style::FontKerning::None));

static_assert(RML_FONT_WEIGHT_AUTO == static_cast<int>(Rml::Style::FontWeight::Auto));
static_assert(RML_FONT_WEIGHT_NORMAL == static_cast<int>(Rml::Style::FontWeight::Normal));
static_assert(RML_FONT_WEIGHT_BOLD == static_cast<int>(Rml::Style::FontWeight::Bold));

static_assert(RML_SCROLL_BEHAVIOR_AUTO == static_cast<int>(Rml::ScrollBehavior::Auto));
static_assert(RML_SCROLL_BEHAVIOR_SMOOTH == static_cast<int>(Rml::ScrollBehavior::Smooth));
static_assert(RML_SCROLL_BEHAVIOR_INSTANT == static_cast<int>(Rml::ScrollBehavior::Instant));

static_assert(RML_SCROLL_ALIGNMENT_START == static_cast<int>(Rml::ScrollAlignment::Start));
static_assert(RML_SCROLL_ALIGNMENT_CENTER == static_cast<int>(Rml::ScrollAlignment::Center));
static_assert(RML_SCROLL_ALIGNMENT_END == static_cast<int>(Rml::ScrollAlignment::End));
static_assert(RML_SCROLL_ALIGNMENT_NEAREST == static_cast<int>(Rml::ScrollAlignment::Nearest));
static_assert(RML_SCROLL_ALIGNMENT_ADAPTIVE == static_cast<int>(Rml::ScrollAlignment::Adaptive));

static_assert(RML_SCROLL_PARENTAGE_ALL == static_cast<int>(Rml::ScrollParentage::All));
static_assert(RML_SCROLL_PARENTAGE_CLOSEST == static_cast<int>(Rml::ScrollParentage::Closest));

static_assert(RML_MODAL_FLAG_NONE == static_cast<int>(Rml::ModalFlag::None));
static_assert(RML_MODAL_FLAG_MODAL == static_cast<int>(Rml::ModalFlag::Modal));
static_assert(RML_MODAL_FLAG_KEEP == static_cast<int>(Rml::ModalFlag::Keep));

static_assert(RML_FOCUS_FLAG_NONE == static_cast<int>(Rml::FocusFlag::None));
static_assert(RML_FOCUS_FLAG_DOCUMENT == static_cast<int>(Rml::FocusFlag::Document));
static_assert(RML_FOCUS_FLAG_KEEP == static_cast<int>(Rml::FocusFlag::Keep));
static_assert(RML_FOCUS_FLAG_AUTO == static_cast<int>(Rml::FocusFlag::Auto));

static_assert(RML_SCROLL_FLAG_NONE == static_cast<int>(Rml::ScrollFlag::None));
static_assert(RML_SCROLL_FLAG_AUTO == static_cast<int>(Rml::ScrollFlag::Auto));

static_assert(RML_CLIP_MASK_OPERATION_SET == static_cast<int>(Rml::ClipMaskOperation::Set));
static_assert(RML_CLIP_MASK_OPERATION_SET_INVERSE == static_cast<int>(Rml::ClipMaskOperation::SetInverse));
static_assert(RML_CLIP_MASK_OPERATION_INTERSECT == static_cast<int>(Rml::ClipMaskOperation::Intersect));

static_assert(RML_BLEND_MODE_BLEND == static_cast<int>(Rml::BlendMode::Blend));
static_assert(RML_BLEND_MODE_REPLACE == static_cast<int>(Rml::BlendMode::Replace));

static_assert(RML_EVENT_PHASE_NONE == static_cast<int>(Rml::EventPhase::None));
static_assert(RML_EVENT_PHASE_CAPTURE == static_cast<int>(Rml::EventPhase::Capture));
static_assert(RML_EVENT_PHASE_TARGET == static_cast<int>(Rml::EventPhase::Target));
static_assert(RML_EVENT_PHASE_BUBBLE == static_cast<int>(Rml::EventPhase::Bubble));

static_assert(RML_DEFAULT_ACTION_PHASE_NONE == static_cast<int>(Rml::DefaultActionPhase::None));
static_assert(RML_DEFAULT_ACTION_PHASE_TARGET == static_cast<int>(Rml::DefaultActionPhase::Target));
static_assert(RML_DEFAULT_ACTION_PHASE_TARGET_AND_BUBBLE == static_cast<int>(Rml::DefaultActionPhase::TargetAndBubble));

static_assert(RML_BOX_AREA_MARGIN == static_cast<int>(Rml::BoxArea::Margin));
static_assert(RML_BOX_AREA_BORDER == static_cast<int>(Rml::BoxArea::Border));
static_assert(RML_BOX_AREA_PADDING == static_cast<int>(Rml::BoxArea::Padding));
static_assert(RML_BOX_AREA_CONTENT == static_cast<int>(Rml::BoxArea::Content));
static_assert(RML_BOX_AREA_AUTO == static_cast<int>(Rml::BoxArea::Auto));

static_assert(sizeof(RmlDataModelConstructor) == sizeof(Rml::DataModelConstructor));
static_assert(alignof(RmlDataModelConstructor) == alignof(Rml::DataModelConstructor));

static_assert(sizeof(RmlDataModelHandle) == sizeof(Rml::DataModelHandle));
static_assert(alignof(RmlDataModelHandle) == alignof(Rml::DataModelHandle));

static_assert(RML_VARIANT_TYPE_NONE == static_cast<int>(Rml::Variant::NONE));
static_assert(RML_VARIANT_TYPE_BOOL == static_cast<int>(Rml::Variant::BOOL));
static_assert(RML_VARIANT_TYPE_BYTE == static_cast<int>(Rml::Variant::BYTE));
static_assert(RML_VARIANT_TYPE_CHAR == static_cast<int>(Rml::Variant::CHAR));
static_assert(RML_VARIANT_TYPE_FLOAT == static_cast<int>(Rml::Variant::FLOAT));
static_assert(RML_VARIANT_TYPE_DOUBLE == static_cast<int>(Rml::Variant::DOUBLE));
static_assert(RML_VARIANT_TYPE_INT == static_cast<int>(Rml::Variant::INT));
static_assert(RML_VARIANT_TYPE_INT64 == static_cast<int>(Rml::Variant::INT64));
static_assert(RML_VARIANT_TYPE_UINT == static_cast<int>(Rml::Variant::UINT));
static_assert(RML_VARIANT_TYPE_UINT64 == static_cast<int>(Rml::Variant::UINT64));
static_assert(RML_VARIANT_TYPE_STRING == static_cast<int>(Rml::Variant::STRING));
static_assert(RML_VARIANT_TYPE_VECTOR2 == static_cast<int>(Rml::Variant::VECTOR2));
static_assert(RML_VARIANT_TYPE_VECTOR3 == static_cast<int>(Rml::Variant::VECTOR3));
static_assert(RML_VARIANT_TYPE_VECTOR4 == static_cast<int>(Rml::Variant::VECTOR4));
static_assert(RML_VARIANT_TYPE_COLOURF == static_cast<int>(Rml::Variant::COLOURF));
static_assert(RML_VARIANT_TYPE_COLOURB == static_cast<int>(Rml::Variant::COLOURB));
static_assert(RML_VARIANT_TYPE_SCRIPTINTERFACE == static_cast<int>(Rml::Variant::SCRIPTINTERFACE));
static_assert(RML_VARIANT_TYPE_TRANSFORMPTR == static_cast<int>(Rml::Variant::TRANSFORMPTR));
static_assert(RML_VARIANT_TYPE_TRANSITIONLIST == static_cast<int>(Rml::Variant::TRANSITIONLIST));
static_assert(RML_VARIANT_TYPE_ANIMATIONLIST == static_cast<int>(Rml::Variant::ANIMATIONLIST));
static_assert(RML_VARIANT_TYPE_DECORATORSPTR == static_cast<int>(Rml::Variant::DECORATORSPTR));
static_assert(RML_VARIANT_TYPE_FILTERSPTR == static_cast<int>(Rml::Variant::FILTERSPTR));
static_assert(RML_VARIANT_TYPE_FONTEFFECTSPTR == static_cast<int>(Rml::Variant::FONTEFFECTSPTR));
static_assert(RML_VARIANT_TYPE_COLORSTOPLIST == static_cast<int>(Rml::Variant::COLORSTOPLIST));
static_assert(RML_VARIANT_TYPE_BOXSHADOWLIST == static_cast<int>(Rml::Variant::BOXSHADOWLIST));
static_assert(RML_VARIANT_TYPE_VOIDPTR == static_cast<int>(Rml::Variant::VOIDPTR));

static_assert(RML_DATA_VARIABLE_TYPE_SCALAR == static_cast<int>(Rml::DataVariableType::Scalar));
static_assert(RML_DATA_VARIABLE_TYPE_ARRAY == static_cast<int>(Rml::DataVariableType::Array));
static_assert(RML_DATA_VARIABLE_TYPE_STRUCT == static_cast<int>(Rml::DataVariableType::Struct));

static_assert(RML_SHORTHAND_TYPE_FALL_THROUGH == static_cast<int>(Rml::ShorthandType::FallThrough));
static_assert(RML_SHORTHAND_TYPE_REPLICATE == static_cast<int>(Rml::ShorthandType::Replicate));
static_assert(RML_SHORTHAND_TYPE_BOX == static_cast<int>(Rml::ShorthandType::Box));
static_assert(RML_SHORTHAND_TYPE_RECURSIVE_REPEAT == static_cast<int>(Rml::ShorthandType::RecursiveRepeat));
static_assert(RML_SHORTHAND_TYPE_RECURSIVE_COMMA_SEPARATED == static_cast<int>(Rml::ShorthandType::RecursiveCommaSeparated));
static_assert(RML_SHORTHAND_TYPE_FLEX == static_cast<int>(Rml::ShorthandType::Flex));

static_assert(sizeof(RmlFontMetrics) == sizeof(Rml::FontMetrics));
static_assert(offsetof(RmlFontMetrics, size) == offsetof(Rml::FontMetrics, size));
static_assert(offsetof(RmlFontMetrics, has_ellipsis) == offsetof(Rml::FontMetrics, has_ellipsis));