#include <iostream>

#define SEPARATOR "$"

int main() {
    std::string message = "aaaaaaaaaa_aaaaaaaaaa_bbbbbbbbbb_aaaaaaaaaa_cccccccccc_aaaaaaaaaa_aaaaaaaaaa_aaaaaaaaaa_aaaaaaaaaa_aaaaaaaaaa$410";



    std::string response = "abc$5$0";

    unsigned long first_index_of_fake_num = response.find_first_of(SEPARATOR);
    unsigned long last_index_of_fake_num = response.find_last_of(SEPARATOR);

    int from_server = std::atoi(response.substr(first_index_of_fake_num + 1, last_index_of_fake_num).c_str());
    std::cout << "from_server = " << from_server << std::endl;

    unsigned long last_index_of_right_num = response.find_last_of(SEPARATOR);
    int right_number = std::atoi(response.substr(last_index_of_right_num + 1, response.size() - 1).c_str());
    std::cout << "right_number = " << right_number << std::endl;

   /* unsigned long n = message.find_first_of("$") + 1;
    size_t size = message.size();

    std::string s = message.substr(n,size);
    int a = std::atoi(s.c_str());
    std::cout << a << std::endl;

    std::cout << message.size() << std::endl;
    size_t BUFFER_SIZE = 40;
    size_t amount_parts = message.size() / BUFFER_SIZE;
    std::cout << amount_parts << std::endl;

    char temp[BUFFER_SIZE];
    std::size_t length = message.copy(temp, 0, BUFFER_SIZE );
    temp[length] = '\0';
    std::cout << temp << std::endl;*/

    return 0;
}