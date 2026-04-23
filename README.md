# Autonomous SVM Safety Module

Bu proje, otonom araç navigasyon sistemlerinde engel sınıflandırması ve güvenli mesafe (marjin) tahmini için sıfırdan geliştirilmiş bir **Hard-Margin Support Vector Machine (SVM)** implementasyonudur. Hiçbir hazır ML kütüphanesi (scikit-learn vb.) kullanılmadan, tamamen **C++17** standartları ile "Scratch" olarak yazılmıştır.

## 🚀 Proje Hakkında
Otonom bir aracın 2D düzlemde karşılaştığı engelleri (Sınıf +1 ve Sınıf -1) ayırırken, her iki sınıfa olan mesafeyi maksimize eden (Maksimum Marjin) optimum güvenlik koridorunu hesaplar.

### Temel Özellikler
* **SMO (Sequential Minimal Optimization) Algoritması:** SVM eğitimini verimli hale getiren analitik SMO implementasyonu.
* **Katmanlı Mimari:** Tek Sorumluluk (SRP) prensibine göre 5 farklı katmanda organize edilmiş modüler tasarım.
* **Bellek Yönetimi:** RAII prensipleri ile sıfır ham pointer (raw pointer) kullanımı; bellek sızıntısı riski olmayan güvenli C++ kodu.
* **ASCII Görselleştirme:** Eğitilen modelin karar sınırını ve marjin bantlarını terminalde gerçek zamanlı render eden `Visualizer` katmanı.

## 🏗 Mimari Yapı
Proje, bağımlılık yönü yukarıdan aşağıya (Main -> Visualizer -> SVM -> DataGenerator -> Point) olacak şekilde katmanlandırılmıştır:

1. **Katman 1 (Domain):** `Point` sınıfı.
2. **Katman 2 (Service):** `DataGenerator` (Tekrarlanabilir rastgele veri üretimi).
3. **Katman 3 (Core):** `SVM` (Eğitim ve tahmin mantığı).
4. **Katman 4 (Presentation):** `Visualizer` (ASCII render motoru).
5. **Katman 5 (Application):** `main.cpp` (Orkestrasyon).

## 🛠 Teknik Çıktı (Demo)
Sistem, eğitim sonucunda karar sınırını ve destek vektörlerini şu şekilde görselleştirir:

```text
╔══════════════════════════════════════════╗
║  SONUCLAR: Rastgele Senaryo              ║
╠══════════════════════════════════════════╣
║  w = [-1.2033, -1.6930]
║  b = 9.0457
║  Karar siniri: -1.2033*x + -1.6930*y + 9.0457 = 0
║  Margin genisligi: 0.9629
║  Dogruluk: 100.0000%
╚══════════════════════════════════════════╝ 
⚙️ Derleme ve Çalıştırma
Proje modern C++ standartlarını kullanır. Derlemek için terminalde şu komutu kullanabilirsiniz:

Bash
g++ -std=c++17 -O2 -o svm main.cpp DataGenerator.cpp SVM.cpp Visualizer.cpp
./svm
