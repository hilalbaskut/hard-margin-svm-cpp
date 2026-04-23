/**
 * @file SVM.cpp
 * @brief Hard-Margin SVM — SMO algoritması implementasyonu.
 *
 * Katmanlı Mimari — Katman 3: Çekirdek / Algoritma Katmanı
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#include "SVM.h"

// ============================================================
//  YAPICI
// ============================================================

/**
 * @brief Hiperparametreler ve başlangıç durumu ayarlanır.
 * Big-O: O(1)
 */
SVM::SVM(double tol, int max_iter)
    : tol_(tol),
      max_iter_(max_iter),
      w1_(0.0),
      w2_(0.0),
      bias_(0.0),
      trained_(false) {}

// ============================================================
//  PUBLIC: train()
// ============================================================

/**
 * @brief SMO algoritması ile SVM'i eğitir.
 *
 * Adım adım algoritma:
 *  1. α'ları sıfırla (tüm αᵢ = 0)                  → O(n)
 *  2. Ana döngü (max_iter iterasyon):
 *     a. Her i için hata Eᵢ = f(xᵢ) − yᵢ hesapla  → O(n) per iter
 *     b. KKT ihlali varsa j seç (|Eᵢ−Eⱼ| maks)    → O(n) per i
 *     c. Kernel matris elemanları hesapla (lazy)     → O(1) per pair
 *     d. η = 2Kᵢⱼ − Kᵢᵢ − Kⱼⱼ (ikinci türev)
 *     e. αⱼ_new = αⱼ − yⱼ(Eᵢ−Eⱼ)/η, sınırlara kırp
 *     f. αᵢ_new = αᵢ + yᵢyⱼ(αⱼ_old − αⱼ_new)
 *  3. Değişim olmadığında dur (erken yakınsama)
 *  4. w = Σ αᵢ yᵢ xᵢ hesapla                       → O(n)
 *  5. b = ort(yₛ − w·xₛ) hesapla                   → O(n)
 *
 * Big-O ZAMAN: O(n² · max_iter)
 *   İç döngü: O(n) × O(n) heuristic = O(n²) per iter.
 *   max_iter sabit (2000) → pratik O(n²).
 *   Worst-case teorik: O(n³).
 *
 * Big-O UZAY: O(n)
 *   alphas_: n double, points_: n Point, ek değişkenler O(1).
 */
void SVM::train(const std::vector<Point>& points) {
    int n = static_cast<int>(points.size());
    assert(n >= 2 && "En az 2 eğitim noktası gerekli");

    // Adım 1: α'ları ve veri kopyasını başlat — O(n)
    alphas_.assign(n, 0.0);
    points_ = points;
    bias_   = 0.0;
    w1_     = 0.0;
    w2_     = 0.0;

    // Adım 2: SMO ana döngüsü — Big-O: O(n² · max_iter)
    for (int iter = 0; iter < max_iter_; ++iter) {
        int num_changed = 0;

        // Tüm noktalara bak — O(n) dış döngü
        for (int i = 0; i < n; ++i) {

            // Hata hesabı — O(n) (decisionFunction iç döngüsü)
            double Ei = decisionFunction(i) - points[i].label;

            // KKT ihlali yoksa bu noktayı atla — O(1)
            if (!kktViolation(i, Ei)) continue;

            // j seçimi: maksimum |Eᵢ − Eⱼ| heuristic — O(n)
            int j = selectSecondAlpha(i, Ei, n);
            if (j < 0) continue;

            double Ej     = decisionFunction(j) - points[j].label;
            double ai_old = alphas_[i];
            double aj_old = alphas_[j];

            // Sınır hesabı (hard-margin: üst sınır = +∞) — O(1)
            double L, H;
            if (points[i].label != points[j].label) {
                L = std::max(0.0, aj_old - ai_old);
                H = 1e9; // hard-margin → sınırsız üst
            } else {
                L = std::max(0.0, ai_old + aj_old);
                H = 1e9;
            }
            if (std::abs(L - H) < 1e-10) continue;

            // Kernel değerleri (lazy — matrix saklanmaz) — O(1) each
            double Kii = kernelLinear(points[i], points[i]);
            double Kjj = kernelLinear(points[j], points[j]);
            double Kij = kernelLinear(points[i], points[j]);

            // İkinci türev (negatif olmalı, konveks garanti) — O(1)
            double eta = 2.0 * Kij - Kii - Kjj;
            if (eta >= 0) continue;

            // αⱼ analitik güncelleme — O(1)
            double aj_new = aj_old - points[j].label * (Ei - Ej) / eta;
            aj_new = std::max(L, std::min(H, aj_new));

            // Anlamlı değişim yok → atla — O(1)
            if (std::abs(aj_new - aj_old) < 1e-8) continue;

            // αᵢ güncelleme (eşitlik kısıtı Σαᵢyᵢ=0'dan) — O(1)
            double ai_new = ai_old
                + points[i].label * points[j].label * (aj_old - aj_new);
            ai_new = std::max(0.0, ai_new);

            alphas_[i] = ai_new;
            alphas_[j] = aj_new;
            ++num_changed;
        }

        // Adım 3: Erken yakınsama — değişim yoksa dur — O(1)
        if (num_changed == 0) break;
    }

    // Adım 4: w = Σ αᵢ yᵢ xᵢ — O(n)
    computeWeights();

    // Adım 5: b = ort(yₛ − w·xₛ) — O(n)
    computeBias();

    trained_ = true;
}

