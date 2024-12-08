#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Lapangan
{
    int id;
    string nama;
    double harga;
};

struct Jadwal
{
    int id;
    int idLapangan;
    string hari;
    string tanggal;
    string jamMulai;
    string jamSelesai;
    string status = "Tersedia";
};

struct Snack
{
    int id;
    string nama;
    int harga;
};

struct Pesanan
{
    int idPesanan;
    string namaUser;
    string nomorHp;
    int idLapangan;
    Jadwal jam;
    vector<int> snackIds;
    int totalHarga;
    bool statusPembayaran;
};

// Data
vector<Lapangan> lapanganList;
vector<Jadwal> jadwalList;
vector<Snack> snackList;
vector<Pesanan> pesananList;

// Prototipe fungsi
void adminMenu();
void userMenu();
void loginMenu();

// Fungsi admin
void crudLapangan();
void loadLapangan();
void simpanLapangan();

void crudJadwal();
void loadJadwal();
void simpanJadwal();

void crudSnack();
void loadSnack();
void simpanSnack();

void crudPemesanan();
void loadPemesanan();
void simpanPemesanan();

// Fungsi user
void buatPesanan();
void cekPesanan(const string &namaUser);

void clearScreen()
{
// Bersihkan layar (bekerja di terminal UNIX dan Windows)
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main()
{
    loadLapangan();
    loadJadwal();
    loadSnack();
    loadPemesanan();

    loginMenu();

    return 0;
}

void loginMenu()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "===== SISTEM RESERVASI FUTSAL =====\n";
        cout << "1. Login sebagai Admin\n";
        cout << "2. Login sebagai User\n";
        cout << "3. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            adminMenu();
            break;
        case 2:
            userMenu();
            break;
        case 3:
            cout << "Terima kasih!\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 3);
}

void adminMenu()
{
    clearScreen();
    string username, password;
    cout << "\n===== LOGIN ADMIN =====\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    if (username == "admin" && password == "1234")
    {
        int pilihan;
        do
        {
            clearScreen();
            cout << "\n===== MENU ADMIN =====\n";
            cout << "1. Kelola Lapangan\n";
            cout << "2. Kelola Jadwal\n";
            cout << "3. Kelola Snack\n";
            cout << "4. Kelola Pemesanan\n";
            cout << "5. Kembali\n";
            cout << "Pilih: ";
            cin >> pilihan;

            switch (pilihan)
            {
            case 1:
                crudLapangan();
                break;
            case 2:
                crudJadwal();
                break;
            case 3:
                crudSnack();
                break;
            case 4:
                crudPemesanan();
                break;
            case 5:
                cout << "Kembali ke menu utama.\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
            }
        } while (pilihan != 5);
    }
    else
    {
        cout << "Username atau password salah!\n";
    }
}

void perbaruiIDLapangan()
{
    for (size_t i = 0; i < lapanganList.size(); ++i)
    {
        lapanganList[i].id = i + 1; // ID diatur ulang mulai dari 1
    }
}

