/**
 * @file SVM.h
 * @brief Hard-Margin SVM — SMO algoritması ile eğitim ve tahmin.
 *
 * Katmanlı Mimari — Katman 3: Çekirdek / Algoritma Katmanı
 * Bağımlılık: Point (Katman 1)
 *
 * Matematiksel Model:
 * ──────────────────
 *   Primal:
 *     Minimize:  (1/2)||w||²
 *     Kısıt:     yᵢ(w·xᵢ + b) ≥ 1   ∀i
 *
 *   Dual (Lagrange dönüşümü):
 *     Maximize:  W(α) = Σᵢ αᵢ − (1/2) Σᵢ Σⱼ αᵢαⱼ yᵢyⱼ (xᵢ·xⱼ)
 *     Kısıt:     Σᵢ αᵢyᵢ = 0,  αᵢ ≥ 0
 *
 *   Sonuç:
 *     w = Σᵢ αᵢ yᵢ xᵢ          (destek vektörlerinden)
 *     b = yₛ − w·xₛ             (SV üzerinden ortalama)
 *     margin = 2 / ||w||
 *
 *   Destek Vektörü: αᵢ > 0 olan noktalar.
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#ifndef SVM_H
#define SVM_H

#include <vector>
#include <utility>
#include <cassert>
#include <cmath>
#include <algorithm>
#include "Point.h"

/**
 * @class SVM
 * @brief Hard-Margin Support Vector Machine.
 *
 * Tek Sorumluluk: SMO ile eğitim, tahmin, marjin ve SV çıkarımı.
 *
 * SMO (Sequential Minimal Optimization) Algoritması:
 *   1. α vektörünü sıfırla           → O(n)
 *   2. KKT ihlali olan i'yi bul      → O(n) per iter
 *   3. Heuristic ile j seç           → O(n) per iter
 *   4. İkili analitik optimizasyon   → O(1) per pair
 *   5. w ve b güncelle               → O(n)
 *   6. Değişim yoksa dur             → yakınsama
 *
 * Big-O ZAMAN eğitim: O(n² · iter) — pratik O(n²), worst O(n³)
 * Big-O UZAY  eğitim: O(n)         — α + pts dizileri
 */
class SVM {
public:
    /**
     * @brief Yapıcı — SVM hiperparametrelerini ayarlar.
     * @param tol      KKT ihlali toleransı (varsayılan 1e-4)
     * @param max_iter Maksimum SMO iterasyonu (varsayılan 1000)
     *
     * Big-O: O(1)
     */
    explicit SVM(double tol = 1e-4, int max_iter = 1000);

    /**
     * @brief SMO algoritması ile SVM'i eğitir.
     * @param points Eğitim noktaları (en az 2 farklı sınıf içermeli)
     *
     * Ön koşul: points.size() >= 2 ve iki farklı etiket var.
     *
     * Big-O ZAMAN: O(n² · max_iter)
     *   Her iter → O(n²): n aday × O(n) heuristic j seçimi
     *   Worst-case yakınsama: O(n³)
     *   Pratik (erken durma): O(n²)
     *
     * Big-O UZAY: O(n)
     *   α dizisi: n double
     *   pts kopyası: n Point
     *   Kernel matrix saklanmaz (lazy hesap) → +O(1)
     */
    void train(const std::vector<Point>& points);

    /**
     * @brief Yeni bir noktanın sınıfını tahmin eder.
     * @param p Tahmin edilecek nokta
     * @return +1 veya -1
     *
     * Ön koşul: train() daha önce çağrılmış olmalı.
     *
     * Big-O ZAMAN: O(1)
     *   w önceden hesaplanmış → tek bir O(d)=O(1) iç çarpım.
     *
     * Big-O UZAY: O(1) — ek bellek kullanılmaz.
     */
    int predict(const Point& p) const;

    /**
     * @brief Güvenlik koridoru genişliğini döndürür.
     * @return 2 / ||w||
     *
     * Big-O ZAMAN: O(1) — 2 kare + karekök + bölme.
     * Big-O UZAY:  O(1)
     */
    double getMargin() const;

    /**
     * @brief Destek vektörlerini döndürür (αᵢ > tol olanlar).
     * @return Destek vektörü listesi (kopyası)
     *
     * Big-O ZAMAN: O(n) — α dizisi tek geçişte taranır.
     * Big-O UZAY:  O(s) — s ≤ n destek vektörü döndürülür.
     */
    std::vector<Point> getSupportVectors() const;

