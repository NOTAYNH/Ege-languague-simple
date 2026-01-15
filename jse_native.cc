#include <napi.h>
#include <string>

std::string RenderElement(Napi::Value val, double startedAt);

std::string ProcessValue(Napi::Value val, double startedAt) {
    if (val.IsString() || val.IsNumber()) {
        return val.ToString().Utf8Value();
    }
    if (val.IsObject()) {
        return RenderElement(val, startedAt);
    }
    return "";
}

std::string RenderElement(Napi::Value val, double startedAt) {
    Napi::Env env = val.Env();
    Napi::Object obj;
    Napi::Array elementArray;

    // Eğer gelen şey senin 'Element' sınıfınsa, içindeki 'element' array'ine bak
    if (val.IsObject() && val.ToObject().Has("element")) {
        elementArray = val.ToObject().Get("element").As<Napi::Array>();
    } else if (val.IsArray()) {
        elementArray = val.As<Napi::Array>();
    } else {
        return val.IsString() ? val.ToString().Utf8Value() : "";
    }

    if (elementArray.Length() < 3) return "";

    std::string tag = elementArray.Get(uint32_t(0)).ToString().Utf8Value();
    Napi::Object props = elementArray.Get(uint32_t(1)).ToObject();
    Napi::Value children = elementArray.Get(uint32_t(2));

    // --- ÖZEL ETİKET KONTROLLERİ ---
    if (tag == "--render") return ProcessValue(children, startedAt);
    if (tag == "pack") return ProcessValue(children, startedAt);
    if (tag == "--render-time") {
        double now = static_cast<double>(Napi::Date::From(env, 0).Now()); // Basitçe süre hesabı
        return "Rendered in some ms (Time sync required)"; 
    }

    // --- HTML OLUŞTURMA ---
    std::string html = "<" + tag;

    // Property (Attribute) Render
    Napi::Array propKeys = props.GetPropertyNames();
    for (uint32_t i = 0; i < propKeys.Length(); i++) {
        std::string key = propKeys.Get(i).ToString().Utf8Value();
        std::string value = props.Get(key).ToString().Utf8Value();
        html += " " + key + "=\"" + value + "\"";
    }
    html += ">";

    // Çocukları işle (Recursion)
    if (children.IsArray()) {
        Napi::Array childArr = children.As<Napi::Array>();
        for (uint32_t i = 0; i < childArr.Length(); i++) {
            html += ProcessValue(childArr.Get(i), startedAt);
        }
    } else {
        html += ProcessValue(children, startedAt);
    }

    html += "</" + tag + ">";
    return html;
}

Napi::String FastRender(const Napi::CallbackInfo& info) {
    // info[0]: this.element, info[1]: startedAt (opsiyonel)
    double start = info.Length() > 1 ? info[1].As<Napi::Number>().DoubleValue() : 0;
    return Napi::String::New(info.Env(), RenderElement(info[0], start));
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("render", Napi::Function::New(env, FastRender));
    return exports;
}

NODE_API_MODULE(jse_native, Init)