// ============================================================
//  PUBLIC: predict()
// ============================================================

/**
 * @brief f(x) = w₁x + w₂y + b; işaret → sınıf
 * Big-O ZAMAN: O(1) — önceden hesaplanmış w kullanılır.
 * Big-O UZAY:  O(1)
 */
int SVM::predict(const Point& p) const {
    assert(trained_ && "Önce train() çağrılmalı");
    // Big-O: O(d) = O(1) for d=2
    double score = w1_ * p.x + w2_ * p.y + bias_;
    return (score >= 0.0) ? +1 : -1;
}

// ============================================================
//  PUBLIC: getMargin()
// ============================================================

/**
 * @brief margin = 2 / ||w||
 * Big-O ZAMAN: O(1) — 2 kare + sqrt + bölme.
 * Big-O UZAY:  O(1)
 */
double SVM::getMargin() const {
    // Big-O: O(d) = O(1)
    double norm_w = std::sqrt(w1_ * w1_ + w2_ * w2_);
    return (norm_w > 1e-12) ? 2.0 / norm_w : 0.0;
}

// ============================================================
//  PUBLIC: getSupportVectors()
// ============================================================

/**
 * @brief αᵢ > tol olan noktalar destek vektörüdür.
 * Big-O ZAMAN: O(n) — doğrusal tarama.
 * Big-O UZAY:  O(s) s ≤ n — yalnızca SV'ler döndürülür.
 */
std::vector<Point> SVM::getSupportVectors() const {
    std::vector<Point> svs;
    // Big-O: O(n)
    for (size_t i = 0; i < alphas_.size(); ++i) {
        if (alphas_[i] > tol_) {
            svs.push_back(points_[i]);
        }
    }
    return svs;
}

// ============================================================
//  PUBLIC: Erişim metodları
// ============================================================

/** Big-O: O(1) */
std::pair<double, double> SVM::getWeights() const { return {w1_, w2_}; }

/** Big-O: O(1) */
double SVM::getBias() const { return bias_; }

/** Big-O: O(1) */
bool SVM::isTrained() const { return trained_; }

/**
 * @brief n predict() çağrısı → doğru olanları say.
 * Big-O ZAMAN: O(n)
 * Big-O UZAY:  O(1)
 */
double SVM::accuracy(const std::vector<Point>& points) const {
    int correct = 0;
    // Big-O: O(n)
    for (const auto& p : points) {
        if (predict(p) == p.label) ++correct;
    }
    return static_cast<double>(correct) / static_cast<double>(points.size());
}

// ============================================================
//  PRIVATE: kernelLinear()
// ============================================================

/**
 * @brief K(a, b) = a · b  (doğrusal iç çarpım kernel)
 * Big-O: O(d) = O(1) for d=2
 */
double SVM::kernelLinear(const Point& a, const Point& b) const {
    return a.x * b.x + a.y * b.y;
}

// ============================================================
//  PRIVATE: decisionFunction()
// ============================================================

