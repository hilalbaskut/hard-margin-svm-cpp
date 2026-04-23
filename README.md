# 🚗 Otonom Araç Güvenlik Modülü — Hard-Margin SVM

## 📋 Özet

Bu proje, otonom araçlar için **karar sınırlaması sistemi** olmak üzere tasarlanmış, C++17 standartlarında yazılmış bir **Hard-Margin Support Vector Machine (SVM)** uygulamasıdır. Proje, **SMO (Sequential Minimal Optimization)** algoritması ile iki sınıflı doğrusal ayrılabilir veri kümelerini sınıflandırır. Otonom araç güvenliği sistemlerinde, tehlikeli ve güvenli senaryoları gerçek zamanlı olarak ayırması gereken karar mekanizmaları için kritik öneme sahiptir.

---

## ✨ Teknik Özellikler

### 🎯 Algoritma ve Yöntem
- **SMO Algoritması**: Lagrange dual formülasyonu üzerinden ikili analitik çözüm ile KKT koşullarını tatmin eder
  - Zaman Karmaşıklığı: **O(n² · iter)** → pratik O(n²)
  - Bellek Karmaşıklığı: **O(n)** (kernel matrix saklanmaz — lazy hesaplama)
- **Hard-Margin SVM**: Doğrusal ayrılabilir veri için optimal hiperplana tüm noktaları doğru sınıflandırır

### 🏗️ Mimari Yapı (5 Katmanlı OOP)
- **Katman 1 — Domain (Veri Modeli)**: `Point` sınıfı
- **Katman 2 — Servis (Veri Üretimi)**: `DataGenerator` sınıfı (Mersenne Twister PRNG)
- **Katman 3 — Çekirdek (Algoritma)**: `SVM` sınıfı (SMO implementasyonu)
- **Katman 4 — Sunum (Görselleştirme)**: `Visualizer` sınıfı (ASCII render)
- **Katman 5 — Uygulama (Giriş Noktası)**: `main.cpp` (3 senaryo testi)

### 💾 Bellek Yönetimi (RAII)
- **Sıfır bellek sızıntısı**: Ham işaretçi (`new`/`delete`) kullanılmaz
- **Otomatik temizleme**: std::vector ve RAII ilkesi ile deterministic kaynak yönetimi
- **Stack allocation**: Tüm nesneler stack'te veya STL konteynerlerinde oluşturulur

### 📊 Performans Analizi
- **Tahmin (predict)**: O(1) — önceden hesaplanan ağırlık vektörü
- **Eğitim (train)**: O(n² · iter) — pratik O(n²)
- **Marjin Genişliği**: O(1) — ||w||⁻¹ sabit zamanda hesaplanır
- **Destek Vektörleri**: O(n) — α dizisi taramas

---

## 🏛️ Katmanlı Mimari Detayı

```
main.cpp (Katman 5)
    ├─ Visualizer (Katman 4)
    │   ├─ SVM (Katman 3)
    │   │   └─ Point (Katman 1)
    │   └─ Point (Katman 1)
    │
    ├─ SVM (Katman 3)
    │   └─ Point (Katman 1)
    │
    └─ DataGenerator (Katman 2)
        └─ Point (Katman 1)
```

### Katman Sorumlulukları
| Katman | Sınıf | Görev | Bağımlılık |
|--------|-------|-------|-----------|
| **1** | `Point` | Veri modeli (x, y, label) | Yok |
| **2** | `DataGenerator` | Deterministik veri üretimi (seed=42) | Katman 1 |
| **3** | `SVM` | SMO algoritması + tahmin | Katman 1 |
| **4** | `Visualizer` | ASCII görselleştirme | Katman 1, 3 |
| **5** | `main.cpp` | Senaryo düzenleme & test | Tüm katmanlar |

---

## 🖥️ Demo Çıktısı

### Senaryo 1: Basit Ayrılabilir Veriler

