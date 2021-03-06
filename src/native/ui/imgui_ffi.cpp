#include "pd_ui.h"
#include "pd_view.h"
#include <imgui.h>
#include <imgui_internal.h> //+Z
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bgfx/bgfx.h>

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ImageData {
    bgfx::TextureHandle tex;
    uint16_t width;
    uint16_t height;
    unsigned int size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FontTextureInfo {
    void* data;
    uint16_t width;
    uint16_t height;
    uint32_t bytes;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ImVec4 pdColorToImVec4(uint32_t color) {
    float r = ((color >> 24) & 0xff) * 1.0f / 255.0f;
    float g = ((color >> 16) & 0xff) * 1.0f / 255.0f;
    float b = ((color >> 8) & 0xff) * 1.0f / 255.0f;
    float a = ((color >> 0) & 0xff) * 1.0f / 255.0f;
    return ImVec4(r, g, b, a);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct PDSCFuncs {
    intptr_t (*send_command)(void* privData, unsigned int message, uintptr_t p0, intptr_t p1);
    void (*update)(void* privData);
    void (*draw)(void* privData);
    void* private_data;
} PDSCFuns;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static intptr_t scSendCommand(void* privData, unsigned int message, uintptr_t p0, intptr_t p1) {
    ImScEditor* editor = (ImScEditor*)privData;
    return editor->SendCommand(message, p0, p1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void scUpdate(void* privData) {
    ImScEditor* editor = (ImScEditor*)privData;
    editor->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void scDraw(void* privData) {
    ImScEditor* editor = (ImScEditor*)privData;
    return editor->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_title(void* private_data, const char* title) {
    /*
    PrivateData* data = (PrivateData*)private_data;

    (void)data;

    if (!strcmp(data->title, title))
        return;

    if (data->title)
        free((void*)data->title);

    data->title = strdup(title);
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_window_size() {
    ImVec2 size = ImGui::GetWindowSize();
    PDVec2 r = { size.x, size.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_window_pos() {
    ImVec2 pos = ImGui::GetWindowPos();
    PDVec2 r = { pos.x, pos.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void begin_child(const char* stringId, PDVec2 size, int border, PDUIInputTextFlags extraFlags) {
    ImGui::BeginChild(stringId, ImVec2(size.x, size.y), !!border, ImGuiWindowFlags(extraFlags));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_child() {
    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_scroll_y() {
    return ImGui::GetScrollY();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_scroll_max_y() {
    return ImGui::GetScrollMaxY();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_scroll_y(float scrollY) {
    ImGui::SetScrollY(scrollY);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_scroll_here(float centerYratio) {
    ImGui::SetScrollHere(centerYratio);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_scroll_from_pos_y(float posY, float centerYratio) {
    ImGui::SetScrollFromPosY(posY, centerYratio);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_keyboard_focus_here(int offset) {
    ImGui::SetKeyboardFocusHere(offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_font(PDUIFont font) {
    ImGui::PushFont((ImFont*)font);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_font() {
    ImGui::PopFont();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_style_color(PDUICol idx, PDColor col) {
    ImGui::PushStyleColor(idx, pdColorToImVec4(col));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_style_color(int count) {
    ImGui::PopStyleColor(count);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_style_var(PDUIStyleVar idx, float val) {
    ImGui::PushStyleVar(ImGuiStyleVar(idx), val);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_style_var_vec(PDUIStyleVar idx, PDVec2 val) {
    ImGui::PushStyleVar(ImGuiStyleVar(idx), ImVec2(val.x, val.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_style_var(int count) {
    ImGui::PopStyleVar(count);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_font_size() {
    return ImGui::GetFontSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_item_width(float itemWidth) {
    ImGui::PushItemWidth(itemWidth);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_item_width() {
    ImGui::PopItemWidth();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float calc_item_width() {
    return ImGui::CalcItemWidth();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_allow_keyboard_focus(int v) {
    ImGui::PushAllowKeyboardFocus(!!v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_allow_keyboard_focus() {
    ImGui::PopAllowKeyboardFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_text_wrap_pos(float wrapPosX) {
    ImGui::PushTextWrapPos(wrapPosX);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_text_wrap_pos() {
    ImGui::PopTextWrapPos();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_repeat(int repeat) {
    ImGui::PushButtonRepeat(!!repeat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_button_repeat() {
    ImGui::PopButtonRepeat();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void begin_group() {
    ImGui::BeginGroup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_group() {
    ImGui::EndGroup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void separator() {
    ImGui::Separator();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void same_line(int columnX, int spacingW) {
    ImGui::SameLine((float)columnX, (float)spacingW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void spacing() {
    ImGui::Spacing();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void dummy(PDVec2 size) {
    ImGui::Dummy(ImVec2(size.x, size.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void indent() {
    ImGui::Indent();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void un_indent() {
    ImGui::Unindent();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void columns(int count, const char* id, int border) {
    ImGui::Columns(count, id, !!border);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void next_column() {
    ImGui::NextColumn();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int get_column_index() {
    return ImGui::GetColumnIndex();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_column_offset(int columnIndex) {
    return ImGui::GetColumnOffset(columnIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_column_offset(int columnIndex, float offset) {
    return ImGui::SetColumnOffset(columnIndex, offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_column_width(int columnIndex) {
    return ImGui::GetColumnWidth(columnIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int get_columns_count() {
    return ImGui::GetColumnsCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_cursor_pos() {
    ImVec2 t = ImGui::GetCursorPos();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_cursor_pos_x() {
    return ImGui::GetCursorPosX();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_cursor_pos_y() {
    return ImGui::GetCursorPosY();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_cursor_pos(PDVec2 pos) {
    ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_cursor_pos_x(float x) {
    ImGui::SetCursorPosX(x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_cursor_pos_y(float y) {
    ImGui::SetCursorPosY(y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_cursor_screen_pos() {
    ImVec2 t = ImGui::GetCursorScreenPos();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_cursor_screen_pos(PDVec2 pos) {
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void align_first_text_height_to_widgets() {
    ImGui::AlignFirstTextHeightToWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_text_line_height() {
    return ImGui::GetTextLineHeight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_text_line_height_with_spacing() {
    return ImGui::GetTextLineHeightWithSpacing();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_items_line_height_with_spacing() {
    return ImGui::GetItemsLineHeightWithSpacing();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_id_str(const char* strId) {
    ImGui::PushID(strId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_id_str_range(const char* strBegin, const char* strEnd) {
    ImGui::PushID(strBegin, strEnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_id_ptr(const void* ptrId) {
    ImGui::PushID(ptrId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_id_int(const int intId) {
    ImGui::PushID(intId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pop_id() {
    ImGui::PopID();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDID get_id_str(const char* strId) {
    return (PDID)ImGui::GetID(strId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDID get_id_str_range(const char* strBegin, const char* strEnd) {
    return (PDID)ImGui::GetID(strBegin, strEnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDID get_id_ptr(const void* ptrId) {
    return (PDID)ImGui::GetID(ptrId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text(const char* text) {
    ImGui::Text("%s", text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_format(const char* format, ...) {
    va_list ap;
    va_start(ap, format);

    ImGui::TextV(format, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_v(const char* fmt, va_list args) {
    ImGui::TextV(fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_colored(const PDColor col, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::TextColoredV(pdColorToImVec4(col), fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_colored_v(const PDColor col, const char* fmt, va_list args) {
    ImGui::TextColoredV(pdColorToImVec4(col), fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_disabled(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::TextDisabledV(fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_disabledV(const char* fmt, va_list args) {
    ImGui::TextDisabledV(fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_wrapped(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::TextWrappedV(fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_wrapped_v(const char* fmt, va_list args) {
    ImGui::TextWrappedV(fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void text_unformatted(const char* text, const char* text_end) {
    ImGui::TextUnformatted(text, text_end);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_text(const char* label, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::LabelTextV(label, fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_textV(const char* label, const char* fmt, va_list args) {
    ImGui::LabelTextV(label, fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bullet() {
    ImGui::Bullet();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bullet_text(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::BulletTextV(fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bullet_text_v(const char* fmt, va_list args) {
    ImGui::BulletTextV(fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int button(const char* label, const PDVec2 size) {
    return ImGui::Button(label, ImVec2(size.x, size.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int small_button(const char* label) {
    return ImGui::SmallButton(label) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int invisible_button(const char* strId, const PDVec2 size) {
    return ImGui::InvisibleButton(strId, ImVec2(size.x, size.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void image(void* image_data, const PDVec2 size, const PDVec2 uv0, const PDVec2 uv1, const PDColor tintColor, const PDColor borderColor) {
    ImageData* data = (ImageData*)image_data;
    union { void* ptr; bgfx::TextureHandle handle; } texture;
    texture.handle = data->tex;

    ImGui::Image((ImTextureID)texture.ptr, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y),
                 pdColorToImVec4(tintColor), pdColorToImVec4(borderColor));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int image_button(PDUITextureID user_texture_id, const PDVec2 size, const PDVec2 uv0, const PDVec2 uv1, int framePadding, const PDColor bgColor, const PDColor tintCol) {
    return ImGui::ImageButton(user_texture_id, ImVec2(size.x, size.y), ImVec2(uv0.x, uv1.y), ImVec2(uv1.x, uv1.y), framePadding, pdColorToImVec4(bgColor), pdColorToImVec4(tintCol)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int collapsing_header(const char* label, const char* strId, int displayFrame, int defaultOpen) {
    return ImGui::CollapsingHeader(label, strId, !!displayFrame, !!defaultOpen) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int checkbox(const char* label, int* v) {
    return ImGui::Checkbox(label, (bool*)v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int checkbox_flags(const char* label, unsigned int* flags, unsigned int flagsValue) {
    return ImGui::CheckboxFlags(label, flags, flagsValue) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int radio_button_bool(const char* label, int active) {
    return ImGui::RadioButton(label, !!active) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int radio_button(const char* label, int* v, int v_button) {
    return ImGui::RadioButton(label, v, v_button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int combo(const char* label, int* currentItem, const char** items, int itemsCount, int heightInItems) {
    return ImGui::Combo(label, currentItem, items, itemsCount, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int combo2(const char* label, int* currentItem, const char* itemsSeparatedByZeros, int heightInItems) {
    return ImGui::Combo(label, currentItem, itemsSeparatedByZeros, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// note: function pointer returns bool - is this potentially a problem in the future?
static int combo3(const char* label, int* currentItem, bool (*itemsGetter)(void* data, int idx, const char** out_text), void* data, int itemsCount, int heightInItems) {
    return ImGui::Combo(label, currentItem, itemsGetter, data, itemsCount, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int color_button(const PDColor col, int smallHeight, int outlineBorder) {
    return ImGui::ColorButton(pdColorToImVec4(col), !!smallHeight, !!outlineBorder) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int color_edit3(const char* label, float col[3]) {
    return ImGui::ColorEdit3(label, col) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int color_edit4(const char* label, float col[4], int showAlpha) {
    return ImGui::ColorEdit4(label, col, !!showAlpha) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void color_edit_mode(PDUIColorEditMode mode) {
    ImGui::ColorEditMode(mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plot_lines(const char* label, const float* values, int valuesCount, int valuesOffset, const char* overlayText, float scaleMin, float scaleMax, PDVec2 graphSize, size_t stride) {
    ImGui::PlotLines(label, values, valuesCount, valuesOffset, overlayText, scaleMin, scaleMax, ImVec2(graphSize.x, graphSize.y), (int)stride);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plot_lines2(const char* label, float (*valuesGetter)(void* data, int idx), void* data, int valuesCount, int valuesOffset, const char* overlayText, float scaleMin, float scaleMax, PDVec2 graphSize) {
    ImGui::PlotLines(label, valuesGetter, data, valuesCount, valuesOffset, overlayText, scaleMin, scaleMax, ImVec2(graphSize.x, graphSize.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plot_histogram(const char* label, const float* values, int valuesCount, int valuesOffset, const char* overlayText, float scaleMin, float scaleMax, PDVec2 graphSize, size_t stride) {
    ImGui::PlotHistogram(label, values, valuesCount, valuesOffset, overlayText, scaleMin, scaleMax, ImVec2(graphSize.x, graphSize.y), (int)stride);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plot_histogram2(const char* label, float (*valuesGetter)(void* data, int idx), void* data, int valuesCount, int valuesOffset, const char* overlayText, float scaleMin, float scaleMax, PDVec2 graphSize) {
    ImGui::PlotHistogram(label, valuesGetter, data, valuesCount, valuesOffset, overlayText, scaleMin, scaleMax, ImVec2(graphSize.x, graphSize.y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDUISCInterface* sc_input_text(const char* label, float xSize, float ySize) {
    ImScEditor* ed = ImGui::ScInputText(label, xSize, ySize, 0, 0);

    if (!ed->userData) {
        PDUISCInterface* funcs = (PDUISCInterface*)malloc(sizeof(PDUISCInterface));
        funcs->send_command = scSendCommand;
        funcs->update = scUpdate;
        funcs->draw = scDraw;
        funcs->private_data = ed;

        ed->userData = (void*)funcs;
    }

    return (PDUISCInterface*)ed->userData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_float(const char* label, float* v, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::SliderFloat(label, v, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_float2(const char* label, float v[2], float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::SliderFloat2(label, v, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_float3(const char* label, float v[3], float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::SliderFloat3(label, v, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_float4(const char* label, float v[4], float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::SliderFloat4(label, v, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_angle(const char* label, float* v_rad, float vDegreesMin, float vDegreesMax) {
    return ImGui::SliderAngle(label, v_rad, vDegreesMin, vDegreesMax) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_int(const char* label, int* v, int vMin, int vMax, const char* displayFormat) {
    return ImGui::SliderInt(label, v, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_int2(const char* label, int v[2], int vMin, int vMax, const char* displayFormat) {
    return ImGui::SliderInt2(label, v, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_int3(const char* label, int v[3], int vMin, int vMax, const char* displayFormat) {
    return ImGui::SliderInt3(label, v, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int slider_int4(const char* label, int v[4], int vMin, int vMax, const char* displayFormat) {
    return ImGui::SliderInt4(label, v, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vslider_float(const char* label, const PDVec2 size, float* v, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::VSliderFloat(label, ImVec2(size.x, size.y), v, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vslider_int(const char* label, const PDVec2 size, int* v, int vMin, int vMax, const char* displayFormat) {
    return ImGui::VSliderInt(label, ImVec2(size.x, size.y), v, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_float(const char* label, float* v, float vSpeed, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::DragFloat(label, v, vSpeed, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_float2(const char* label, float v[2], float vSpeed, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::DragFloat2(label, v, vSpeed, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_float3(const char* label, float v[3], float vSpeed, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::DragFloat3(label, v, vSpeed, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_float4(const char* label, float v[4], float vSpeed, float vMin, float vMax, const char* displayFormat, float power) {
    return ImGui::DragFloat4(label, v, vSpeed, vMin, vMax, displayFormat, power) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_int(const char* label, int* v, float vSpeed, int vMin, int vMax, const char* displayFormat) {
    return ImGui::DragInt(label, v, vSpeed, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_int2(const char* label, int v[2], float vSpeed, int vMin, int vMax, const char* displayFormat) {
    return ImGui::DragInt2(label, v, vSpeed, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_int3(const char* label, int v[3], float vSpeed, int vMin, int vMax, const char* displayFormat) {
    return ImGui::DragInt3(label, v, vSpeed, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int drag_int4(const char* label, int v[4], float vSpeed, int vMin, int vMax, const char* displayFormat) {
    return ImGui::DragInt4(label, v, vSpeed, vMin, vMax, displayFormat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*InputCallback)(PDUIInputTextCallbackData*);

struct PDInputTextUserData {
    InputCallback callback;
    void* user_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void input_textDeleteChars(PDUIInputTextCallbackData* data, int pos, int byteCount) {
    char* dst = data->buf + pos;
    const char* src = data->buf + pos + byteCount;
    while (char c = *src++)
        *dst++ = c;
    *dst = '\0';

    data->buf_dirty = true;
    if (data->cursor_pos + byteCount >= pos)
        data->cursor_pos -= byteCount;
    else if (data->cursor_pos >= pos)
        data->cursor_pos = pos;
    data->selection_start = data->selection_end = data->cursor_pos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void input_textInsertChars(PDUIInputTextCallbackData* data, int pos, const char* text, const char* textEnd = NULL) {
    const int textLen = int(strlen(data->buf));
    if (!textEnd)
        textEnd = text + strlen(text);

    const int newTextLen = (int)(textEnd - text);

    if (newTextLen + textLen + 1 >= data->buf_size)
        return;

    size_t upos = (size_t)pos;
    if ((size_t)textLen != upos)
        memmove(data->buf + upos + newTextLen, data->buf + upos, (size_t)textLen - upos);
    memcpy(data->buf + upos, text, (size_t)newTextLen * sizeof(char));
    data->buf[textLen + newTextLen] = '\0';

    data->buf_dirty = true;
    if (data->cursor_pos >= pos)
        data->cursor_pos += (int)newTextLen;
    data->selection_start = data->selection_end = data->cursor_pos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int textEditCallbackStub(ImGuiTextEditCallbackData* data) {
    PDInputTextUserData* wrappedUserData = (PDInputTextUserData*)data->UserData;
    PDUIInputTextCallbackData callbackData = { 0 };

    // Transfer over ImGui callback data into our generic wrapper version
    callbackData.user_data = wrappedUserData->user_data;
    callbackData.buf = data->Buf;
    callbackData.buf_size = int(data->BufSize);
    callbackData.buf_dirty = data->BufDirty;
    callbackData.event_char = data->EventChar;
    callbackData.event_flag = data->EventFlag;
    callbackData.flags = PDUIInputTextFlags(data->Flags);
    callbackData.cursor_pos = data->CursorPos;
    callbackData.selection_start = data->SelectionStart;
    callbackData.selection_end  = data->SelectionEnd;
    callbackData.delete_chars = input_textDeleteChars;
    callbackData.insert_chars = input_textInsertChars;

    // Translate ImGui event key into our own PDKey mapping
    ImGuiIO& io = ImGui::GetIO();
    callbackData.event_key = io.KeyMap[data->EventKey];

    // Invoke the callback (synchronous)
    wrappedUserData->callback(&callbackData);

    // We need to mirror any changes to the callback wrapper into the actual ImGui version
    data->UserData = callbackData.user_data;
    data->Buf = callbackData.buf;
    data->BufSize = (int)callbackData.buf_size;
    data->BufDirty = callbackData.buf_dirty;
    data->EventChar = callbackData.event_char;
    data->Flags = ImGuiInputTextFlags(callbackData.flags);
    data->CursorPos = callbackData.cursor_pos;
    data->SelectionStart = callbackData.selection_start;
    data->SelectionEnd   = callbackData.selection_end;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_text(const char* label, char* buf, int buf_size, PDUIInputTextFlags flags, void (*callback)(PDUIInputTextCallbackData*), void* user_data) {
    PDInputTextUserData wrappedUserData;
    wrappedUserData.callback = callback;
    wrappedUserData.user_data = user_data;
    return ImGui::InputText(label, buf, (size_t)buf_size, ImGuiInputTextFlags(flags), &textEditCallbackStub, &wrappedUserData) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_text_multiline(const char* label, char* buf, size_t buf_size, const PDVec2 size, PDUIInputTextFlags flags, void (*callback)(PDUIInputTextCallbackData*), void* user_data) {
    PDInputTextUserData wrappedUserData;
    wrappedUserData.callback = callback;
    wrappedUserData.user_data = user_data;
    return ImGui::InputTextMultiline(label, buf, buf_size, ImVec2(size.x, size.y), flags, &textEditCallbackStub, &wrappedUserData) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_float(const char* label, float* v, float step, float step_fast, int decimal_precision, PDUIInputTextFlags extraFlags) {
    return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_float2(const char* label, float v[2], int decimal_precision, PDUIInputTextFlags extraFlags) {
    return ImGui::InputFloat2(label, v, decimal_precision, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_float3(const char* label, float v[3], int decimal_precision, PDUIInputTextFlags extraFlags) {
    return ImGui::InputFloat3(label, v, decimal_precision, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_float4(const char* label, float v[4], int decimal_precision, PDUIInputTextFlags extraFlags) {
    return ImGui::InputFloat4(label, v, decimal_precision, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_int(const char* label, int* v, int step, int step_fast, PDUIInputTextFlags extraFlags) {
    return ImGui::InputInt(label, v, step, step_fast, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_int2(const char* label, int v[2], PDUIInputTextFlags extraFlags) {
    return ImGui::InputInt2(label, v, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_int3(const char* label, int v[3], PDUIInputTextFlags extraFlags) {
    return ImGui::InputInt3(label, v, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int input_int4(const char* label, int v[4], PDUIInputTextFlags extraFlags) {
    return ImGui::InputInt4(label, v, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tree_node(const char* str_label_id) {
    return ImGui::TreeNode(str_label_id) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tree_node_str(const char* strId, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    bool ret = ImGui::TreeNodeV(strId, fmt, ap);

    va_end(ap);

    return ret ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tree_node_ptr(const void* ptrId, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    bool ret = ImGui::TreeNodeV(ptrId, fmt, ap);

    va_end(ap);

    return ret ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tree_node_str_v(const char* strId, const char* fmt, va_list args) {
    return ImGui::TreeNodeV(strId, fmt, args) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tree_node_ptr_v(const void* ptrId, const char* fmt, va_list args) {
    return ImGui::TreeNodeV(ptrId, fmt, args) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tree_push_str(const char* strId) {
    ImGui::TreePush(strId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tree_push_ptr(const void* ptrId) {
    ImGui::TreePush(ptrId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tree_pop() {
    ImGui::TreePop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_next_tree_node_opened(int opened, PDUISetCond cond) {
    ImGui::SetNextTreeNodeOpen(!!opened, cond);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int selectable(const char* label, int selected, PDUISelectableFlags flags, const PDVec2 size) {
    return ImGui::Selectable(label, !!selected, flags, ImVec2(size.x, size.y)) ? 1 : 0;
}

//+Z
extern "C" bool imgui_tab(const char* label, bool selected, bool last) {
    bool res = ImGui::Selectable(label, selected, 0, ImGui::CalcTextSize(label));
    if (!last) {
        ImGui::SameLine(); ImGui::Text("|"); ImGui::SameLine();
    }
    else {
        //ImGui::Separator();
    }
    return res;
}

extern "C" void imgui_separator() {
    ImGui::Separator();
}

extern "C" float imgui_tab_pos() {
    return ImGui::GetItemRectMax().x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int selectable_ex(const char* label, int* p_selected, PDUISelectableFlags flags, const PDVec2 size) {
  return ImGui::Selectable(label, (bool*)p_selected, flags, ImVec2(size.x, size.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int list_box(const char* label, int* currentItem, const char** items, int itemsCount, int heightInItems) {
    return ImGui::ListBox(label, currentItem, items, itemsCount, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// note: function pointer returns bool - is this potentially a problem in the future?
static int list_box2(const char* label, int* currentItem, bool (*itemsGetter)(void* data, int idx, const char** out_text), void* data, int itemsCount, int heightInItems) {
    return ImGui::ListBox(label, currentItem, itemsGetter, data, itemsCount, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int list_box_header(const char* label, const PDVec2 size) {
    return ImGui::ListBoxHeader(label, ImVec2(size.x, size.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int list_box_header2(const char* label, int itemsCount, int heightInItems) {
    return ImGui::ListBoxHeader(label, itemsCount, heightInItems) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_box_footer() {
    ImGui::ListBoxFooter();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_tooltip(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ImGui::SetTooltipV(fmt, ap);

    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_tooltipV(const char* fmt, va_list args) {
    ImGui::SetTooltipV(fmt, args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void begin_tooltip() {
    ImGui::BeginTooltip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_tooltip() {
    ImGui::EndTooltip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_main_menu_bar() {
    return ImGui::BeginMainMenuBar() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_main_menu_bar() {
    ImGui::EndMainMenuBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_menu_bar() {
    return ImGui::BeginMenuBar() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_menu_bar() {
    ImGui::EndMenuBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_menu(const char* label, int enabled) {
    return ImGui::BeginMenu(label, !!enabled) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_menu() {
    ImGui::EndMenu();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int menu_item(const char* label, const char* shortcut, int selected, int enabled) {
    return ImGui::MenuItem(label, shortcut, !!selected, !!enabled) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int menu_item_ptr(const char* label, const char* shortcut, int* p_selected, int enabled) {
  return ImGui::MenuItem(label, shortcut, (bool*)p_selected, !!enabled) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void open_popup(const char* strId) {
    ImGui::OpenPopup(strId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_popup(const char* strId) {
    return ImGui::BeginPopup(strId) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_popup_modal(const char* name, int* p_opened, PDUIWindowFlags extraFlags) {
  return ImGui::BeginPopupModal(name, (bool*)p_opened, extraFlags) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_popup_context_item(const char* strId, int mouse_button) {
    return ImGui::BeginPopupContextItem(strId, mouse_button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_popup_context_window(int also_over_items, const char* strId, int mouse_button) {
    return ImGui::BeginPopupContextWindow(!!also_over_items, strId, mouse_button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_popup_context_void(const char* strId, int mouse_button) {
    return ImGui::BeginPopupContextVoid(strId, mouse_button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_popup() {
    ImGui::EndPopup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void close_current_popup() {
    ImGui::CloseCurrentPopup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int begin_popup_context(void* priv_data) {
    PrivateData* data = (PrivateData*)priv_data;

    if (data->show_popup) {
        ImGui::OpenPopup("_select");
        data->showed_popup = true;
    }

    bool showed_menu = ImGui::BeginPopup("_select");

    if (showed_menu)
    {
        ImGui::Text("Inside");
        ImGui::MenuItem("Click me");
    }

    return showed_menu ? 1 : 0;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_popup_context(void* priv_data) {
    (void)priv_data;
    ImGui::EndPopup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void value_bool(const char* prefix, int b) {
    ImGui::Value(prefix, !!b);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void value_int(const char* prefix, int v) {
    ImGui::Value(prefix, v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void value_u_int(const char* prefix, unsigned int v) {
    ImGui::Value(prefix, v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void value_float(const char* prefix, float v, const char* float_format) {
    ImGui::Value(prefix, v, float_format);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void color(const char* prefix, const PDColor col) {
    ImGui::ValueColor(prefix, pdColorToImVec4(col));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void log_to_tty(int maxDepth) {
    ImGui::LogToTTY(maxDepth);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void log_to_file(int maxDepth, const char* filename) {
    ImGui::LogToFile(maxDepth, filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void log_to_clipboard(int maxDepth) {
    ImGui::LogToClipboard(maxDepth);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void log_finish() {
    ImGui::LogFinish();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void log_buttons() {
    ImGui::LogButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_item_hovered() {
    return ImGui::IsItemHovered() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_item_hovered_rect() {
    return ImGui::IsItemHoveredRect() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_item_active() {
    return ImGui::IsItemActive() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_item_visible() {
    return ImGui::IsItemVisible() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_any_item_hovered() {
    return ImGui::IsAnyItemHovered() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_any_item_active() {
    return ImGui::IsAnyItemActive() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_item_rect_min() {
    ImVec2 t = ImGui::GetItemRectMin();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_item_rect_max() {
    ImVec2 t = ImGui::GetItemRectMax();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_item_rect_size() {
    ImVec2 t = ImGui::GetItemRectSize();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_window_hovered() {
    return ImGui::IsWindowHovered() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_window_focused() {
    return ImGui::IsWindowFocused() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_root_window_focused() {
    return ImGui::IsRootWindowFocused() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_root_window_or_any_child_focused() {
    return ImGui::IsRootWindowOrAnyChildFocused() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_rect_visible(const PDVec2 itemSize) {
    return ImGui::IsRectVisible(ImVec2(itemSize.x, itemSize.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_pos_hovering_any_window(const PDVec2 pos) {
    return ImGui::IsPosHoveringAnyWindow(ImVec2(pos.x, pos.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_time() {
    return ImGui::GetTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int get_frame_count() {
    return ImGui::GetFrameCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* get_style_col_name(PDUICol idx) {
    return ImGui::GetStyleColName(ImGuiCol(idx));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 calc_item_rect_closest_point(const PDVec2 pos, int onEdge, float outward) {
    ImVec2 t = ImGui::CalcItemRectClosestPoint(ImVec2(pos.x, pos.y), !!onEdge, outward);
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 calc_text_size(const char* text, const char* text_end, int hide_text_after_double_hash, float wrap_width) {
    ImVec2 t = ImGui::CalcTextSize(text, text_end, !!hide_text_after_double_hash, wrap_width);
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void calc_list_clipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) {
    ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int begin_child_frame(PDID id, const struct PDVec2 size) {
    return ImGui::BeginChildFrame(id, ImVec2(size.x, size.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void end_child_frame() {
    ImGui::EndChildFrame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void color_convert_rg_bto_hsv(float r, float g, float b, float* out_h, float* out_s, float* out_v) {
    ImGui::ColorConvertRGBtoHSV(r, g, b, *out_h, *out_s, *out_v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void color_convert_hs_vto_rgb(float h, float s, float v, float* out_r, float* out_g, float* out_b) {
    ImGui::ColorConvertHSVtoRGB(h, s, v, *out_r, *out_g, *out_b);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_key_down(int key_index) {
    return ImGui::IsKeyDown(key_index) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_key_pressed(int key_index, int repeat) {
    return ImGui::IsKeyPressed(key_index, !!repeat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_key_released(int key_index) {
    return ImGui::IsKeyReleased(key_index) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Fix me

static int is_key_down_id(uint32_t keyId, int repeat) {
    if (!ImGui::IsWindowFocused())
        return 0;

    return 0; //!!InputState_isKeyDown(keyId >> 4, keyId & 0xf, repeat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_down(int button) {
    return ImGui::IsMouseDown(button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_clicked(int button, int repeat) {
    return ImGui::IsMouseClicked(button, !!repeat) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_double_clicked(int button) {
    return ImGui::IsMouseDoubleClicked(button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_released(int button) {
    return ImGui::IsMouseReleased(button) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_hovering_window() {
    return ImGui::IsMouseHoveringWindow() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_hovering_any_window() {
    return ImGui::IsMouseHoveringAnyWindow() ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_hovering_rect(const struct PDVec2 rectMin, const struct PDVec2 rectMax) {
    return ImGui::IsMouseHoveringRect(ImVec2(rectMin.x, rectMin.y), ImVec2(rectMax.x, rectMax.y)) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int is_mouse_dragging(int button, float lockThreshold) {
    return ImGui::IsMouseDragging(button, lockThreshold) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_mouse_pos() {
    ImVec2 t = ImGui::GetMousePos();
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDVec2 get_mouse_drag_delta(int button, float lockThreshold) {
    ImVec2 t = ImGui::GetMouseDragDelta(button, lockThreshold);
    PDVec2 r = { t.x, t.y };
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void reset_mouse_drag_delta(int button) {
    ImGui::ResetMouseDragDelta(button);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDUIMouseCursor get_mouse_cursor() {
    return (PDUIMouseCursor)ImGui::GetMouseCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_mouse_cursor(PDUIMouseCursor type) {
    ImGui::SetMouseCursor(type);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float get_mouse_wheel() {
    return ImGui::GetIO().MouseWheel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void fill_rect(PDRect rect, PDColor color) {
    ImGui::FillRect(ImVec2(rect.x, rect.y), ImVec2(rect.width, rect.height), color);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void fill_convex_poly(void* vertices, unsigned int count, PDColor color, int anti_aliased) {
    ImGui::ConvexPolyFilled(vertices, count, color, !!anti_aliased);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void fill_circle(PDVec2 pos, float radius, PDColor color, unsigned int num_seg, int aa) {
    ImGui::CircleFilled(ImVec2(pos.x, pos.y), radius, color, num_seg, !!aa);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" int imgui_begin_with_flags(const char* name, int show, int flags) {
    bool s = !!show;
    ImGui::Begin(name, &s, ImVec2(0, 0), true, flags);
    return s ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" int imgui_begin(const char* name, int show) {
    return imgui_begin_with_flags(name, show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" int imgui_begin_float(const char* name, int show) {
    return imgui_begin_with_flags(name, show, ImGuiWindowFlags_NoCollapse);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_begin_child(const char* name, float h) {
    ImGui::BeginChild(name, ImVec2(0, h), false, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_end_child() {
    ImGui::EndChild();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void* image_create_rgba(unsigned int width, unsigned int height) {
    bgfx::TextureHandle tex = bgfx::createTexture2D(
            (uint16_t)width,
            (uint16_t)height,
            1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,
            0);

    if (!isValid(tex)) {
        return 0;
    }

    ImageData* data = (ImageData*)malloc(sizeof(ImageData));

    data->tex = tex;
    data->size = width * height * 4;
    data->width = (uint16_t)width;
    data->height = (uint16_t)height;

    const bgfx::Memory* mem = bgfx::alloc((uint32_t)data->size);
    memset(mem->data, 0, data->size);

    // Update with cleared data section to not have "empty" texture

    bgfx::updateTexture2D(tex, 0, 0, 0, (uint16_t)width, (uint16_t)height, mem);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void image_update(void* imageData, const void* src, unsigned int size) {
    ImageData* data = (ImageData*)imageData;

    // clamp size if it's being incorrect
    if (size > data->size)
        size = data->size;

    const bgfx::Memory* mem = bgfx::alloc((uint32_t)data->size);
    memcpy(mem->data, src, size);

    bgfx::updateTexture2D(data->tex, 0, 0, 0, data->width, data->height, mem);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_end() {
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static PDUI s_uiFuncs[] =
{
    0,

    // Windows

    set_title,
    get_window_size,
    get_window_pos,
    begin_child,
    end_child,

    get_scroll_y,
    get_scroll_max_y,
    set_scroll_y,
    set_scroll_here,
    set_scroll_from_pos_y,
    set_keyboard_focus_here,

    // Parameters stacks (shared)

    push_font,
    pop_font,
    push_style_color,
    pop_style_color,
    push_style_var,
    push_style_var_vec,
    pop_style_var,
    get_font_size,

    // Parameters stacks (current window)

    push_item_width,
    pop_item_width,
    calc_item_width,
    push_allow_keyboard_focus,
    pop_allow_keyboard_focus,
    push_text_wrap_pos,
    pop_text_wrap_pos,
    push_button_repeat,
    pop_button_repeat,

    // Layout

    begin_group,
    end_group,
    separator,
    same_line,
    spacing,
    dummy,
    indent,
    un_indent,
    columns,
    next_column,
    get_column_index,
    get_column_offset,
    set_column_offset,
    get_column_width,
    get_columns_count,
    get_cursor_pos,
    get_cursor_pos_x,
    get_cursor_pos_y,
    set_cursor_pos,
    set_cursor_pos_x,
    set_cursor_pos_y,
    get_cursor_screen_pos,
    set_cursor_screen_pos,
    align_first_text_height_to_widgets,
    get_text_line_height,
    get_text_line_height_with_spacing,
    get_items_line_height_with_spacing,

    // ID scopes
    // If you are creating widgets in a loop you most likely want to push a unique identifier so PDUI can differentiate them
    // You can also use "##extra" within your widget name to distinguish them from each others (see 'Programmer Guide')

    push_id_str,
    push_id_str_range,
    push_id_ptr,
    push_id_int,
    pop_id,
    get_id_str,
    get_id_str_range,
    get_id_ptr,

    // Widgets

    text,
    text_format,
    text_v,
    text_colored,
    text_colored_v,
    text_disabled,
    text_disabledV,
    text_wrapped,
    text_wrapped_v,
    text_unformatted,
    label_text,
    label_textV,
    bullet,
    bullet_text,
    bullet_text_v,
    button,
    small_button,
    invisible_button,
    image,
    image_button,
    collapsing_header,
    checkbox,
    checkbox_flags,
    radio_button_bool,
    radio_button,
    combo,
    combo2,
    combo3,
    color_button,
    color_edit3,
    color_edit4,
    color_edit_mode,
    plot_lines,
    plot_lines2,
    plot_histogram,
    plot_histogram2,

    // Widgets: Scintilla text interface
    sc_input_text,

    // Widgets: Sliders (tip: ctrl+click on a slider to input text)
    slider_float,
    slider_float2,
    slider_float3,
    slider_float4,
    slider_angle,
    slider_int,
    slider_int2,
    slider_int3,
    slider_int4,
    vslider_float,
    vslider_int,

    // Widgets: Drags (tip: ctrl+click on a drag box to input text)
    drag_float,
    drag_float2,
    drag_float3,
    drag_float4,
    drag_int,
    drag_int2,
    drag_int3,
    drag_int4,

    // Widgets: Input
    input_text,
    input_text_multiline,
    input_float,
    input_float2,
    input_float3,
    input_float4,
    input_int,
    input_int2,
    input_int3,
    input_int4,

    // Widgets: Trees
    tree_node,
    tree_node_str,
    tree_node_ptr,
    tree_node_str_v,
    tree_node_ptr_v,
    tree_push_str,
    tree_push_ptr,
    tree_pop,
    set_next_tree_node_opened,

    // Widgets: Selectable / Lists
    selectable,
    selectable_ex,
    list_box,
    list_box2,
    list_box_header,
    list_box_header2,
    list_box_footer,

    // Tooltip
    set_tooltip,
    set_tooltipV,
    begin_tooltip,
    end_tooltip,

    // Widgets: Menus
    begin_main_menu_bar,
    end_main_menu_bar,
    begin_menu_bar,
    end_menu_bar,
    begin_menu,
    end_menu,
    menu_item,
    menu_item_ptr,

    // Popup
    open_popup,
    begin_popup,
    begin_popup_modal,
    begin_popup_context_item,
    begin_popup_context_window,
    begin_popup_context_void,
    end_popup,
    close_current_popup,
    // Set by Rust
    0, // begin_popup_context,
    end_popup_context,

    // Widgets: value() Helpers. Output single value in "name: value" format
    value_bool,
    value_int,
    value_u_int,
    value_float,
    color,

    // Logging: all text output from interface is redirected to tty/file/clipboard. Tree nodes are automatically opened.
    log_to_tty,
    log_to_file,
    log_to_clipboard,
    log_finish,
    log_buttons,
    //logText,

    // Utilities
    is_item_hovered,
    is_item_hovered_rect,
    is_item_active,
    is_item_visible,
    is_any_item_hovered,
    is_any_item_active,
    get_item_rect_min,
    get_item_rect_max,
    get_item_rect_size,
    is_window_hovered,
    is_window_focused,
    is_root_window_focused,
    is_root_window_or_any_child_focused,
    is_rect_visible,
    is_pos_hovering_any_window,
    get_time,
    get_frame_count,
    get_style_col_name,
    calc_item_rect_closest_point,
    calc_text_size,
    calc_list_clipping,

    begin_child_frame,
    end_child_frame,

    color_convert_rg_bto_hsv,
    color_convert_hs_vto_rgb,
    is_key_down,
    is_key_pressed,
    is_key_released,

    is_key_down_id,
    is_mouse_down,
    is_mouse_clicked,
    is_mouse_double_clicked,
    is_mouse_released,
    is_mouse_hovering_window,
    is_mouse_hovering_any_window,
    is_mouse_hovering_rect,
    is_mouse_dragging,
    get_mouse_pos,
    get_mouse_drag_delta,
    reset_mouse_drag_delta,
    get_mouse_cursor,
    set_mouse_cursor,
    get_mouse_wheel,

    // Rendering

    fill_rect,
    fill_convex_poly,
    fill_circle,

    // Image

     // Image support

    image_create_rgba,
    image_update,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void* imgui_create_ui_funcs() {
    // TODO: This is leaking, store pointers in a list and free on exit?
    PDUI* ui = (PDUI*)malloc(sizeof(PDUI));
    *ui = *s_uiFuncs;

    // set by the Rust code
    ui->private_data = 0;

    return ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void* imgui_get_ui_funcs() {
    return s_uiFuncs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_window_pos(float x, float y) {
    ImGui::SetNextWindowPos(ImVec2(x, y));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_window_size(float w, float h) {
    ImGui::SetNextWindowSize(ImVec2(w, h));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_render_frame(float x, float y, float width, float height, int fill_col) {
    ImGui::RenderFrame(ImVec2(x, y), ImVec2(x + width,y + height), fill_col, false, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_update_size(uint16_t width, uint16_t height) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DeltaTime = 1.0f / 60.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_pre_update(float deltaTime) {
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaTime;
    ImGui::NewFrame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_mouse_state(int index, int state) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[index] = !!state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_mouse_pos(float x, float y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(x, y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_scroll(float scroll) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = scroll;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_clear_keys() {
    ImGuiIO& io = ImGui::GetIO();
    int len = (int)sizeof_array(io.KeysDown);
    for (int i = 0; i < len; ++i) {
        io.KeysDown[i] = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_key_down(int key, int modifier) {
    ImGuiIO& io = ImGui::GetIO();
    assert(key >= 0 && key <= (int)sizeof_array(io.KeysDown));
    io.KeysDown[key] = true;
    //io.KeyCtrl = !!(modifier & PDKEY_CTRL);
    //io.KeyShift = !!(modifier & PDKEY_SHIFT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_set_key_up(int key, int modifier) {
    ImGuiIO& io = ImGui::GetIO();
    assert(key >= 0 && key <= (int)sizeof_array(io.KeysDown));
    io.KeysDown[key] = false;
    //io.KeyCtrl = !!(modifier & PDKEY_CTRL);
    //io.KeyShift = !!(modifier & PDKEY_SHIFT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_add_input_character(unsigned short c) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_map_key(int key_target, int key_source) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[key_target] = key_source;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_post_update() {
    ImGui::Render();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void imgui_setup(const char* font, float font_size, int width, int height) {
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DeltaTime = 1.0f / 60.0f;

    // TODO: Add this as config?
    // Update the style

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.50f, 0.50f, 0.50f, 0.45f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(0.60f, 0.60f, 0.60f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.60f);
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = style.Colors[ImGuiCol_TitleBg];
    style.WindowPadding = ImVec2(4, 0);

    style.WindowRounding = 0.0f;

    if (font) {
         io.Fonts->AddFontFromFileTTF(font, font_size);
    }

    io.RenderDrawListsFn = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" FontTextureInfo imgui_get_font_tex_data() {
    unsigned char* font_data;
    int width, height, bytes;
    ImGuiIO& io = ImGui::GetIO();
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_data, &width, &height, &bytes);
    printf("C: font_data %p\n", font_data);
    FontTextureInfo data = { font_data, (uint16_t)width, (uint16_t)height, (uint32_t)bytes };
    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void* imgui_get_draw_data() {
    return (void*)ImGui::GetDrawData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0

    /*
    UIRender_init();

    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&fontData, &fWidth, &fHeight, &outBytes);

    const bgfx::Memory* mem = bgfx::alloc((uint32_t)(fWidth * fHeight * outBytes));
    memcpy(mem->data, fontData, size_t(fWidth * fHeight * outBytes));

    s_textureId = bgfx::createTexture2D((uint16_t)fWidth, (uint16_t)fHeight, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE, mem);
    */


static void imgui_render(ImDrawData* draw_data) {
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;

    float ortho[16];
    bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, -1.0f, 1.0f);

    bgfx::setViewTransform(0, NULL, ortho);

    // Render command lists
    for (int32_t ii = 0; ii < draw_data->CmdListsCount; ++ii) {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        const ImDrawList* cmd_list = draw_data->CmdLists[ii];
        uint32_t vtx_size = (uint32_t)cmd_list->VtxBuffer.size();
        uint32_t idx_size = (uint32_t)cmd_list->IdxBuffer.size();

        UIRender_allocPosTexColorTb(&tvb, (uint32_t)vtx_size);
        bgfx::allocTransientIndexBuffer(&tib, idx_size);

        ImDrawVert* verts = (ImDrawVert*)tvb.data;
        memcpy(verts, cmd_list->VtxBuffer.begin(), vtx_size * sizeof(ImDrawVert) );

        ImDrawIdx* indices = (ImDrawIdx*)tib.data;
        memcpy(indices, cmd_list->IdxBuffer.begin(), idx_size * sizeof(ImDrawIdx) );

        uint32_t elem_offset = 0;
        const ImDrawCmd* pcmd_begin = cmd_list->CmdBuffer.begin();
        const ImDrawCmd* pcmd_end = cmd_list->CmdBuffer.end();

        for (const ImDrawCmd* pcmd = pcmd_begin; pcmd != pcmd_end; pcmd++) {
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
                elem_offset += pcmd->ElemCount;
                continue;
            }

            if (0 == pcmd->ElemCount)
                continue;

            bgfx::setState(0
                           | BGFX_STATE_RGB_WRITE
                           | BGFX_STATE_ALPHA_WRITE
                           | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                           | BGFX_STATE_MSAA
                           );
            bgfx::setScissor(uint16_t(bx::fmax(pcmd->ClipRect.x, 0.0f) )
                             , uint16_t(bx::fmax(pcmd->ClipRect.y, 0.0f) )
                             , uint16_t(bx::fmin(pcmd->ClipRect.z, 65535.0f) - bx::fmax(pcmd->ClipRect.x, 0.0f) )
                             , uint16_t(bx::fmin(pcmd->ClipRect.w, 65535.0f) - bx::fmax(pcmd->ClipRect.y, 0.0f) )
                             );
            union { void* ptr; bgfx::TextureHandle handle; } texture = { pcmd->TextureId };

            UIRender_posIdxTexColor(&tvb, &tib, vtx_size, elem_offset, pcmd->ElemCount, 0 != texture.handle.idx ? texture.handle : s_textureId);

            elem_offset += pcmd->ElemCount;
        }
    }
}

#endif


