/**
 * @file main.cpp
 * @brief Otonom Araç Güvenlik Modülü — Hard-Margin SVM Demo
 *
 * Katmanlı Mimari — Katman 5: Uygulama / Giriş Noktası
 *
 * Bağımlılıklar (katman sırası):
 *   main.cpp  →  Visualizer  →  SVM  →  DataGenerator  →  Point
 *
 * Derleme (tek komut):
 *   g++ -std=c++17 -O2 -o svm main.cpp DataGenerator.cpp SVM.cpp Visualizer.cpp
 *
 * Test Senaryoları:
 *   1. Basit Ayrılabilir   — 3+3 nokta, geniş marjin
 *   2. Yakın Marjin        — 3+3 nokta, dar marjin
 *   3. Rastgele Üretilmiş  — DataGenerator seed=42, 10+10 nokta
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#include <iostream>
#include <vector>
#include <string>

#include "Point.h"
#include "DataGenerator.h"
#include "SVM.h"
#include "Visualizer.h"

// ============================================================
//  YARDIMCI FONKSİYON: runScenario
// ============================================================

/**
 * @brief Tek bir senaryoyu uçtan uca çalıştırır.
 *
 * Adımlar:
 *  1. Nokta listesini konsola yaz          → O(n)
 *  2. SVM eğit                             → O(n² · iter)
 *  3. ASCII görselleştirme                 → O(W·H + n)
 *  4. Sayısal sonuçları yazdır             → O(n + s)
 *
 * @param points Eğitim noktaları
 * @param name   Senaryo adı
 *
 * Big-O ZAMAN: O(n² · iter)  (egemen terim: train)
 * Big-O UZAY:  O(n)
 */
void runScenario(const std::vector<Point>& points, const std::string& name) {
    std::cout << "\n\n";
    std::cout << "████████████████████████████████████████████████\n";
    std::cout << "  SENARYO: " << name << "\n";
    std::cout << "████████████████████████████████████████████████\n";

    // Nokta listesini göster — O(n)
    std::cout << "\nEgitim noktalari (" << points.size() << " adet):\n";
    for (const auto& p : points) {
        std::cout << "  " << p.toString() << "\n";
    }

    // SVM eğit (Katman 3) — O(n² · iter)
    SVM svm(1e-4, 2000);
    svm.train(points);

    // ASCII görselleştirme (Katman 4) — O(W·H + n)
    Visualizer::draw(svm, points, name);

    // Sayısal sonuçlar (Katman 4) — O(n + s)
    Visualizer::printResults(svm, points, name);
}

// ============================================================
//  MAIN
// ============================================================

int main() {
    // Başlık banner
    std::cout << R"(
  _____ _      _______  __  _____     ___
 / ____| |    |  __ \ \/ / / ____|   / _ \
| (___ | |    | |__) >  < | (___    | | | |_ __
 \___ \| |    |  ___/ /\ \ \___ \   | | | | '_ \
 ____) | |____| |  / ____ \____) |  | |_| | |_) |
|_____/|______|_| /_/    \_\_____/   \___/| .__/
  Otonom Arac Guvenlik Modulu             |_|
  Hard-Margin SVM  --  SMO Algoritmasi
)";

    // -------------------------------------------------------
    // SENARYO 1 — Basit Ayrılabilir
    //   Sınıf +1: (1,2), (2,3), (3,1)
    //   Sınıf -1: (5,4), (6,5), (5,6)
    //   Beklenti: geniş marjin, %100 doğruluk
    // -------------------------------------------------------
    {
        std::vector<Point> pts = {
            {1.0, 2.0, +1}, {2.0, 3.0, +1}, {3.0, 1.0, +1},
            {5.0, 4.0, -1}, {6.0, 5.0, -1}, {5.0, 6.0, -1}
        };
        runScenario(pts, "Senaryo 1 -- Basit Ayrilabilir");
    }

    // -------------------------------------------------------
    // SENARYO 2 — Yakın Marjin
    //   Sınıf +1: (1,1), (2,2), (1,3)
    //   Sınıf -1: (3,1), (4,2), (3,3)
    //   Beklenti: dar marjin, daha fazla SV
    // -------------------------------------------------------
    {
        std::vector<Point> pts = {
            {1.0, 1.0, +1}, {2.0, 2.0, +1}, {1.0, 3.0, +1},
            {3.0, 1.0, -1}, {4.0, 2.0, -1}, {3.0, 3.0, -1}
        };
        runScenario(pts, "Senaryo 2 -- Yakin Marjin");
    }

    // -------------------------------------------------------
    // SENARYO 3 — DataGenerator ile Rastgele (seed=42, n=10)
    //   DataGenerator: Katman 2 servisi çağrısı
    //   Beklenti: büyük marjin (sınıflar uzak), %100 doğruluk
    // -------------------------------------------------------
    {
        DataGenerator gen(42);                     // Katman 2
        auto pts = gen.generate(10, 1.0);          // O(n)
        runScenario(pts, "Senaryo 3 -- Rastgele Uretilmis (seed=42)");
    }

    // -------------------------------------------------------
    // BIG-O ANALİZ ÖZETİ
    // -------------------------------------------------------
    std::cout << R"(

╔══════════════════════════════════════════════════════════════════════╗
║                   BIG-O ANALIZ TABLOSU                              ║
╠══════════════════╦══════════════════╦══════════════╦════════════════╣
║ Operasyon        ║ Zaman            ║ Uzay         ║ Neden          ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ train()  SMO     ║ O(n^2 * iter)    ║ O(n)         ║ n^2 cift tara  ║
║                  ║ worst: O(n^3)    ║              ║ a ve pts sakla ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ predict(x)       ║ O(1)             ║ O(1)         ║ onceden        ║
║                  ║  (w sakli)       ║              ║ hesaplanan w   ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ getSupportVec()  ║ O(n)             ║ O(s)         ║ a diz. tarama  ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ getMargin()      ║ O(1)             ║ O(1)         ║ 2/sqrt(w1+w2)  ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ accuracy()       ║ O(n)             ║ O(1)         ║ n tahmin       ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ Visualizer::draw ║ O(W*H + n)       ║ O(W*H)       ║ izgara + nokta ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ generate()       ║ O(n)             ║ O(n)         ║ 2n PRNG cekimi ║
╠══════════════════╬══════════════════╬══════════════╬════════════════╣
║ TOPLAM UZAY      ║       -          ║ O(n)         ║ a + pts        ║
╚══════════════════╩══════════════════╩══════════════╩════════════════╝

  n    = egitim noktasi sayisi
  s    = destek vektoru sayisi  (s <= n)
  iter = iterasyon sayisi (sabit: 2000)
  W, H = izgara boyutu (sabit: 60x24)

)";

    std::cout << "Program basariyla tamamlandi.\n";
    return 0;
}
