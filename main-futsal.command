#!/bin/bash

# Bersihkan layar terminal
clear

# Navigasi ke folder program
cd /Users/alvinnugraha/Documents/AMIKOM/TUGAS/TUGAS/ALPROG/FUTSAL || { 
    echo -e "\033[1;31mGagal mengakses folder program. Pastikan foldernya benar.\033[0m"; 
    exit 1; 
}
# Menampilkan judul aplikasi
echo -e "\033[1;32m▗▄▄▖ ▗▄▄▄▖▗▄▄▖ ▗▖ ▗▖ ▗▄▖ ▗▖ ▗▖    ▗▄▄▄▖▗▖ ▗▖▗▄▄▄▖ ▗▄▄▖ ▗▄▖ ▗▖   \033[0m"
echo -e "\033[1;32m▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌▗▞▘▐▌ ▐▌▐▌ ▐▌    ▐▌   ▐▌ ▐▌  █  ▐▌   ▐▌ ▐▌▐▌   \033[0m"
echo -e "\033[1;32m▐▛▀▚▖▐▛▀▀▘▐▛▀▚▖▐▛▚▖ ▐▛▀▜▌▐▛▀▜▌    ▐▛▀▀▘▐▌ ▐▌  █   ▝▀▚▖▐▛▀▜▌▐▌   \033[0m"
echo -e "\033[1;32m▐▙▄▞▘▐▙▄▄▖▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌   ▝▚▄▞▘  █  ▗▄▄▞▘▐▌ ▐▌▐▙▄▄▖\033[0m"



# Kompilasi file C++ menjadi executable
echo -e "\033[1;33mMengompilasi program...\033[0m"
clang++ -std=c++11 -o main main.cpp -lcurl

# Cek jika kompilasi berhasil
if [ $? -ne 0 ]; then
    echo -e "\033[1;31mTerjadi kesalahan saat kompilasi. Periksa kode Anda.\033[0m"
    exit 1
else
    echo -e "\033[1;32mKompilasi berhasil!\033[0m"
fi

# Menjalankan program
echo -e "\033[1;33mMenjalankan program...\033[0m"
./main

# Cek jika program berjalan dengan sukses
if [ $? -ne 0 ]; then
    echo -e "\033[1;31mTerjadi kesalahan saat menjalankan program.\033[0m"
    exit 1
else
    echo -e "\033[1;32mProgram selesai dijalankan.\033[0m"
fi

# Bersihkan layar dan tampilkan pesan akhir
clear
echo -e "\033[1;34m===========================================\033[0m"
echo -e "\033[1;32m         TERIMA KASIH TELAH MENGGUNAKAN    \033[0m"
echo -e "\033[1;32m            APLIKASI FUTSAL MANAGER        \033[0m"
echo -e "\033[1;34m===========================================\033[0m"
