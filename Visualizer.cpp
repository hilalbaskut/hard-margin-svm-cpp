/**
 * @file Visualizer.cpp
 * @brief Visualizer sınıfının implementasyonu — ASCII görselleştirme.
 *
 * Katmanlı Mimari — Katman 4: Sunum / Görünüm Katmanı
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#include "Visualizer.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

// ============================================================
//  PUBLIC: draw()
// ============================================================

/**
 * @brief Karar sınırı, marjin ve noktaları ASCII ızgarada render eder.
 *
 * Adım adım:
 *  1. Koordinat sınırları: [xmin,xmax]×[ymin,ymax] + 1.5 boşluk  → O(n)
 *  2. SVM parametrelerini al (O(1) erişim)
 *  3. W×H overlay matrisi oluştur, her hücreyi f değerine göre doldur → O(W·H)
 *  4. Nokta koordinatlarını ızgara indeksine dönüştür                 → O(n·s)
 *  5. Oluşan ızgarayı satır satır konsola yaz                        → O(W·H)
 *
 * Big-O ZAMAN: O(W·H + n·s)  W=60, H=24 sabit → O(n)
 * Big-O UZAY:  O(W·H) — overlay char matrisi   → O(1) (sabit boyut)
 */
void Visualizer::draw(const SVM&                  svm,
                      const std::vector<Point>&   points,
                      const std::string&          title)
{
    // Adım 1: Koordinat sınırlarını bul — O(n)
    double xmin =  1e9, xmax = -1e9;
    double ymin =  1e9, ymax = -1e9;
    for (const auto& p : points) {
        xmin = std::min(xmin, p.x);
        xmax = std::max(xmax, p.x);
        ymin = std::min(ymin, p.y);
        ymax = std::max(ymax, p.y);
    }
    // Çerçeve boşluğu
    const double pad = 1.5;
    xmin -= pad; xmax += pad;
    ymin -= pad; ymax += pad;

    // Adım 2: SVM parametrelerini al — O(1) erişim
    std::pair<double,double> weights = svm.getWeights();
    double w1 = weights.first;
    double w2 = weights.second;
    double b      = svm.getBias();
    auto   svs    = svm.getSupportVectors(); // O(n)

    // Adım 3: W×H overlay matrisi doldur — O(W·H)
    std::vector<std::vector<char>> overlay(H, std::vector<char>(W, ' '));

    for (int row = 0; row < H; ++row) {
        double y_val = ymax - (row / static_cast<double>(H - 1)) * (ymax - ymin);
        for (int col = 0; col < W; ++col) {
            double x_val = xmin + (col / static_cast<double>(W - 1)) * (xmax - xmin);
            double f     = w1 * x_val + w2 * y_val + b;

            // Sınır: |f| < 0.15  → '#'  (karar sınırı)
            // Marjin: |f| < 1.05 → '.'  (marjin bandı)
            // Diğer              → ' '  (sınıflandırılmış bölge)
            if      (std::abs(f) < 0.15) overlay[row][col] = '#';
            else if (std::abs(f) < 1.05) overlay[row][col] = '.';
            // else ' ' (varsayılan)
        }
    }

    // Adım 4: Noktaları ızgaraya işle — O(n·s)
    for (const auto& p : points) {
        // Koordinat → ızgara indeksi dönüşümü — O(1)
        int col = static_cast<int>((p.x - xmin) / (xmax - xmin) * (W - 1));
        int row = static_cast<int>((ymax - p.y) / (ymax - ymin) * (H - 1));
        col = std::max(0, std::min(W - 1, col));
        row = std::max(0, std::min(H - 1, row));

        // Destek vektörü mü? — O(s)
        bool is_sv = false;
        for (const auto& sv : svs) {
            if (std::abs(sv.x - p.x) < 1e-9 &&
                std::abs(sv.y - p.y) < 1e-9) {
                is_sv = true;
                break;
            }
        }

        // Sembol ata: SV > sınıf etiketi
        if      (is_sv)          overlay[row][col] = '*';
        else if (p.label == +1)  overlay[row][col] = '+';
        else                     overlay[row][col] = '-';
    }

    // Adım 5: Oluşan ızgarayı konsola yaz — O(W·H)
    const std::string border(W + 4, '=');
    std::cout << "\n" << border << "\n";
    std::cout << "  " << title << "\n";
    std::cout << border << "\n";

    for (int row = 0; row < H; ++row) {
        std::cout << "| ";
        for (int col = 0; col < W; ++col) {
            std::cout << overlay[row][col];
        }
        std::cout << " |\n";
    }

    std::cout << border << "\n";
    std::cout << "  Semboller: "
              << "[+] Sinif+1  "
              << "[-] Sinif-1  "
              << "[*] Destek Vektoru  "
              << "[#] Karar Siniri  "
              << "[.] Marjin Bandi\n";
}

// ============================================================
//  PUBLIC: printResults()
// ============================================================

/**
 * @brief w, b, margin, karar denklemi, doğruluk ve SV listesini yazar.
 *
 * Big-O ZAMAN: O(n + s)
 *   accuracy() → O(n), SV yazdırma → O(s)
 * Big-O UZAY:  O(s) — getSupportVectors() kopyası
 */
void Visualizer::printResults(const SVM&                svm,
                               const std::vector<Point>& points,
                               const std::string&        scenario_name)
{
    std::pair<double,double> weights = svm.getWeights(); // O(1)
    double w1 = weights.first;
    double w2 = weights.second;
    double b      = svm.getBias();          // O(1)
    auto   svs    = svm.getSupportVectors();// O(n)
    double margin = svm.getMargin();        // O(1)
    double acc    = svm.accuracy(points);   // O(n)

    // Kutu çerçevesi — sabit genişlik 44
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║  SONUCLAR: " << scenario_name;

    // Sağ sütunu hizala
    int pad = 30 - static_cast<int>(scenario_name.size());
    for (int i = 0; i < pad; ++i) std::cout << ' ';
    std::cout << "║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "║  w = [" << w1 << ", " << w2 << "]\n";
    std::cout << "║  b = " << b << "\n";
    std::cout << "║  Karar siniri: "
              << w1 << "*x + " << w2 << "*y + " << b << " = 0\n";
    std::cout << "║  Margin genisligi = 2/||w|| = " << margin << "\n";
    std::cout << "║  Siniflandirma dogrulugu = "
              << acc * 100.0 << "%\n";
    std::cout << "║  Destek Vektorleri (" << svs.size() << " adet):\n";

    // O(s)
    for (const auto& sv : svs) {
        std::cout << "║    " << sv.toString() << "\n";
    }

    std::cout << "╚══════════════════════════════════════════╝\n";
}