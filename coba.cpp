#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

int main() {
    // Ambil waktu saat ini
    time_t t = time(0);
    tm *now = localtime(&t);

    // Format tanggal menjadi YYYY-MM-DD
    stringstream tanggal;
    tanggal << (now->tm_year + 1900) << "-"  // Tahun (YYYY)
            << (now->tm_mon + 1 < 10 ? "0" : "") << (now->tm_mon + 1) << "-"  // Bulan (MM)
            << (now->tm_mday < 10 ? "0" : "") << now->tm_mday;  // Tanggal (DD)

    cout << "Tanggal saat ini: " << tanggal.str() << endl;
    return 0;
}
