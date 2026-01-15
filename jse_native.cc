#include <napi.h>
#include <string>
#include <vector>

// JSE Performans Motoru
// JavaScript'ten gelen dizi (Array) verisini alır ve hızlıca HTML listesine çevirir.
Napi::String RenderToHtml(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 1. Gelen argümanı kontrol et (Dizi mi?)
    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Dizi (Array) bekleniyordu").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    Napi::Array input = info[0].As<Napi::Array>();
    uint32_t len = input.Length();

    // 2. Performans Sırrı: String Reserve
    // C++'da string birleştirirken sürekli bellek ayırmamak için kapasiteyi baştan tahmin ediyoruz.
    // 1 milyon veri için bu PHP'yi nakavt eden kısımdır.
    std::string result;
    result.reserve(len * 50); // Ortalama 50 karakter per element tahmini
    
    result += "<ul>";

    for (uint32_t i = 0; i < len; i++) {
        Napi::Value val = input[i];
        
        // JS verisini C++ stringine çevir ve ekle
        if (val.IsString() || val.IsNumber()) {
            result += "<li>";
            result += val.ToString().Utf8Value();
            result += "</li>";
        }
    }

    result += "</ul>";

    // 3. Nihai HTML stringini JS dünyasına geri gönder
    return Napi::String::New(env, result);
}

// Modül Başlatma (Node.js bu kısmı okur)
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "toHtml"),
                Napi::Function::New(env, RenderToHtml));
    return exports;
}

NODE_API_MODULE(jse_native, Init)