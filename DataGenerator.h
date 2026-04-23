/**
 * @file DataGenerator.h
 * @brief Doğrusal ayrılabilir, tekrarlanabilir rastgele veri üretici.
 *
 * Katmanlı Mimari — Katman 2: Servis / Veri Üretim Katmanı
 * Bağımlılık: Point (Katman 1)
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <vector>
#include <random>
#include "Point.h"

/**
 * @class DataGenerator
 * @brief Seed destekli, deterministik iki sınıflı 2D veri üreticisi.
 *
 * Tek Sorumluluk: Eğitim verisi üretmek.
 * Aynı seed → aynı veri (tekrarlanabilirlik garantisi).
 *
 * Üretim stratejisi:
 *   Sınıf +1 merkezi: (-3, 0)
 *   Sınıf -1 merkezi: (+3, 0)
 *   Gürültü: N(0, spread*0.3) normal dağılım
 *
 * Big-O üretim: O(n)
 * Big-O uzay:   O(n) — 2n nokta döndürülür
 */
class DataGenerator {
public:
    /**
     * @brief Yapıcı — seed ile Mersenne Twister PRNG başlatır.
     * @param seed Rastgelelik tohumu (varsayılan 42)
     *
     * Big-O: O(1)
     */
    explicit DataGenerator(unsigned int seed = 42);

    /**
     * @brief n noktalı, doğrusal ayrılabilir iki sınıf üretir.
     * @param n      Her sınıftan üretilecek nokta sayısı (toplam 2n nokta)
     * @param spread Sınıf içi yayılım katsayısı (varsayılan 1.0)
     * @return Karışık sıralı nokta listesi (2n eleman)
     *
     * Big-O ZAMAN: O(n) — n kez normal dağılım örneği, her biri O(1).
     * Big-O UZAY:  O(n) — 2n Point nesnesi döndürülür.
     */
    std::vector<Point> generate(int n, double spread = 1.0);

private:
    std::mt19937 rng_; ///< Mersenne Twister PRNG — deterministik kaynak
};

#endif // DATAGENERATOR_H
