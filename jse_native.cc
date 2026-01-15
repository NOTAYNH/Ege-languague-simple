#include <napi.h>
#include <string>
#include <vector>

std::string RenderElement(Napi::Value val);

// Yardımcı Fonksiyon: Çocukları (Children) işle
std::string ProcessChildren(Napi::Value children) {
    std::string result = "";
    if (children.IsArray()) {
        Napi::Array arr = children.As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            result += RenderElement(arr.Get(i));
        }
    } else if (children.IsString() || children.IsNumber()) {
        result += children.ToString().Utf8Value();
    }
    return result;
}

std::string RenderElement(Napi::Value val) {
    // 1. Basit yazı veya sayı gelirse direkt döndür
    if (val.IsString() || val.IsNumber()) {
        return val.ToString().Utf8Value();
    }

    // 2. Dizi gelirse (Asıl senin if-else mantığın burada)
    if (val.IsArray()) {
        Napi::Array input = val.As<Napi::Array>();
        uint32_t len = input.Length();
        if (len == 0) return "";

        Napi::Value v0 = input.Get(uint32_t(0));

        // --- PACK DESTEĞİ ---
        // Eğer ilk eleman bir diziyse veya adı "pack" ise içindekileri direkt çıkar (Fragment gibi)
        if (v0.IsArray() || (v0.IsString() && v0.ToString().Utf8Value() == "pack")) {
            Napi::Value packContent = v0.IsArray() ? v0 : (len > 2 ? input.Get(uint32_t(2)) : input.Get(uint32_t(0)));
            return ProcessChildren(packContent);
        }

        // --- STANDART TAG İŞLEME ---
        std::string tag = v0.ToString().Utf8Value();
        std::string result = "<" + tag + ">";

        // Değişken yapıları kontrol et
        if (len == 1) { // Sadece ["div"]
            result += "";
        } else if (len == 2) {
            Napi::Value v1 = input.Get(uint32_t(1));
            if (v1.IsArray()) { // ["div", ["çocuklar"]]
                result += ProcessChildren(v1);
            } else if (v1.IsObject() && !v1.IsArray()) { // ["div", {props}]
                // Şimdilik props'ları render etmiyoruz ama istersen ekleyebiliriz
            } else { // ["div", "yazı"]
                result += v1.ToString().Utf8Value();
            }
        } else if (len >= 3) { // ["div", {props}, ["çocuklar"]]
            result += ProcessChildren(input.Get(uint32_t(2)));
        }

        result += "</" + tag + ">";
        return result;
    }

    return "";
}

Napi::String FastRender(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return Napi::String::New(env, "");
    
    // Bellek optimizasyonu: Çıktı stringi için baştan yer ayır
    std::string finalHtml = RenderElement(info[0]);
    return Napi::String::New(env, finalHtml);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("render", Napi::Function::New(env, FastRender));
    return exports;
}

NODE_API_MODULE(jse_native, Init)