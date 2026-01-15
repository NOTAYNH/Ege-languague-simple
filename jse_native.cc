#include <napi.h>
#include <string>

std::string RenderElement(Napi::Value val, double startedAt);

// Yardımcı: Değerleri stringe çevirir veya nesneyi render eder
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
    Napi::Array elementArray;

    // Element nesnesi mi yoksa ham dizi mi kontrolü
    if (val.IsObject() && val.ToObject().Has("element")) {
        elementArray = val.ToObject().Get("element").As<Napi::Array>();
    } else if (val.IsArray()) {
        elementArray = val.As<Napi::Array>();
    } else {
        return val.IsString() ? val.ToString().Utf8Value() : "";
    }

    if (elementArray.Length() < 1) return "";

    std::string tag = elementArray.Get(uint32_t(0)).ToString().Utf8Value();

    // Özel Etiketler (Tag-based logic)
    if (tag == "--render" || tag == "pack") {
        if (elementArray.Length() >= 3) {
            return ProcessValue(elementArray.Get(uint32_t(2)), startedAt);
        }
        return "";
    }

    if (tag == "--render-time") {
        // Zamanı C++ içinde hesaplamak yerine JS tarafındaki StartedAt'i kullanabiliriz
        return "Rendered."; 
    }

    // Normal HTML Etiketi Başlangıcı
    std::string html = "<" + tag;

    // Özellikleri (Attributes) ekle
    if (elementArray.Length() >= 2) {
        Napi::Value propsVal = elementArray.Get(uint32_t(1));
        if (propsVal.IsObject() && !propsVal.IsArray()) {
            Napi::Object props = propsVal.As<Napi::Object>();
            Napi::Array propKeys = props.GetPropertyNames();
            for (uint32_t i = 0; i < propKeys.Length(); i++) {
                std::string key = propKeys.Get(i).ToString().Utf8Value();
                std::string value = props.Get(key).ToString().Utf8Value();
                html += " " + key + "=\"" + value + "\"";
            }
        }
    }
    html += ">";

    // Çocukları (Children) ekle
    if (elementArray.Length() >= 3) {
        Napi::Value children = elementArray.Get(uint32_t(2));
        if (children.IsArray()) {
            Napi::Array childArr = children.As<Napi::Array>();
            for (uint32_t i = 0; i < childArr.Length(); i++) {
                html += ProcessValue(childArr.Get(i), startedAt);
            }
        } else {
            html += ProcessValue(children, startedAt);
        }
    }

    html += "</" + tag + ">";
    return html;
}

Napi::String FastRender(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    double startedAt = 0;
    if (info.Length() > 1 && info[1].IsNumber()) {
        startedAt = info[1].As<Napi::Number>().DoubleValue();
    }
    
    std::string result = RenderElement(info[0], startedAt);
    return Napi::String::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("render", Napi::Function::New(env, FastRender));
    return exports;
}

NODE_API_MODULE(jse_native, Init)