void crudLapangan()
{
    int pilihan;
    do
    {
        cout << "\n===== KELOLA LAPANGAN =====\n";
        cout << "1. Tambah Lapangan\n";
        cout << "2. Tampilkan Lapangan\n";
        cout << "3. Hapus Lapangan\n";
        cout << "4. Edit Lapangan\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        { // Tambah Lapangan
            Lapangan lap;
            lap.id = lapanganList.size() + 1; // ID dihitung berdasarkan jumlah elemen
            cout << "Nama Lapangan: ";
            cin.ignore();
            getline(cin, lap.nama);
            cout << "Harga Lapangan: ";
            cin >> lap.harga;
            lapanganList.push_back(lap);
            simpanLapangan();
            cout << "Lapangan berhasil ditambahkan.\n";
            break;
        }
        case 2:
        { // Tampilkan Lapangan
            cout << "\nDaftar Lapangan:\n";
            if (lapanganList.empty())
            {
                cout << "Tidak ada lapangan.\n";
            }
            else
            {
                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    cout << lapanganList[i].id << ". " << lapanganList[i].nama
                         << " - Harga: Rp" << lapanganList[i].harga << endl;
                }
            }
            break;
        }
        case 3:
        { // Hapus Lapangan
            int index;
            cout << "Masukkan nomor data yang ingin dihapus: ";
            cin >> index;
            if (index < 1 || index > static_cast<int>(lapanganList.size()))
            {
                cout << "Data tidak ditemukan.\n";
            }
            else
            {
                lapanganList.erase(lapanganList.begin() + index - 1);
                perbaruiIDLapangan(); // Perbarui ID agar tetap berurutan
                cout << "Data berhasil dihapus.\n";
            }
            break;
        }
        case 4:
        { // Edit Lapangan
            int id;
            cout << "ID Lapangan yang ingin diedit: ";
            cin >> id;
            if (id < 1 || id > static_cast<int>(lapanganList.size()))
            {
                cout << "Data tidak ditemukan.\n";
            }
            else
            {
                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    if (lapanganList[i].id == id)
                    {
                        cout << "Nama Lapangan Baru: ";
                        cin.ignore();
                        getline(cin, lapanganList[i].nama);
                        cout << "Harga Lapangan Baru: ";
                        cin >> lapanganList[i].harga;
                        cout << "Lapangan berhasil diedit.\n";
                        break;
                    }
                }
            }
            break;
        }
        case 5: // Kembali
            cout << "Kembali ke menu utama.\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 5);
}
void loadLapangan()
{
    ifstream inFile("lapangan.txt");
    if (inFile.is_open())
    {
        Lapangan lapangan;
        while (inFile >> lapangan.id)
        {
            inFile.ignore(); // Mengabaikan karakter koma
            getline(inFile, lapangan.nama, ',');
            inFile >> lapangan.harga;
            lapanganList.push_back(lapangan);
        }
        inFile.close();
    }
    else
    {
        cout << "Gagal membuka file lapangan.txt.\n";
    }
}

void simpanLapangan()
{
    if (lapanganList.empty())
    {
        cout << "Tidak ada data lapangan untuk disimpan.\n";
        return;
    }

    ofstream outFile("lapangan.txt");
    if (outFile.is_open())
    {
        for (const Lapangan &lapangan : lapanganList)
        { // Hindari auto jika diperlukan
            outFile << lapangan.id << "," << lapangan.nama << "," << lapangan.harga << "\n";
        }
        outFile.close();
        cout << "Daftar lapangan berhasil disimpan ke 'lapangan.txt'.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk menyimpan data lapangan.\n";
    }
}

void perbaruiIDJadwal()
{
    for (size_t i = 0; i < jadwalList.size(); ++i)
    {
        jadwalList[i].id = i + 1; // ID diatur ulang mulai dari 1
    }
}

void tampilkanJadwal()
{
    cout << "\n===== DAFTAR JADWAL =====\n";
    cout << "-----------------------------------------------------------\n";
    cout << "ID  | Lapangan ID | Hari   | Tanggal     | Jam Mulai | Jam Selesai | Status\n";
    cout << "-----------------------------------------------------------\n";
    for (size_t i = 0; i < jadwalList.size(); ++i)
    {
        cout << jadwalList[i].id << "   | "
             << jadwalList[i].idLapangan << "            | "
             << jadwalList[i].hari << "   | "
             << jadwalList[i].tanggal << "  | "
             << jadwalList[i].jamMulai << " | "
             << jadwalList[i].jamSelesai << " | "
             << jadwalList[i].status << "\n";
    }
    cout << "-----------------------------------------------------------\n";
}

