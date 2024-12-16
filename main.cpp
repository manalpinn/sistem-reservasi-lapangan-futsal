#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <limits>
#include <curl/curl.h>

using namespace std;

struct Lapangan
{
    int id;
    string nama;
    double harga;
    string status;
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
void cekPesanan();

void clearScreen()
{
// Bersihkan layar (bekerja di terminal UNIX dan Windows)
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int generateId()
{
    int idNumber = pesananList.size() + 1; // ID dimulai dari 1

    // Ambil tanggal saat ini
    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Format tanggal (YYYYMMDD)
    int year = 1900 + ltm->tm_year; // Tahun
    int month = 1 + ltm->tm_mon;    // Bulan
    int day = ltm->tm_mday;         // Tanggal

    // Gabungkan elemen untuk menghasilkan ID dalam bentuk integer
    int generatedId = year * 10000 + month * 100 + day; // YYYYMMDD

    // Gabungkan dengan idNumber (urutan pesanan)
    generatedId = generatedId * 100 + idNumber; // Format: YYYYMMDD<idNumber> (sebagai integer)

    return generatedId;
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
        cout << R"(
▗▄▄▖ ▗▄▄▄▖▗▄▄▖ ▗▖ ▗▖ ▗▄▖ ▗▖ ▗▖    ▗▄▄▄▖▗▖ ▗▖▗▄▄▄▖ ▗▄▄▖ ▗▄▖ ▗▖   
▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌▗▞▘▐▌ ▐▌▐▌ ▐▌    ▐▌   ▐▌ ▐▌  █  ▐▌   ▐▌ ▐▌▐▌   
▐▛▀▚▖▐▛▀▀▘▐▛▀▚▖▐▛▚▖ ▐▛▀▜▌▐▛▀▜▌    ▐▛▀▀▘▐▌ ▐▌  █   ▝▀▚▖▐▛▀▜▌▐▌   
▐▙▄▞▘▐▙▄▄▖▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌   ▝▚▄▞▘  █  ▗▄▄▞▘▐▌ ▐▌▐▙▄▄▖                                                            
        )" << endl;
        cout << "================================================================\n";
        cout << "1. Login sebagai Admin\n";
        cout << "2. Login sebagai User\n";
        cout << "3. Keluar\n";
        cout << "================================================================\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (cin.fail())
        {
            cin.clear();                                         // Membersihkan status kesalahan
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Mengabaikan sisa input yang salah
            cout << "Input tidak valid. Harap masukkan angka yang benar.\n";
            continue; // Kembali ke awal do-while untuk meminta input lagi
        }

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
        cout << "\nTekan Enter untuk melanjutkan...\n";
        cin.ignore(); // Membersihkan newline di buffer
        cin.get();    // Menunggu input Enter dari pengguna

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
        clearScreen();
        int pilihan;
        do
        {

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
void tampilkanLapangan()
{

    if (lapanganList.empty())
    {
        cout << "Belum ada data lapangan yang tersedia." << endl;
        return;
    }
    // Menampilkan header tabel
    cout << left << setw(5) << "ID" << setw(25) << "Nama Lapangan" << setw(15) << "Harga" << setw(15) << "Status" << endl;
    cout << "--------------------------------------------------" << endl;

    // Menampilkan data lapangan dalam format tabel
    for (size_t i = 0; i < lapanganList.size(); ++i)
    {
        cout << left
             << setw(5) << lapanganList[i].id               // Lebar 5 untuk ID
             << setw(25) << lapanganList[i].nama            // Lebar 25 untuk Nama Lapangan
             << "Rp. " << setw(10) << lapanganList[i].harga // Lebar 15 untuk Harga dengan "Rp"
             << setw(15) << lapanganList[i].status
             << endl;
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
            lap.status = "Tersedia";
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
                tampilkanLapangan();
            }
            break;
        }
        case 3:
        { // Hapus Lapangan
            if (lapanganList.empty())
            {
                cout << "Tidak ada data lapangan untuk dihapus.\n";
                break;
            }

            cout << "\nDaftar Lapangan:\n";
            tampilkanLapangan();
            int id;
            bool found = false;

            // Looping untuk meminta ID hingga valid
            while (!found)
            {
                cout << "Masukkan ID Lapangan yang ingin dihapus: ";
                cin >> id;

                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    if (lapanganList[i].id == id)
                    {
                        // Konfirmasi sebelum menghapus
                        cout << "Anda yakin ingin menghapus \"" << lapanganList[i].nama << "\"? (y/n): ";
                        char konfirmasi;
                        cin >> konfirmasi;

                        if (konfirmasi == 'y' || konfirmasi == 'Y')
                        {
                            lapanganList.erase(lapanganList.begin() + i);
                            perbaruiIDLapangan(); // Perbarui ID agar tetap berurutan
                            simpanLapangan();
                            cout << "Data berhasil dihapus.\n";
                        }
                        else
                        {
                            cout << "Penghapusan dibatalkan.\n";
                        }
                        found = true; // Keluar dari loop jika ID ditemukan
                        break;
                    }
                }

                if (!found)
                {
                    cout << "ID Lapangan tidak ditemukan. Silakan coba lagi.\n";
                }
            }
            break;
        }

        case 4:
        { // Edit Lapangan
            if (lapanganList.empty())
            {
                cout << "Tidak ada data lapangan untuk diedit.\n";
                break;
            }

            tampilkanLapangan();

            int id;
            bool found = false;

            // Looping hingga pengguna memasukkan ID yang valid
            while (!found)
            {
                cout << "Masukkan ID Lapangan yang ingin diedit: ";
                cin >> id;

                // Cari ID dalam daftar
                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    if (lapanganList[i].id == id)
                    {
                        found = true;

                        // Input nama lapangan baru
                        cout << "Nama Lapangan Baru (" << lapanganList[i].nama << "): ";
                        cin.ignore();
                        getline(cin, lapanganList[i].nama);

                        // Input harga lapangan baru
                        cout << "Harga Lapangan Baru (" << lapanganList[i].harga << "): ";
                        while (!(cin >> lapanganList[i].harga) || lapanganList[i].harga <= 0)
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Harga tidak valid. Masukkan angka positif.\nHarga Lapangan Baru: ";
                        }

                        // Input status lapangan baru
                        cout << "Status Lapangan Baru (" << lapanganList[i].status << ") [Tersedia/Penuh]: ";
                        cin.ignore();
                        string status;
                        while (true)
                        {
                            getline(cin, status);
                            if (status == "Tersedia" || status == "Penuh")
                            {
                                lapanganList[i].status = status;
                                break;
                            }
                            else
                            {
                                cout << "Status tidak valid. Masukkan 'Tersedia' atau 'Penuh': ";
                            }
                        }

                        simpanLapangan();
                        cout << "Lapangan berhasil diedit.\n";
                        break;
                    }
                }

                if (!found)
                {
                    cout << "ID Lapangan tidak ditemukan. Silakan coba lagi.\n";
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
        lapanganList.clear(); // Menghapus data lapangan yang sudah ada
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            Lapangan lapangan;
            string status;
            getline(ss, line, ',');
            lapangan.id = stoi(line);
            getline(ss, lapangan.nama, ',');
            getline(ss, line, ',');
            lapangan.harga = stoi(line);
            getline(ss, status);
            lapangan.status = status; // Memuat status lapangan
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
        {
            outFile << lapangan.id << "," << lapangan.nama << "," << lapangan.harga << "," << lapangan.status << "\n";
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

string NamaLapangan(const vector<Lapangan> &lapanganList, int idLapangan)
{
    // Menggunakan tipe data eksplisit dalam loop

    for (size_t i = 0; i < lapanganList.size(); ++i)
    {
        if (lapanganList[i].id == idLapangan)
        {
            return lapanganList[i].nama; // Kembalikan nama lapangan yang sesuai
        }
    }
    return "Lapangan Tidak Ditemukan"; // Jika ID tidak ditemukan
}

void tampilkanJadwal()
{

    if (jadwalList.empty())
    {
        cout << "Belum ada data jadwal yang tersedia." << endl;
        return;
    }
    // Menampilkan header tabel dengan format yang rapi
    cout << "\n===== DAFTAR JADWAL =====\n";
    cout << "-----------------------------------------------------------\n";
    cout << left
         << setw(5) << "ID"
         << setw(15) << "Nama Lapangan"
         << setw(10) << "Hari"
         << setw(15) << "Tanggal"
         << setw(12) << "Jam Mulai"
         << setw(12) << "Jam Selesai"
         << setw(10) << "Status"
         << endl;
    cout << "-----------------------------------------------------------\n";

    // Menampilkan data jadwal dalam format tabel
    for (size_t i = 0; i < jadwalList.size(); ++i)

    {
        string namaLapangan = NamaLapangan(lapanganList, jadwalList[i].idLapangan);
        cout << left
             << setw(5) << jadwalList[i].id
             << setw(15) << namaLapangan
             << setw(10) << jadwalList[i].hari
             << setw(15) << jadwalList[i].tanggal
             << setw(12) << jadwalList[i].jamMulai
             << setw(12) << jadwalList[i].jamSelesai
             << setw(10) << jadwalList[i].status
             << endl;
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
            jadwal.id = jadwalList.empty() ? 1 : jadwalList.back().id + 1; // ID unik berdasarkan ID terakhir

            // Menampilkan daftar lapangan yang tersedia
            cout << "Daftar Lapangan yang Tersedia:\n";
            if (lapanganList.empty())
            {
                cout << "Tidak ada lapangan yang tersedia.\n";
                break;
            }
            else
            {
                for (const auto &lapangan : lapanganList)
                {
                    cout << lapangan.id << ". " << lapangan.nama << "\n";
                }
            }

            // Validasi ID Lapangan
            while (true)
            {
                cout << "Masukkan ID Lapangan: ";
                cin >> jadwal.idLapangan;
                bool idValid = false;
                for (const auto &lapangan : lapanganList)
                {
                    if (jadwal.idLapangan == lapangan.id)
                    {
                        idValid = true;
                        break;
                    }
                }
                if (idValid)
                    break;
                else
                    cout << "ID Lapangan tidak valid. Silakan masukkan ID yang tersedia.\n";
            }
            cin.ignore(); // Membersihkan buffer input

            // Validasi Hari
            while (true)
            {
                cout << "Masukkan Hari (contoh: Senin): ";
                getline(cin, jadwal.hari);
                if (regex_match(jadwal.hari, regex("^[A-Za-z]+$"))) // Hanya huruf
                    break;
                else
                    cout << "Input hari tidak valid. Masukkan hanya nama hari (contoh: Senin).\n";
            }

            // Validasi Tanggal
            while (true)
            {
                cout << "Masukkan Tanggal (format DD-MM-YYYY): ";
                getline(cin, jadwal.tanggal);
                if (regex_match(jadwal.tanggal, regex("^\\d{2}-\\d{2}-\\d{4}$"))) // Format tanggal
                    break;
                else
                    cout << "Format tanggal tidak valid. Silakan masukkan sesuai format (DD-MM-YYYY).\n";
            }

            // Validasi Jam Mulai
            while (true)
            {
                cout << "Masukkan Jam Mulai (format HH:MM): ";
                getline(cin, jadwal.jamMulai);
                if (regex_match(jadwal.jamMulai, regex("^\\d{2}:\\d{2}$"))) // Format jam
                    break;
                else
                    cout << "Format jam tidak valid. Silakan masukkan sesuai format (HH:MM).\n";
            }

            // Validasi Jam Selesai
            while (true)
            {
                cout << "Masukkan Jam Selesai (format HH:MM): ";
                getline(cin, jadwal.jamSelesai);
                if (regex_match(jadwal.jamSelesai, regex("^\\d{2}:\\d{2}$")))
                {
                    if (jadwal.jamSelesai > jadwal.jamMulai)
                        break;
                    else
                        cout << "Jam Selesai harus lebih besar dari Jam Mulai. Silakan masukkan ulang.\n";
                }
                else
                    cout << "Format jam tidak valid. Silakan masukkan sesuai format (HH:MM).\n";
            }

            // Set status awal sebagai "Tersedia"
            jadwal.status = "Tersedia";

            // Tambahkan ke daftar jadwal
            jadwalList.push_back(jadwal);

            // Simpan ke file
            simpanJadwal();
            cout << "Jadwal berhasil ditambahkan.\n";
            break;
        }

        case 2:
        { // Tampilkan Jadwal
            cout << "\nDaftar Jadwal:\n";
            tampilkanJadwal();
            break;
        }

        case 3:
        {
            // Hapus Jadwal
            if (jadwalList.empty())
            {
                cout << "Tidak ada jadwal yang tersedia untuk dihapus.\n";
                break;
            }

            tampilkanJadwal(); // Tampilkan daftar jadwal
            int index;

            // Loop untuk validasi input ID jadwal
            while (true)
            {
                cout << "Masukkan nomor data yang ingin dihapus: ";
                cin >> index;

                // Validasi apakah ID jadwal ada
                if (index >= 1 && index <= static_cast<int>(jadwalList.size()))
                {
                    // Tampilkan detail jadwal yang akan dihapus
                    cout << "Anda yakin ingin menghapus jadwal berikut?\n";
                    cout << "Hari: " << jadwalList[index - 1].hari << "\n";
                    cout << "Konfirmasi (y/n): ";
                    char konfirmasi;
                    cin >> konfirmasi;

                    if (konfirmasi == 'y' || konfirmasi == 'Y')
                    {
                        // Jika konfirmasi ya, hapus data
                        jadwalList.erase(jadwalList.begin() + index - 1);
                        perbaruiIDJadwal(); // Perbarui ID agar tetap berurutan
                        cout << "Data berhasil dihapus.\n";
                    }
                    else
                    {
                        // Jika konfirmasi tidak, pembatalan
                        cout << "Penghapusan dibatalkan.\n";
                    }
                    break; // Keluar dari loop setelah valid atau batal
                }
                else
                {
                    cout << "Nomor jadwal tidak ditemukan. Silakan masukkan nomor yang benar.\n";
                }
            }

            break;
        }

        case 4:
        { // Edit Jadwal
            tampilkanJadwal();
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
                        found = true;
                        break;
                    }

                    // Validasi hari baru
                    cout << "Hari Baru (" << jadwalList[i].hari << "): ";
                    cin.ignore();
                    getline(cin, jadwalList[i].hari);

                    // Validasi tanggal baru
                    while (true)
                    {
                        cout << "Tanggal Baru (format DD-MM-YYYY, " << jadwalList[i].tanggal << "): ";
                        string tanggalBaru;
                        getline(cin, tanggalBaru);
                        if (regex_match(tanggalBaru, regex("\\d{2}-\\d{2}-\\d{4}")))
                        {
                            jadwalList[i].tanggal = tanggalBaru;
                            break;
                        }
                        else
                        {
                            cout << "Format tanggal tidak valid. Silakan masukkan lagi.\n";
                        }
                    }

                    // Validasi jam mulai baru
                    while (true)
                    {
                        cout << "Jam Mulai Baru (format HH:MM, " << jadwalList[i].jamMulai << "): ";
                        string jamMulaiBaru;
                        getline(cin, jamMulaiBaru);
                        if (regex_match(jamMulaiBaru, regex("\\d{2}:\\d{2}")))
                        {
                            jadwalList[i].jamMulai = jamMulaiBaru;
                            break;
                        }
                        else
                        {
                            cout << "Format jam tidak valid. Silakan masukkan lagi.\n";
                        }
                    }

                    // Validasi jam selesai baru
                    while (true)
                    {
                        cout << "Jam Selesai Baru (format HH:MM, " << jadwalList[i].jamSelesai << "): ";
                        string jamSelesaiBaru;
                        getline(cin, jamSelesaiBaru);
                        if (regex_match(jamSelesaiBaru, regex("\\d{2}:\\d{2}")))
                        {
                            if (jamSelesaiBaru > jadwalList[i].jamMulai)
                            {
                                jadwalList[i].jamSelesai = jamSelesaiBaru;
                                break;
                            }
                            else
                            {
                                cout << "Jam Selesai harus lebih besar dari Jam Mulai. Silakan masukkan lagi.\n";
                            }
                        }
                        else
                        {
                            cout << "Format jam tidak valid. Silakan masukkan lagi.\n";
                        }
                    }

                    // Validasi status baru
                    while (true)
                    {
                        cout << "Status Baru (" << jadwalList[i].status << ") [Tersedia/Sudah Dipesan]: ";
                        string statusBaru;
                        getline(cin, statusBaru);
                        if (statusBaru == "Tersedia" || statusBaru == "Sudah Dipesan")
                        {
                            jadwalList[i].status = statusBaru;
                            break;
                        }
                        else
                        {
                            cout << "Input tidak valid. Masukkan 'Tersedia' atau 'Sudah Dipesan'.\n";
                        }
                    }

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

            getline(ss, jadwal.status);

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
    ofstream file("jadwal.txt");
    if (file.is_open())
    {
        for (const auto &jadwal : jadwalList)
        {
            file << jadwal.id << ",";
            file << jadwal.idLapangan << ",";
            file << jadwal.hari << ",";
            file << jadwal.tanggal << ",";
            file << jadwal.jamMulai << ",";
            file << jadwal.jamSelesai << ",";
            file << jadwal.status << "\n"; // Simpan status jadwal
        }
        file.close();
    }
    else
    {
        cout << "Gagal menyimpan data jadwal!\n";
    }
}

void perbaruiIDSnack()
{
    for (size_t i = 0; i < snackList.size(); ++i)
    {
        snackList[i].id = i + 1; // ID diatur ulang mulai dari 1
    }
}

void tampilkanSnack()
{

    if (snackList.empty())
    {
        cout << "Belum ada data snack yang tersedia." << endl;
        return;
    }
    // Header
    cout << left << setw(5) << "ID"
         << setw(20) << "Nama"
         << setw(15) << "Harga" << endl;
    cout << "---------------------------------------------" << endl;

    // Menampilkan daftar snack
    for (size_t i = 0; i < snackList.size(); ++i)
    {
        cout << left << setw(5) << snackList[i].id
             << setw(20) << snackList[i].nama
             << setw(15) << "Rp" + to_string(snackList[i].harga) << endl;
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
            simpanSnack();
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
                tampilkanSnack();
            }
            break;
        }
        case 3:
        { // Hapus Snack
            if (snackList.empty())
            {
                cout << "Tidak ada snack untuk dihapus.\n";
                break;
            }

            tampilkanSnack(); // Tampilkan daftar snack

            int id;
            bool found = false;

            // Loop untuk memastikan pengguna memasukkan ID yang valid
            while (true)
            {
                cout << "Masukkan ID Snack yang ingin dihapus: ";
                cin >> id;

                for (size_t i = 0; i < snackList.size(); ++i)
                {
                    if (snackList[i].id == id)
                    {
                        // Tampilkan detail snack yang akan dihapus
                        cout << "Anda yakin ingin menghapus snack \"" << snackList[i].nama
                             << "\" dengan harga " << snackList[i].harga << "? (y/n): ";
                        char konfirmasi;
                        cin >> konfirmasi;

                        if (konfirmasi == 'y' || konfirmasi == 'Y')
                        {
                            snackList.erase(snackList.begin() + i); // Hapus snack
                            perbaruiIDSnack();                      // Perbarui ID agar tetap berurutan
                            cout << "Snack berhasil dihapus.\n";
                        }
                        else
                        {
                            cout << "Penghapusan dibatalkan.\n";
                        }

                        found = true;
                        break;
                    }
                }

                if (found)
                {
                    break; // Keluar dari loop jika ID valid ditemukan
                }
                else
                {
                    cout << "Snack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\n";
                }
            }

            break;
        }

        case 4:
        { // Edit Snack
            if (snackList.empty())
            {
                cout << "Tidak ada snack untuk diedit.\n";
                break;
            }

            tampilkanSnack();

            int id;
            bool found = false;

            // Loop untuk memastikan pengguna memasukkan ID yang valid
            while (true)
            {
                cout << "Masukkan ID Snack yang ingin diedit: ";
                cin >> id;

                for (size_t i = 0; i < snackList.size(); ++i)
                {
                    if (snackList[i].id == id)
                    {
                        // Edit nama snack
                        cout << "Nama Snack Baru (" << snackList[i].nama << "): ";
                        cin.ignore();
                        getline(cin, snackList[i].nama);

                        // Edit harga snack
                        cout << "Harga Snack Baru (" << snackList[i].harga << "): ";
                        while (!(cin >> snackList[i].harga) || snackList[i].harga <= 0)
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Harga tidak valid. Masukkan harga yang benar: ";
                        }

                        cout << "Snack berhasil diedit.\n";
                        found = true;
                        break;
                    }
                }

                if (found)
                {
                    break; // Keluar dari loop jika ID valid ditemukan
                }
                else
                {
                    cout << "Snack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\n";
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

void tampilkanPesanan()
{

    if (pesananList.empty())
    {
        cout << "Belum ada data pesanan yang tersedia." << endl;
        return;
    }
    // Menampilkan header tabel
    cout << left << setw(5) << "No"
         << setw(12) << "Pesanan ID"
         << setw(15) << "Nama Tim"
         << setw(15) << "No HP"
         << setw(20) << "Nama Lapangan"
         << setw(20) << "Hari & Tanggal"
         << setw(15) << "Jam"
         << setw(20) << "Status"
         << setw(15) << "Total Harga" << endl;

    cout << "----------------------------------------------------------------------------------------------------------" << endl;

    // Menampilkan daftar pesanan
    for (size_t i = 0; i < pesananList.size(); ++i)
    {
        // Mengambil nama lapangan berdasarkan ID
        string namaLapangan = NamaLapangan(lapanganList, pesananList[i].idLapangan);

        // Menampilkan detail pesanan
        cout << left
             << setw(5) << i + 1                                                                  // Nomor urut
             << setw(12) << pesananList[i].idPesanan                                              // ID Pesanan
             << setw(15) << pesananList[i].namaUser                                               // Nama User
             << setw(15) << pesananList[i].nomorHp                                                // No HP
             << setw(20) << namaLapangan                                                          // Nama Lapangan
             << setw(20) << pesananList[i].jam.hari + ", " + pesananList[i].jam.tanggal           // Jam (Hari, Tanggal)
             << setw(15) << pesananList[i].jam.jamMulai + " - " + pesananList[i].jam.jamSelesai   // Jam Mulai - Selesai
             << setw(20) << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") // Status Pembayaran
             << setw(15) << pesananList[i].totalHarga                                             // Total Harga
             << endl;
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
                tampilkanPesanan();
            }
            break;
        }
        case 2:
        { // Proses Pembayaran
            if (pesananList.empty())
            {
                cout << "\n===== Daftar Pesanan =====\n";
                cout << "Tidak ada pesanan yang tersedia.\n";
                break;
            }

            // Menampilkan daftar pesanan yang ada
            tampilkanPesanan();

            int idPesanan;
            bool valid = false;

            // Loop untuk memastikan input ID yang benar
            while (true)
            {
                cout << "Masukkan ID Pesanan yang akan diproses pembayaran: ";
                cin >> idPesanan;

                // Validasi apakah ID pesanan ada
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

                    valid = true; // Menandai bahwa ID valid telah diproses
                    break;
                }
                else
                {
                    cout << "Pesanan tidak ditemukan. Silakan coba lagi.\n";
                }
            }

            if (valid)
            {
                simpanPemesanan(); // Simpan perubahan setelah pembayaran berhasil
            }

            break;
        }

        case 3:
        { // Batalkan Pesanan
            if (pesananList.empty())
            {
                cout << "Tidak ada pesanan yang tersedia untuk dibatalkan.\n";
                break;
            }

            tampilkanPesanan(); // Tampilkan daftar pesanan
            int idPesanan;
            bool valid = false;

            // Loop untuk memastikan input ID yang valid
            while (true)
            {
                cout << "Masukkan ID Pesanan yang akan dibatalkan: ";
                cin >> idPesanan;

                if (idPesanan > 0 && idPesanan <= pesananList.size())
                {
                    pesananList.erase(pesananList.begin() + idPesanan - 1); // Batalkan pesanan
                    cout << "Pesanan ID " << idPesanan << " berhasil dibatalkan.\n";
                    valid = true; // Menandakan ID yang valid telah diproses
                    break;
                }
                else
                {
                    cout << "Pesanan tidak ditemukan. Silakan coba lagi.\n";
                }
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

bool isPesananExist(const Pesanan &pesanan)
{
    for (const auto &p : pesananList)
    {
        if (p.idPesanan == pesanan.idPesanan) // Cek berdasarkan ID Pesanan
            return true;
    }
    return false;
}

void loadPemesanan()
{
    ifstream inFile("pemesanan.txt"); // Membuka file pemesanan
    if (inFile.is_open())
    {
        string line;
        while (getline(inFile, line))
        {
            Pesanan pesanan;
            stringstream ss(line);
            string field;
            string id;
            string statusPembayaran;

            // Baca ID Pesanan
            getline(ss, id, ',');
            pesanan.idPesanan = stoi(id); // Menambahkan pembacaan ID Pesanan

            // Baca Nama User
            getline(ss, pesanan.namaUser, ',');

            // Baca Nomor HP
            getline(ss, pesanan.nomorHp, ',');

            // Baca ID Lapangan
            getline(ss, field, ',');
            pesanan.idLapangan = stoi(field);

            // Baca informasi Jam
            getline(ss, pesanan.jam.hari, ',');       // Hari
            getline(ss, pesanan.jam.tanggal, ',');    // Tanggal
            getline(ss, pesanan.jam.jamMulai, ',');   // Jam Mulai
            getline(ss, pesanan.jam.jamSelesai, ','); // Jam Selesai

            // Baca Status Pembayaran
            getline(ss, statusPembayaran, ',');
            pesanan.statusPembayaran = (statusPembayaran == "Sudah Dibayar"); // Mengonversi status pembayaran

            // Tambahkan pesanan ke daftar pesanan
            pesananList.push_back(pesanan);
        }
        inFile.close();
        cout << "Pemesanan berhasil dimuat dari file.\n";
    }
    else
    {
        cout << "Gagal membuka file pemesanan.txt.\n";
    }
}

void updatePesananStatus(int idPesanan, bool statusPembayaran)
{
    // Mencari pesanan berdasarkan idPesanan
    for (Pesanan &pesanan : pesananList)
    {
        if (pesanan.idPesanan == idPesanan)
        {
            pesanan.statusPembayaran = statusPembayaran; // Memperbarui status pembayaran
            cout << "Status pesanan dengan ID " << idPesanan << " telah diperbarui.\n";
            return; // Setelah menemukan pesanan dan memperbarui status, keluar dari fungsi
        }
    }
    cout << "Pesanan dengan ID " << idPesanan << " tidak ditemukan.\n";
}

void simpanPemesanan()
{
    // Menyimpan data pemesanan
    ofstream outFile("pemesanan.txt", ios::trunc);
    if (outFile.is_open())
    {
        for (const Pesanan &pesanan : pesananList)
        {
            outFile
                << pesanan.idPesanan << ","
                << pesanan.namaUser << ","
                << pesanan.nomorHp << ","
                << pesanan.idLapangan << ","
                << pesanan.jam.hari << ","
                << pesanan.jam.tanggal << ","
                << pesanan.jam.jamMulai << ","
                << pesanan.jam.jamSelesai << ","
                << (pesanan.statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << "\n";
        }
        outFile.close();
        cout << "Data pemesanan berhasil disimpan.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk menyimpan data pemesanan.\n";
    }

    // Menyimpan data lapangan yang sudah diperbarui
    ofstream outLapangan("lapangan.txt", ios::trunc);
    if (outLapangan.is_open())
    {
        for (const Lapangan &lapangan : lapanganList)
        {
            outLapangan << lapangan.id << ","
                        << lapangan.nama << ","
                        << lapangan.harga << ","
                        << lapangan.status << "\n"; // Simpan status lapangan
        }
        outLapangan.close();
        cout << "Data lapangan berhasil disimpan.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk menyimpan data lapangan.\n";
    }

    // Menyimpan data jadwal yang sudah diperbarui
    ofstream outJadwal("jadwal.txt", ios::trunc);
    if (outJadwal.is_open())
    {
        for (const Jadwal &jadwal : jadwalList)
        {
            outJadwal << jadwal.id << ","
                      << jadwal.idLapangan << ","
                      << jadwal.hari << ","
                      << jadwal.tanggal << ","
                      << jadwal.jamMulai << ","
                      << jadwal.jamSelesai << ","
                      << jadwal.status << "\n"; // Simpan status jadwal
        }
        outJadwal.close();
        cout << "Data jadwal berhasil disimpan.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk menyimpan data jadwal.\n";
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

            cekPesanan();

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
    const string &namaLapangan,
    const string &jam,
    const string &status,
    const string &totalHarga)
{
    string sid = "AC525efca105257724b6b5a5be52d71215";        // Twilio Account SID
    string token = "0a3b9d9871388e05d5ac676b731687a1";        // Twilio Auth Token
    string contentSid = "HXe3a4cf8a57187688c94df5b4e3a2c6b0"; // ContentSid untuk template
    string url = "https://api.twilio.com/2010-04-01/Accounts/" + sid + "/Messages.json";

    // Buat ContentVariables JSON sesuai dengan parameter
    string contentVariables = "{\"1\":\"" + idPesanan +
                              "\",\"2\":\"" + nama +
                              "\",\"3\":\"" + nomor +
                              "\",\"4\":\"" + namaLapangan +
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

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    ((string *)userp)->append((char *)contents, totalSize);
    return totalSize;
}

// Fungsi untuk mengirimkan request POST ke API Midtrans
string sendPostRequest(const string &url, const string &postData, const string &auth_token)
{
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    string response_string;
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, ("Authorization: Basic " + auth_token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
    return response_string;
}

// Fungsi untuk mengekstrak redirect_url dari respons JSON
string extractRedirectUrl(const string &response)
{
    size_t startPos = response.find("\"redirect_url\":\"");
    if (startPos != string::npos)
    {
        startPos += 16; // Panjang dari "\"redirect_url\":\""
        size_t endPos = response.find("\"", startPos);
        if (endPos != string::npos)
        {
            return response.substr(startPos, endPos - startPos);
        }
    }
    return "";
}

void bukaURL(const string &redirectUrl)
{
    string command;

#ifdef _WIN32
    // Untuk Windows
    command = "start " + redirectUrl; // Perintah untuk Windows
#else
    // Untuk macOS atau Linux
    command = "open " + redirectUrl; // Perintah untuk macOS
#ifdef __linux__
                                     // Perintah untuk Linux
    command = "xdg-open " + redirectUrl;
#endif
#endif

    // Menjalankan perintah untuk membuka URL
    system(command.c_str());
}

// Fungsi untuk membuat pesanan
void buatPesanan()
{
    Pesanan pesanan;
    clearScreen();

    cout << "\n===== BUAT PESANAN =====\n";
    pesanan.idPesanan = generateId(); // Pastikan generateId() menghasilkan ID unik
    // Input nama
    cout << "Nama Tim Anda: ";
    cin.ignore(); // Untuk membersihkan buffer setelah membaca integer
    getline(cin, pesanan.namaUser);

    // Input nomor telepon
    pesanan.nomorHp = formatNomorInternasional(); // Format nomor menjadi internasional

    // Menampilkan daftar lapangan
    while (true)
    {
        // Tampilkan daftar lapangan
        cout << "\nDaftar Lapangan yang Tersedia:\n";
        bool adaLapanganTersedia = false;
        for (size_t i = 0; i < lapanganList.size(); ++i)
        {
            if (lapanganList[i].status == "Tersedia")
            {
                cout << lapanganList[i].id << ". " << lapanganList[i].nama
                     << " - Rp " << lapanganList[i].harga << "/jam"
                     << " - Status: " << lapanganList[i].status << "\n";
                adaLapanganTersedia = true;
            }
        }

        // Jika tidak ada lapangan tersedia, kembali ke menu utama
        if (!adaLapanganTersedia)
        {
            cout << "Maaf, tidak ada lapangan yang tersedia saat ini. Kembali ke menu utama.\n";
            break; // Keluar dari loop utama dan kembali ke menu utama
        }

        // Meminta input ID lapangan
        cout << "\nPilih ID Lapangan: ";
        cin.ignore();
        cin >> pesanan.idLapangan;

        // Validasi ID lapangan
        bool lapanganValid = false;
        for (size_t i = 0; i < lapanganList.size(); ++i)
        {
            if (lapanganList[i].id == pesanan.idLapangan && lapanganList[i].status == "Tersedia")
            {
                lapanganValid = true;
                break;
            }
        }

        if (!lapanganValid)
        {
            cout << "ID Lapangan tidak valid atau tidak tersedia. Silakan pilih ID yang sesuai.\n";
            continue; // Ulangi input ID lapangan jika tidak valid
        }

        // Tampilkan jadwal untuk lapangan yang dipilih
        bool jadwalDitemukan = false; // Flag untuk memastikan ada jadwal yang ditemukan
        while (true)
        {
            cout << "\nPilih Jam Tersedia untuk Lapangan ID " << pesanan.idLapangan << ":\n";

            vector<Jadwal> jadwalTersedia;
            for (size_t i = 0; i < jadwalList.size(); ++i)
            {
                if (jadwalList[i].idLapangan == pesanan.idLapangan && jadwalList[i].status == "Tersedia")
                {
                    jadwalTersedia.push_back(jadwalList[i]);
                }
            }

            // Jika tidak ada jadwal tersedia untuk lapangan ini
            if (jadwalTersedia.empty())
            {
                cout << "Tidak ada jadwal tersedia untuk lapangan ini. Silakan pilih lapangan lain.\n";
                break; // Kembali ke pemilihan lapangan
            }

            // Tampilkan jadwal yang tersedia
            for (size_t i = 0; i < jadwalTersedia.size(); ++i)
            {
                cout << i + 1 << ". " << jadwalTersedia[i].hari << ", " << jadwalTersedia[i].tanggal
                     << " (" << jadwalTersedia[i].jamMulai << " - " << jadwalTersedia[i].jamSelesai << ") - "
                     << jadwalTersedia[i].status << "\n";
            }

            // Meminta input nomor jadwal
            int jamIndex;
            cout << "Pilih nomor jadwal: ";
            cin.ignore();
            cin >> jamIndex;

            if (jamIndex >= 1 && jamIndex <= static_cast<int>(jadwalTersedia.size()))
            {
                // Simpan jadwal yang dipilih
                pesanan.jam = jadwalTersedia[jamIndex - 1];

                // Hitung durasi dan total harga
                int jamMulai = stoi(pesanan.jam.jamMulai.substr(0, pesanan.jam.jamMulai.find(':')));
                int jamSelesai = stoi(pesanan.jam.jamSelesai.substr(0, pesanan.jam.jamSelesai.find(':')));
                int durasiJam = jamSelesai - jamMulai;

                if (durasiJam <= 0)
                {
                    cout << "Durasi pemesanan tidak valid.\n";
                    continue; // Ulangi pemilihan jadwal
                }

                pesanan.totalHarga = lapanganList[pesanan.idLapangan - 1].harga * durasiJam;

                // Ubah status jadwal menjadi "Sudah Dipesan"
                for (size_t i = 0; i < jadwalList.size(); ++i)
                {
                    if (jadwalList[i].id == pesanan.jam.id)
                    {
                        jadwalList[i].status = "Sudah Dipesan";
                        break;
                    }
                }

                jadwalDitemukan = true;
                break; // Keluar dari loop pemilihan jadwal
            }
            else
            {
                cout << "Pilihan tidak valid, silakan pilih nomor jadwal yang benar.\n";
            }
        }

        // Jika jadwal ditemukan, lanjutkan ke pemilihan snack
        if (jadwalDitemukan)
        {
            break; // Keluar dari pemilihan lapangan dan lanjut ke pemilihan snack
        }
    }

    // Pemilihan Snack
    if (snackList.empty())
    {
        cout << "\nTidak ada snack yang tersedia.\n";
    }
    else
    {
        cout << "\nPilih Snack (masukkan ID, 0 untuk selesai):\n";

        for (size_t i = 0; i < snackList.size(); ++i)
        {
            cout << snackList[i].id << ". " << snackList[i].nama << " (" << snackList[i].harga << ")\n";
        }

        while (true)
        {
            int snackId;
            cout << "\nMasukkan ID Snack (0 untuk selesai): ";
            cin >> snackId;
            if (snackId == 0)
                break; // Berhenti jika ID snack 0

            bool snackValid = false;
            for (size_t i = 0; i < snackList.size(); ++i)
            {
                if (snackList[i].id == snackId)
                {
                    int jumlah;
                    cout << "Masukkan jumlah " << snackList[i].nama << ": ";
                    cin >> jumlah;
                    pesanan.snackIds.push_back(snackId);
                    pesanan.totalHarga += snackList[i].harga * jumlah; // Menambahkan harga snack ke total
                    snackValid = true;
                    break;
                }
            }

            if (!snackValid)
            {
                cout << "Snack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\n";
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
        pesanan.statusPembayaran = true;
        updatePesananStatus(pesanan.idPesanan, true);
        cout << "Status Pembayaran: Dibayar\n";          // Debugging
        string idPesanan = to_string(pesanan.idPesanan); // ID Pesanan
        string postData = "{"
                          "\"transaction_details\": {"
                          "\"order_id\": \"" +
                          idPesanan + "\","
                                      "\"gross_amount\": " +
                          to_string(pesanan.totalHarga) + "}"
                                                          "}";

        // URL API Midtrans dan server key
        string url = "https://app.sandbox.midtrans.com/snap/v1/transactions";
        string serverKey = "SB-Mid-server-I87PSHHP3LXI0gPl8Q6eHI_q"; // Ganti dengan server key Anda
        string auth_token = "U0ItTWlkLXNlcnZlci1JODdQU0hIUDNMWEkwZ1BsOFE2ZUhJX3E=";

        // Kirim request ke Midtrans
        string response = sendPostRequest(url, postData, auth_token);
        cout << "Response dari Midtrans: " << response << endl;

        // Mengekstrak URL redirect untuk pembayaran
        string redirectUrl = extractRedirectUrl(response);
        if (!redirectUrl.empty())
        {
            cout << "\nMengarahkan Anda ke halaman pembayaran Midtrans...\n";
            bukaURL(redirectUrl);
            // Mengarahkan ke URL pembayaran

            simpanPemesanan();
        }
        else
        {
            cout << "\nTerjadi kesalahan saat mendapatkan URL pembayaran.\n";
        }
    }
    else if (pilihanBayar == 2)
    {
        pesanan.statusPembayaran = false;
        updatePesananStatus(pesanan.idPesanan, false);
        cout << "Status Pembayaran: Belum Dibayar\n"; // Debugging
        cout << "\nPembayaran ditunda, pesanan berhasil dibuat dan menunggu pembayaran.\n";
        simpanPemesanan();
    }

    // Simpan data pesanan yang telah diperbarui
    // pesananList.push_back(pesanan); // Pastikan pesanan yang sudah diupdate statusnya ditambahkan ke list

    // Mengirimkan notifikasi WhatsApp setelah pesanan selesai
    cout << "\nMengirimkan notifikasi WhatsApp...\n";

    // Memastikan data yang benar dikirim ke fungsi kirimPesanWhatsApp
    string statusPembayaran = (pilihanBayar == 1) ? "Sudah Dibayar" : "Belum Dibayar";
    string namaLapangan = NamaLapangan(lapanganList, pesanan.idLapangan);
    kirimPesanWhatsApp(pesanan.nomorHp, to_string(pesanan.idPesanan), pesanan.namaUser, namaLapangan,
                       pesanan.jam.hari + ", " + pesanan.jam.tanggal + " (" + pesanan.jam.jamMulai + " - " + pesanan.jam.jamSelesai + ")",
                       statusPembayaran, to_string(pesanan.totalHarga));

    // Kembali ke menu pengguna
    cout << "\nKembali ke menu pengguna...\n";
    userMenu(); // Misalnya fungsi ini yang menampilkan menu pengguna
}

void cekPesanan()
{
    clearScreen();
    cout << "\n===== CEK PESANAN =====\n";

    // Memuat data dari file pemesanan.txt
    loadPemesanan();

    if (pesananList.empty())
    {
        cout << "Tidak ada data pesanan.\n";
        return;
    }

    // Meminta pengguna memasukkan nama
    string namaUser;
    cout << "Masukkan nama untuk mencari pesanan: ";
    cin.ignore(); // Mengabaikan karakter newline yang tersisa dari input sebelumnya
    getline(cin, namaUser);

    // Jika nama kosong, tampilkan pesan kesalahan
    if (namaUser.empty())
    {
        cout << "Nama tidak boleh kosong!\n";
        return;
    }

    // Proses pencarian nama dalam pesananList
    bool ditemukan = false;
    for (size_t i = 0; i < pesananList.size(); ++i)
    {
        if (pesananList[i].namaUser == namaUser) // Perbandingan langsung
        {
            ditemukan = true;
            string namaLapangan = NamaLapangan(lapanganList, jadwalList[i].idLapangan);
            cout << "Pesanan ID : " << pesananList[i].idPesanan << "\n";
            cout << "Nama: " << pesananList[i].namaUser << "\n";
            cout << "Lapangan ID: " << namaLapangan << "\n";
            cout << "Jam: " << pesananList[i].jam.hari << ", " << pesananList[i].jam.tanggal
                 << " (" << pesananList[i].jam.jamMulai << " - " << pesananList[i].jam.jamSelesai << ")\n";
            cout << "Total Harga: Rp " << pesananList[i].totalHarga << "\n";
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