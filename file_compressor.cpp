#include <iostream>
#include <fstream>
#include <string>
#include <zlib.h>

void compressFile(const std::string &inFile, const std::string &outFile) {
    std::ifstream input(inFile, std::ios::binary);
    std::ofstream output(outFile, std::ios::binary);

    if (!input || !output) {
        std::cerr << "File open error.\n";
        return;
    }

    const int bufferSize = 16384;
    char inBuffer[bufferSize];
    char outBuffer[bufferSize];

    z_stream zs{};
    deflateInit(&zs, Z_BEST_COMPRESSION);

    int flush;
    do {
        input.read(inBuffer, bufferSize);
        zs.avail_in = input.gcount();
        zs.next_in = reinterpret_cast<Bytef *>(inBuffer);

        flush = input.eof() ? Z_FINISH : Z_NO_FLUSH;

        do {
            zs.avail_out = bufferSize;
            zs.next_out = reinterpret_cast<Bytef *>(outBuffer);
            deflate(&zs, flush);
            int have = bufferSize - zs.avail_out;
            output.write(outBuffer, have);
        } while (zs.avail_out == 0);

    } while (flush != Z_FINISH);

    deflateEnd(&zs);
    std::cout << "File compressed successfully.\n";
}

void decompressFile(const std::string &inFile, const std::string &outFile) {
    std::ifstream input(inFile, std::ios::binary);
    std::ofstream output(outFile, std::ios::binary);

    if (!input || !output) {
        std::cerr << "File open error.\n";
        return;
    }

    const int bufferSize = 16384;
    char inBuffer[bufferSize];
    char outBuffer[bufferSize];

    z_stream zs{};
    inflateInit(&zs);

    do {
        input.read(inBuffer, bufferSize);
        zs.avail_in = input.gcount();
        if (zs.avail_in == 0) break;

        zs.next_in = reinterpret_cast<Bytef *>(inBuffer);

        do {
            zs.avail_out = bufferSize;
            zs.next_out = reinterpret_cast<Bytef *>(outBuffer);
            inflate(&zs, Z_NO_FLUSH);
            int have = bufferSize - zs.avail_out;
            output.write(outBuffer, have);
        } while (zs.avail_out == 0);

    } while (!input.eof());

    inflateEnd(&zs);
    std::cout << "File decompressed successfully.\n";
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage:\n  compress <input> <output>\n  decompress <input> <output>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    if (mode == "compress") {
        compressFile(inputFile, outputFile);
    } else if (mode == "decompress") {
        decompressFile(inputFile, outputFile);
    } else {
        std::cerr << "Invalid mode. Use 'compress' or 'decompress'.\n";
        return 1;
    }

    return 0;
}