void crudJadwal()
{
    int pilihan;

    do
    {
        cout << "\n===== KELOLA JADWAL =====\n";
        cout << "1. Tambah Jadwal\n";
        cout << "2. Tampilkan Jadwal\n";
        cout << "3. Hapus Jadwal\n";
        cout << "4. Edit Jadwal\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        { // Tambah Jadwal
            Jadwal jadwal;
            jadwal.id = jadwalList.size() + 1; // ID dihitung berdasarkan jumlah elemen

            // Menampilkan daftar lapangan yang tersedia
            cout << "Daftar Lapangan yang Tersedia:\n";
            if (lapanganList.empty())
            {
                cout << "Tidak ada lapangan yang tersedia.\n";
            }
            else
            {
                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    cout << lapanganList[i].id << ". " << lapanganList[i].nama << "\n";
                }
            }

            cout << "ID Lapangan: ";
            cin >> jadwal.idLapangan;
            cin.ignore();
            cout << "Hari (contoh: Senin): ";
            getline(cin, jadwal.hari);
            cout << "Tanggal (format DD-MM-YYYY): ";
            getline(cin, jadwal.tanggal);
            cout << "Jam Mulai (format HH:MM): ";
            getline(cin, jadwal.jamMulai);
            cout << "Jam Selesai (format HH:MM): ";
            getline(cin, jadwal.jamSelesai);

            // Set status awal "Tersedia"
            jadwal.status = "Tersedia";

            jadwalList.push_back(jadwal);
            simpanJadwal();
            cout << "Jadwal berhasil ditambahkan.\n";
            break;
        }
        case 2:
        { // Tampilkan Jadwal
            cout << "\nDaftar Jadwal:\n";
            for (size_t i = 0; i < jadwalList.size(); ++i)
            {
                cout << "ID: " << jadwalList[i].id << " | "
                     << "Lapangan ID: " << jadwalList[i].idLapangan << " | "
                     << "Hari: " << jadwalList[i].hari << " | "
                     << "Tanggal: " << jadwalList[i].tanggal << " | "
                     << "Jam: " << jadwalList[i].jamMulai << " - " << jadwalList[i].jamSelesai
                     << " | Status: " << jadwalList[i].status << "\n";
            }
            break;
        }

        case 3:
        { // Hapus Jadwal
            int index;
            cout << "Masukkan nomor data yang ingin dihapus: ";
            cin >> index;
            if (index < 1 || index > static_cast<int>(jadwalList.size()))
            {
                cout << "Data tidak ditemukan.\n";
                break;
            }
            jadwalList.erase(jadwalList.begin() + index - 1);
            perbaruiIDJadwal(); // Perbarui ID agar tetap berurutan
            cout << "Data berhasil dihapus.\n";
            break;
        }
        case 4:
        { // Edit Jadwal
            int id;
            cout << "ID Jadwal yang ingin diedit: ";
            cin >> id;
            bool found = false;
            for (size_t i = 0; i < jadwalList.size(); ++i)
            {
                if (jadwalList[i].id == id)
                {
                    // Jika jadwal sudah dipesan, tidak bisa diedit
                    if (jadwalList[i].status == "Sudah Dipesan")
                    {
                        cout << "Jadwal ini sudah dipesan dan tidak bisa diedit.\n";
                        break;
                    }

                    cout << "Hari Baru (" << jadwalList[i].hari << "): ";
                    cin.ignore();
                    getline(cin, jadwalList[i].hari);
                    cout << "Tanggal Baru (" << jadwalList[i].tanggal << "): ";
                    getline(cin, jadwalList[i].tanggal);
                    cout << "Jam Mulai Baru (" << jadwalList[i].jamMulai << "): ";
                    getline(cin, jadwalList[i].jamMulai);
                    cout << "Jam Selesai Baru (" << jadwalList[i].jamSelesai << "): ";
                    getline(cin, jadwalList[i].jamSelesai);
                    cout << "Status Baru (" << jadwalList[i].status << ") [Tersedia/Sudah Dipesan]: ";
                    getline(cin, jadwalList[i].status);
                    cout << "Jadwal berhasil diedit.\n";
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "Jadwal dengan ID tersebut tidak ditemukan.\n";
            }
            break;
        }

        case 5: // Kembali
            cout << "Kembali ke menu utama.\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 5);
}

void loadJadwal()
{
    ifstream inFile("jadwal.txt"); // Membuka file
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            Jadwal jadwal;
            stringstream ss(line);
            string field;

            // Baca setiap field dari satu baris
            getline(ss, field, ',');
            jadwal.id = stoi(field);

            getline(ss, field, ',');
            jadwal.idLapangan = stoi(field);

            getline(ss, jadwal.hari, ',');

            getline(ss, jadwal.tanggal, ',');

            getline(ss, jadwal.jamMulai, ',');

            getline(ss, jadwal.jamSelesai, ',');

            // Tambahkan ke jadwalList
            jadwalList.push_back(jadwal);
        }
        inFile.close();
        cout << "Jadwal berhasil dimuat dari file.\n";
    }
    else
    {
        cout << "Gagal membuka file jadwal.txt.\n";
    }
}

void simpanJadwal()
{
    ofstream outFile("jadwal.txt"); // Membuka file untuk menulis (overwrite)
    if (outFile.is_open())
    {
        for (const Jadwal &jadwal : jadwalList)
        {
            outFile << jadwal.id << ","
                    << jadwal.idLapangan << ","
                    << jadwal.hari << ","
                    << jadwal.tanggal << ","
                    << jadwal.jamMulai << ","
                    << jadwal.jamSelesai << "\n";
        }
        outFile.close();
        cout << "Jadwal berhasil disimpan ke file.\n";
    }
    else
    {
        cout << "Gagal membuka file jadwal.txt untuk menyimpan data.\n";
    }
}