```
████████████████████████████████████████████████
  SENARYO: Senaryo 1 -- Basit Ayrilabilir
████████████████████████████████████████████████

Egitim noktalari (6 adet):
  (1.00, 2.00) label=1
  (2.00, 3.00) label=1
  (3.00, 1.00) label=1
  (5.00, 4.00) label=-1
  (6.00, 5.00) label=-1
  (5.00, 6.00) label=-1

================================================================
  Senaryo 1 -- Basit Ayrilabilir
================================================================
| .....                                                        |
| .........                                                    |
| ###.........                                                 |
| ...###.........                                              |
| ......###.........                      -                    |
| .........###.........                                        |
|    .........###..........                                    |
|       .........####.........                   *             |
|          ..........###.........                              |
|              .........###.........                           |
|                 .........###.........   *                    |
|                    .........###..........                    |
|                   *   .........####.........                 |
|                          ..........###.........              |
|                              .........###.........           |
|            +                    .........###.........        |
|                                    .........###..........    |
|                                       .........####......... |
|                          *               ..........###...... |
|                                              .........###... |
|                                                 .........### |
|                                                    ......... |
|                                                       ...... |
|                                                          ... |
================================================================
  Semboller: [+] Sinif+1  [-] Sinif-1  [*] Destek Vektoru  [#] Karar Siniri  [.] Marjin Bandi

────────────────────────────────────────────────
SONUCLAR: Senaryo 1 -- Basit Ayrilabilir
────────────────────────────────────────────────
  w = [-0.7171, -0.8931]
  b = 5.7713
  Karar siniri: -0.7171*x + -0.8931*y + 5.7713 = 0
  Margin genisligi = 2/||w|| = 1.7461
  Siniflandirma dogrulugu = 100.0000%
  Destek Vektorleri (4 adet):
    (2.00, 3.00) label=1
    (3.00, 1.00) label=1
    (5.00, 4.00) label=-1
    (6.00, 5.00) label=-1
```

### Senaryo 2: Yakın Marjin (Dar Boşluk)

```
████████████████████████████████████████████████
  SENARYO: Senaryo 2 -- Yakin Marjin
████████████████████████████████████████████████

Egitim noktalari (6 adet):
  (1.00, 1.00) label=1
  (2.00, 2.00) label=1
  (1.00, 3.00) label=1
  (3.00, 1.00) label=-1
  (4.00, 2.00) label=-1
  (3.00, 3.00) label=-1

================================================================
  Senaryo 2 -- Yakin Marjin
================================================================
|               ...#....                                       |
|                ...#....                                      |
|                 ...#....                                     |
|                  ...##...                                    |
|                   ...##...                                   |
|                    ....#...                                  |
|               *     ....#...      *                          |
|                      ....#...                                |
|                       ....#...                               |
|                        ....#...                              |
|                         ....#...                             |
|                         +....#...           -                |
|                            ...#...                           |
|                             ...#...                          |
|                              ...#...                         |
|                               ...#....                       |
|               +                ...-....                      |
|                                 ...#....                     |
|                                  ...#....                    |
|                                   ...#....                   |
|                                    ...#....                  |
|                                     ...##...                 |
|                                      ...##...                |
|                                       ....#...               |
================================================================
  Semboller: [+] Sinif+1  [-] Sinif-1  [*] Destek Vektoru  [#] Karar Siniri  [.] Marjin Bandi

────────────────────────────────────────────────
SONUCLAR: Senaryo 2 -- Yakin Marjin
────────────────────────────────────────────────
  w = [-2.6333, -1.3000]
  b = 9.1667
  Karar siniri: -2.6333*x + -1.3000*y + 9.1667 = 0
  Margin genisligi = 2/||w|| = 0.6810
  Siniflandirma dogrulugu = 100.0000%
  Destek Vektorleri (2 adet):
    (1.00, 3.00) label=1
    (3.00, 3.00) label=-1
```

### Senaryo 3: Rastgele Üretilmiş Veriler (seed=42, n=10)

