#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/crc.hpp>
#include <boost/endian/conversion.hpp>
using boost::asio::ip::tcp;
class AsyncClient {
public:
    AsyncClient(boost::asio::io_context& io_context,
                const std::string& host, const std::string& service)
      : resolver_(io_context), socket_(io_context) {
        resolver_.async_resolve(host, service,
            [this](auto ec, auto endpoints) {
                if (!ec) {
                    boost::asio::async_connect(socket_, endpoints,
                        [this](auto ec, auto) {
                            if (!ec) {
                                do_write();
                            }
                        });
                }
            });
    }

private:
    void do_write() {
        std::string message = "{\"userID\": \"admin\",\n\"secret\": \"admin\"\n}\n";
        std::string method = "user.auth";
        char buffer[1024] = {0};
        int size = message.length() + method.length() + 2;

        strncpy(buffer+sizeof(int32_t), method.c_str(), 1024);
        memcpy(buffer + sizeof(int32_t) + method.length() +1, message.c_str(), message.length());

        boost::crc_32_type crc32;
        crc32.process_bytes(buffer + sizeof(int32_t), size);

        int32_t crc32_value = crc32.checksum();
        boost::endian::native_to_big_inplace(crc32_value);
        memcpy(buffer + sizeof(int32_t) + size, &crc32_value, sizeof(int32_t));

        int32_t size_be = size;
        boost::endian::native_to_big_inplace(size_be);
        memcpy(buffer, &size_be, sizeof(int32_t));
        
        boost::asio::async_write(socket_, boost::asio::buffer(buffer, size + 8),
            [this](auto ec, auto) {
                if (!ec) {
                    do_read();
                }
            });
    }
    void do_read() {
        socket_.async_read_some(boost::asio::buffer(buf_),
            [this](auto ec, auto length) {
                if (!ec) {
                    std::cout << std::string(buf_.data(), length) << std::endl;
                    do_read();
                }
            });
    }

    tcp::resolver resolver_;
    tcp::socket socket_;
    std::array<char, 1024> buf_;
};

void deleteSubTree(void *pRoot) {
    void *pChild = nullptr;
    void *pSibling = pRoot;

    if (pRoot == nullptr) {
        return;
    }

    do
    {
        pChild = getChild(pRoot);
        deleteSubTree(pChild);
    } while (pSibling = getSibling(pSibling));

    delete pRoot;
}

int main() {
    boost::asio::io_context io_context;
    AsyncClient client(io_context, "127.0.0.1", "10000");
    io_context.run();
    return 0;
}
