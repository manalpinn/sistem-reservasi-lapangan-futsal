#!/bin/bash
# Navigasi ke folder program
cd /Users/alvinnugraha/Documents/AMIKOM/TUGAS/TUGAS/ALPROG/FUTSAL

# Kompilasi file C++ menjadi executable
clang++ -std=c++11 -o main main.cpp -lcurl

# Cek jika kompilasi berhasil
if [ $? -ne 0 ]; then
    echo "Terjadi kesalahan saat kompilasi."
    exit 1
fi

# Menjalankan program
./main