/**
 * @brief f(xᵢ) = Σⱼ αⱼ yⱼ K(xⱼ, xᵢ) + b
 * Yalnızca αⱼ > 0 olan terimler hesaba katılır (seyreklik optimizasyonu).
 *
 * Big-O ZAMAN: O(n) — n α değeri taranır.
 * Big-O UZAY:  O(1) — yalnızca bir toplam değişkeni.
 */
double SVM::decisionFunction(int i) const {
    double sum = 0.0;
    // Big-O: O(n)
    for (size_t j = 0; j < alphas_.size(); ++j) {
        if (alphas_[j] > 1e-10) { // sıfır α'ları atla (seyreklik)
            sum += alphas_[j]
                 * static_cast<double>(points_[j].label)
                 * kernelLinear(points_[j], points_[i]);
        }
    }
    return sum + bias_;
}

// ============================================================
//  PRIVATE: kktViolation()
// ============================================================

/**
 * @brief KKT koşulunun ihlal edilip edilmediğini kontrol eder.
 *
 * KKT tamamlayıcı uyum: αᵢ[yᵢ(w·xᵢ+b) − 1] = 0
 *   → yᵢ·f(xᵢ) < 1 − tol  (kısıt ihlali)
 *   → αᵢ > 0 ve yᵢ·f(xᵢ) > 1 + tol  (sınırdan uzaklaşma)
 *
 * Big-O: O(1)
 */
bool SVM::kktViolation(int i, double Ei) const {
    double yi = static_cast<double>(points_[i].label);
    double yf  = yi * (Ei + yi); // = yᵢ · f(xᵢ)
    return (yf < 1.0 - tol_) ||
           (alphas_[i] > tol_ && yf > 1.0 + tol_);
}

// ============================================================
//  PRIVATE: selectSecondAlpha()
// ============================================================

/**
 * @brief Heuristic: adım büyüklüğünü maksimize etmek için
 *        |Eᵢ − Eⱼ| en büyük olan j seçilir.
 *
 * Big-O ZAMAN: O(n) — tüm j adayları değerlendirilir.
 * Big-O UZAY:  O(1)
 */
int SVM::selectSecondAlpha(int i, double Ei, int n) const {
    double maxDiff = -1.0;
    int    best_j  = -1;
    // Big-O: O(n)
    for (int j = 0; j < n; ++j) {
        if (j == i) continue;
        double Ej   = decisionFunction(j) - points_[j].label;
        double diff = std::abs(Ei - Ej);
        if (diff > maxDiff) {
            maxDiff = diff;
            best_j  = j;
        }
    }
    return best_j;
}

// ============================================================
//  PRIVATE: computeWeights()
// ============================================================

/**
 * @brief w = Σᵢ αᵢ yᵢ xᵢ  (KKT birinci türev koşulundan)
 *
 * Big-O ZAMAN: O(n)
 * Big-O UZAY:  O(1) — w1_, w2_ yerinde güncellenir.
 */
void SVM::computeWeights() {
    w1_ = 0.0;
    w2_ = 0.0;
    // Big-O: O(n)
    for (size_t i = 0; i < alphas_.size(); ++i) {
        w1_ += alphas_[i] * static_cast<double>(points_[i].label) * points_[i].x;
        w2_ += alphas_[i] * static_cast<double>(points_[i].label) * points_[i].y;
    }
}

// ============================================================
//  PRIVATE: computeBias()
// ============================================================

/**
 * @brief b = ortalama(yₛ − w·xₛ)  tüm destek vektörleri için
 *
 * Ortalama alma, sayısal kararlılığı artırır.
 *
 * Big-O ZAMAN: O(n)
 * Big-O UZAY:  O(1) — bias_ yerinde güncellenir.
 */
void SVM::computeBias() {
    bias_    = 0.0;
    int count = 0;
    // Big-O: O(n)
    for (size_t i = 0; i < alphas_.size(); ++i) {
        if (alphas_[i] > tol_) {
            bias_ += static_cast<double>(points_[i].label)
                   - (w1_ * points_[i].x + w2_ * points_[i].y);
            ++count;
        }
    }
    if (count > 0) bias_ /= static_cast<double>(count);
}