```
████████████████████████████████████████████████
  SENARYO: Senaryo 3 -- Rastgele Uretilmis (seed=42)
████████████████████████████████████████████████

Egitim noktalari (20 adet):
  (-3.17, 0.15) label=1
  (-2.86, 0.41) label=1
  (-3.28, -0.04) label=1
  ...
  (3.34, 0.11) label=-1
  (2.95, 0.36) label=-1

================================================================
  Senaryo 3 -- Rastgele Uretilmis (seed=42)
================================================================
|                 .............#####..............             |
|                 .............#####..............             |
|                 .............#####.............              |
|                 .............#####.............              |
|                 .............#####.............              |
|                 .............#####.............              |
|                ..............#####.............              |
|                ..............#####.............              |
|                ..............#####.............-             |
|              * ..............#####.............              |
|                ..............####...........-..-             |
|            +   ..............####..............   -          |
|           +++  ..............####............-.-  *          |
|         +   +* ..............####..............  -           |
|                ..............####..............- -           |
|                ..............####..............              |
|                ..............####..............              |
|                ..............####..............              |
|                .............#####..............              |
|                .............#####..............              |
|                .............#####..............              |
|                .............#####..............              |
|                .............#####.............               |
|                .............#####.............               |
================================================================
  Semboller: [+] Sinif+1  [-] Sinif-1  [*] Destek Vektoru  [#] Karar Siniri  [.] Marjin Bandi

────────────────────────────────────────────────
SONUCLAR: Senaryo 3 -- Rastgele Uretilmis (seed=42)
────────────────────────────────────────────────
  w = [-0.3918, 0.0201]
  b = 0.0192
  Karar siniri: -0.3918*x + 0.0201*y + 0.0192 = 0
  Margin genisligi = 2/||w|| = 5.0974
  Siniflandirma dogrulugu = 100.0000%
  Destek Vektorleri (3 adet):
    (-2.86, 0.41) label=1
    (-2.88, -0.28) label=1
    (3.34, -0.03) label=-1
```

---

## 🔨 Derleme Talimatları

### Adım 1: Ön Hazırlık
Tüm kaynak dosyaları aynı dizinde bulunduğundan emin olun:
```bash
dir /B
# Çıktı:
# DataGenerator.cpp
# DataGenerator.h
# main.cpp
# Point.h
# SVM.cpp
# SVM.h
# Visualizer.cpp
# Visualizer.h
```

### Adım 2: Derle
```bash
g++ -std=c++17 -O2 -o svm main.cpp DataGenerator.cpp SVM.cpp Visualizer.cpp
```

**Bayrakların Anlamı:**
- `-std=c++17`: C++17 standartını etkinleştir (std::optional, structured bindings vb. için)
- `-O2`: İkinci seviye optimizasyon (hız/boyut dengesi)
- `-o svm`: Çıktı dosyasını `svm.exe` olarak adlandır

### Adım 3: Çalıştır
```bash
./svm
# veya Windows PowerShell'de:
.\svm.exe
```

---

## 📊 Big-O Analiz Tablosu

| Operasyon | Zaman Karmaşıklığı | Uzay Karmaşıklığı | Açıklama |
|-----------|-------------------|------------------|----------|
| `train()` — SMO | **O(n² · iter)** <br/> worst: O(n³) | **O(n)** | n² nokta çifti, her iter'da αᵢ ve αⱼ güncelle. Kernel matrix saklanmaz (lazy). |
| `predict(p)` | **O(1)** | **O(1)** | w vektörü önceden hesaplanmış; tek dot product. |
| `getSupportVectors()` | **O(n)** | **O(s)** | α dizisini tarası (αᵢ > 0 olanları filtrele); s ≤ n. |
| `getMargin()` | **O(1)** | **O(1)** | 2 / √(w₁² + w₂²) doğrudan hesap. |
| `accuracy()` | **O(n)** | **O(1)** | Her n nokta için tahmin; toplama. |
| `Visualizer::draw()` | **O(W·H + n)** | **O(W·H)** | W=60, H=24 ızgara (sabit) + n nokta; W·H = 1440. |
| `DataGenerator::generate(n)` | **O(n)** | **O(n)** | 2n nokta oluştur (n PRNG çekimi per sınıf). |
| **Genel Program** | **O(n² · iter)** | **O(n)** | Egemen terim: train(). iter=2000 (sabit). |

### Notlar:
- **n**: Eğitim noktası sayısı
- **s**: Destek vektörü sayısı (s ≤ n)
- **iter**: SMO iterasyon sayısı (maksimum: 2000)
- **W, H**: ASCII ızgara boyutu (sabit: 60×24)
---

## 🎓 Akademik Açıklamalar

### Hard-Margin SVM Matematiksel Formülasyon

**Primal Problem:**
$$\min_{w,b} \frac{1}{2}\|w\|^2$$
$$\text{s.t. } y_i(w \cdot x_i + b) \geq 1 \quad \forall i$$