void perbaruiIDSnack()
{
    for (size_t i = 0; i < snackList.size(); ++i)
    {
        snackList[i].id = i + 1; // ID diatur ulang mulai dari 1
    }
}

void crudSnack()
{
    int pilihan;

    do
    {
        cout << "\n===== KELOLA SNACK =====\n";
        cout << "1. Tambah Snack\n";
        cout << "2. Tampilkan Snack\n";
        cout << "3. Hapus Snack\n";
        cout << "4. Edit Snack\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        { // Tambah Snack
            Snack snack;
            snack.id = snackList.size() + 1; // ID otomatis berdasarkan ukuran list
            cout << "Nama Snack: ";
            cin.ignore();
            getline(cin, snack.nama);
            cout << "Harga Snack: ";
            cin >> snack.harga;

            snackList.push_back(snack);
            simpanJadwal();
            cout << "Snack berhasil ditambahkan.\n";
            break;
        }
        case 2:
        { // Tampilkan Snack
            cout << "\nDaftar Snack:\n";
            if (snackList.empty())
            {
                cout << "Tidak ada snack.\n";
            }
            else
            {
                for (size_t i = 0; i < snackList.size(); ++i)
                {
                    cout << snackList[i].id << ". " << snackList[i].nama
                         << " - Harga: Rp" << snackList[i].harga << endl;
                }
            }
            break;
        }
        case 3:
        { // Hapus Snack
            int id;
            cout << "Masukkan ID Snack yang ingin dihapus: ";
            cin >> id;

            bool found = false;
            for (size_t i = 0; i < snackList.size(); ++i)
            {
                if (snackList[i].id == id)
                {
                    snackList.erase(snackList.begin() + i);
                    perbaruiIDSnack(); // Perbarui ID setelah penghapusan
                    cout << "Snack berhasil dihapus.\n";
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "Snack dengan ID tersebut tidak ditemukan.\n";
            }
            break;
        }
        case 4:
        { // Edit Snack
            int id;
            cout << "Masukkan ID Snack yang ingin diedit: ";
            cin >> id;

            bool found = false;
            for (size_t i = 0; i < snackList.size(); ++i)
            {
                if (snackList[i].id == id)
                {
                    cout << "Nama Snack Baru (" << snackList[i].nama << "): ";
                    cin.ignore();
                    getline(cin, snackList[i].nama);
                    cout << "Harga Snack Baru (" << snackList[i].harga << "): ";
                    cin >> snackList[i].harga;

                    cout << "Snack berhasil diedit.\n";
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "Snack dengan ID tersebut tidak ditemukan.\n";
            }
            break;
        }
        case 5: // Kembali
            cout << "Kembali ke menu utama.\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 5);
}

void loadSnack()
{
    ifstream inFile("snack.txt");
    if (inFile.is_open())
    {
        snackList.clear(); // Bersihkan daftar sebelum memuat ulang
        string line;
        while (getline(inFile, line))
        {
            Snack snack;
            stringstream ss(line);
            string temp;

            getline(ss, temp, ',');
            snack.id = stoi(temp); // Konversi string ke integer

            getline(ss, snack.nama, ',');

            getline(ss, temp, ',');
            snack.harga = stoi(temp); // Konversi string ke integer

            snackList.push_back(snack);
        }
        inFile.close();
        cout << "Daftar snack berhasil dimuat dari file.\n";
    }
    else
    {
        cout << "Gagal membuka file snack.txt untuk memuat data.\n";
    }
}

void simpanSnack()
{
    ofstream outFile("snack.txt");
    if (outFile.is_open())
    {
        for (const Snack &snack : snackList)
        {
            outFile << snack.id << "," << snack.nama << "," << snack.harga << "\n";
        }
        outFile.close();
        cout << "Daftar snack berhasil disimpan ke file.\n";
    }
    else
    {
        cout << "Gagal membuka file snack.txt untuk menyimpan data.\n";
    }
}

void crudPemesanan()
{
    int pilihan;
    do
    {

        cout << "\n===== KELOLA PEMESANAN =====\n";
        cout << "1. Tampilkan Semua Pesanan\n";
        cout << "2. Proses Pembayaran\n";
        cout << "3. Batalkan Pesanan\n";
        cout << "4. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        { // Tampilkan Semua Pesanan
            cout << "\nDaftar Pesanan:\n";
            if (pesananList.empty())
            {
                cout << "Tidak ada pesanan.\n";
            }
            else
            {
                for (size_t i = 0; i < pesananList.size(); ++i)
                {
                    cout << "Pesanan ID: " << i + 1 << "\n";
                    cout << "Nama: " << pesananList[i].namaUser << "\n";
                    cout << "No hp: " << pesananList[i].nomorHp << "\n";
                    cout << "Lapangan ID: " << pesananList[i].idLapangan << "\n";
                    cout << "Jam: " << pesananList[i].jam.hari << ", " << pesananList[i].jam.tanggal
                         << " (" << pesananList[i].jam.jamMulai << " - " << pesananList[i].jam.jamSelesai << ")\n";
                    cout << "Status Pembayaran: " << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << "\n";
                    cout << "Total Harga: " << pesananList[i].totalHarga << "\n";
                    cout << "------------------------------------\n";
                }
            }
            break;
        }
        case 2:
        { // Proses Pembayaran
            // Menampilkan daftar pesanan yang ada
            cout << "\n===== Daftar Pesanan =====\n";
            if (pesananList.empty())
            {
                cout << "Tidak ada pesanan yang tersedia.\n";
            }
            else
            {
                for (size_t i = 0; i < pesananList.size(); ++i)
                {
                    Pesanan &pesanan = pesananList[i];
                    cout << "ID Pesanan: " << i + 1 << "\n";
                    cout << "Nama Tim: " << pesanan.namaUser << "\n";
                    cout << "Nomor HP: " << pesanan.nomorHp << "\n";
                    cout << "Lapangan ID: " << pesanan.idLapangan << "\n";
                    cout << "Jam: " << pesanan.jam.hari << ", " << pesanan.jam.tanggal
                         << " (" << pesanan.jam.jamMulai << " - " << pesanan.jam.jamSelesai << ")\n";
                    cout << "Status Pembayaran: " << (pesanan.statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << "\n";
                    cout << "Total Harga: Rp " << pesanan.totalHarga << "\n";
                    cout << "--------------------------\n";
                }
            }

            // Meminta input ID pesanan untuk diproses pembayaran
            int idPesanan;
            cout << "Masukkan ID Pesanan yang akan diproses pembayaran: ";
            cin >> idPesanan;

            if (idPesanan > 0 && idPesanan <= static_cast<int>(pesananList.size()))
            {
                Pesanan &pesanan = pesananList[idPesanan - 1]; // Referensi ke pesanan yang dipilih

                if (pesanan.statusPembayaran)
                {
                    cout << "Pesanan sudah dibayar.\n";
                }
                else
                {
                    pesanan.statusPembayaran = true;
                    cout << "Pembayaran untuk pesanan ID " << idPesanan << " berhasil diproses.\n";
                }
            }
            else
            {
                cout << "Pesanan tidak ditemukan.\n";
            }

            simpanPemesanan();
            break;
        }

        case 3:
        { // Batalkan Pesanan
            int idPesanan;
            cout << "Masukkan ID Pesanan yang akan dibatalkan: ";
            cin >> idPesanan;
            if (idPesanan > 0 && idPesanan <= pesananList.size())
            {
                pesananList.erase(pesananList.begin() + idPesanan - 1);
                cout << "Pesanan ID " << idPesanan << " berhasil dibatalkan.\n";
            }
            else
            {
                cout << "Pesanan tidak ditemukan.\n";
            }
            break;
        }
        case 4: // Kembali
            cout << "Kembali ke menu utama.\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 4);
}

void loadPemesanan()
{
    ifstream inFile("pemesanan.txt");
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            Pesanan pesanan;
            string statusPembayaran;

            // Membaca data dari satu baris
            stringstream ss(line);
            getline(ss, pesanan.namaUser, ',');       // Nama
            getline(ss, pesanan.nomorHp, ',');        // Nomor HP
            getline(ss, pesanan.jam.hari, ',');       // Hari
            getline(ss, pesanan.jam.tanggal, ',');    // Tanggal
            getline(ss, pesanan.jam.jamMulai, ',');   // Jam Mulai
            getline(ss, pesanan.jam.jamSelesai, ','); // Jam Selesai
            getline(ss, statusPembayaran, ',');       // Status Pembayaran

            pesanan.statusPembayaran = (statusPembayaran == "Dibayar");
            pesananList.push_back(pesanan); // Menambahkan ke list
        }
        inFile.close();
        cout << "Data pemesanan berhasil dimuat.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk memuat data pemesanan.\n";
    }
}

void simpanPemesanan()
{
    ofstream outFile("pemesanan.txt", ios::app); // Mode append untuk menambahkan data baru
    if (outFile.is_open())
    {
        for (const Pesanan &pesanan : pesananList)
        {
            outFile << pesanan.namaUser << ","
                    << pesanan.nomorHp << ","
                    << pesanan.jam.hari << ","
                    << pesanan.jam.tanggal << ","
                    << pesanan.jam.jamMulai << ","
                    << pesanan.jam.jamSelesai << ","
                    << (pesanan.statusPembayaran ? "Dibayar" : "Belum Dibayar") << "\n";
        }
        outFile.close();
        cout << "Data pemesanan berhasil disimpan.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk menyimpan data pemesanan.\n";
    }
}

void userMenu()
{
    int pilihan;
    string namaUser;
    clearScreen();

    do
    {
        cout << "\n===== MENU USER =====\n";
        cout << "1. Buat Pesanan\n";
        cout << "2. Cek Pesanan\n";
        cout << "3. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            buatPesanan();
            break;
        case 2:
            cekPesanan(namaUser);
            break;
        case 3:
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 3);
}

// Fungsi untuk memformat nomor telepon ke format internasional
string formatNomorInternasional()
{
    string nomor;
    while (true)
    {
        cout << "Masukkan nomor HP (dalam format lokal atau internasional): ";
        cin >> nomor;

        // Memeriksa apakah nomor sudah dalam format internasional
        if (nomor.substr(0, 3) == "+62")
        {
            return nomor; // Nomor sudah dalam format internasional
        }
        // Memeriksa apakah nomor dimulai dengan 0, jika iya, ubah ke format internasional
        else if (nomor[0] == '0')
        {
            return "+62" + nomor.substr(1); // Ubah nomor lokal menjadi format internasional
        }
        else
        {
            // Jika nomor tidak valid, tampilkan pesan dan ulangi input
            cerr << "Nomor tidak valid. Pastikan dimulai dengan +62 atau 0.\n";
        }
    }
}

// Fungsi untuk mengirim pesan WhatsApp
void kirimPesanWhatsApp(
    const string &nomor,
    const string &idPesanan,
    const string &nama,
    const string &lapanganID,
    const string &jam,
    const string &status,
    const string &totalHarga)
{
    string sid = "AC525efca105257724b6b5a5be52d71215";        // Twilio Account SID
    string token = "ae3e58d497ca7bb68b504aa82d38fcdb";        // Twilio Auth Token
    string contentSid = "HXe3a4cf8a57187688c94df5b4e3a2c6b0"; // ContentSid untuk template
    string url = "https://api.twilio.com/2010-04-01/Accounts/" + sid + "/Messages.json";

    // Buat ContentVariables JSON sesuai dengan parameter
    string contentVariables = "{\"1\":\"" + idPesanan +
                              "\",\"2\":\"" + nama +
                              "\",\"3\":\"" + nomor +
                              "\",\"4\":\"" + lapanganID +
                              "\",\"5\":\"" + jam +
                              "\",\"6\":\"" + status +
                              "\",\"7\":\"" + totalHarga + "\"}";

    // Perintah curl untuk mengirim pesan menggunakan Twilio
    string perintah = "curl -X POST '" + url + "' "
                                               "--data-urlencode 'To=whatsapp:" +
                      nomor + "' "
                              "--data-urlencode 'From=whatsapp:+14155238886' " // Nomor pengirim Twilio
                              "--data-urlencode 'ContentSid=" +
                      contentSid + "' "
                                   "--data-urlencode 'ContentVariables=" +
                      contentVariables + "' "
                                         "-u " +
                      sid + ":" + token;

    // Jalankan perintah curl
    cout << "Mengirim pesan ke WhatsApp...\n";
    int statusCurl = system(perintah.c_str());

    // Cek status pengiriman
    if (statusCurl != 0)
    {
        cerr << "Gagal mengirim pesan. Periksa kredensial atau parameter.\n";
    }
    else
    {
        cout << "Pesan berhasil terkirim ke nomor: " << nomor << "\n";
    }
}

// Fungsi untuk membuat pesanan
void buatPesanan()
{
    Pesanan pesanan;
    clearScreen();
    cout << "\n===== BUAT PESANAN =====\n";

    // Input nama
    cout << "Nama Tim Anda: ";
    cin.ignore(); // Untuk membersihkan buffer setelah membaca integer
    getline(cin, pesanan.namaUser);

    // Input nomor telepon

    pesanan.nomorHp = formatNomorInternasional(); // Format nomor menjadi internasional

    // Menampilkan daftar lapangan
    cout << "\nDaftar Lapangan yang Tersedia:\n";
    for (size_t i = 0; i < lapanganList.size(); ++i)
    {
        cout << lapanganList[i].id << ". " << lapanganList[i].nama << "\n";
    }
    cout << "\nPilih ID Lapangan: ";
    cin >> pesanan.idLapangan;

    // Menampilkan jadwal yang tersedia untuk lapangan yang dipilih
    cout << "\nPilih Jam Tersedia untuk Lapangan ID " << pesanan.idLapangan << ":\n";
    vector<Jadwal> jadwalTersedia;
    for (size_t i = 0; i < jadwalList.size(); ++i)
    {
        if (jadwalList[i].idLapangan == pesanan.idLapangan && jadwalList[i].status == "Tersedia")
        {
            jadwalTersedia.push_back(jadwalList[i]);
        }
    }

    if (jadwalTersedia.empty())
    {
        cout << "Tidak ada jadwal tersedia untuk lapangan ini.\n";
        return;
    }

    // Tampilkan jadwal yang sudah difilter dan urutkan dengan benar
    for (size_t i = 0; i < jadwalTersedia.size(); ++i)
    {
        cout << i + 1 << ". " << jadwalTersedia[i].hari << ", " << jadwalTersedia[i].tanggal
             << " (" << jadwalTersedia[i].jamMulai << " - " << jadwalTersedia[i].jamSelesai << ") - "
             << jadwalTersedia[i].status << "\n";
    }

    int jamIndex;
    while (true)
    {
        cout << "Pilih nomor jadwal: ";
        cin >> jamIndex;

        if (jamIndex >= 1 && jamIndex <= static_cast<int>(jadwalTersedia.size()))
        {
            pesanan.jam = jadwalTersedia[jamIndex - 1]; // Menyimpan jadwal yang dipilih
            // Setelah pemesanan, ubah status jadwal menjadi "Sudah Dipesan"
            for (size_t i = 0; i < jadwalList.size(); ++i)
            {
                if (jadwalList[i].id == pesanan.jam.id)
                {
                    jadwalList[i].status = "Sudah Dipesan"; // Update status jadi sudah dipesan
                    break;
                }
            }
            break;
        }
        else
        {
            cout << "Pilihan tidak valid, silakan pilih nomor jadwal yang benar.\n";
        }
    }

    // Memilih snack
    cout << "\nPilih Snack (masukkan ID, 0 untuk selesai):\n";
    for (size_t i = 0; i < snackList.size(); ++i)
    {
        cout << snackList[i].id << ". " << snackList[i].nama << " (" << snackList[i].harga << ")\n";
    }

    pesanan.totalHarga = lapanganList[pesanan.idLapangan - 1].harga; // Menambahkan harga lapangan ke total
    while (true)
    {
        int snackId;
        cout << "Masukkan ID Snack (0 untuk selesai): ";
        cin >> snackId;
        if (snackId == 0)
            break; // Berhenti jika ID snack 0
        for (size_t i = 0; i < snackList.size(); ++i)
        {
            if (snackList[i].id == snackId)
            {
                int jumlah;
                cout << "Masukkan jumlah " << snackList[i].nama << ": ";
                cin >> jumlah;
                pesanan.snackIds.push_back(snackId);
                pesanan.totalHarga += snackList[i].harga * jumlah; // Menambahkan harga snack ke total
                break;
            }
        }
    }

    pesananList.push_back(pesanan);
    cout << "\nPesanan berhasil dibuat! Total Harga: Rp " << pesanan.totalHarga << "\n";

    // Pilihan pembayaran
    int pilihanBayar;
    cout << "\nPilih opsi pembayaran:\n";
    cout << "1. Bayar Sekarang\n";
    cout << "2. Bayar Nanti\n";
    cout << "Pilih: ";
    cin >> pilihanBayar;

    if (pilihanBayar == 1)
    {
        long long nominalBayar;
        while (true)
        {
            cout << "Masukkan jumlah pembayaran: Rp ";
            cin >> nominalBayar;

            if (nominalBayar < pesanan.totalHarga)
            {
                cout << "Jumlah pembayaran kurang. Silakan masukkan jumlah yang benar.\n";
            }
            else
            {
                long long kembalian = nominalBayar - pesanan.totalHarga;
                cout << "Pembayaran berhasil. Kembalian: Rp " << kembalian << "\n";
                break;
            }
        }
        // Proses pembayaran berhasil
        cout << "\nPembayaran berhasil diproses. Terima kasih!\n";
    }
    else if (pilihanBayar == 2)
    {
        // Pembayaran ditunda
        cout << "\nPembayaran ditunda, pesanan berhasil dibuat dan menunggu pembayaran.\n";
    }
    else
    {
        cout << "\nPilihan tidak valid. Pesanan tetap disimpan dengan status menunggu pembayaran.\n";
    }

    simpanPemesanan();

    // Mengirimkan notifikasi WhatsApp setelah pesanan selesai
    cout << "\nMengirimkan notifikasi WhatsApp...\n";

    // Memastikan data yang benar dikirim ke fungsi kirimPesanWhatsApp
    string statusPembayaran = (pilihanBayar == 1) ? "Sudah Dibayar" : "Belum Dibayar";
    kirimPesanWhatsApp(pesanan.nomorHp, to_string(pesanan.idPesanan), pesanan.namaUser, to_string(pesanan.idLapangan),
                       pesanan.jam.hari + ", " + pesanan.jam.tanggal + " (" + pesanan.jam.jamMulai + " - " + pesanan.jam.jamSelesai + ")",
                       statusPembayaran, to_string(pesanan.totalHarga));

    // Kembali ke menu pengguna
    cout << "\nKembali ke menu pengguna...\n";
    loginMenu(); // Misalnya fungsi ini yang menampilkan menu pengguna
}

void cekPesanan(const string &namaUser)
{
    clearScreen();
    cout << "\n===== CEK PESANAN =====\n";

    // Konversi namaUser ke lowercase untuk perbandingan case-insensitive
    string namaUserLower = namaUser;
    transform(namaUserLower.begin(), namaUserLower.end(), namaUserLower.begin(), ::tolower);

    bool ditemukan = false;
    for (size_t i = 0; i < pesananList.size(); ++i)
    {
        // Konversi namaUser dari daftar pesanan untuk perbandingan
        string namaPesananLower = pesananList[i].namaUser;
        transform(namaPesananLower.begin(), namaPesananLower.end(), namaPesananLower.begin(), ::tolower);

        if (namaPesananLower == namaUserLower)
        {
            ditemukan = true;
            cout << "\n--- Pesanan Ke-" << i + 1 << " ---\n";
            cout << "Nama: " << pesananList[i].namaUser << "\n";
            cout << "Lapangan ID: " << pesananList[i].idLapangan << "\n";
            cout << "Jam: " << pesananList[i].jam.hari << ", " << pesananList[i].jam.tanggal
                 << " (" << pesananList[i].jam.jamMulai << " - " << pesananList[i].jam.jamSelesai << ")\n";

            // Tampilkan daftar snack
            cout << "Snack: ";
            if (!pesananList[i].snackIds.empty())
            {
                for (size_t j = 0; j < pesananList[i].snackIds.size(); ++j)
                {
                    for (size_t k = 0; k < snackList.size(); ++k)
                    {
                        if (snackList[k].id == pesananList[i].snackIds[j])
                        {
                            cout << snackList[k].nama;
                            if (j < pesananList[i].snackIds.size() - 1)
                                cout << ", ";
                        }
                    }
                }
            }
            else
            {
                cout << "Tidak ada snack yang dipesan.";
            }
            cout << "\nTotal Harga: Rp " << pesananList[i].totalHarga << "\n";

            // Tambahkan status pembayaran
            cout << "Status Pembayaran: "
                 << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << "\n";
            cout << "--------------------------\n";
        }
    }

    if (!ditemukan)
    {
        cout << "Pesanan tidak ditemukan untuk nama \"" << namaUser << "\".\n";
    }
}
