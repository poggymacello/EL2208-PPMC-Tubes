# Panduan lengkap sumber data untuk prediksi kebakaran hutan Indonesia

**NASA FIRMS dan Open-Meteo API adalah dua fondasi data utama yang dapat dikombinasikan untuk membangun model machine learning prediksi kebakaran hutan Indonesia secara komprehensif.** FIRMS menyediakan data hotspot kebakaran global sejak November 2000 dengan latensi hanya ~3 jam, sementara Open-Meteo memberikan akses gratis ke data cuaca historis ERA5 hingga 86 tahun ke belakang (sejak 1940). Keduanya menyediakan API yang mudah diintegrasikan ke pipeline ML. Dengan tambahan dataset seperti Copernicus Fire Weather Index, GFED peat fire data, dan peta gambut Indonesia, proyek ini memiliki seluruh bahan baku yang dibutuhkan untuk model prediksi yang robust.

---

## 1. NASA FIRMS: data hotspot kebakaran real-time dan historis

### Akses utama dan endpoint API

Portal utama FIRMS berada di **https://firms.modaps.eosdis.nasa.gov/** dengan peta interaktif di https://firms.modaps.eosdis.nasa.gov/map/. Seluruh REST API tersedia di base URL `https://firms.modaps.eosdis.nasa.gov/api/` dengan dua endpoint utama yang paling relevan untuk proyek ML:

**Area API** — query berdasarkan bounding box koordinat:
```
https://firms.modaps.eosdis.nasa.gov/api/area/csv/{MAP_KEY}/{SOURCE}/{west,south,east,north}/{DAY_RANGE}
```

**Country API** — query berdasarkan kode negara ISO 3166-1 alpha-3:
```
https://firms.modaps.eosdis.nasa.gov/api/country/csv/{MAP_KEY}/{SOURCE}/{COUNTRY_CODE}/{DAY_RANGE}
```

Untuk data historis, tambahkan parameter tanggal di akhir URL:
```
https://firms.modaps.eosdis.nasa.gov/api/area/csv/{MAP_KEY}/{SOURCE}/{AREA}/{DAY_RANGE}/{YYYY-MM-DD}
```

Endpoint pendukung lainnya meliputi Data Availability API (`/api/data_availability/`), Countries List (`/api/countries/`), WMS (`/mapserver/wms/fires/{MAP_KEY}/`), dan WFS (`/mapserver/wfs/`). Arsip download untuk data lebih dari 7 hari tersedia di https://firms.modaps.eosdis.nasa.gov/download/.

### Dataset yang tersedia

FIRMS mendistribusikan data dari empat instrumen satelit utama, masing-masing dengan varian Near Real-Time (NRT) dan Standard Processing (SP):

| Dataset | ID API (NRT) | Resolusi Spasial | Arsip Mulai |
|---------|-------------|-------------------|-------------|
| MODIS (Terra + Aqua) | `MODIS_NRT` / `MODIS_SP` | **1 km** | **November 2000** |
| VIIRS Suomi-NPP | `VIIRS_SNPP_NRT` / `VIIRS_SNPP_SP` | **375 m** | **Januari 2012** |
| VIIRS NOAA-20 (JPSS-1) | `VIIRS_NOAA20_NRT` / `VIIRS_NOAA20_SP` | **375 m** | **April 2018** |
| VIIRS NOAA-21 (JPSS-2) | `VIIRS_NOAA21_NRT` | **375 m** | **Januari 2024** |
| LANDSAT 8/9 | `LANDSAT_NRT` | **30 m** | Juni 2022 (hanya AS/Kanada) |

MODIS memberikan **25+ tahun data historis** — ideal untuk training set ML. VIIRS menawarkan resolusi spasial **hampir 3x lebih tajam** (375m vs 1km). Produk tambahan mencakup Burned Area (MCD64A1) dan data geostationary dari GOES, SEVIRI, dan Himawari AHI.

### Format download dan coverage

Data tersedia dalam **CSV, JSON, KML, dan Shapefile** melalui API (cukup ubah path `/csv/` menjadi `/json/`, `/kml/`, atau `/shp/`). Archive download juga menyediakan format TXT (tab-delimited). Web services mendukung WMS dan WFS dengan proyeksi EPSG:4326 dan EPSG:3857.

