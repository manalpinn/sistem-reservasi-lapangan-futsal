#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <limits>
#include <iomanip>
#include <thread>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <curl/curl.h>

#ifdef _WIN32
#include <conio.h> // Untuk getch() di Windows
#else
#include <termios.h>
#include <unistd.h>
#endif

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
    int nomorUrut;
    int idPesanan;
    string namaUser;
    string nomorHp;
    int idLapangan;
    Jadwal jam;
    vector<int> snackIds;
    vector<int> snackQuantities;
    int totalHarga;
    int totalHargaLapangan;
    bool statusPembayaran;
    string tanggalPesan;
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
void perbaruiStatusJadwal();

void crudSnack();
void loadSnack();
void simpanSnack();

void crudPemesanan();
void loadPemesanan();
void simpanPemesanan();

// Fungsi user
void buatPesanan();
void cekPesanan();

int main()
{

    loadLapangan();
    loadJadwal();
    loadSnack();
    loadPemesanan();
    perbaruiStatusJadwal();
    loginMenu();

    return 0;
}

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

        if (cin.fail() || pilihan < 1 || pilihan > 3) // Validasi input
        {
            cin.clear();                                                                         // Menghapus flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n');                                 // Mengosongkan buffer input
            cout << "\033[1;31m\nInput tidak valid. Masukkan angka antara 1 hingga 3.\033[0m\n"; // Pesan berwarna merah
            this_thread::sleep_for(chrono::seconds(2));                                          // Jeda 2 detik
            continue;                                                                            // Kembali ke awal loop
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
        }
    } while (pilihan != 3);
}

#ifdef _WIN32
string inputPassword()
{
    string password;
    char ch;

    cout << "Password: ";
    while ((ch = _getch()) != '\r')
    { // '\r' adalah Enter di Windows
        if (ch == '\b')
        { // Backspace
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b"; // Hapus karakter dari layar
            }
        }
        else
        {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}
#else
void disableEcho()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;   // Menonaktifkan echo
    t.c_lflag &= ~ICANON; // Menonaktifkan input baris
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableEcho()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;   // Mengaktifkan echo
    t.c_lflag |= ICANON; // Mengaktifkan input baris
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

string inputPassword()
{
    string password;
    char ch;

    disableEcho(); // Nonaktifkan echo
    cout << "Password: ";
    while (true)
    {
        ch = getchar(); // Membaca satu karakter
        if (ch == '\n') // Jika Enter ditekan, akhiri input
            break;
        if (ch == '\b' || ch == 127)
        { // Backspace
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b"; // Menghapus karakter terakhir dari terminal
            }
        }
        else
        {
            password += ch;
            cout << '*';
        }
    }
    enableEcho(); // Aktifkan kembali echo
    cout << endl;

    return password;
}
#endif

