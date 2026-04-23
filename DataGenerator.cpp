/**
 * @file DataGenerator.cpp
 * @brief DataGenerator sınıfının implementasyonu.
 *
 * Katmanlı Mimari — Katman 2: Servis / Veri Üretim Katmanı
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#include "DataGenerator.h"

// -------------------------------------------------------
// YAPICI
// -------------------------------------------------------

/**
 * @brief Seed ile Mersenne Twister PRNG'yi başlatır.
 * Big-O: O(1)
 */
DataGenerator::DataGenerator(unsigned int seed)
    : rng_(seed) {}

// -------------------------------------------------------
// generate()
// -------------------------------------------------------

/**
 * @brief n noktalı, doğrusal ayrılabilir iki sınıf üretir.
 *
 * Algoritma:
 *   1. Normal dağılım nesnesi oluştur: N(0, spread*0.3)
 *   2. Sınıf +1: merkez (-3, 0) + gürültü  → n nokta   [O(n)]
 *   3. Sınıf -1: merkez (+3, 0) + gürültü  → n nokta   [O(n)]
 *   4. Toplam 2n nokta döndür
 *
 * Big-O ZAMAN: O(n) — 2n kez PRNG çekimi, her biri O(1).
 * Big-O UZAY:  O(n) — points vektörü 2n eleman tutar.
 */
std::vector<Point> DataGenerator::generate(int n, double spread) {
    // Normal dağılım gürültüsü — Big-O: O(1) nesne oluşturma
    std::normal_distribution<double> noise(0.0, spread * 0.3);

    std::vector<Point> points;
    points.reserve(2 * n); // Yeniden tahsis önlenir — O(1) amortize

    // Sınıf +1: merkez (-3, 0) — Big-O: O(n)
    for (int i = 0; i < n; ++i) {
        double px = -3.0 + noise(rng_); // PRNG çekimi O(1)
        double py =  0.0 + noise(rng_);
        points.emplace_back(px, py, +1);
    }

    // Sınıf -1: merkez (+3, 0) — Big-O: O(n)
    for (int i = 0; i < n; ++i) {
        double px = +3.0 + noise(rng_);
        double py =  0.0 + noise(rng_);
        points.emplace_back(px, py, -1);
    }

    return points; // RVO (Return Value Optimization) ile kopyasız
}
