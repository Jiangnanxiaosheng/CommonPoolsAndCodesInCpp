#include <iostream>
#include <string>
#include <fstream>
#include "Singleton.hpp"

class FileMgr : public Singleton<FileMgr> {
    friend class Singleton<FileMgr>;
    FileMgr() {}
public:

    void writeToFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << content;
            file.close();
        }
    }

    std::string readFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

    ~FileMgr() {}
};

int main() {
    CHOOSE = 1; // Specify CHOOSE = 1 or 0

#if CHOOSE
    FileMgr& fileMgr = FileMgr::getInstance();
    FileMgr& fileMgr2 = FileMgr::getInstance();
    fileMgr.writeToFile("example.txt", "Hello, World!");
    std::string content = fileMgr.readFromFile("example.txt");

    std::cout << &fileMgr << '\n';
    std::cout << &fileMgr2 << '\n';

# else
    std::shared_ptr<FileMgr> fileMgr = FileMgr::getInstance();
    std::shared_ptr<FileMgr> fileMgr2 = FileMgr::getInstance();
    fileMgr->writeToFile("example.txt", "Hello, World!");
    std::string content = fileMgr->readFromFile("example.txt");

    std::cout << fileMgr.get() << '\n';
    std::cout << fileMgr2.get() << '\n';
#endif

    std::cout << "File content: " << content << std::endl;

    return 0;
}