void adminMenu()
{

    string username, password;
    int attempts = 3; // Batas percobaan login
    clearScreen();
    while (attempts > 0)
    {
        cout << "\n===== LOGIN ADMIN =====\n";
        cout << "Username: ";
        cin >> username;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        password = inputPassword();

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

                // Validasi input pilihan
                if (cin.fail() || pilihan < 1 || pilihan > 5)
                {
                    cin.clear();                                         // Membersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Menghapus input yang salah
                    cout << "\033[1;31m\nInput tidak valid. Harap masukkan angka antara 1 hingga 5.\033[0m\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    continue;
                }

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
                    cout << "\nKembali ke menu utama.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }
            } while (pilihan != 5);
            return; // Keluar dari fungsi jika login berhasil dan menu selesai
        }
        else
        {
            attempts--;
            cout << "\033[1;31m\nUsername atau password salah! Anda memiliki " << attempts << " percobaan lagi.\033[0m\n";
        }
    }

    int countdown = 10; // Waktu hitung mundur dalam detik
    cout << "\n";
    while (countdown > 0)
    {
        cout << "\rMenunggu " << countdown << " detik sebelum mencoba lagi..." << flush;
        this_thread::sleep_for(chrono::seconds(1));
        countdown--;
    }

    clearScreen(); // Bersihkan layar setelah hitungan mundur selesai
    cout << "Silakan coba lagi nanti.\n";
    this_thread::sleep_for(chrono::seconds(2));
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
    // Menampilkan header tabel
    cout << endl;
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
        clearScreen();
        cout << "\n===== KELOLA LAPANGAN =====\n";
        cout << "1. Tambah Lapangan\n";
        cout << "2. Tampilkan Lapangan\n";
        cout << "3. Hapus Lapangan\n";
        cout << "4. Edit Lapangan\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        // Validasi input pilihan
        if (cin.fail() || pilihan < 1 || pilihan > 5)
        {
            cin.clear();                                         // Membersihkan flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer
            cout << "\033[1;31m\nPilihan tidak valid. Masukkan angka antara 1 hingga 5.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            continue; // Kembali ke awal loop
        }

        switch (pilihan)
        {

        case 1: // Tambah Lapangan
        {
            Lapangan lap;
            lap.id = lapanganList.size() + 1; // ID dihitung berdasarkan jumlah elemen

            cin.ignore(); // Membersihkan buffer input sebelum loop
            while (true)
            {
                cout << "\nNama Lapangan (atau ketik '0' untuk batal): ";
                getline(cin, lap.nama);

                // Cek apakah pengguna ingin membatalkan
                if (lap.nama == "0")
                {
                    cout << "\nKembali ke menu kelola lapangan.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                    break; // Keluar dari case
                }

                // Validasi input kosong atau hanya spasi
                if (lap.nama.empty() || lap.nama.find_first_not_of(" \t") == string::npos)
                {
                    cout << "\033[31mNama lapangan tidak boleh kosong atau hanya spasi!\033[0m\n";
                    continue;
                }

                // Validasi nama duplikat
                bool duplikat = false;
                for (const auto &l : lapanganList)
                {
                    if (l.nama == lap.nama)
                    {
                        cout << "\033[31mNama lapangan sudah ada, gunakan nama lain!\033[0m\n";
                        duplikat = true;
                        break;
                    }
                }

                if (!duplikat)
                    break; // Keluar dari loop jika valid
            }

            if (lap.nama == "0")
            {
                break;
            }

            // Validasi harga lapangan
            while (true)
            {
                cout << "\nHarga Lapangan: ";
                cin >> lap.harga;

                if (cin.fail() || lap.harga <= 0)
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\033[31mHarga lapangan harus berupa angka positif!\033[0m\n";
                }
                else
                {
                    break;
                }
            }

            lap.status = "Tersedia"; // Status default
            lapanganList.push_back(lap);
            simpanLapangan(); // Simpan ke file atau database
            cout << "\033[1;32m\nLapangan berhasil ditambahkan.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
        }

        case 2: // Tampilkan Lapangan
        {
            if (lapanganList.empty())
            {
                cout << "\nTidak ada data lapangan.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanLapangan(); // Menampilkan semua lapangan yang ada

                // Memberikan pesan kepada pengguna untuk kembali ke menu setelah menampilkan lapangan
                cout << "\nTekan Enter untuk kembali ke menu kelola lapangan...";
                cin.ignore();
                cin.get(); // Menunggu pengguna menekan Enter
                break;
            }
            break;
        }

        case 3: // Hapus Lapangan
        {
            if (lapanganList.empty())
            {
                cout << "\nTidak ada data lapangan untuk dihapus.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }

            tampilkanLapangan();
            int id;
            bool found = false;

            while (!found)
            {
                cout << "\nMasukkan ID Lapangan yang ingin dihapus (atau ketik '0' untuk batal) : ";
                cin >> id;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\033[31mInput tidak valid. Masukkan angka ID.\033[0m\n";
                    continue;
                }

                if (id == 0)
                {
                    cout << "\nKembali ke menu kelola lapangan.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                    break;
                }

                for (size_t i = 0; i < lapanganList.size(); ++i)
                {
                    if (lapanganList[i].id == id)
                    {
                        while (true)
                        {
                            cout << "\033[1;33m\nYakin ingin menghapus? (y/n):\033[0m";
                            char konfirmasi;
                            cin >> konfirmasi;
                            // Validasi input
                            if (konfirmasi == 'y' || konfirmasi == 'Y')
                            {
                                lapanganList.erase(lapanganList.begin() + i);
                                perbaruiIDLapangan(); // Perbarui ID
                                simpanLapangan();
                                cout << "\033[1;32m\nLapangan berhasil dihapus.\n\033[0m\n";
                                this_thread::sleep_for(chrono::seconds(1));
                                break; // Keluar dari loop setelah konfirmasi berhasil
                            }
                            else if (konfirmasi == 'n' || konfirmasi == 'N')
                            {
                                cout << "\nLapangan tidak dihapus.\n";
                                this_thread::sleep_for(chrono::seconds(1));
                                break; // Keluar dari loop jika user memilih tidak
                            }
                            else
                            {
                                // Jika input tidak valid
                                cout << "\nInput tidak valid! Harap masukkan 'y' untuk Ya atau 'n' untuk Tidak.\n";
                            }
                        }
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    cout << "\033[31mID tidak ditemukan. Coba lagi.\033[0m\n";
                }
            }
            break;
        }

        case 4: // Edit Lapangan
        {
            if (lapanganList.empty())
            {
                cout << "\nTidak ada data lapangan untuk diedit.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }

            tampilkanLapangan(); // Menampilkan daftar lapangan
            int id;
            bool found = false;

            while (!found)
            {
                cout << "\nMasukkan ID Lapangan yang ingin diedit (atau ketik '0' untuk batal) : ";
                cin >> id;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\033[31mInput tidak valid. Masukkan angka ID.\033[0m\n";
                    continue;
                }

                if (id == 0)
                {
                    cout << "\nKembali ke menu kelola lapangan.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                    break;
                }

                for (auto &lap : lapanganList)
                {
                    if (lap.id == id)
                    {
                        found = true;

                        // Edit nama lapangan
                        while (true)
                        {
                            cout << "\nNama Baru (" << lap.nama << "): ";
                            cin.ignore(); // Membersihkan buffer input
                            string namaBaru;
                            getline(cin, namaBaru);

                            if (namaBaru.empty() || namaBaru.find_first_not_of(" \t") == string::npos)
                            {
                                cout << "\033[31mNama tidak boleh kosong atau hanya spasi!\033[0m\n";
                                continue;
                            }

                            // Periksa apakah nama baru sudah ada
                            bool duplikat = false;
                            for (const auto &l : lapanganList)
                            {
                                if (l.nama == namaBaru && l.id != lap.id)
                                {
                                    cout << "\033[31mNama lapangan sudah ada, silakan masukkan nama yang berbeda.\033[0m\n";
                                    duplikat = true;
                                    break;
                                }
                            }

                            if (!duplikat)
                            {
                                lap.nama = namaBaru;
                                break;
                            }
                        }

                        // Edit harga lapangan
                        cout << "\nHarga Baru (" << lap.harga << "): ";
                        while (!(cin >> lap.harga) || lap.harga <= 0)
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "\033[31mHarga tidak valid. Masukkan angka positif!\033[0m\n\nHarga Lapangan Baru: ";
                        }

                        // Edit status lapangan
                        cout << "\nStatus Baru (" << lap.status << ") [Tersedia/Penuh]: ";
                        cin.ignore(); // Membersihkan buffer input
                        while (true)
                        {
                            string statusBaru;
                            getline(cin, statusBaru);

                            if (statusBaru == "Tersedia" || statusBaru == "Penuh")
                            {
                                lap.status = statusBaru;
                                break;
                            }

                            cout << "\033[31mStatus tidak valid. Masukkan 'Tersedia' atau 'Penuh'.\033[0m\n\nStatus Lapangan Baru: ";
                        }

                        // Simpan perubahan
                        simpanLapangan();
                        cout << "\033[1;32m\nLapangan berhasil diedit.\033[0m\n";
                        this_thread::sleep_for(chrono::seconds(1));
                        break;
                    }
                }

                if (!found)
                {
                    cout << "\033[31mID tidak ditemukan. Coba lagi.\033[0m\n";
                }
            }
            break;
        }

        case 5:
            cout << "\nKembali ke menu utama.\n";
            this_thread::sleep_for(chrono::seconds(1));
            break;
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

string getWaktuSekarang()
{
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", now);
    return string(buf);
}

string getWaktuPesan()
{
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now); // Format hanya sampai jam
    return string(buf);
}

// Fungsi untuk mengecek dan memperbarui status jadwal
void perbaruiStatusJadwal()
{
    string waktuSekarangStr = getWaktuSekarang(); // Dapatkan waktu sekarang (format: "YYYY-MM-DD HH:MM")

    // Hapus jadwal yang sudah lewat hari
    jadwalList.erase(
        remove_if(jadwalList.begin(), jadwalList.end(),
                  [&](const Jadwal &jadwal)
                  {
                      // Mengambil waktu selesai dari jadwal
                      string waktuAkhir = jadwal.tanggal + " " + jadwal.jamSelesai; // Format: "YYYY-MM-DD HH:MM"

                      // Hapus jadwal jika waktu selesai lebih kecil dari waktu sekarang
                      return waktuAkhir < waktuSekarangStr;
                  }),
        jadwalList.end());

    // Perbarui status jadwal yang masih aktif
    for (auto &jadwal : jadwalList)
    {
        string waktuAkhir = jadwal.tanggal + " " + jadwal.jamSelesai; // Format: "YYYY-MM-DD HH:MM"

        if (jadwal.status == "Sudah Dipesan" && waktuAkhir < waktuSekarangStr)
        {
            jadwal.status = "Tersedia"; // Ubah status menjadi Tersedia
            cout << "Jadwal pada " << jadwal.tanggal << " " << jadwal.jamSelesai << " diperbarui menjadi Tersedia.\n";
        }
    }

    for (size_t i = 0; i < jadwalList.size(); ++i)
    {
        jadwalList[i].id = i + 1; // ID diatur ulang mulai dari 1
    }
    simpanJadwal(); // Simpan perubahan ke file
    cout << "\033[32mJadwal yang sudah melewati hari telah dihapus.\033[0m\n";
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
    // Menampilkan header tabel dengan format yang rapi
    cout << "\n===== DAFTAR JADWAL =====\n";
    cout << "-----------------------------------------------------------------------------------\n";
    cout << left
         << setw(5) << "ID"
         << setw(22) << "Nama Lapangan"
         << setw(12) << "Hari"
         << setw(15) << "Tanggal"
         << setw(20) << "Jam"
         << setw(12) << "Status"
         << endl;
    cout << "-----------------------------------------------------------------------------------\n";

    // Menampilkan data jadwal dalam format tabel
    for (size_t i = 0; i < jadwalList.size(); ++i)
    {
        string namaLapangan = NamaLapangan(lapanganList, jadwalList[i].idLapangan);

        cout << left
             << setw(5) << jadwalList[i].id
             << setw(22) << namaLapangan
             << setw(12) << jadwalList[i].hari
             << setw(15) << jadwalList[i].tanggal
             << setw(20) << (jadwalList[i].jamMulai + " - " + jadwalList[i].jamSelesai)
             << setw(12) << jadwalList[i].status
             << endl;
    }

    cout << "-----------------------------------------------------------------------------------\n";
}

string getNamaHari(int dayOfWeek)
{
    switch (dayOfWeek)
    {
    case 0:
        return "Minggu";
    case 1:
        return "Senin";
    case 2:
        return "Selasa";
    case 3:
        return "Rabu";
    case 4:
        return "Kamis";
    case 5:
        return "Jumat";
    case 6:
        return "Sabtu";
    default:
        return "";
    }
}

void crudJadwal()
{
    int pilihan;

    do
    {
        clearScreen();
        cout << "\n===== KELOLA JADWAL =====\n";
        cout << "1. Tambah Jadwal\n";
        cout << "2. Tampilkan Jadwal\n";
        cout << "3. Hapus Jadwal\n";
        cout << "4. Edit Jadwal\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (cin.fail() || pilihan < 1 || pilihan > 5)
        {
            cin.clear();                                         // Membersihkan flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer
            cout << "\033[31m\nPilihan tidak valid. Masukkan angka antara 1 hingga 5.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            continue; // Kembali ke awal loop
        }

        switch (pilihan)
        {
        case 1:
        {
            // Tambah Jadwal
            Jadwal jadwal;
            jadwal.id = jadwalList.empty() ? 1 : jadwalList.back().id + 1; // ID unik berdasarkan ID terakhir

            // Menampilkan daftar lapangan yang tersedia
            if (lapanganList.empty())
            {
                cout << "\nTidak ada data jadwal.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanLapangan(); // Menampilkan semua lapangan yang ada
            }

            // Validasi ID Lapangan
            while (true)
            {
                cout << "\nMasukkan ID Lapangan (atau ketik '0' untuk batal): ";
                cin >> jadwal.idLapangan;

                // Validasi apakah input valid
                if (cin.fail() || jadwal.idLapangan < 0)
                {
                    cout << "\033[31mInput ID tidak valid. Masukkan angka id.\033[0m\n";
                    cin.clear();                                         // Membersihkan flag error
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Mengabaikan input yang salah
                    continue;
                }

                // Jika pengguna memilih batal
                if (jadwal.idLapangan == 0)
                {
                    cout << "\nKembali ke menu kelola jadwal\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }

                // Validasi apakah ID Lapangan ada dalam daftar
                bool idValid = false;
                for (const auto &lapangan : lapanganList)
                {
                    if (jadwal.idLapangan == lapangan.id)
                    {
                        idValid = true;
                        break;
                    }
                }

                if (!idValid)
                {
                    cout << "\033[31mID Lapangan tidak ditemukan. Silakan masukkan ID yang benar.\033[0m\n";
                }
                else
                {
                    break;
                }
            }

            if (jadwal.idLapangan == 0)
            {
                break; // Jika batal, keluar dari case dan kembali ke menu utama
            }

            cin.ignore(); // Membersihkan buffer input

            // Validasi Hari
            int jumlahHari;
            while (true)
            {
                cout << "\nMasukkan jumlah hari yang ingin dijadwalkan: ";
                cin >> jumlahHari;

                // Validasi input jumlah hari
                if (cin.fail() || jumlahHari <= 0)
                {
                    cout << "\033[31mInput tidak valid. Masukkan angka positif untuk jumlah hari.\033[0m\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }

                // Jumlah hari valid
                break;
            }

            // Menentukan tanggal hari ini
            time_t t = time(0); // Ambil waktu sekarang
            tm *now = localtime(&t);

            // Menambah jadwal untuk setiap hari
            for (int i = 0; i < jumlahHari; ++i)
            {
                // Increment tanggal
                now->tm_mday += 1; // Menambah satu hari
                mktime(now);       // Update tanggal untuk menangani perubahan bulan/tahun

                // Format tanggal menjadi YYYY-MM-DD
                stringstream tanggal;
                tanggal << (now->tm_year + 1900) << "-"                                  // Tahun (YYYY)
                        << (now->tm_mon + 1 < 10 ? "0" : "") << (now->tm_mon + 1) << "-" // Bulan (MM)
                        << (now->tm_mday < 10 ? "0" : "") << now->tm_mday;               // Tanggal (DD)                                  // Tahun (YYYY)

                // Dapatkan nama hari
                string hari = getNamaHari(now->tm_wday);

                // Tambah jadwal untuk setiap jam mulai dari 08:00 sampai 21:00
                for (int jam = 8; jam <= 21; ++jam)
                {
                    // Cek apakah sudah ada jadwal di waktu ini
                    bool jadwalTumpangTindih = false;
                    for (const auto &j : jadwalList)
                    {
                        // Periksa apakah jadwal untuk lapangan, hari, tanggal, dan jam tertentu sudah ada
                        if (j.idLapangan == jadwal.idLapangan &&
                            j.hari == hari &&
                            j.tanggal == tanggal.str() &&
                            j.jamMulai == (jam < 10 ? "0" : "") + to_string(jam) + ":00")
                        {
                            jadwalTumpangTindih = true;
                            break;
                        }
                    }

                    // Jika jadwal tumpang tindih ditemukan
                    if (jadwalTumpangTindih)
                    {
                        cout << "\033[33mJadwal untuk " << hari << ", " << tanggal.str() << " pukul "
                             << (jam < 10 ? "0" : "") << jam << ":00 sudah tersedia. Lewati...\033[0m\n";
                        continue;
                    }

                    // Tambah jadwal baru
                    Jadwal jadwalBaru;
                    jadwalBaru.id = jadwalList.empty() ? 1 : jadwalList.back().id + 1; // ID unik berdasarkan ID terakhir
                    jadwalBaru.idLapangan = jadwal.idLapangan;                         // Menggunakan ID Lapangan yang dipilih
                    jadwalBaru.hari = hari;
                    jadwalBaru.tanggal = tanggal.str();

                    // Set jam mulai dan selesai
                    jadwalBaru.jamMulai = (jam < 10 ? "0" : "") + to_string(jam) + ":00";
                    jadwalBaru.jamSelesai = ((jam + 1) < 10 ? "0" : "") + to_string(jam + 1) + ":00";

                    // Set status awal sebagai "Tersedia"
                    jadwalBaru.status = "Tersedia";

                    // Tambahkan ke daftar jadwal
                    jadwalList.push_back(jadwalBaru);
                }
            }

            // Simpan jadwal ke file jika diperlukan
            simpanJadwal(); // Panggil fungsi simpanJadwal jika ada
            cout << "\033[32m\nJadwal berhasil ditambahkan.\033[0m\n";

            this_thread::sleep_for(chrono::seconds(2));

            break;
        }

        case 2:
        { // Tampilkan Jadwal
            if (jadwalList.empty())
            {
                cout << "\nTidak ada data jadwal.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanJadwal(); // Menampilkan semua lapangan yang ada
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            break;
        }

        case 3:
        {
            // Hapus Jadwal
            if (jadwalList.empty())
            {
                cout << "\nTidak ada data jadwal.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanJadwal(); // Menampilkan semua lapangan yang ada
            }

            int index;
            // Loop untuk validasi input ID jadwal
            while (true)
            {
                cout << "Masukkan nomor data yang ingin dihapus (atau ketik '0' untuk batal): ";
                cin >> index;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\033[31mInput tidak valid. Masukkan angka ID.\033[0m\n";
                    continue;
                }

                if (index == 0)
                {
                    cout << "\nKembali ke menu Kelola jadwal.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break; // Kembali ke menu Kelola Lapangan
                }

                // Validasi apakah ID jadwal ada
                if (index >= 1 && index <= static_cast<int>(jadwalList.size()))
                {
                    // Tampilkan detail jadwal yang akan dihapus
                    cout << "\033[1;33m\nYakin ingin menghapus? (y/n):\033[0m";
                    char konfirmasi;
                    cin >> konfirmasi;

                    if (konfirmasi == 'y' || konfirmasi == 'Y')
                    {
                        // Jika konfirmasi ya, hapus data
                        jadwalList.erase(jadwalList.begin() + index - 1);
                        perbaruiIDJadwal(); // Perbarui ID agar tetap berurutan
                        simpanJadwal();
                        cout << "\033[32m\nData berhasil dihapus.\033[0m\n";
                        this_thread::sleep_for(chrono::seconds(1));
                        break;
                    }
                    else if (konfirmasi == 'n' || konfirmasi == 'N')
                    {
                        // Jika konfirmasi tidak, pembatalan
                        cout << "Hapus jadwal dibatalkan.\n";
                        this_thread::sleep_for(chrono::seconds(1));
                        break; // Keluar dari loop jika user memilih tidak
                    }
                    else
                    {
                        // Jika input tidak valid
                        cout << "\033[31mInput tidak valid! Harap masukkan 'y' untuk Ya atau 'n' untuk Tidak.\033[0m\n";
                    }
                }
                else
                {
                    cout << "\033[31mNomor jadwal tidak ditemukan. Silakan masukkan nomor yang benar.\033[0m\n";
                }
            }

            break;
        }

        case 4:
        {
            if (jadwalList.empty())
            {
                cout << "\nTidak ada data jadwal.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanJadwal(); // Menampilkan semua jadwal yang ada
            }

            int id;
            bool found = false;

            // Looping untuk memasukkan ID Jadwal yang ingin diedit
            while (true)
            {
                cout << "\nMasukkan ID Jadwal yang ingin diedit (atau ketik '0' untuk batal): ";
                cin >> id;

                // Validasi input ID
                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\033[31mInput tidak valid. Masukkan angka ID.\033[0m\n";
                    continue; // Kembali ke penginputan ID jika gagal
                }

                // Jika ID 0, batalkan dan kembali ke menu
                if (id == 0)
                {
                    cout << "Kembali ke menu kelola jadwal.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                    break;
                }

                // Mencari jadwal berdasarkan ID
                for (size_t i = 0; i < jadwalList.size(); ++i)
                {
                    if (jadwalList[i].id == id)
                    {
                        int idLapangan = jadwalList[i].idLapangan; // Ambil ID Lapangan dari jadwal
                        found = true;

                        // Cek apakah jadwal sudah dipesan
                        if (jadwalList[i].status == "Sudah Dipesan")
                        {
                            cout << "\033[31mJadwal ini sudah dipesan dan tidak bisa diedit.\033[0m\n";
                            this_thread::sleep_for(chrono::seconds(2));
                            break; // Keluar jika jadwal sudah dipesan
                        }

                        // Proses pengeditan jadwal
                        cin.ignore();
                        while (true)
                        {
                            // Edit Hari
                            cout << "\nHari Baru (" << jadwalList[i].hari << "): ";
                            string hariBaru;
                            getline(cin, hariBaru);

                            // Validasi hari
                            if (hariBaru.empty() || hariBaru.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != string::npos)
                            {
                                cout << "\033[31mNama hari tidak valid. Silakan coba lagi.\033[0m\n";
                                continue;
                            }

                            transform(hariBaru.begin(), hariBaru.end(), hariBaru.begin(), ::tolower);
                            hariBaru[0] = toupper(hariBaru[0]); // Format kapital pada huruf pertama
                            jadwalList[i].hari = hariBaru;      // Set hari baru
                            break;
                        }

                        // Edit Tanggal
                        while (true)
                        {
                            cout << "\nTanggal Baru (format YYYY-MM-DD, " << jadwalList[i].tanggal << "): ";
                            string tanggalBaru;
                            getline(cin, tanggalBaru);

                            if (regex_match(tanggalBaru, regex("^\\d{4}-\\d{2}-\\d{2}$")))
                            {
                                jadwalList[i].tanggal = tanggalBaru;
                                break;
                            }

                            cout << "\033[31mFormat atau nilai tanggal tidak valid. Silakan coba lagi.\033[0m\n";
                        }

                        // Edit Jam Mulai
                        while (true)
                        {
                            cout << "\nJam Mulai Baru (format HH:MM, " << jadwalList[i].jamMulai << "): ";
                            string jamMulaiBaru;
                            getline(cin, jamMulaiBaru);

                            if (regex_match(jamMulaiBaru, regex("^\\d{2}:\\d{2}$")))
                            {
                                bool jamTersedia = true;

                                for (const auto &jadwal : jadwalList)
                                {
                                    if (jadwal.idLapangan == idLapangan && jadwal.id != jadwalList[i].id &&
                                        jadwal.hari == jadwalList[i].hari && jadwal.tanggal == jadwalList[i].tanggal)
                                    {
                                        int hhMulai, mmMulai;
                                        sscanf(jamMulaiBaru.c_str(), "%d:%d", &hhMulai, &mmMulai);
                                        int waktuMulai = hhMulai * 60 + mmMulai;

                                        int jhhMulai, jmmMulai, jhhSelesai, jmmSelesai;
                                        sscanf(jadwal.jamMulai.c_str(), "%d:%d", &jhhMulai, &jmmMulai);
                                        sscanf(jadwal.jamSelesai.c_str(), "%d:%d", &jhhSelesai, &jmmSelesai);

                                        int jWaktuMulai = jhhMulai * 60 + jmmMulai;
                                        int jWaktuSelesai = jhhSelesai * 60 + jmmSelesai;

                                        if ((waktuMulai >= jWaktuMulai && waktuMulai < jWaktuSelesai))
                                        {
                                            jamTersedia = false;
                                            break;
                                        }
                                    }
                                }

                                if (jamTersedia)
                                {
                                    jadwalList[i].jamMulai = jamMulaiBaru;
                                    break;
                                }
                                else
                                {
                                    cout << "\033[31mJam mulai bertabrakan dengan jadwal lain. Silakan masukkan waktu berbeda.\033[0m\n";
                                }
                            }
                            else
                            {
                                cout << "\033[31mFormat jam tidak valid. Silakan masukkan sesuai format (HH:MM).\033[0m\n";
                            }
                        }

                        // Edit Jam Selesai
                        while (true)
                        {
                            cout << "\nJam Selesai Baru (format HH:MM, " << jadwalList[i].jamSelesai << "): ";
                            string jamSelesaiBaru;
                            getline(cin, jamSelesaiBaru);

                            if (regex_match(jamSelesaiBaru, regex("^\\d{2}:\\d{2}$")))
                            {
                                if (jamSelesaiBaru > jadwalList[i].jamMulai)
                                {
                                    jadwalList[i].jamSelesai = jamSelesaiBaru;
                                    break;
                                }
                                else
                                {
                                    cout << "\033[31mJam selesai harus lebih besar dari jam mulai. Silakan masukkan ulang.\033[0m\n";
                                }
                            }
                            else
                            {
                                cout << "\033[31mFormat jam tidak valid. Silakan masukkan sesuai format (HH:MM).\033[0m\n";
                            }
                        }

                        // Simpan Perubahan
                        simpanJadwal();
                        cout << "\033[1;32m\nJadwal berhasil diedit.\033[0m\n";
                        this_thread::sleep_for(chrono::seconds(2));
                        break;
                    }
                }

                if (!found)
                {
                    cout << "\033[31mJadwal dengan ID tersebut tidak ditemukan. Silakan coba lagi.\033[0m\n";
                }
                else
                {
                    break;
                }
            }
            break;
        }

        case 5: // Kembali
            cout << "\nKembali ke menu utama.\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
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
    // Header
    cout << endl;
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
        clearScreen();
        cout << "\n===== KELOLA SNACK =====\n";
        cout << "1. Tambah Snack\n";
        cout << "2. Tampilkan Snack\n";
        cout << "3. Hapus Snack\n";
        cout << "4. Edit Snack\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (cin.fail() || pilihan < 1 || pilihan > 5)
        {
            cin.clear();                                         // Membersihkan flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer
            cout << "\033[31m\nPilihan tidak valid. Masukkan angka antara 1 hingga 5.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            continue; // Kembali ke awal loop
        }
        switch (pilihan)
        {
        case 1:
        { // Tambah Snack
            Snack snack;
            snack.id = snackList.empty() ? 1 : snackList.back().id + 1; // ID otomatis berdasarkan ID terakhir

            // Validasi Nama Snack
            cin.ignore(); // Membersihkan buffer input sebelum loop
            while (true)
            {
                cout << "\nMasukkan nama Snack (atau ketik '0' untuk batal): ";
                getline(cin, snack.nama);

                // Cek apakah pengguna ingin membatalkan
                if (snack.nama == "0")
                {
                    cout << "\nKembali ke menu kelola snack.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                    break; // Keluar dari case
                }

                // Validasi input kosong atau hanya spasi
                if (snack.nama.empty() || snack.nama.find_first_not_of(" \t") == string::npos)
                {
                    cout << "\033[31mNama Snack tidak boleh kosong atau hanya spasi!\033[0m\n";
                    continue;
                }

                // Validasi nama duplikat
                bool duplikat = false;
                for (const auto &s : snackList)
                {
                    if (s.nama == snack.nama)
                    {
                        cout << "\033[31mNama Snack sudah ada, gunakan nama lain!\033[0m\n";
                        duplikat = true;
                        break;
                    }
                }

                if (!duplikat)
                    break; // Keluar dari loop jika valid
            }

            if (snack.nama == "0")
            {
                break;
            }

            // Validasi Harga Snack
            while (true)
            {
                cout << "\nHarga Snack: ";
                cin >> snack.harga;

                if (cin.fail() || snack.harga <= 0) // Harga harus angka positif
                {
                    cin.clear();                                         // Membersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan input buffer
                    cout << "\033[31mHarga tidak valid. Masukkan angka positif.\033[0m\n";
                }
                else
                {
                    break;
                }
            }

            // Tambahkan snack ke daftar
            snackList.push_back(snack);

            // Simpan data snack ke file
            simpanSnack();
            cout << "\033[32m\nSnack berhasil ditambahkan.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
        }

        case 2:
        { // Tampilkan Snack

            if (snackList.empty())
            {
                cout << "\nTidak ada data snack.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanSnack(); // Menampilkan semua lapangan yang ada
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            break;
        }
        case 3:
        { // Hapus Snack
            if (jadwalList.empty())
            {
                cout << "\nTidak ada data snack.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanSnack();
            }

            int id;
            bool found = false;

            // Loop untuk memastikan pengguna memasukkan ID yang valid
            while (true)
            {
                cout << "\nMasukkan ID Snack yang ingin dihapus (atau ketik '0' untuk batal) : ";
                cin >> id;

                // Validasi jika input bukan angka
                if (cin.fail())
                {
                    cin.clear();                                         // Membersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan input buffer
                    cout << "\033[31mInput tidak valid. Masukkan ID berupa angka.\033[0m\n";
                    continue;
                }

                if (id == 0)
                {
                    cout << "Kembali ke menu kelola snack.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }

                // Cari snack berdasarkan ID
                for (size_t i = 0; i < snackList.size(); ++i)
                {
                    if (snackList[i].id == id)
                    {
                        found = true;

                        // Tampilkan detail snack yang akan dihapus
                        cout << "\033[33m\nAnda yakin ingin menghapus? (y/n): \033[0m";
                        char konfirmasi;
                        while (true)
                        {
                            cin >> konfirmasi;

                            // Validasi input konfirmasi
                            if (konfirmasi == 'y' || konfirmasi == 'Y')
                            {
                                snackList.erase(snackList.begin() + i); // Hapus snack
                                perbaruiIDSnack();                      // Perbarui ID agar tetap berurutan
                                cout << "\033[32m\nSnack berhasil dihapus.\033[0m\n";
                                this_thread::sleep_for(chrono::seconds(2));
                                break; // Keluar dari loop setelah penghapusan
                            }
                            else if (konfirmasi == 'n' || konfirmasi == 'N')
                            {
                                cout << "Hapus snack dibatalkan.\n";
                                this_thread::sleep_for(chrono::seconds(2));
                                break; // Keluar dari loop jika penghapusan dibatalkan
                            }
                            else
                            {
                                // Jika input tidak valid, minta konfirmasi ulang
                                cout << "\033[31mInput tidak valid. Harap masukkan 'y' untuk ya atau 'n' untuk tidak: \033[0m";
                            }
                        }

                        break; // Keluar dari loop utama setelah pencarian
                    }
                }

                // Jika ID ditemukan, keluar dari loop
                if (found)
                {
                    break;
                }
                else
                {
                    cout << "\033[31mSnack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\033[0m\n";
                }
            }

            if (id == 0)
            {
                break;
            }

            break;
        }

        case 4:
        {
            // Edit Snack
            if (jadwalList.empty())
            {
                cout << "\nTidak ada data snack.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanSnack();
            }

            int id;
            bool found = false;

            // Loop untuk memastikan pengguna memasukkan ID yang valid
            while (true)
            {
                cout << "\nMasukkan ID Snack yang ingin diedit (atau ketik '0' untuk batal) : ";
                cin >> id;

                // Validasi input ID agar hanya menerima angka
                if (cin.fail())
                {
                    cin.clear();                                         // Bersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan buffer input
                    cout << "\033[31mInput tidak valid. Masukkan ID berupa angka.\033[0m\n";
                    continue;
                }

                if (id == 0)
                {
                    cout << "Kembali ke menu kelola snack.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }

                // Cari ID Snack dalam daftar
                for (size_t i = 0; i < snackList.size(); ++i)
                {
                    if (snackList[i].id == id)
                    {
                        found = true;

                        // Edit nama snack
                        string namaBaru;
                        while (true) // Meminta input nama snack hingga valid
                        {
                            cout << "\nNama Snack Baru (" << snackList[i].nama << "): ";
                            cin.ignore(); // Bersihkan buffer sebelum menerima input string
                            getline(cin, namaBaru);

                            // Validasi nama snack baru
                            if (!regex_match(namaBaru, regex("^[A-Za-z0-9 ]+$")))
                            {
                                cout << "\033[31mNama snack tidak valid. Hanya huruf, angka, dan spasi yang diperbolehkan.\033[0m\n";
                                continue;
                            }

                            // Cek duplikat nama snack
                            bool duplikat = false;
                            for (size_t j = 0; j < snackList.size(); ++j)
                            {
                                if (snackList[j].nama == namaBaru && snackList[j].id != snackList[i].id)
                                {
                                    duplikat = true;
                                    break;
                                }
                            }

                            if (duplikat)
                            {
                                cout << "\033[31mNama snack sudah ada dalam daftar. Silakan masukkan nama yang berbeda.\033[0m\n";
                                continue;
                            }

                            // Jika nama tidak duplikat, keluar dari loop input nama
                            snackList[i].nama = namaBaru;
                            break;
                        }

                        // Edit harga snack
                        cout << "\nHarga Snack Baru (" << snackList[i].harga << "): ";
                        double hargaBaru;
                        while (!(cin >> hargaBaru) || hargaBaru <= 0)
                        {
                            cin.clear();                                         // Bersihkan error flag
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan buffer input
                            cout << "\033[31mHarga tidak valid. Masukkan harga berupa angka positif: \033[0m";
                        }
                        snackList[i].harga = hargaBaru;

                        simpanSnack();
                        cout << "\033[32m\nSnack berhasil diedit.\033[0m\n";
                        this_thread::sleep_for(chrono::seconds(2));
                        break;
                    }
                }

                if (!found)
                {
                    cout << "\033[31mSnack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\033[0m\n";
                }
                else
                {
                    break; // Keluar dari loop jika ID valid ditemukan dan snack berhasil diedit
                }
            }

            if (id == 0)
            {
                break;
            }

            break;
        }

        case 5: // Kembali
            cout << "\nKembali ke menu utama.\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
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
    cout << endl;

    // Menampilkan header tabel dengan lebar yang lebih pas
    cout << left << setw(5) << "No"
         << setw(12) << "Pesanan ID"
         << setw(20) << "Nama Tim"
         << setw(15) << "No HP"
         << setw(20) << "Nama Lapangan"
         << setw(20) << "Hari & Tanggal"
         << setw(15) << "Jam"
         << setw(20) << "Status"
         << setw(12) << "Total Harga"
         << setw(25) << "Waktu Pesan" << endl;

    cout << string(160, '-') << endl; // Garis pemisah tabel

    // Menampilkan daftar pesanan
    for (size_t i = 0; i < pesananList.size(); ++i)
    {
        // Mengambil nama lapangan berdasarkan ID
        string namaLapangan = NamaLapangan(lapanganList, pesananList[i].idLapangan);

        // Menampilkan detail pesanan
        cout << left
             << setw(5) << i + 1                                                                  // Nomor urut
             << setw(12) << pesananList[i].idPesanan                                              // ID Pesanan
             << setw(20) << pesananList[i].namaUser                                               // Nama User
             << setw(15) << pesananList[i].nomorHp                                                // No HP
             << setw(20) << namaLapangan                                                          // Nama Lapangan
             << setw(20) << pesananList[i].jam.hari + ", " + pesananList[i].jam.tanggal           // Hari & Tanggal
             << setw(15) << pesananList[i].jam.jamMulai + " - " + pesananList[i].jam.jamSelesai   // Jam Mulai - Selesai
             << setw(20) << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") // Status Pembayaran
             << setw(12) << pesananList[i].totalHarga                                             // Total Harga
             << setw(25) << pesananList[i].tanggalPesan                                           // Waktu Pesan
             << endl;
    }
}

void crudPemesanan()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "\n===== KELOLA PEMESANAN =====\n";
        cout << "1. Tampilkan Semua Pesanan\n";
        cout << "2. Proses Pembayaran\n";
        cout << "3. Batalkan Pesanan\n";
        cout << "4. Total Pemasukan Perhari\n";
        cout << "5. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (cin.fail() || pilihan < 1 || pilihan > 5)
        {
            cin.clear();                                         // Membersihkan flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer
            cout << "\033[31m\nPilihan tidak valid. Masukkan angka antara 1 hingga 5.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(2));
            continue; // Kembali ke awal loop
        }

        switch (pilihan)
        {
        case 1:
        { // Tampilkan Semua Pesanan
            if (pesananList.empty())
            {
                cout << "\nTidak ada data pemesanan.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanPesanan(); // Menampilkan semua lapangan yang ada
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            break;
        }
        case 2:
        { // Proses Pembayaran
            if (pesananList.empty())
            {
                cout << "\nTidak ada data pemesanan.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanPesanan();
            }

            int idPesanan;
            bool valid = false;

            // Loop untuk memastikan input ID yang benar
            while (true)
            {
                cout << "\nMasukkan ID Pesanan yang akan diproses pembayaran (atau ketik '0' untuk batal) : ";
                cin >> idPesanan;

                // Validasi input ID (hanya angka positif)
                if (cin.fail())
                {
                    cin.clear();                                         // Bersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan buffer input
                    cout << "\033[31mInput tidak valid. Masukkan ID berupa angka.\033[0m\n";
                    continue;
                }

                if (idPesanan == 0)
                {
                    cout << "\nKembali ke menu kelola pemesanan.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }

                // Validasi apakah ID pesanan ada
                if (idPesanan > 0 && idPesanan <= static_cast<int>(pesananList.size()))
                {
                    Pesanan &pesanan = pesananList[idPesanan - 1]; // Referensi ke pesanan yang dipilih

                    if (pesanan.statusPembayaran)
                    {
                        cout << "\033[33mPesanan dengan ID " << idPesanan << " sudah dibayar.\033[0m\n";
                    }
                    else
                    {
                        // Konfirmasi sebelum memproses pembayaran
                        cout << "\033[33mApakah Anda yakin ingin memproses pembayaran untuk pesanan ID " << idPesanan << "? (y/n): \033[0m";
                        char konfirmasi;
                        cin >> konfirmasi;

                        if (konfirmasi == 'y' || konfirmasi == 'Y')
                        {
                            pesanan.statusPembayaran = true;
                            cout << "\033[32mPembayaran untuk pesanan ID " << idPesanan << " berhasil diproses.\033[0m";
                        }
                        else
                        {
                            cout << "Pembayaran dibatalkan.\n";
                        }
                    }

                    valid = true; // Menandai bahwa ID valid telah diproses
                    break;
                }
                else
                {
                    cout << "\033[31mPesanan dengan ID tersebut tidak ditemukan. Silakan coba lagi.\033[0m";
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
                cout << "\nTidak ada data pemesanan.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }
            else
            {
                tampilkanPesanan(); // Menampilkan semua lapangan yang ada
            }

            int idPesanan;
            bool valid = false;

            // Loop untuk memastikan input ID yang valid
            while (true)
            {
                cout << "\nMasukkan ID Pesanan yang akan dibatalkan (atau ketik '0' untuk batal) : ";
                cin >> idPesanan;

                // Validasi input ID (hanya angka positif)
                if (cin.fail())
                {
                    cin.clear();                                         // Bersihkan error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan buffer input
                    cout << "\033[31mInput tidak valid. Masukkan ID berupa angka.\033[0m\n";
                    continue;
                }

                if (idPesanan == 0)
                {
                    cout << "\nKembali ke menu Kelola pesanan.\n";
                    this_thread::sleep_for(chrono::seconds(2));
                    break;
                }

                // Validasi apakah ID pesanan ada
                if (idPesanan > 0 && idPesanan <= static_cast<int>(pesananList.size()))
                {
                    Pesanan &pesanan = pesananList[idPesanan - 1]; // Referensi ke pesanan yang dipilih

                    // Konfirmasi sebelum pembatalan pesanan
                    char konfirmasi;
                    while (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N')
                    {
                        cout << "\033[31mInput tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak: \033[0m";
                        cin >> konfirmasi;
                    }

                    if (konfirmasi == 'y' || konfirmasi == 'Y')
                    {
                        pesananList.erase(pesananList.begin() + idPesanan - 1); // Batalkan pesanan
                        cout << "\033[32\nmPesanan ID " << idPesanan << " berhasil dibatalkan.\n\033[0m";
                        this_thread::sleep_for(chrono::seconds(2));
                    }
                    else
                    {
                        cout << "Pembatalan pesanan dibatalkan.\n";
                        this_thread::sleep_for(chrono::seconds(2));
                    }

                    valid = true; // Menandai bahwa ID valid telah diproses
                    break;
                }
                else
                {
                    cout << "\033[31mPesanan dengan ID tersebut tidak ditemukan. Silakan coba lagi.\n\033[0m";
                }
            }

            if (valid)
            {
                simpanPemesanan(); // Simpan perubahan setelah pembatalan berhasil
            }

            break;
        }

        case 4: // Tampilkan Pesanan Berdasarkan Tanggal dan Total Pemasukan
        {
            if (pesananList.empty())
            {
                cout << "\nTidak ada data pemesanan.\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break;
            }

            string tanggal;
            bool adaPesanan;          // Flag untuk memeriksa apakah ada pesanan
            long long totalPemasukan; // Total pemasukan

            cin.ignore(); // Membersihkan buffer input
            while (true)
            {
                // Reset flag dan total pemasukan di setiap iterasi
                adaPesanan = false;
                totalPemasukan = 0;

                cout << "\nMasukkan tanggal (format YYYY-MM-DD) (atau ketik '0' untuk batal): ";
                getline(cin, tanggal);

                // Kembali ke menu kelola pemesanan jika input adalah '0'
                if (tanggal == "0")
                {
                    cout << "\nKembali ke menu kelola pemesanan\n";
                    this_thread::sleep_for(chrono::seconds(2)); // Menambahkan delay untuk pengalaman yang lebih baik
                    break;                                      // Keluar dari fungsi ini dan kembali ke menu Kelola Pemesanan
                }

                // Validasi format tanggal menggunakan regex
                if (!regex_match(tanggal, regex("^\\d{4}-\\d{2}-\\d{2}$")))
                {
                    cout << "\033[31mFormat tanggal tidak valid. Pastikan format YYYY-MM-DD.\033[0m\n";
                    continue; // Kembali ke awal loop jika format tidak valid
                }

                // Menampilkan pesanan berdasarkan tanggal
                int noPesanan = 1; // Penomoran pesanan
                for (const auto &pesanan : pesananList)
                {
                    if (pesanan.tanggalPesan == tanggal && pesanan.statusPembayaran)
                    {
                        // Tampilkan header tabel hanya sekali
                        if (!adaPesanan)
                        {
                            cout << "\nPesanan pada tanggal " << tanggal << ":\n";
                            cout << left
                                 << setw(5) << "No"
                                 << setw(15) << "ID Pesanan"
                                 << setw(20) << "Nama"
                                 << setw(15) << "Total (Rp)"
                                 << setw(15) << "Status" << endl;
                            cout << string(65, '-') << endl; // Tambahkan garis pemisah
                        }

                        cout << left
                             << setw(5) << noPesanan++
                             << setw(15) << pesanan.idPesanan
                             << setw(20) << pesanan.namaUser
                             << setw(15) << pesanan.totalHarga
                             << setw(15) << "Sudah DiBayar" << endl;

                        totalPemasukan += pesanan.totalHarga;
                        adaPesanan = true;
                    }
                }

                if (!adaPesanan)
                {
                    cout << "\033[33mTidak ada pesanan yang dibayar pada tanggal tersebut. Coba masukkan tanggal lain.\033[0m\n";
                    continue; // Meminta input ulang jika tidak ada pesanan
                }

                cout << "\n\033[32mTotal Pemasukan pada tanggal " << tanggal << ": Rp " << totalPemasukan << "\033[0m\n";
                cout << "\nTekan Enter untuk kembali ke menu...";
                cin.ignore();
                cin.get();
                break; // Keluar dari loop dan kembali ke menu Kelola Pemesanan
            }

            if (tanggal == "0")
            {
                break;
            }
        }

        case 5:
            cout << "\nKembali ke menu utama.\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
        }
    } while (pilihan != 5);
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
    pesananList.clear();

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
            string nomorUrut;

            // Baca Nomor Urut
            getline(ss, nomorUrut, ','); // Membaca nomor urut
            pesanan.nomorUrut = stoi(nomorUrut);

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

            getline(ss, id, ',');
            pesanan.totalHarga = stoi(id);

            getline(ss, pesanan.tanggalPesan, ','); // Tanggal

            // Tambahkan pesanan ke daftar pesanan
            pesananList.push_back(pesanan);
        }
        inFile.close();
        cout << "Data pemesanan berhasil dimuat.\n";
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
    cin.ignore();
    ofstream outFile("pemesanan.txt", ios::trunc);
    if (outFile.is_open())
    {
        for (size_t i = 0; i < pesananList.size(); ++i)
        {
            outFile
                << i + 1 << "," // Menggunakan indeks i untuk nomor urut
                << pesananList[i].idPesanan << ","
                << pesananList[i].namaUser << ","
                << pesananList[i].nomorHp << ","
                << pesananList[i].idLapangan << ","
                << pesananList[i].jam.hari << ","
                << pesananList[i].jam.tanggal << ","
                << pesananList[i].jam.jamMulai << ","
                << pesananList[i].jam.jamSelesai << ","
                << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << ","
                << pesananList[i].totalHarga << ","
                << pesananList[i].tanggalPesan << "\n";
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

    do
    {
        clearScreen();
        cout << "\n===== MENU USER =====\n";
        cout << "1. Buat Pesanan\n";
        cout << "2. Cek Pesanan\n";
        cout << "3. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (cin.fail() || pilihan < 1 || pilihan > 3)
        {
            cout << "\033[31m\nInput tidak valid! Harap masukkan angka antara 1 hingga 3.\033[0m\n";
            cin.clear();                                         // Menghapus status error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Menghapus sisa input
            this_thread::sleep_for(chrono::seconds(2));
            continue; // Kembali ke awal do-while untuk mencoba lagi
        }

        switch (pilihan)
        {
        case 1:
            buatPesanan();
            break;
        case 2:
            cekPesanan();
            break;
        case 3:
            cout << "\nKembali ke menu utama...\n";
            this_thread::sleep_for(chrono::seconds(2));
            break;
        }
    } while (pilihan != 3);
}

// Fungsi untuk memformat nomor telepon ke format internasional
string formatNomorInternasional()
{
    string nomor;
    while (true)
    {
        cout << "\nMasukkan nomor HP (dalam format lokal atau internasional): ";
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
            cerr << "\033[31mNomor tidak valid. Pastikan dimulai dengan +62 atau 0.\033[0m\n";
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
    string token = "7710317ab52f54ea6f739ab4773ea4f0";        // Twilio Auth Token
    string contentSid = "HX6deef8a5d3f4a06a1ce49e3d52ff21c3"; // ContentSid untuk template
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
    // cout << "Mengirim pesan ke WhatsApp...\n";
    perintah += " > /dev/null 2>&1"; // Mengarahkan stdout dan stderr ke /dev/null
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

string sendGetRequest(const string &url, const string &auth_token)
{
    CURL *curl;
    CURLcode res;
    string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Basic " + auth_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            cout << "Curl request failed: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    // Mengembalikan hasil respons yang disimpan dalam readBuffer
    return readBuffer;
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

int getSnackPrice(int snackId, const vector<Snack> &snackList)
{
    for (const auto &snack : snackList)
    {
        if (snack.id == snackId)
        {
            return snack.harga;
        }
    }
    return 0; // Jika snack tidak ditemukan
}

// Fungsi untuk mendapatkan nama snack berdasarkan ID
string getSnackName(int snackId, const vector<Snack> &snackList)
{
    for (const auto &snack : snackList)
    {
        if (snack.id == snackId)
        {
            return snack.nama;
        }
    }
    return "Unknown Snack";
}

// Fungsi untuk membuat pesanan
void buatPesanan()
{
    Pesanan pesanan;
    clearScreen();
    cout << "\n===== BUAT PESANAN =====\n";
    pesanan.idPesanan = generateId(); // Pastikan generateId() menghasilkan ID unik
    // Input nama
    cout << "Nama Tim Anda (atau ketik '0' untuk batal): ";
    cin.ignore(); // Untuk membersihkan buffer setelah membaca integer
    getline(cin, pesanan.namaUser);

    if (pesanan.namaUser == "0")
    {
        cout << "\nKembali ke menu.\n";
        this_thread::sleep_for(chrono::seconds(2));
        return;
    }

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
            cout << "\033[31mMaaf, tidak ada lapangan yang tersedia saat ini. Kembali ke menu utama.\033[0m\n";
            break; // Keluar dari loop utama dan kembali ke menu utama
        }

        // Meminta input ID lapangan
        cout << "\nPilih ID Lapangan: ";
        cin >> pesanan.idLapangan;

        cin.ignore();

        if (cin.fail())
        {
            cout << "\033[31mInput tidak valid! Harap masukkan ID lapangan yang valid (angka).\033[0m\n";
            cin.clear();                                         // Reset error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the invalid input from buffer
            continue;                                            // Re-attempt input
        }

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
            cout << "\033[31mID Lapangan tidak valid atau tidak tersedia. Silakan pilih ID yang sesuai.\033[0m\n";
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
                cout << "\033[31mTidak ada jadwal tersedia untuk lapangan ini. Silakan pilih lapangan lain.\033[0m\n";
                break; // Kembali ke pemilihan lapangan
            }

            // Tampilkan jadwal yang tersedia
            cout << "\nDaftar Jadwal Tersedia:\n";
            cout << setw(4) << left << "No" << "  "
                 << setw(10) << "Hari" << "  "
                 << setw(10) << "Tanggal" << "  "
                 << setw(17) << "Jam" << "  "
                 << "Status\n";

            for (size_t i = 0; i < jadwalTersedia.size(); ++i)
            {
                cout << setw(4) << i + 1 << "  "
                     << setw(10) << left << jadwalTersedia[i].hari << "  "
                     << setw(10) << jadwalTersedia[i].tanggal << "  "
                     << setw(17) << jadwalTersedia[i].jamMulai + " - " + jadwalTersedia[i].jamSelesai << "  "
                     << jadwalTersedia[i].status << "\n";
            }

            // Meminta input nomor jadwal
            int jamIndex;
            cout << "\nPilih nomor jadwal: ";
            cin >> jamIndex;

            if (cin.fail())
            {
                cout << "\033[31mInput tidak valid! Harap masukkan angka yang sesuai.\033[0m\n";
                cin.clear();                                         // Menghapus status error
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Menghapus sisa input di buffer
                continue;
            }

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
                    cout << "\033[31mDurasi pemesanan tidak valid.\033[0m\n";
                    continue; // Ulangi pemilihan jadwal
                }

                pesanan.totalHargaLapangan = lapanganList[pesanan.idLapangan - 1].harga * durasiJam;
                // pesananList.push_back(pesanan);

                // Ubah status jadwal menjadi "Sudah Dipesan"
                for (size_t i = 0; i < jadwalList.size(); ++i)
                {
                    if (jadwalList[i].id == pesanan.jam.id)
                    {
                        jadwalList[i].status = "Sudah Dipesan";
                        break;
                    }
                }
                pesanan.totalHarga = pesanan.totalHargaLapangan;
                jadwalDitemukan = true;
                break; // Keluar dari loop pemilihan jadwal
            }
            else
            {
                cout << "\033[31mPilihan tidak valid, silakan pilih nomor jadwal yang benar.\033[0m\n";
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

        // Menampilkan daftar snack yang tersedia
        for (size_t i = 0; i < snackList.size(); ++i)
        {
            cout << snackList[i].id << ". " << snackList[i].nama << " (" << snackList[i].harga << ")\n";
        }

        while (true)
        {
            int snackId;
            cout << "\nMasukkan ID Snack (0 untuk selesai): ";
            cin >> snackId;

            // Menangani input yang tidak valid untuk ID Snack
            if (cin.fail())
            {
                cout << "\033[31mInput tidak valid! Harap masukkan angka yang sesuai dengan ID Snack.\033[0m\n";
                cin.clear();                                         // Reset error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input from buffer
                continue;                                            // Kembali ke awal loop untuk mencoba input ID snack lagi
            }

            // Jika pengguna memasukkan 0, keluar dari loop
            if (snackId == 0)
            {
                cout << "\nPemilihan snack selesai.\n";
                break;
            }

            // Validasi ID Snack
            bool snackValid = false;
            for (size_t i = 0; i < snackList.size(); ++i)
            {
                if (snackList[i].id == snackId)
                {
                    snackValid = true;
                    int jumlah;

                    // Validasi input jumlah snack
                    while (true)
                    {
                        cout << "\nMasukkan jumlah " << snackList[i].nama << ": ";
                        cin >> jumlah;

                        // Pengecekan input jumlah
                        if (cin.fail())
                        {
                            cout << "\033[31mInput jumlah tidak valid! Harap masukkan angka yang sesuai.\033[0m\n";
                            cin.clear();                                         // Reset error flag
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input from buffer
                            continue;                                            // Kembali ke awal loop untuk mencoba input jumlah lagi
                        }

                        if (jumlah <= 0)
                        {
                            cout << "\033[31mJumlah harus lebih besar dari 0.\033[0m\n";
                            continue; // Kembali ke awal loop untuk mencoba input jumlah lagi
                        }

                        // Jika input jumlah valid, proses pesanan snack
                        bool snackSudahAda = false;
                        for (size_t j = 0; j < pesanan.snackIds.size(); ++j)
                        {
                            if (pesanan.snackIds[j] == snackId)
                            {
                                pesanan.snackQuantities[j] += jumlah;              // Tambahkan jumlah snack yang sama
                                pesanan.totalHarga += snackList[i].harga * jumlah; // Update total harga
                                snackSudahAda = true;
                                break;
                            }
                        }

                        if (!snackSudahAda)
                        {
                            pesanan.snackIds.push_back(snackId);
                            pesanan.snackQuantities.push_back(jumlah);
                            pesanan.totalHarga += snackList[i].harga * jumlah;
                        }

                        break; // Keluar dari loop pemilihan jumlah setelah berhasil
                    }

                    break; // Keluar dari loop pemilihan snack setelah berhasil
                }
            }

            if (!snackValid)
            {
                cout << "\033[31mSnack dengan ID tersebut tidak ditemukan. Silakan coba lagi.\033[0m\n";
            }
        }
    }

    // Output total harga
    cout << "\nPesanan berhasil dibuat! Total Harga: Rp " << pesanan.totalHarga << "\n";

    // Pilihan pembayaran
    int pilihanBayar;
    cout << "\nPilih opsi pembayaran:\n";
    cout << "1. Bayar Sekarang\n";
    cout << "2. Bayar Di Tempat\n";
    while (true)
    {
        cout << "Pilih: ";
        cin >> pilihanBayar;

        // Memeriksa apakah input valid dan berupa angka
        if (cin.fail() || pilihanBayar < 1 || pilihanBayar > 2)
        {
            // Menghapus input yang salah
            cin.clear();                                         // Menghapus status error cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Menghapus karakter yang tersisa di buffer
            cout << "\033[31mInput tidak valid. Harap masukkan angka 1 atau 2.\033[0m\n";
        }
        else
        {
            // Jika input valid
            break; // Keluar dari loop jika input sudah benar
        }
    }

    if (pilihanBayar == 1)
    {
        string idPesanan = to_string(pesanan.idPesanan); // ID Pesanan
        string namaLapangan = NamaLapangan(lapanganList, pesanan.idLapangan);

        // Menyusun item_details, termasuk lapangan dan snack
        string itemDetails = "[";

        // Menambahkan lapangan ke item_details
        itemDetails += "{"
                       "\"id\": \"" +
                       to_string(pesanan.idLapangan) + "\","
                                                       "\"name\": \"" +
                       namaLapangan + "\","
                                      "\"price\": " +
                       to_string(pesanan.totalHargaLapangan) + ","
                                                               "\"quantity\": 1"
                                                               "}";

        // Menambahkan snack ke item_details
        for (size_t i = 0; i < pesanan.snackIds.size(); ++i)
        {
            string snackName = getSnackName(pesanan.snackIds[i], snackList); // Ambil nama snack berdasarkan ID
            int snackPrice = getSnackPrice(pesanan.snackIds[i], snackList);  // Ambil harga snack berdasarkan ID
            int snackId = pesanan.snackIds[i];
            int snackQuantity = pesanan.snackQuantities[i]; // Ambil jumlah snack yang dipesan

            // Menambahkan snack ke itemDetails
            itemDetails += ",{"
                           "\"id\": \"" +
                           to_string(snackId) + "\","
                                                "\"name\": \"" +
                           snackName + "\","
                                       "\"price\": " +
                           to_string(snackPrice) + ","
                                                   "\"quantity\": " +
                           to_string(snackQuantity) + "}";
        }

        // Menutup array item_details
        itemDetails += "]";

        // Menyusun postData dan memastikan gross_amount sesuai dengan total harga item
        string postData = "{"
                          "\"transaction_details\": {"
                          "\"order_id\": \"" +
                          to_string(pesanan.idPesanan) + "\","
                                                         "\"gross_amount\": " +
                          to_string(pesanan.totalHarga) + "},"
                                                          "\"item_details\": " +
                          itemDetails + "}";

        // Output untuk memverifikasi JSON yang dikirim
        // cout << "JSON yang dikirim: " << postData << endl;

        // URL API Midtrans dan server key
        string url = "https://app.sandbox.midtrans.com/snap/v1/transactions";
        string serverKey = "SB-Mid-server-I87PSHHP3LXI0gPl8Q6eHI_q"; // Ganti dengan server key Anda
        string auth_token = "U0ItTWlkLXNlcnZlci1JODdQU0hIUDNMWEkwZ1BsOFE2ZUhJX3E=";

        // Kirim request ke Midtrans
        string response = sendPostRequest(url, postData, auth_token);

        // response midtrans
        // cout << "Response dari Midtrans: " << response << endl;

        // Mengekstrak URL redirect untuk pembayaran
        string redirectUrl = extractRedirectUrl(response);
        if (!redirectUrl.empty())
        {
            cout << "\nMengarahkan Anda ke halaman pembayaran Midtrans...\n";
            bukaURL(redirectUrl);
            string statusResponse;
            bool pembayaranSelesai = false;

            // Loop untuk meminta konfirmasi pembayaran sampai selesai
            while (!pembayaranSelesai)
            {
                string konfirmasi;
                cout << "\033[33m\nApakah Anda sudah melakukan pembayaran? (y/n): \033[0m";
                cin >> konfirmasi;

                if (konfirmasi == "y" || konfirmasi == "Y")
                {
                    // Jika pengguna sudah bayar, cek status pembayaran
                    cout << "\nMemeriksa status pembayaran...\n";

                    // URL untuk cek status transaksi menggunakan order_id
                    string url = "https://api.sandbox.midtrans.com/v2/" + to_string(pesanan.idPesanan) + "/status";

                    // Pastikan sendGetRequest mengembalikan respons status
                    statusResponse = sendGetRequest(url, auth_token);

                    if (statusResponse.empty())
                    {
                        cout << "\033[31mGagal mendapatkan respons dari server. Coba lagi nanti.\n\033[0m";
                        continue; // Lanjutkan loop untuk mencoba lagi
                    }

                    cout << "Status Response: " << statusResponse << endl;

                    // Cek apakah transaksi tidak ditemukan (status_code = 404)
                    if (statusResponse.find("\"status_code\":\"404\"") != string::npos)
                    {
                        cout << "\033[31mTransaksi tidak ditemukan. Mengarahkan Anda kembali ke halaman pembayaran...\n\033[0m";
                        bukaURL(redirectUrl); // Buka kembali URL pembayaran
                        continue;             // Lanjutkan loop untuk menunggu konfirmasi ulang
                    }

                    // Memeriksa apakah pembayaran berhasil (transaction_status = settlement)
                    if (statusResponse.find("\"transaction_status\": \"settlement\"") != string::npos)
                    {
                        cout << "\033[32mPembayaran berhasil! Pesanan selesai.\n\033[0m";
                        this_thread::sleep_for(chrono::seconds(2));
                        pesanan.statusPembayaran = true;
                        updatePesananStatus(pesanan.idPesanan, true); // Update status pesanan ke berhasil
                        pembayaranSelesai = true;                     // Keluar dari loop karena pembayaran berhasil
                    }
                    // Memeriksa apakah pembayaran masih pending (transaction_status = pending)
                    else if (statusResponse.find("\"transaction_status\": \"pending\"") != string::npos)
                    {
                        cout << "\033[33mPembayaran masih pending. Memeriksa lagi dalam 10 detik...\n\033[0m";
                        this_thread::sleep_for(chrono::seconds(10)); // Menunggu 10 detik untuk memeriksa lagi
                    }
                    else
                    {
                        cout << "\033[32mPembayaran berhasil! Pesanan selesai.\n\033[0m";
                        pesanan.statusPembayaran = true;
                        updatePesananStatus(pesanan.idPesanan, true); // Update status pesanan ke gagal
                        pembayaranSelesai = true;                     // Keluar dari loop jika status tidak valid
                    }
                }
                else if (konfirmasi == "n" || konfirmasi == "N")
                {
                    // Jika pengguna belum bayar, beri tahu dan minta untuk membayar
                    cout << "\033[31m\nSilakan lakukan pembayaran dan coba lagi.\n\033[0m";
                    bukaURL(redirectUrl);
                }
                else
                {
                    cout << "\033[31m\nInput tidak valid. Harap masukkan 'y' atau 'n'.\n\033[0m";
                }
            }
        }
        else
        {
            cout << "\033[31m\nPembayaran belum selesai, menunggu konfirmasi lebih lanjut...\n\033[0m";
        }
    }
    else if (pilihanBayar == 2)
    {
        pesanan.statusPembayaran = false;
        updatePesananStatus(pesanan.idPesanan, false);
        cout << "Status Pembayaran: Belum Dibayar\n"; // Debugging
        cout << "\nPembayaran ditunda, pesanan berhasil dibuat dan silahkan bayar di tempat.\n";
    }

    pesanan.tanggalPesan = getWaktuPesan();
    pesananList.push_back(pesanan);
    simpanPemesanan();

    // Mengirimkan notifikasi WhatsApp setelah pesanan selesai
    // cout << "\nMengirimkan notifikasi WhatsApp...\n";

    // Memastikan data yang benar dikirim ke fungsi kirimPesanWhatsApp
    string statusPembayaran = (pilihanBayar == 1) ? "Sudah Dibayar" : "Belum Dibayar";
    string namaLapangan = NamaLapangan(lapanganList, pesanan.idLapangan);
    kirimPesanWhatsApp(pesanan.nomorHp, to_string(pesanan.idPesanan), pesanan.namaUser, namaLapangan,
                       pesanan.jam.hari + ", " + pesanan.jam.tanggal + " (" + pesanan.jam.jamMulai + " - " + pesanan.jam.jamSelesai + ")",
                       statusPembayaran, to_string(pesanan.totalHarga));

    // Kembali ke menu pengguna
    cout << "\nKembali ke menu pengguna...\n";

    int countdown = 10; // Waktu hitung mundur dalam detik

    while (countdown > 0)
    {
        cout << "\rMenunggu " << countdown << " kembali ke menu pengguna..." << flush;
        this_thread::sleep_for(chrono::seconds(1));
        countdown--;
    }
    userMenu();
}

void cekPesanan()
{
    clearScreen();
    cout << "\n===== CEK PESANAN =====\n";

    // Memuat data dari file pemesanan.txt
    loadPemesanan();

    if (pesananList.empty())
    {
        cout << "\nTidak ada data pesanan.\n";
        cout << "\nTekan Enter untuk kembali ke menu...\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get(); // Tunggu input Enter dari pengguna
        return;
    }

    // Meminta pengguna memasukkan nama
    string namaUser;
    cout << "\nMasukkan nama untuk mencari pesanan (atau 0 untuk batal): ";
    cin.ignore(); // Mengabaikan karakter newline yang tersisa dari input sebelumnya
    getline(cin, namaUser);

    // Jika nama kosong, tampilkan pesan kesalahan
    if (namaUser.empty())
    {
        cout << "\033[31mNama tidak boleh kosong!\033[0m\n";

        // Menunggu hingga input yang valid, tanpa harus tekan Enter
        while (namaUser.empty())
        {
            cout << "\nMasukkan nama yang valid: ";
            getline(cin, namaUser);
        }
    }

    if (namaUser == "0")
    {
        cout << "\nTekan Enter untuk kembali ke menu...\n";
        cin.get(); // Tunggu input Enter dari pengguna
        return;
    }

    // Mengubah input nama pengguna menjadi huruf kecil agar tidak peka terhadap kapitalisasi
    transform(namaUser.begin(), namaUser.end(), namaUser.begin(), ::tolower);

    // Proses pencarian nama dalam pesananList
    bool ditemukan = false;
    for (size_t i = 0; i < pesananList.size(); ++i)
    {
        string namaPesanan = pesananList[i].namaUser;

        // Mengubah nama yang ada dalam pesananList menjadi huruf kecil juga
        transform(namaPesanan.begin(), namaPesanan.end(), namaPesanan.begin(), ::tolower);

        if (namaPesanan == namaUser) // Perbandingan tidak sensitif terhadap huruf kapital
        {
            ditemukan = true;
            string namaLapangan = NamaLapangan(lapanganList, jadwalList[i].idLapangan);
            cout << "\nNomor Urut Pesanan : " << pesananList[i].nomorUrut << "\n";
            cout << "Pesanan ID : " << pesananList[i].idPesanan << "\n";
            cout << "Nama: " << pesananList[i].namaUser << "\n";
            cout << "Nama Lapangan: " << namaLapangan << "\n";
            cout << "Jam: " << pesananList[i].jam.hari << ", " << pesananList[i].jam.tanggal
                 << " (" << pesananList[i].jam.jamMulai << " - " << pesananList[i].jam.jamSelesai << ")\n";
            cout << "Status Pembayaran: "
                 << (pesananList[i].statusPembayaran ? "Sudah Dibayar" : "Belum Dibayar") << "\n";
            cout << "Total Harga : " << pesananList[i].totalHarga << "\n";
            cout << "--------------------------\n";
        }
    }

    if (!ditemukan)
    {
        cout << "\033[33mPesanan tidak ditemukan untuk nama \"" << namaUser << "\".\033[0m\n";
    }

    // Tunggu pengguna membaca output sebelum kembali ke menu
    cout << "\nTekan Enter untuk kembali ke menu...\n";
    cin.get(); // Tunggu input Enter dari pengguna
}