    /**
     * @brief Ağırlık vektörü bileşenlerini döndürür.
     * @return {w1, w2} çifti
     *
     * Big-O: O(1)
     */
    std::pair<double, double> getWeights() const;

    /**
     * @brief Bias terimini döndürür.
     * @return b değeri
     *
     * Big-O: O(1)
     */
    double getBias() const;

    /**
     * @brief Eğitim tamamlandı mı?
     * @return true → train() başarıyla çağrıldı
     *
     * Big-O: O(1)
     */
    bool isTrained() const;

    /**
     * @brief Verilen nokta listesi üzerinde sınıflandırma doğruluğunu hesaplar.
     * @param points Test nokta listesi
     * @return [0.0, 1.0] arası doğruluk oranı
     *
     * Big-O ZAMAN: O(n) — n predict() çağrısı, her biri O(1).
     * Big-O UZAY:  O(1) — yalnızca sayaç tutar.
     */
    double accuracy(const std::vector<Point>& points) const;

private:
    // -------------------------------------------------------
    // YARDIMCI METODLAR (private — dışa kapalı)
    // -------------------------------------------------------

    /**
     * @brief Doğrusal kernel fonksiyonu: K(a, b) = a · b
     * @param a Birinci nokta
     * @param b İkinci nokta
     * @return Skalar iç çarpım değeri
     *
     * Big-O: O(d) = O(1) for d=2
     */
    double kernelLinear(const Point& a, const Point& b) const;

    /**
     * @brief i. nokta için karar fonksiyonu değerini hesaplar.
     *        f(xᵢ) = Σⱼ αⱼ yⱼ K(xⱼ, xᵢ) + b
     * @param i Nokta indeksi
     * @return f(xᵢ) skalar değeri
     *
     * Big-O ZAMAN: O(n) — tüm α'larla toplam.
     * Big-O UZAY:  O(1)
     */
    double decisionFunction(int i) const;

    /**
     * @brief i. nokta için KKT ihlali var mı kontrol eder.
     * @param i  Nokta indeksi
     * @param Ei f(xᵢ) − yᵢ hatası
     * @return true → KKT ihlali var, güncelleme gerekli
     *
     * Koşul: (yᵢ·f(xᵢ) < 1 − tol) ∨ (αᵢ > tol ∧ yᵢ·f(xᵢ) > 1 + tol)
     *
     * Big-O: O(1)
     */
    bool kktViolation(int i, double Ei) const;

    /**
     * @brief Heuristic ile ikinci α indeksini (j) seçer.
     *        Kriter: maksimum |Eᵢ − Eⱼ|
     * @param i  Birinci α indeksi
     * @param Ei Birinci α'nın hatası
     * @param n  Toplam nokta sayısı
     * @return Seçilen j indeksi, bulunamazsa -1
     *
     * Big-O ZAMAN: O(n) — tüm j adayları değerlendirilir.
     * Big-O UZAY:  O(1)
     */
    int selectSecondAlpha(int i, double Ei, int n) const;

    /**
     * @brief w = Σᵢ αᵢ yᵢ xᵢ  ağırlık vektörünü günceller.
     *
     * Big-O ZAMAN: O(n)
     * Big-O UZAY:  O(1) — yerinde güncelleme
     */
    void computeWeights();

    /**
     * @brief b = ortalama(yₛ − w·xₛ)  bias'ı günceller.
     *        Tüm destek vektörleri üzerinden ortalama alınır.
     *
     * Big-O ZAMAN: O(n)
     * Big-O UZAY:  O(1) — yerinde güncelleme
     */
    void computeBias();

    // -------------------------------------------------------
    // ÜYE DEĞİŞKENLER
    // -------------------------------------------------------
    double              tol_;       ///< KKT toleransı (ε)
    int                 max_iter_;  ///< Maksimum SMO iterasyonu
    double              w1_;        ///< Ağırlık vektörü — x bileşeni
    double              w2_;        ///< Ağırlık vektörü — y bileşeni
    double              bias_;      ///< Bias terimi b
    bool                trained_;   ///< Eğitim tamamlandı bayrağı
    std::vector<double> alphas_;    ///< Lagrange çarpanları αᵢ — O(n) uzay
    std::vector<Point>  points_;    ///< Eğitim veri kopyası   — O(n) uzay
};

#endif // SVM_H