Coverage bersifat **global**, termasuk seluruh wilayah Indonesia. WMS/WFS di-update **setiap 15 menit**. Data NRT tersedia dalam waktu **~3 jam** dari observasi satelit.

### Cara mendapatkan MAP_KEY (API Key)

Proses registrasi sepenuhnya **gratis** dan instan:

1. Buka https://firms.modaps.eosdis.nasa.gov/api/map_key/
2. Masukkan alamat email
3. Submit — MAP_KEY dikirim langsung ke email
4. Gunakan MAP_KEY (string alfanumerik 32 karakter) di setiap request API

Batas penggunaan: **5.000 transaksi per 10 menit**. Cek status di `https://firms.modaps.eosdis.nasa.gov/mapserver/mapkey_status/?MAP_KEY={KEY}`. Untuk arsip download, diperlukan juga NASA Earthdata Login di https://urs.earthdata.nasa.gov/.

### Cara filter data untuk Indonesia

**Metode 1 — Bounding Box:** Gunakan koordinat `95,-11,141,6` (west, south, east, north) yang mencakup seluruh wilayah Indonesia:
```
https://firms.modaps.eosdis.nasa.gov/api/area/csv/{MAP_KEY}/VIIRS_NOAA20_NRT/95,-11,141,6/1
```

**Metode 2 — Country Code:** Gunakan kode `IDN` yang mengikuti batas polygon aktual negara (lebih presisi):
```
https://firms.modaps.eosdis.nasa.gov/api/country/csv/{MAP_KEY}/VIIRS_NOAA20_NRT/IDN/1
```

**Metode 3 — Python:**
```python
import pandas as pd
MAP_KEY = 'your_key_here'
url = f'https://firms.modaps.eosdis.nasa.gov/api/country/csv/{MAP_KEY}/VIIRS_NOAA20_NRT/IDN/1'
df = pd.read_csv(url)
```

Parameter `DAY_RANGE` menerima nilai 1–5 hari per request (Area API) atau 1–10 hari (Country API).

### Confidence score dan kolom data

Kolom **`confidence`** pada data FIRMS memiliki format berbeda antara MODIS dan VIIRS:

- **MODIS**: Nilai integer **0–100** (persentase). Kategori: low (rendah), nominal (sedang), high (tinggi).
- **VIIRS**: Nilai kategoris **`l`** (low), **`n`** (nominal), **`h`** (high). Low sering terkait area Sun glint; high adalah deteksi paling terpercaya.

Untuk training ML, disarankan filter **nominal + high confidence saja** untuk mengurangi false positive. Kolom lengkap data VIIRS meliputi: `latitude`, `longitude`, `bright_ti4` (brightness temperature band I4), `scan`, `track`, `acq_date`, `acq_time`, `satellite`, `instrument`, `confidence`, `version`, `bright_ti5`, **`frp`** (Fire Radiative Power dalam MW — sangat penting sebagai proxy intensitas api), dan `daynight`.

### Dokumentasi resmi utama

- API Documentation: https://firms.modaps.eosdis.nasa.gov/api/
- Area API: https://firms.modaps.eosdis.nasa.gov/api/area/
- Country API: https://firms.modaps.eosdis.nasa.gov/api/country/
- MAP_KEY Registration: https://firms.modaps.eosdis.nasa.gov/api/map_key/
- FAQ: https://www.earthdata.nasa.gov/data/tools/firms/faq
- Atribut Data: https://www.earthdata.nasa.gov/data/tools/firms/active-fire-data-attributes-modis-viirs
- Python Tutorial: https://firms.modaps.eosdis.nasa.gov/content/academy/data_api/firms_api_use.html
- MODIS Fire User Guide (PDF): https://modis-fire.umd.edu/files/MODIS_C6_C6.1_Fire_User_Guide_1.0.pdf

---

## 2. Open-Meteo API: data cuaca gratis tanpa API key

### Akses utama dan endpoint

Website utama: **https://open-meteo.com/**. Open-Meteo adalah layanan cuaca open-source yang sepenuhnya **gratis untuk penggunaan non-komersial tanpa memerlukan API key atau registrasi**. Rate limit free tier: **10.000 request/hari** dan **600 request/menit**.

