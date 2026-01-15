#include <napi.h>
#include <string>

// İleri tanımlama
std::string RenderElement(Napi::Value val, double startedAt);

std::string ProcessValue(Napi::Value val, double startedAt) {
    if (val.IsString() || val.IsNumber()) {
        return val.ToString().Utf8Value();
    }
    
    if (val.IsObject()) {
        Napi::Object obj = val.ToObject();
        // 1. Eğer bu senin Element class'ınsa, içindeki .element'i al
        if (obj.Has("element")) {
            return RenderElement(obj.Get("element"), startedAt);
        }
        // 2. Eğer bu zaten bir array ise direkt işle
        if (val.IsArray()) {
            return RenderElement(val, startedAt);
        }
    }
    return "";
}

std::string RenderElement(Napi::Value val, double startedAt) {
    Napi::Env env = val.Env();
    
    // Gelen verinin kesinlikle bir dizi olduğundan emin olalım
    if (!val.IsArray()) {
        return ProcessValue(val, startedAt);
    }

    Napi::Array elementArray = val.As<Napi::Array>();
    if (elementArray.Length() < 1) return "";

    Napi::Value v0 = elementArray.Get(uint32_t(0));
    std::string tag = v0.ToString().Utf8Value();

    // Özel etiketler
    if (tag == "--render" || tag == "pack") {
        if (elementArray.Length() >= 3) {
            return ProcessValue(elementArray.Get(uint32_t(2)), startedAt);
        }
        return "";
    }

    if (tag == "--render-time") {
        return "Rendered."; 
    }

    // HTML oluşturma
    std::string html = "<" + tag;

    // Özellikler (Attributes) - İndis 1
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

    // Çocuklar (Children) - İndis 2
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