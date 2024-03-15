# Analisis dari permasalahan Car Rental

## Table of Contents

- [Analisis dari permasalahan Car Rental](#analisis-dari-permasalahan-car-rental)
  - [Table of Contents](#table-of-contents)
  - [Problem](#problem)
  - [Objective](#objective)
  - [Events](#events)
  - [List yang diperlukan](#list-yang-diperlukan)
  - [Random number stream](#random-number-stream)
  - [Variabel Sampst yang diperlukan](#variabel-sampst-yang-diperlukan)
  - [Variabel Timest yang diperlukan](#variabel-timest-yang-diperlukan)

## Problem

- Orang datang di lokasi i (dengan i = 1,2,3) dengan rate exponential interarrival times (14, 10, 24) tiap jam.
- Tiap lokasi pake FIFO dengan kapasitas unlimited
- Ada satu bus yang bisa bawa 20 orang dengan kecepatan 30 mph. Lokasi initial di 3 dan pergi counter clockwise (3 - 1 - 2).
- Car rental itu 3. 1 dan 2 itu air terminal. Orang yang datang di 1 dan 2 ingin ke 3.
- Orang yang datang di 3 ingin ke 1 atau 2 dengan probabilitas ke 1 = 0.583 dan ke 2 = 0.417
  Rules ketika bus sampe di lokasi:
- Orang diturunkan FIFO, dengan waktu yang terdistribusi uniform antara 16 - 24 detik
- Orang diambil sesuai kapasitas, dengan waktu yang terdistribusi uniform antara 15 - 25 detik
- Bus berhenti setidaknya 5 menit. Kalau tidak ada orang, berhenti 5 menit terus berangkat.
- Jarak ke lokasi 1 = 4.5 miles, ke lokasi 2 = 1 miles, ke lokasi 3 = 4.5 miles (Counter clockwise)

## Objective

Simulasi 80 jam
a. Rata-rata dan jumlah maksimal di masing-masing antrian
b. Rata-rata dan delay maksimal di masing-masing antrian
c. Rata-rata dan jumlah maksmial di bus
d. Waktu rata-rata, maks, dan minimum bus berhenti di lokasi masing-masing
e. Waktu rata-rata, maks, dan minimum bus melakukan loop
f. Waktu rata-rata, maks, dan minimum orang di dalam sistem ketika datang di lokasi

## Events

<ol>
    <li>Orang datang di lokasi 1</li>
    <li>Orang datang di lokasi 2</li>
    <li>Orang datang di lokasi 3</li>
    <li>Bus datang di lokasi 1</li>
    <li>Bus datang di lokasi 2</li>
    <li>Bus datang di lokasi 3</li>
    <li>Bus depart dari lokasi 1</li>
    <li>Bus depart dari lokasi 2</li>
    <li>Bus depart dari lokasi 3</li>
    <li>Bus load orang di lokasi 1</li>
    <li>Bus load orang di lokasi 2</li>
    <li>Bus load orang di lokasi 3</li>
    <li> Bus unload orang di lokasi 1</li>
    <li> Bus unload orang di lokasi 2</li>
    <li> Bus unload orang di lokasi 3</li>
    <li> Simulasi selesai</li>
</ol>

## List yang diperlukan

| No. | List Name                         | Attribute 1 | Attribute 2 | Attribute 3    | Attribute 4 |
| --- | --------------------------------- | ----------- | ----------- | -------------- | ----------- |
| 1   | List air terminal 1               | time of arrival  | -           | event_location | -           |
| 2   | List air terminal 2               | time of arrival  | -           | event_location | -           |
| 3   | List car rental                   | time of arrival  | -           | event_location | -           |
| 4   | List bus to air terminal 1        | time of arrival  | -           | event_location | -           |
| 5   | List bus to air terminal 2        | time of arrival  | -           | event_location | -           |
| 6   | List bus to car rental            | time of arrival  | -           | event_location | -           |
| 25  | Event list (untuk event schedule) | event_time  | event_type  | event_location | -           |

## Random number stream

1. Stream interarrival time orang di lokasi 1
2. Stream interarrival time orang di lokasi 2
3. Stream interarrival time orang di lokasi 3
4. Stream waktu unload orang dari bus
5. Stream waktu load orang ke bus
6. Stream menentukan tujuan orang dari lokasi 3

## Variabel Sampst yang diperlukan

Karena delay itu dihitung secara diskrit tiap waktunya - tidak bergantung pada event sebelumnya, cuma mengumpulkan statistik di event saat itu saja. Maka pake sampst

1. Sampst delay di lokasi 1 (Buat jawab nomor b)
2. Sampst delay di lokasi 2
3. Sampst delay di lokasi 3
4. Sampst delay bus berhenti di lokasi 1 (Buat jawab nomor d)
5. Sampst delay bus berhenti di lokasi 2
6. Sampst delay bus berhenti di lokasi 3
7. Sampst delay bus melakukan loop (Buat jawab nomor e)
8. Sampst delay orang di dalam sistem (Buat jawab nomor f)

## Variabel Timest yang diperlukan

Karena event di bawah ini kontinu - jumlah antrian selama 80 jam diitung semua. Maka pake timest

1. Timest lokasi 1 (Buat jawab nomor a)
2. Timest lokasi 2
3. Timest lokasi 3
4. Timest bus (Buat jawab nomor c)

## Input

```
[num_location num_air_terminal simulation_length]
[location_1_interarrival_rate location_2_interarrival_rate location_3_interarrival_rate]
[bus_speed bus_capacity bus_stop_time]
[destination_3_probability destination_1_probability destination_2_probability(equals to 1, because of last element)]
[unload_time_min unload_time_max]
[load_time_min load_time_max]
[distance_to_1 distance_to_2 distance_to_3]
```

Menjadi

```
3 2 80
14 10 24
30 20 5
0 0.583 1
16 24
15 25
4.5 1 4.5
```

## Notes and Conventions

1. Semua waktu dalam menit

