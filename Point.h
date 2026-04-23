/**
 * @file Point.h
 * @brief 2D koordinat ve sınıf etiketini tutan veri modeli.
 *
 * Katmanlı Mimari — Katman 1: Domain / Veri Modeli
 * Bu sınıf hiçbir katmana bağımlı değildir (en alt katman).
 *
 * @author  Otonom Araç Güvenlik Modülü Projesi
 * @version 1.0
 */

#ifndef POINT_H
#define POINT_H

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @class Point
 * @brief 2 boyutlu koordinat ve sınıf etiketini tutan veri sınıfı.
 *
 * Tek Sorumluluk: Koordinat verisi + temel vektör işlemleri.
 *
 * RAII: Ham üye değişken yok; destructor gereksiz,
 *       derleyici otomatik üretir.
 *
 * Big-O uzay: O(1) — sabit boyut (x, y, label).
 */
class Point {
public:
    double x;     ///< X koordinatı
    double y;     ///< Y koordinatı
    int    label; ///< Sınıf etiketi: +1 veya -1

    /**
     * @brief Parametreli yapıcı.
     * @param x_     X koordinatı
     * @param y_     Y koordinatı
     * @param label_ Sınıf etiketi (+1 veya -1)
     */
    Point(double x_, double y_, int label_)
        : x(x_), y(y_), label(label_) {}

    /**
     * @brief İki nokta arasındaki iç çarpım (dot product).
     * @param other Diğer nokta
     * @return w · x skalar değeri
     *
     * Big-O ZAMAN: O(d) — d = boyut sayısı = 2 (sabit) → O(1).
     * Big-O UZAY:  O(1) — ek bellek yok.
     */
    double dot(const Point& other) const {
        // Big-O: O(d) = O(1) for d=2
        return x * other.x + y * other.y;
    }

    /**
     * @brief Nokta bilgisini okunabilir string olarak döndürür.
     * @return "(x.xx, y.yy) label=±1" formatında string
     *
     * Big-O: O(1) — sabit uzunlukta formatlama.
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << "(" << std::fixed << std::setprecision(2)
            << x << ", " << y << ") label=" << label;
        return oss.str();
    }
};

#endif // POINT_H
