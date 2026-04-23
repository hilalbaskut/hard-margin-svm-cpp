/**
 * @file Visualizer.h
 * @brief ASCII terminal görselleştirici — karar sınırı, marjin, noktalar.
 *
 * Katmanlı Mimari — Katman 4: Sunum / Görünüm Katmanı
 * Bağımlılık: Point (Katman 1), SVM (Katman 3)
 *
 * Bu katman yalnızca görüntüleme sorumluluğu taşır;
 * hiçbir hesaplama veya veri üretimi yapmaz.
 *
 * ASCII Sembol Sözlüğü:
 *   +   → Sınıf +1 noktası
 *   -   → Sınıf -1 noktası
 *   *   → Destek vektörü (her iki sınıftan olabilir)
 *   #   → Karar sınırı (|w·x + b| < 0.15)
 *   .   → Marjin bandı (0.15 ≤ |w·x + b| < 1.05)
 *   ' ' → Sınıflandırılmış bölge (boş)
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include <string>
#include "Point.h"
#include "SVM.h"

/**
 * @class Visualizer
 * @brief SVM sonuçlarını terminalde görselleştiren statik yardımcı sınıf.
 *
 * Tek Sorumluluk: Sunum / çıktı üretimi.
 * Tüm metodlar statiktir — nesne oluşturulmaz, durum tutulmaz.
 *
 * Big-O draw():        O(W·H + n·s)  W=60, H=24 (sabit), n=nokta, s=SV
 * Big-O printResults(): O(s)          s = destek vektörü sayısı
 */
class Visualizer {
public:
    static const int W = 60; ///< Izgara genişliği (sütun sayısı)
    static const int H = 24; ///< Izgara yüksekliği (satır sayısı)

    /**
     * @brief Karar sınırı, marjin bantları ve noktaları ASCII ızgarada çizer.
     * @param svm    Eğitilmiş SVM modeli (const referans)
     * @param points Görselleştirilecek nokta listesi
     * @param title  Senaryo başlığı (başlık satırında gösterilir)
     *
     * Algoritma:
     *  1. Nokta koordinatlarından [xmin,xmax]×[ymin,ymax] belirle  → O(n)
     *  2. W×H ızgara hücrelerini f=w·x+b değerine göre doldur      → O(W·H)
     *  3. Nokta koordinatlarını ızgara hücrelerine eşle             → O(n·s)
     *  4. Izgarayı konsola yaz                                      → O(W·H)
     *
     * Big-O ZAMAN: O(W·H + n·s)  → pratik O(n) (W,H,s sabit)
     * Big-O UZAY:  O(W·H) — overlay matrisi
     */
    static void draw(const SVM& svm,
                     const std::vector<Point>& points,
                     const std::string& title);

    /**
     * @brief Model sonuçlarını (w, b, margin, SV, doğruluk) konsola yazar.
     * @param svm           Eğitilmiş SVM modeli
     * @param points        Doğruluk hesabı için nokta listesi
     * @param scenario_name Senaryo adı (başlık için)
     *
     * Big-O ZAMAN: O(n + s)
     *   accuracy() → O(n), SV listesi → O(s)
     * Big-O UZAY:  O(s) — SV listesi kopyası
     */
    static void printResults(const SVM& svm,
                             const std::vector<Point>& points,
                             const std::string& scenario_name);
};

#endif // VISUALIZER_H