| API | Endpoint | Dokumentasi |
|-----|----------|-------------|
| Weather Forecast | `https://api.open-meteo.com/v1/forecast` | https://open-meteo.com/en/docs |
| **Historical Weather** | `https://archive-api.open-meteo.com/v1/archive` | https://open-meteo.com/en/docs/historical-weather-api |
| ECMWF Forecast | `https://api.open-meteo.com/v1/ecmwf` | https://open-meteo.com/en/docs/ecmwf-api |
| Ensemble Models | — | https://open-meteo.com/en/docs/ensemble-api |
| Climate Change | — | https://open-meteo.com/en/docs/climate-api |
| Air Quality | — | https://open-meteo.com/en/docs/air-quality-api |
| Geocoding | `https://geocoding-api.open-meteo.com/v1/search` | https://open-meteo.com/en/docs/geocoding-api |
| Elevation | — | https://open-meteo.com/en/docs/elevation-api |

Untuk penggunaan komersial tersedia paket berbayar mulai ~$29/bulan (1 juta request) hingga enterprise, menggunakan domain `customer-api.open-meteo.com` dengan parameter `&apikey=`.

### Variabel cuaca yang relevan untuk prediksi kebakaran

Open-Meteo menyediakan seluruh variabel kritis untuk fire weather modeling. Berikut variabel paling relevan:

| Variabel | Parameter API | Unit | Relevansi Kebakaran |
|----------|--------------|------|---------------------|
| Suhu udara | `temperature_2m` | °C | Input dasar fire weather |
| Kelembapan relatif | `relative_humidity_2m` | % | Kelembapan rendah = risiko tinggi |
| Vapour Pressure Deficit | `vapour_pressure_deficit` | kPa | VPD >1.6 = vegetasi stres |
| Kecepatan angin | `wind_speed_10m` | km/h | Kecepatan penyebaran api |
| Arah angin | `wind_direction_10m` | ° | Arah penyebaran api |
| Gust angin | `wind_gusts_10m` | km/h | Perilaku api ekstrem |
| Presipitasi | `precipitation` | mm | Input kelembapan bahan bakar |
| Evapotranspirasi (FAO) | `et0_fao_evapotranspiration` | mm | Potensi pengeringan standar |
| Kelembapan tanah | `soil_moisture_0_to_1cm` s/d `soil_moisture_27_to_81cm` | m³/m³ | Proxy kelembapan fuel |
| Radiasi matahari | `shortwave_radiation` | W/m² | Pengeringan bahan bakar |
| Tutupan awan | `cloud_cover` | % | Berbanding terbalik dgn radiasi |
| Tekanan permukaan | `surface_pressure` | hPa | Stabilitas atmosfer |

Variabel tersedia dalam resolusi **hourly** dan **daily**. Format response default adalah **JSON**, dengan opsi **CSV** (`&format=csv`), **XLSX** (`&format=xlsx`), dan **FlatBuffers** (`&format=flatbuffers`) untuk performa tinggi.

### ERA5 reanalysis: tersedia lengkap

**Open-Meteo menyediakan akses penuh ke data ERA5** melalui Historical Weather API, menjadikannya alternatif yang jauh lebih mudah dibanding mengakses Copernicus CDS secara langsung:

| Model | Parameter API | Resolusi | Cakupan Waktu |
|-------|--------------|----------|---------------|
| ERA5 | `&models=era5` | **0.25° (~25 km)** | **1940–sekarang** |
| ERA5-Land | `&models=era5_land` | **0.1° (~11 km)** | **1950–sekarang** |
| ERA5-Seamless | `&models=era5_seamless` | Gabungan | Terbaik dari keduanya |

Data di-update harian dengan delay ~5 hari. Ini berarti Anda bisa mengakses **86 tahun data cuaca historis** per titik koordinat mana pun di dunia tanpa registrasi. Contoh query untuk satu titik di Kalimantan:

```
https://archive-api.open-meteo.com/v1/archive?latitude=-1.5&longitude=116.0&start_date=2015-09-01&end_date=2015-11-30&hourly=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m,vapour_pressure_deficit,soil_moisture_0_to_7cm&models=era5&timezone=Asia/Makassar
```

Hingga **1.000 lokasi** dapat di-query sekaligus menggunakan koordinat yang dipisah koma.

### Catatan penting tentang Fire Weather Index