**Dual Problem (Lagrange):**
$$\max_{\alpha} W(\alpha) = \sum_i \alpha_i - \frac{1}{2}\sum_i\sum_j \alpha_i\alpha_j y_i y_j (x_i \cdot x_j)$$
$$\text{s.t. } \sum_i \alpha_i y_i = 0, \quad \alpha_i \geq 0$$

**Çözüm:**
$$w = \sum_i \alpha_i y_i x_i$$
$$b = \frac{1}{n_s}\sum_{s \in SV} \left(y_s - w \cdot x_s\right)$$
$$\text{Margin} = \frac{2}{\|w\|}$$

### SMO Algoritması Özeti

1. **Başlatma**: αᵢ = 0 ∀i
2. **Iterasyon** (max 2000):
   - KKT ihlali olan αᵢ seç → O(n)
   - Heuristic: |Eᵢ - Eⱼ| maks olan αⱼ seç → O(n)
   - İkili analitik çözüm:
     - η = 2K_{ij} - K_{ii} - K_{jj}
     - α_j^{new} = α_j - y_j(E_i - E_j) / η
     - Kutuya kırp: 0 ≤ α_j ≤ C
     - α_i^{new} = α_i + y_i y_j (α_j^{old} - α_j^{new})
   - w ve b güncelle
3. **Çıkış**: Değişim < tol (1e-4) ⟹ Yakınsamış

### Otonom Araç Uygulaması
Otonom araçlarda güvenlik sistemleri, sensör verilerini *güvenli* ve *tehlikeli* bölgelere ayırmalıdır. Bu SVM modeli:
- ✅ **Hızlı karar**: O(1) tahmin (real-time)
- ✅ **Geniş marjin**: Gürültüye karşı robust
- ✅ **Deterministic**: Seed=42 ile tekrarlanabilir
- ✅ **Bellek-verimli**: RAII ile sızıntısız

---

## 📝 Kodlama Standartları

### Kullanılan Prensipler
1. **RAII (Resource Acquisition Is Initialization)**
   - Ham işaretçi yok
   - std::vector, std::string otomatik yönetim
   
2. **Single Responsibility (Tek Sorumluluk)**
   - Her sınıf bir katmanda bir görevi yapar
   
3. **Separation of Concerns (Kaygıların Ayrılması)**
   - Veri modeli (Point) ≠ Algoritma (SVM) ≠ Sunum (Visualizer)
   
4. **DRY (Don't Repeat Yourself)**
   - Kernel hesaplaması private method: `kernelProduct()`
   - KKT kontrol: `kktViolation()`

### Doxygen Dokümantasyon
Tüm public metodlara ait:
```cpp
/**
 * @brief Kısa tanım.
 * @param param1 Parametre açıklaması
 * @return Dönüş değeri açıklaması
 * Big-O ZAMAN: ...
 * Big-O UZAY: ...
 */
```

---

## 🧪 Test Senaryoları

| Senaryo | Veri | Beklenti | Sonuç |
|---------|------|----------|--------|
| **1** — Basit Ayrılabilir | 6 nokta (3+3), geniş boşluk | Geniş marjin (>1.7), %100 doğruluk | ✅ Margin=1.75, 4 SV, 100% |
| **2** — Yakın Marjin | 6 nokta (3+3), dar boşluk | Dar marjin (<0.7), %100 doğruluk | ✅ Margin=0.68, 2 SV, 100% |
| **3** — Rastgele (seed=42) | DataGenerator: 10+10=20 nokta | Çok geniş marjin (>5), %100 doğruluk | ✅ Margin=5.10, 3 SV, 100% |

---

## 🚀 Gelecek İyileştirmeler

- [ ] **Soft-Margin SVM**: Hata toleransı (C parametresi)
- [ ] **Non-linear kernels**: RBF, polynomial çekirdekler
- [ ] **Multi-class**: One-vs-One veya One-vs-Rest stratejileri
- [ ] **GPU Acceleration**: CUDA ile kernel hesaplamaları
- [ ] **Batch Processing**: Büyük veri setleri (n > 10k)

---

## ✉️ İletişim ve Soru-Cevap

Proje hakkında sorularınız için kod yorumlarındaki `@brief`, `@param`, `Big-O` açıklamalarını inceleyiniz. Tüm metodlar tam belgelenmiştir.

---

**Son Güncelleme**: Nisan 2025 | **Versiyon**: 1.0 — Stable Release