Open-Meteo **tidak menyediakan indeks kebakaran yang sudah dihitung** (FWI, KBDI, Fosberg), namun menyediakan **semua input yang diperlukan** untuk menghitungnya sendiri. Library Python `firedanger` (https://github.com/steidani/FireDanger) dapat menghitung Canadian FWI dari variabel Open-Meteo.

### Dokumentasi resmi

- Docs utama: https://open-meteo.com/en/docs
- Historical Weather API: https://open-meteo.com/en/docs/historical-weather-api
- Pricing: https://open-meteo.com/en/pricing
- GitHub: https://github.com/open-meteo/open-meteo
- Status: https://status.open-meteo.com/
- Lisensi: https://open-meteo.com/en/licence (CC BY 4.0)

---

## 3. Dataset tambahan yang krusial untuk prediksi kebakaran Indonesia

### NASA Earthdata — portal induk untuk semua data penginderaan jauh NASA

NASA Earthdata (https://earthdata.nasa.gov/) adalah platform single sign-on yang menyatukan akses ke **22+ petabyte** data observasi bumi. Registrasi gratis di https://urs.earthdata.nasa.gov/. Dataset paling relevan untuk kebakaran Indonesia:

- **MODIS Burned Area (MCD64A1 v6.1)** — produk area terbakar bulanan resolusi 500m, tersedia sejak November 2000. Akses: https://www.earthdata.nasa.gov/data/catalog/lpcloud-mcd64a1-061
- **MODIS NDVI/EVI (MOD13Q1 v6.1)** — indeks vegetasi 16-harian resolusi 250m, proxy kondisi bahan bakar. EVI lebih sensitif untuk kawasan tropis biomasa tinggi seperti Indonesia. Akses: https://lpdaac.usgs.gov/products/mod13q1v061/
- **MODIS Land Cover (MCD12Q1 v6.1)** — klasifikasi tutupan lahan tahunan 500m. Akses: https://www.earthdata.nasa.gov/data/catalog/lpcloud-mcd12q1-061
- **Earthdata Search**: https://search.earthdata.nasa.gov/ untuk mencari lintas seluruh DAAC
- **AppEEARS**: https://appeears.earthdatacloud.nasa.gov/ untuk subsetting data ke area spesifik

### Copernicus ERA5 — reanalysis iklim resolusi tinggi

ERA5 adalah reanalysis atmosfer generasi ke-5 dari ECMWF, menggabungkan data model dengan observasi menjadi dataset yang konsisten secara global. Meskipun Open-Meteo sudah menyediakan akses ERA5, akses langsung melalui **Climate Data Store** (https://cds.climate.copernicus.eu/) memberikan kontrol lebih besar atas variabel dan format data.

Spesifikasi: resolusi **0.25° (~31 km)**, **137 level atmosfer**, cakupan **1940–sekarang**, dan ERA5-Land pada resolusi **0.1° (~9 km)**. Akses melalui CDS API (`pip install cdsapi`) dengan Personal Access Token dari https://cds.climate.copernicus.eu/how-to-api. ERA5 juga tersedia di Google Earth Engine sebagai `ECMWF/ERA5/DAILY`.

### Sentinel-2 — citra multispektral resolusi tinggi

Sentinel-2 memberikan citra **10–20 meter** dalam **13 band spektral** dengan revisit **5 hari**. Tersedia sejak Juni 2015 melalui **Copernicus Data Space Ecosystem** (https://dataspace.copernicus.eu/) — pengganti Copernicus Open Access Hub yang sudah discontinued. Relevansi utama: perhitungan **NDVI** (kesehatan vegetasi), **NBR** (Normalized Burn Ratio untuk area terbakar), dan **NDMI** (moisture content vegetasi). Juga tersedia di Google Earth Engine (`COPERNICUS/S2_SR`).

### Fire Weather Index dari Copernicus — indeks bahaya kebakaran siap pakai

**Copernicus Emergency Management Service** menyediakan dataset FWI historis global yang sudah dihitung, tersedia di https://ewds.climate.copernicus.eu/datasets/cems-fire-historical-v1. Dataset ini mencakup seluruh komponen sistem Canadian FWI (FFMC, DMC, DC, ISI, BUI, FWI, DSR) **dari tahun 1940 hingga sekarang** pada resolusi 0.25°, dihitung dari data ERA5. Ini menghemat waktu signifikan dibanding menghitung FWI sendiri dari raw weather data. Format NetCDF dan GRIB2, download gratis melalui CDS.

### GFED — data emisi dan area terbakar dengan komponen gambut

**Global Fire Emissions Database** (https://www.globalfiredata.org/) sangat relevan untuk Indonesia karena secara spesifik memodelkan pembakaran lahan gambut. GFED v5 menyediakan data bulanan burned area dan emisi (CO₂, CO, CH₄, PM2.5) pada resolusi 0.25° sejak 1997, termasuk **PeatFraction layer** yang mengidentifikasi kontribusi kebakaran gambut. Peristiwa kebakaran Indonesia 1997 dan 2015 termasuk yang paling intensif dipelajari menggunakan data GFED. Akses via ORNL DAAC: https://daac.ornl.gov/VEGETATION/guides/fire_emissions_v4_R1.html.

### Sumber data lokal Indonesia

Beberapa sumber data Indonesia yang bernilai tinggi untuk ground truth dan konteks lokal:

- **SiPongi (KLHK)**: https://sipongi.gakkum.kehutanan.go.id/ — sistem monitoring kebakaran hutan Kementerian LHK, update setiap 30 menit, data hotspot hingga level desa
- **BRIN/LAPAN Hotspot**: https://hotspot.brin.go.id/ — data hotspot yang diproses secara lokal, termasuk kalkulasi FWI dari data Himawari-8
- **BMKG Open Data**: https://data.bmkg.go.id/ — prakiraan cuaca API (`https://api.bmkg.go.id/publik/prakiraan-cuaca?adm4={kode_wilayah}`) dengan variabel suhu, kelembapan, angin, dan tutupan awan
- **Peta Gambut Indonesia**: https://data.globalforestwatch.org/datasets/indonesia-peat-lands — peta distribusi ~22,5 juta hektar lahan gambut Indonesia (fitur statis penting untuk model ML)
- **Global Forest Watch Indonesia**: https://www.globalforestwatch.org/dashboards/country/IDN/?category=fires — dashboard terintegrasi fire alerts, deforestasi, dan data konsesi (sawit, kayu)
- **Pantau Gambut**: https://en.pantaugambut.id/peat-map/overview — monitoring ekosistem gambut interaktif

---

## Arsitektur data yang direkomendasikan untuk pipeline ML

Untuk membangun model prediksi kebakaran hutan Indonesia yang komprehensif, kombinasi optimal dataset membentuk tiga layer:

**Target variable** — NASA FIRMS menyediakan labeled data hotspot kebakaran (latitude, longitude, FRP, confidence, tanggal) yang dapat dijadikan target klasifikasi atau regresi. Data MODIS sejak 2000 dan VIIRS sejak 2012 memberikan volume training data yang substansial.

**Feature cuaca** — Open-Meteo Historical API dengan model ERA5 menyediakan fitur cuaca hourly (suhu, kelembapan, angin, presipitasi, VPD, soil moisture, radiasi) untuk setiap titik koordinat tanpa biaya atau registrasi. Copernicus FWI historical menambahkan indeks bahaya kebakaran yang sudah pre-computed.

**Feature kontekstual** — MODIS NDVI/EVI sebagai proxy kondisi vegetasi dan fuel load; peta gambut sebagai fitur statis risiko tinggi (53% kebakaran 2015 terjadi di lahan gambut); MODIS Land Cover untuk klasifikasi tutupan lahan; dan data konsesi dari Global Forest Watch untuk konteks land-use.

---

## Kesimpulan

Ekosistem data untuk prediksi kebakaran hutan Indonesia kini sangat matang dan hampir seluruhnya **open access**. NASA FIRMS dengan arsip 25 tahun dan Open-Meteo dengan 86 tahun data ERA5 merupakan kombinasi yang sangat kuat — keduanya gratis, well-documented, dan mudah diintegrasikan via REST API ke pipeline Python. Keunggulan kritis yang sering terlewat: **Copernicus FWI historical dataset sudah menyediakan indeks bahaya kebakaran siap pakai sejak 1940**, mengeliminasi kebutuhan untuk menghitung FWI dari scratch. Untuk konteks lokal Indonesia, peta gambut BRIN/Global Forest Watch dan data SiPongi KLHK memberikan ground truth yang tidak tersedia di dataset global. Strategi paling efisien adalah memulai dengan FIRMS + Open-Meteo sebagai backbone, lalu secara iteratif menambahkan fitur dari NDVI, FWI, dan peta gambut berdasarkan hasil feature importance analysis dari model awal.
